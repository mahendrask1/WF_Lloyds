/* Modbus.c
 * Modbus RTU slave implementation (LPC17xx UART3, half-duplex RS-485 via MAX487).
 *
 * Supported function codes:
 *   FC 0x01 - Read Coils: read/write bits M000–M039 from ram_values; addr 0 = M000; bits packed LSB first.
 *   FC 0x02 - Read Discrete Inputs: read-only bits M040–M099 from ram_values; addr 40 = M040; bits packed LSB first.
 *   FC 3     - Read Holding Registers: data from writable array modarray_wr (addr 0..(MAX_HOLDING_REG-1)).
 *   FC 4     - Read Input Registers:  data from read-only array modarray_rd (addr 0-based).
 *   FC 0x05 - Write Single Coil: write one coil M000–M039 (addr 0–39); 0xFF00=ON, 0x0000=OFF.
 *   FC 0x06 - Write Single Register: one 16-bit value to modarray_wr; register 0 updates Common_Setpoint.
 *   FC 0x0F - Write Multiple Coils: write coils M000–M039 (start addr 0–39, quantity 1–40); bits LSB first.
 *   FC 0x10 - Write Multiple Registers; writes into modarray_wr; register 0 updates Common_Setpoint.
 *   FC 8    - Diagnostic (echo subfunction).
 *
 * CRC byte order: responses send CRC as [CRCHi][CRCLo] (reversed from standard Modbus
 * [CRCLo][CRCHi]) to comply with existing master/system. DO NOT CHANGE this order.
 *
 * ---------------------------------------------------------------------------
 * STATE MACHINE: Modbus slave response (FC 3 / FC 4)
 * ---------------------------------------------------------------------------
 *
 *   [IDLE]  RX mode, UART receiving bytes into ModBusMessage[], ModBusIdx incremented per byte
 *      |
 *      |  (Timer: 3.5 char silence after last byte)
 *      v
 *   [FRAME_READY]  ModBusIdx > 6 -> rx_msg=1, DIRN=TX (stop further RX)
 *      |
 *      |  modbus() called (main loop)
 *      v
 *   [CRC_CHECK]  GetCRC16(ModBusMessage, ModBusIdx-1); compare with received CRC
 *      |
 *      +-- CRC BAD  --> [EXCEPTION]  Build [Addr][FC|0x80][0x17][CRCLo][CRCHi], ModTxLen=5, send
 *      |
 *      +-- CRC OK   --> Parse ModAddr, ModFunction, ModData[]; ModDataLen = ModBusIdx-3
 *      |                    |
 *      |                    v
 *      |              [ADDR_MATCH?]  (ModAddr == Comm_Address || 0 || 255) && (ModDataLen != 0)
 *      |                    |
 *      |                    +-- NO  --> [DISCARD]  ModBusIdx=-1, DIRN=RX, no response
 *      |                    |
 *      |                    +-- YES --> switch(ModFunction)
 *      |                                    |
 *      |                                    v
 *      |              [FC 3 / FC 4]  Read Holding / Input Registers
 *      |                    |
 *      |                    |  FC 3: data from modarray_wr (holding); FC 4: from modarray_rd (input)
 *      |                    |  Append CRC; ModTxLen = DataByteCount+5; send first byte
 *      |                    v
 *      |              [TX_RESPONSE]  Rest of response sent by UART ISR (ModTxCount, ModTxLen)
 *      |                    |
 *      v                    v
 *   [IDLE]  DIRN=RX (after TX complete), wait next frame
 *
 * Request  (master):  [SlaveAddr][3 or 4][StartAddrHi][StartAddrLo][QtyHi][QtyLo][CRCLo][CRCHi]
 * Response (slave):   [SlaveAddr][3 or 4][ByteCount][Reg0Hi][Reg0Lo]...[CRCLo][CRCHi]
 */
#include "stdio.h"
#include "string.h"
#include "modbus.h"
#include "LPC17xx.h"
#include "GPIO_LPC17xx.h"
#include "lpc17xx_uart.h"
#include "parameters.h"
#include "keys.h"
#include "timer.h"
#include <stdint.h>

/* Holding registers: valid indices 0..24 (25 registers). Reject if start + count > 25. e.g. start=21 count=5 → error. */
#define MAX_HOLDING_REG       25
/* Coils (read/write) M000–M039: Modbus coil addresses 0–39. */
#define COIL_ADDR_MIN         0
#define COIL_ADDR_MAX         39
#define MAX_COILS             40
#define MARKER_RAM_OFFSET     80   /* M000 = ram_values[80], ... M099 = ram_values[179] (plc.h) */

/* FC02 address map (read-only):
 * 40..99   -> M040..M099
 * 100..149 -> reserved gap (invalid)
 * 150..189 -> I000..I039
 * 190..219 -> reserved gap (invalid)
 * 220..259 -> O000..O039
 */
#define DI_MARKER_ADDR_MIN    40
#define DI_MARKER_ADDR_MAX    99
#define DI_INPUT_ADDR_MIN     150
#define DI_INPUT_ADDR_MAX     189
#define DI_OUTPUT_ADDR_MIN    220
#define DI_OUTPUT_ADDR_MAX    259
#define INPUT_RAM_OFFSET      0
#define OUTPUT_RAM_OFFSET     40

unsigned int ij;
/* modarray_rd[] and modarray_wr[] are 8-bit (char) arrays. Register values are stored with
 * MSB first, LSB second: byte index 2*k = high byte, 2*k+1 = low byte (per earlier design).
 * No int16_t array; copy directly to/from ModBusMessage as bytes. */
volatile char modarray_rd[MAX_LENGTH], modarray_wr[MAX_LENGTH];
/* CRC in frame is sent as CRCHi then CRCLo (intentional; do not change). */
unsigned char CRCHi,CRCLo;
unsigned char ModAddr,ModFunction,DataByteCount;
int ModDataLen;
int ModTxLen, ModTxCount;
extern int ModBusIdx,Common_Setpoint;
/* FC 3/4 response: 3 header + (2*RegCount) data + 2 CRC. 25 regs = 55 bytes. */
unsigned char ModBusMessage[MODBUS_TX_BUF_SIZE], ModData[MODBUS_PDU_DATA_MAX];
//char ram_value[8];
unsigned char Baud_Rate_Modbus,Comm_Address;
extern int rx_msg;
extern uint16_t New_menu_parameters[105];
extern char Check_Start ;
extern char Status_No,Error_No;
extern char Start_Stop_status;
uint32_t Modbus_Baud_Rate;
uint8_t Modbus_Parity ;
uint8_t  Modbus_Monitor_Time,Mod_Monitoring_Flag ;
unsigned char Modbus_Comm_Fault = 0;
extern unsigned char Zero_Tacho_Feed_Buffer_Flag;
extern uint8_t ram_values[200];   /* marker status M000–M099 = ram_values[80..179] */

unsigned char CRC16Hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* CRC16 Table Low byte */
unsigned char CRC16Lo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2,
    0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
    0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
    0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6,
    0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
    0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE,
    0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA,
    0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
    0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
    0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62,
    0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE,
    0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
    0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76,
    0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
    0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
    0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A,
    0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86,
    0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

void Modbus_Monitoring_Time(void)
{
    if ((Check_Start != 10) && (SIO_Monitor > 0)) {
        if ((Error_No != 2) && (SIO_Interface > 0)) {
            Modbus_Monitor_Time--;
        }

        if (Mod_Monitoring_Flag == 1) {
            Modbus_Monitor_Time = SIO_Monitor;
            Mod_Monitoring_Flag = 0;
            Modbus_Comm_Fault = 0;
            Error_No = 0;
        }
        if (Modbus_Monitor_Time == 0) {
            Modbus_Monitor_Time = 0;
            Modbus_Comm_Fault = 1;
            Error_No = 2;
            Start_Stop_status = STOP;
            Zero_Tacho_Feed_Buffer_Flag = 2;
        }
    }
}

void DIRN_MAX487(char on)
{
    GPIO_PinWrite(1, 1, on);
}

/*************************************************************************
 * Serial port initialization
 * Baudrate: 9600-00, 19200-01, 38400-02, 57600-03, 115200-04
 *************************************************************************/
void serial_init(void)
{
    Modbus_Monitor_Time = SIO_Monitor;

    if (SIO_Interface == 1) {
        Comm_Address = SIO_Address;
        switch (SIO_Baud_Rate) {
            case 0: Modbus_Baud_Rate = 19200;   break;
            case 1: Modbus_Baud_Rate = 38400;   break;
            case 2: Modbus_Baud_Rate = 57600;   break;
            default: break;
        }
        switch (SIO_Setup) {
            case 0: Modbus_Parity = UART_PARITY_NONE;  break;  /* 8-1-N */
            case 1: Modbus_Parity = UART_PARITY_EVEN;   break;  /* 8-1-E */
            case 2: Modbus_Parity = UART_PARITY_ODD;   break;  /* 8-1-O */
            default: break;
        }
        Timer2_init();
    } else if ((SIO_Interface == 2) || (SIO_Interface == 3)) { //msk Modbus-TCP/Profibus 26-may-2026
        Timer3_init();
        Modbus_Baud_Rate = 57600;
        Modbus_Parity = UART_PARITY_NONE;
    } else {
        Modbus_Baud_Rate = 19200; //msk SIO off: UART3 ISR discards RX 26-may-2026
        Modbus_Parity = UART_PARITY_NONE;
    }

    GPIO_SetDir(1, 1, GPIO_DIR_OUTPUT);
    UART_MODBUS_CONFIG();
    DIRN_MAX487(0); //msk 26-may-2026
    if (SIO_Interface == 0) {
        ModBusIdx = -1; //msk SIO off: no RTU frame assembly 26-may-2026
    }
}

/*************************************************************************
 * CRC calculation - CRC16 based on lookup table
 *************************************************************************/
unsigned int GetCRC16(unsigned char *puchMsg, int DataLen)
{
    unsigned int Index;

    CRCHi = 0xFF;
    CRCLo = 0xFF;

    while (DataLen--) {
        Index = CRCHi ^ *puchMsg++;
        CRCHi = CRCLo ^ CRC16Hi[Index];
        CRCLo = CRC16Lo[Index];
    }
    return (unsigned)CRCHi << 8 | CRCLo;
}

/*************************************************************************
                    Compose Modbus message
*************************************************************************/

/*--------------------------------------------------------------------------
 * Write Single Register (FC 0x06).
 * Request: [RegAddr Hi/Lo] [RegVal Hi/Lo] (4 data bytes).
 * Writes one 16-bit value to modarray_wr[]; valid reg addr 0..(MAX_HOLDING_REG-1). Reg 0 -> Common_Setpoint.
 * Response: echo of request (addr, 0x06, reg addr, value) + CRC.
 *--------------------------------------------------------------------------*/
static int WriteSingleReg(int data_len)
{
    unsigned short int RegAddr;
    uint16_t value;

    if (data_len < 4) {
        return -1;
    }
    RegAddr = ((unsigned)ModData[0] << 8) + ModData[1];
    if (RegAddr >= MAX_HOLDING_REG) {
        return -1;
    }

    value = (uint16_t)((ModData[2] << 8) | ModData[3]);
    modarray_wr[2 * RegAddr]     = (char)(value >> 8);
    modarray_wr[2 * RegAddr + 1] = (char)(value & 0xFF);
    if (RegAddr == 0) {
        Common_Setpoint = (int)value;
    }

    ModBusMessage[0] = Comm_Address;
    ModBusMessage[1] = ModFunction;
    ModBusMessage[2] = ModData[0];
    ModBusMessage[3] = ModData[1];
    ModBusMessage[4] = ModData[2];
    ModBusMessage[5] = ModData[3];
    GetCRC16(ModBusMessage, 6);
    ModBusMessage[6] = CRCHi;
    ModBusMessage[7] = CRCLo;
    ModTxLen = 8;
    ModTxCount = 1;
    DIRN_MAX487(1);
    UART_SendByte((LPC_UART_TypeDef *)LPC_UART3, ModBusMessage[0]);
    return 0;
}

/*--------------------------------------------------------------------------
 * Write Multiple Registers (FC 0x10).
 * Request: [StartAddr Hi/Lo] [RegCount Hi/Lo] [ByteCount] [Reg0 Hi/Lo] [Reg1 Hi/Lo] ...
 * Writes into modarray_wr[] (8-bit: MSB at 2*k, LSB at 2*k+1). Valid range 0..(MAX_HOLDING_REG-1). Reg 0 -> Common_Setpoint.
 * Response: echo of request (addr, function, start addr, reg count) + CRC.
 *--------------------------------------------------------------------------*/
/* Returns 0 on success, -1 on validation failure (caller should send exception). */
static int WriteMultiReg(int data_len)
{
    unsigned short int StartAddr, RegCount;
    unsigned char byteCount;
    unsigned int i;
    uint16_t value;

    if (data_len < 5) {
        return -1;
    }
    StartAddr = ((unsigned)ModData[0]<<8) + ModData[1];
    RegCount = ((unsigned)ModData[2]<<8) + ModData[3];
    byteCount = ModData[4];
    if ((StartAddr + RegCount) > MAX_HOLDING_REG) {
        return -1;
    }
    if (byteCount != RegCount * 2) {
        return -1;
    }
    if ((unsigned)(5 + byteCount) > (unsigned)data_len) {
        return -1;
    }
    for (i = 0; i < RegCount; i++) {
        value = (uint16_t)((ModData[5 + 2*i] << 8) | ModData[6 + 2*i]);
        modarray_wr[2 * (StartAddr + i)]     = (char)(value >> 8);
        modarray_wr[2 * (StartAddr + i) + 1] = (char)(value & 0xFF);
        if ((StartAddr + i) == 0) {
            Common_Setpoint = (int)value;
        }
    }

    /* Build response: echo request header (no data in FC 0x10 response) */
    ModBusMessage[0] = ModAddr;
    ModBusMessage[1] = ModFunction;
    ModBusMessage[2] = ModData[0];
    ModBusMessage[3] = ModData[1];
    ModBusMessage[4] = ModData[2];
    ModBusMessage[5] = ModData[3];
    GetCRC16(ModBusMessage, 6);
    ModBusMessage[6] = CRCHi;
    ModBusMessage[7] = CRCLo;
    ModTxLen = 8;
    ModTxCount = 1;
    DIRN_MAX487(1);
    UART_SendByte((LPC_UART_TypeDef *)LPC_UART3, ModBusMessage[0]);
    return 0;
}

/* Write single coil (FC 0x05): coil addr 0-39 (M000-M039). Value 0xFF00=ON, 0x0000=OFF. Returns 0 on success, -1 on error. */
static int WriteSingleCoil(int data_len)
{
    unsigned int CoilAddr;
    uint16_t value;

    if (data_len < 4) {
        return -1;
    }
    CoilAddr = ((unsigned)ModData[0] << 8) + ModData[1];
    value = (uint16_t)((ModData[2] << 8) | ModData[3]);
    if (CoilAddr > COIL_ADDR_MAX) {
        return -1;
    }
    if (value == 0xFF00) {
        ram_values[MARKER_RAM_OFFSET + CoilAddr] = 1;
    } else if (value == 0x0000) {
        ram_values[MARKER_RAM_OFFSET + CoilAddr] = 0;
    } else {
        return -1;   /* Illegal data value */
    }
    ModBusMessage[0] = ModAddr;
    ModBusMessage[1] = ModFunction;
    ModBusMessage[2] = ModData[0];
    ModBusMessage[3] = ModData[1];
    ModBusMessage[4] = ModData[2];
    ModBusMessage[5] = ModData[3];
    GetCRC16(ModBusMessage, 6);
    ModBusMessage[6] = CRCHi;
    ModBusMessage[7] = CRCLo;
    ModTxLen = 8;
    ModTxCount = 1;
    DIRN_MAX487(1);
    UART_SendByte((LPC_UART_TypeDef *)LPC_UART3, ModBusMessage[0]);
    return 0;
}

/* Write multiple coils (FC 0x0F): start addr 0-39, quantity 1-40. Coil bytes LSB first. Returns 0 on success, -1 on error. */
static int WriteMultiCoil(int data_len)
{
    unsigned int StartAddr, Qty, ByteCount, n, b;
    unsigned char byteVal;

    if (data_len < 5) {
        return -1;
    }
    StartAddr = ((unsigned)ModData[0] << 8) + ModData[1];
    Qty       = ((unsigned)ModData[2] << 8) + ModData[3];
    ByteCount = (unsigned)ModData[4];
    if (StartAddr > COIL_ADDR_MAX || Qty == 0 || Qty > MAX_COILS || (StartAddr + Qty) > MAX_COILS) {
        return -1;
    }
    if (ByteCount != (unsigned)((Qty + 7) / 8)) {
        return -1;
    }
    if ((5 + ByteCount) > (unsigned)data_len) {
        return -1;
    }
    for (n = 0; n < Qty; n++) {
        b = n % 8;
        byteVal = ModData[5 + (n / 8)];
        ram_values[MARKER_RAM_OFFSET + StartAddr + n] = ((byteVal >> b) & 1) ? 1 : 0;
    }
    ModBusMessage[0] = ModAddr;
    ModBusMessage[1] = ModFunction;
    ModBusMessage[2] = ModData[0];
    ModBusMessage[3] = ModData[1];
    ModBusMessage[4] = ModData[2];
    ModBusMessage[5] = ModData[3];
    GetCRC16(ModBusMessage, 6);
    ModBusMessage[6] = CRCHi;
    ModBusMessage[7] = CRCLo;
    ModTxLen = 8;
    ModTxCount = 1;
    DIRN_MAX487(1);
    UART_SendByte((LPC_UART_TypeDef *)LPC_UART3, ModBusMessage[0]);
    return 0;
}

/* Send Modbus exception response: [Addr][FC|0x80][ExceptionCode][CRC]. */
static void Modbus_SendException(unsigned char ex_code)
{
    ModBusMessage[0] = ModAddr;
    ModBusMessage[1] = (unsigned char)(ModFunction | 0x80);
    ModBusMessage[2] = ex_code;
    GetCRC16(ModBusMessage, 3);
    ModBusMessage[3] = CRCHi;
    ModBusMessage[4] = CRCLo;
    ModTxLen = 5;
    ModTxCount = 1;
    DIRN_MAX487(1);
    UART_SendByte((LPC_UART_TypeDef *)LPC_UART3, ModBusMessage[0]);
}

/*---------------------------------------------------------------------------
 * Compose RTU response for this slave. Only responds if ModAddr matches
 * Comm_Address, or broadcast 0/255. Dispatches by function code.
 * State machine: address match -> ModDataLen check -> switch(ModFunction)
 * -> build response or exception -> send first byte (rest via UART ISR).
 *---------------------------------------------------------------------------*/
void ComposeMessageRtu(void)
{
    unsigned int StartAddr, RegCount;
    unsigned char SIOModAddr = 0;

    SIOModAddr = Comm_Address;

    if ((ModAddr == SIOModAddr) || (ModAddr == 0) || (ModAddr == 255)) {
        if (ModDataLen != 0) {
            int mod_data_len = ModDataLen;
            ModDataLen = 0;

            switch (ModFunction) {
            case 1:  /* Read Coils: M000–M039 (read/write bits); addr 0 = M000 */
                if (mod_data_len < 4) { Modbus_SendException(0x03); break; }
                StartAddr = ((unsigned)ModData[0] << 8) + ModData[1];
                RegCount  = ((unsigned)ModData[2] << 8) + ModData[3];
                if (RegCount == 0 || StartAddr > COIL_ADDR_MAX || (StartAddr + RegCount) > MAX_COILS) {
                    Modbus_SendException(0x02);
                    break;
                }
                DataByteCount = (unsigned char)((RegCount + 7) / 8);
                if ((DataByteCount + 5) > MODBUS_TX_BUF_SIZE) {
                    Modbus_SendException(0x03);
                    break;
                }
                ModBusMessage[0] = Comm_Address;
                ModBusMessage[1] = ModFunction;
                ModBusMessage[2] = DataByteCount;
                for (ij = 0; ij < DataByteCount; ij++) {
                    unsigned int b;
                    unsigned char byteVal = 0;
                    for (b = 0; b < 8; b++) {
                        unsigned int n = StartAddr + 8 * ij + b;
                        if (n >= StartAddr + RegCount) {
                            break;
                        }
                        if (ram_values[MARKER_RAM_OFFSET + n] != 0) {
                            byteVal |= (unsigned char)(1 << b);
                        }
                    }
                    ModBusMessage[3 + ij] = byteVal;
                }
                GetCRC16(ModBusMessage, DataByteCount + 3);
                ModBusMessage[DataByteCount + 3] = CRCHi;
                ModBusMessage[DataByteCount + 4] = CRCLo;
                ModTxLen = DataByteCount + 5;
                ModTxCount = 1;
                DIRN_MAX487(1);
                UART_SendByte((LPC_UART_TypeDef *)LPC_UART3, ModBusMessage[0]);
                break;

            case 2:  /* Read Discrete Inputs (FC02): M040..M099, I000..I039, O000..O039 */
                if (mod_data_len < 4) { Modbus_SendException(0x03); break; }
                StartAddr = ((unsigned)ModData[0] << 8) + ModData[1];
                RegCount  = ((unsigned)ModData[2] << 8) + ModData[3];
                if (RegCount == 0) {
                    Modbus_SendException(0x02);
                    break;
                }
                {
                    unsigned int k;
                    for (k = 0; k < RegCount; k++) {
                        unsigned int a = StartAddr + k;
                        if (!((a >= DI_MARKER_ADDR_MIN && a <= DI_MARKER_ADDR_MAX) ||
                              (a >= DI_INPUT_ADDR_MIN && a <= DI_INPUT_ADDR_MAX) ||
                              (a >= DI_OUTPUT_ADDR_MIN && a <= DI_OUTPUT_ADDR_MAX))) {
                            Modbus_SendException(0x02);
                            break;
                        }
                    }
                    if (k < RegCount) {
                        break;
                    }
                }
                DataByteCount = (unsigned char)((RegCount + 7) / 8);
                if ((DataByteCount + 5) > MODBUS_TX_BUF_SIZE) {
                    Modbus_SendException(0x03);
                    break;
                }
                ModBusMessage[0] = Comm_Address;
                ModBusMessage[1] = ModFunction;
                ModBusMessage[2] = DataByteCount;
                for (ij = 0; ij < DataByteCount; ij++) {
                    unsigned int b;
                    unsigned char byteVal = 0;
                    for (b = 0; b < 8; b++) {
                        unsigned int n = StartAddr + 8 * ij + b;
                        if (n >= StartAddr + RegCount) {
                            break;
                        }
                        if ((n >= DI_MARKER_ADDR_MIN) && (n <= DI_MARKER_ADDR_MAX)) {
                            if (ram_values[MARKER_RAM_OFFSET + n] != 0) {
                                byteVal |= (unsigned char)(1 << b);
                            }
                        } else if ((n >= DI_INPUT_ADDR_MIN) && (n <= DI_INPUT_ADDR_MAX)) {
                            if (ram_values[INPUT_RAM_OFFSET + (n - DI_INPUT_ADDR_MIN)] != 0) {
                                byteVal |= (unsigned char)(1 << b);
                            }
                        } else if ((n >= DI_OUTPUT_ADDR_MIN) && (n <= DI_OUTPUT_ADDR_MAX)) {
                            if (ram_values[OUTPUT_RAM_OFFSET + (n - DI_OUTPUT_ADDR_MIN)] != 0) {
                                byteVal |= (unsigned char)(1 << b);
                            }
                        } else {
                            /* Should never occur due to range validation above. */
                            Modbus_SendException(0x02);
                            break;
                        }
                    }
                    if (b < 8 && (StartAddr + 8 * ij + b) < (StartAddr + RegCount)) {
                        /* Invalid address encountered mid-byte, response already sent. */
                        break;
                    }
                    if (b == 8 || (StartAddr + 8 * ij + b) >= (StartAddr + RegCount)) {
                        ModBusMessage[3 + ij] = byteVal;
                    } else {
                        break;
                    }
                }
                if (ij < DataByteCount && (StartAddr + 8 * ij) < (StartAddr + RegCount)) {
                    break;
                }
                GetCRC16(ModBusMessage, DataByteCount + 3);
                ModBusMessage[DataByteCount + 3] = CRCHi;
                ModBusMessage[DataByteCount + 4] = CRCLo;
                ModTxLen = DataByteCount + 5;
                ModTxCount = 1;
                DIRN_MAX487(1);
                UART_SendByte((LPC_UART_TypeDef *)LPC_UART3, ModBusMessage[0]);
                break;

            case 3:  /* Read Holding Registers: data from writable array modarray_wr only */
                if (mod_data_len < 4) { Modbus_SendException(0x03); break; }
                StartAddr = ((unsigned)ModData[0] << 8) + ModData[1];
                RegCount  = ((unsigned)ModData[2] << 8) + ModData[3];
                DataByteCount = RegCount * 2;
                if (DataByteCount > (MODBUS_TX_BUF_SIZE - 5)) {
                    Modbus_SendException(0x03);  /* Illegal Data Value: buffer limit */
                    break;
                }
                /* Reject if start + count exceeds 25 (valid indices 0..24). e.g. start=21 count=5 → 26 > 25 */
                if ((StartAddr + RegCount) > MAX_HOLDING_REG) { Modbus_SendException(0x02); break; }
                ModBusMessage[0] = Comm_Address;
                ModBusMessage[1] = ModFunction;
                ModBusMessage[2] = (unsigned char)DataByteCount;
                for (ij = 0; ij < RegCount; ij++) {
                    ModBusMessage[3 + 2 * ij]     = (unsigned char)modarray_wr[2 * (StartAddr + ij)];     /* high byte */
                    ModBusMessage[3 + 2 * ij + 1] = (unsigned char)modarray_wr[2 * (StartAddr + ij) + 1]; /* low byte */
                }
                GetCRC16(ModBusMessage, DataByteCount + 3);
                ModBusMessage[DataByteCount + 3] = CRCHi;
                ModBusMessage[DataByteCount + 4] = CRCLo;
                ModTxLen = DataByteCount + 5;
                ModTxCount = 1;
                DIRN_MAX487(1);
                UART_SendByte((LPC_UART_TypeDef *)LPC_UART3, ModBusMessage[0]);
                break;

            case 4:  /* Read Input Registers: data from read-only array modarray_rd only */
                if (mod_data_len < 4) { Modbus_SendException(0x03); break; }
                StartAddr = ((unsigned)ModData[0] << 8) + ModData[1];
                RegCount  = ((unsigned)ModData[2] << 8) + ModData[3];
                DataByteCount = RegCount * 2;
                if (DataByteCount > (MODBUS_TX_BUF_SIZE - 5)) {
                    Modbus_SendException(0x03);  /* Illegal Data Value: buffer limit */
                    break;
                }
                if ((StartAddr * 2 + DataByteCount) > MAX_LENGTH) { Modbus_SendException(0x02); break; }
                ModBusMessage[0] = Comm_Address;
                ModBusMessage[1] = ModFunction;
                ModBusMessage[2] = (unsigned char)DataByteCount;
                for (ij = 0; ij < RegCount; ij++) {
                    ModBusMessage[3 + 2 * ij]     = (unsigned char)modarray_rd[2 * (StartAddr + ij)];     /* high byte */
                    ModBusMessage[3 + 2 * ij + 1] = (unsigned char)modarray_rd[2 * (StartAddr + ij) + 1]; /* low byte */
                }
                GetCRC16(ModBusMessage, DataByteCount + 3);
                ModBusMessage[DataByteCount + 3] = CRCHi;
                ModBusMessage[DataByteCount + 4] = CRCLo;
                ModTxLen = DataByteCount + 5;
                ModTxCount = 1;
                DIRN_MAX487(1);
                UART_SendByte((LPC_UART_TypeDef *)LPC_UART3, ModBusMessage[0]);
                break;

            case 0x05:  /* Write Single Coil: M000–M039 (addr 0–39); 0xFF00=ON, 0x0000=OFF */
                if (WriteSingleCoil(mod_data_len) != 0) {
                    Modbus_SendException(0x03);
                }
                break;

            case 0x06:  /* Write Single Register: one register in modarray_wr, reg 0 -> Common_Setpoint */
                if (WriteSingleReg(mod_data_len) != 0) {
                    Modbus_SendException(0x02);
                }
                break;

            case 0x0F:  /* Write Multiple Coils: M000–M039 (start 0–39, qty 1–40) */
                if (WriteMultiCoil(mod_data_len) != 0) {
                    Modbus_SendException(0x03);
                }
                break;

            case 0x10:  /* Write Multiple Registers: modarray_wr, reg 0 -> Common_Setpoint */
                if (WriteMultiReg(mod_data_len) != 0) {
                    Modbus_SendException(0x02);
                }
                break;

            case 8:  /* Diagnostic (echo subfunction + data) */
                if (ModAddr != 0) {
                    ModBusMessage[0] = ModAddr;
                    ModBusMessage[1] = ModFunction;
                    ModBusMessage[2] = 0x00;
                    ModBusMessage[3] = 0x00;
                    ModBusMessage[4] = ModData[2];
                    ModBusMessage[5] = ModData[3];
                    GetCRC16(ModBusMessage, 6);
                    ModBusMessage[6] = CRCHi;
                    ModBusMessage[7] = CRCLo;
                    ModTxLen = 8;
                    ModTxCount = 1;
                    DIRN_MAX487(1);
                    UART_SendByte((LPC_UART_TypeDef *)LPC_UART3, ModBusMessage[0]);
                }
                break;

            default:  /* Unsupported function: exception 01 Illegal Function */
                Modbus_SendException(0x01);
                break;
            }
        }
    } else {
        ModDataLen = 0;
        ModBusIdx = -1;
        DIRN_MAX487(0);  /* MAX487 IN RX MODE */
    }
}

void modbus(void)
{
    if (SIO_Interface != 1) {
        return;
    }

    if (rx_msg == 1) {
        Mod_Monitoring_Flag = 1;
        rx_msg = 0;

        if (ModBusIdx < 0 || ModBusIdx >= MODBUS_TX_BUF_SIZE) {
            ModBusIdx = -1;
            DIRN_MAX487(0);
            return;
        }

        GetCRC16(ModBusMessage, ModBusIdx - 1);

        if (CRCHi == ModBusMessage[ModBusIdx - 1] && CRCLo == ModBusMessage[ModBusIdx]) {
            ModAddr = ModBusMessage[0];
            ModFunction = ModBusMessage[1];
            if ((ModBusIdx - 3) > MODBUS_PDU_DATA_MAX) {
                ModBusIdx = -1;
                DIRN_MAX487(0);
                return;
            }
            for (ij = 2; ij <= ModBusIdx - 2; ij++) {
                if ((ij - 2) >= MODBUS_PDU_DATA_MAX) {
                    break;
                }
                ModData[ij - 2] = ModBusMessage[ij];
            }
            ModDataLen = ModBusIdx - 3;
            ComposeMessageRtu();
            ModBusIdx = -1;
        } else {
            /* CRC error: send exception response */
            ModBusMessage[1] = 0x80 | ModBusMessage[1];
            ModBusMessage[2] = 0x17;  /* exception code 17 (CRC-16 error) */
            GetCRC16(ModBusMessage, 3);
            ModBusMessage[3] = CRCHi;
            ModBusMessage[4] = CRCLo;
            ModBusIdx = -1;
            ModTxLen = 5;
            ModTxCount = 1;
            DIRN_MAX487(1);
            UART_SendByte((LPC_UART_TypeDef *)LPC_UART3, ModBusMessage[0]);
        }
    }
}

/*
 * 23/08/2018: Added if condition for SIO Interface selection in serial_init().
 * Depending on SIO_Interface, Baudrate & Timer get selected.
 */
