/*Modbus.c*/
# include "stdio.h"
# include "string.h"
#    include    "modbus.h"
#include "modbus_dio.h"

#include "lpc17xx_pinsel.h"

#    include "LPC17xx.h"
# include "GPIO_LPC17xx.h"
# include "lpc17xx_uart.h"
# include "parameters.h"
# include "keys.h"

extern unsigned char Dev_ID;

extern unsigned char Mod_Data[];

extern char Check_Start ;
extern char Status_No,Error_No;
extern char Start_Stop_status;
extern uint16_t New_menu_parameters[105];
extern signed int Belt_Load;
extern unsigned int uiTest,ij;
char DIO_ModBusIdx = -1,DIO_Err_Gen;
unsigned char DIO_CRCHi,DIO_CRCLo;
unsigned char DIO_ModAddr,DIO_ModFunction,DIO_DataByteCount;
volatile char DIO_modarray_rd[2],DIO_modarray_wr[2];
unsigned char DIO_ModBusMessage[34],DIO_ModData[34];
unsigned char tx_over,Rx_byte_count, DIO_status;
unsigned char read_ok,write_ok;

uint8_t  DIO_Modbus_Monitor_Time,DIO_Mod_Monitoring_Flag ;
char DIO_ram_value[8];
uint16_t Rx_Err_Counter;
int DIO_ModDataLen;
int DIO_ModTxLen, DIO_ModTxCount;

unsigned char DIO_CRC16Hi[] = {
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
   unsigned char DIO_CRC16Lo[] = {
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
                                        } ;

//void Modbus_Monitoring_Time()
//{
//    if((Check_Start != 10) && (SIO_Monitor > 0))  //Do not enter if Offline menu Routine and if monitor time is set to '0'
//        {
//            if((Error_No !=2)&&(SIO_Interface == 1))
//                Modbus_Monitor_Time--;
//
//            if(Mod_Monitoring_Flag == 1) //Reception started
//                {
//                    Modbus_Monitor_Time = SIO_Monitor;
//                    Mod_Monitoring_Flag = 0;
//                    Error_No = 0;
//                }
//            if(Modbus_Monitor_Time == 0)
//                {
//
//                    Modbus_Monitor_Time = 0;
//                    Error_No = 2;
//                    Start_Stop_status = STOP ;
//                }
//        }

//}
//
void DIO_DIRN_MAX487(char on)
{
    int i;
  GPIO_PinWrite (1,0, on);
    for (i=0;i<20;i++){;}            //msk 06-dec-2025
}

 void DIO_serial_init()
 {
   UART_DIO_MODBUS_CONFIG();
     GPIO_SetDir(1,0,GPIO_DIR_OUTPUT);   /************************ Modbus Timeout************************/

 /************************ Init Timer3 ************************/
    // Initilize Timer 3 in Periodic mode with timeout period of (3.5char)-1.5msec //50msec

        //Timer3_init();
 /************************ Init Timer2 ************************/
 /************************ Modbus Timeout************************/
}

/*************************************************************************
                    CRC calculation
CRC16 calculation based on CRC table
*************************************************************************/

unsigned int DIO_GetCRC16(unsigned char *puchMsg, int DataLen)
{
unsigned int Index ;     // will index into CRC16 lookup table

DIO_CRCHi = 0xFF ;             // high byte of CRC16 initialized
DIO_CRCLo = 0xFF ;             // low byte of CRC16 initialized

while (DataLen--)
    {
    Index = DIO_CRCHi ^ *puchMsg++ ; // calculate the CRC16
    DIO_CRCHi = DIO_CRCLo ^ DIO_CRC16Hi[Index] ;
    DIO_CRCLo = DIO_CRC16Lo[Index] ;
    }
return ((unsigned)DIO_CRCHi << 8 | DIO_CRCLo);
}

/*
void dio_init()
{
    tx_over=0;
    DIO_ModBusMessage[0]= 0x01;           //address
    DIO_ModBusMessage[1]= 16;           // Function code
    DIO_ModBusMessage[2]= 0x00;//add_hi;         //0x00;  //reg address hi
    DIO_ModBusMessage[3]= 0x45;//add_lo;         //0x40;   //reg address low
    DIO_ModBusMessage[4]= 0x00;//Qty_reg_hi; //0x00;   // reg val  hi
    DIO_ModBusMessage[5]= 0x01;//Qty_reg_lo; // DIO_modarray_wr[0];//0x03;  //reg val  lo

    DIO_ModBusMessage[6]= 0x02;//byte_count*2;
    DIO_ModBusMessage[7]= 0x00;//reg_val_hi;
    DIO_ModBusMessage[8]= 0x01;//reg_val_lo;

    DIO_GetCRC16(DIO_ModBusMessage,6);
    DIO_ModBusMessage[6]=  DIO_CRCHi;  // 0x89;  //CRC_16 hi
    DIO_ModBusMessage[7]= DIO_CRCLo;  //0xDD;  //CRC_16 lo
    DIO_ModTxCount = 1;
    DIO_ModTxLen = 8;
    DIO_DIRN_MAX487(1);
    UART_SendByte((LPC_UART_TypeDef *)LPC_UART3,DIO_ModBusMessage[0]);
    while( tx_over == 0 );

}
*/

/*************************************************************************
                    Compose Modbus message
*************************************************************************/
/*
void DIO_ComposeMessageRtu(unsigned char Modbus_function_DIO,unsigned char add_hi,unsigned char add_lo,unsigned char reg_val_hi,unsigned char reg_val_lo)
{
unsigned int  StartAddr;
unsigned char SIOModAddr=0;
    DIO_ModFunction = Modbus_function_DIO;
        switch(DIO_ModFunction)
            {
                        case 3:
                                        DIO_ModBusMessage[0]= 0x01;       //address
                                        DIO_ModBusMessage[1]= 0x03;       // Function code
                                        DIO_ModBusMessage[2]= add_hi;     //0x00;  //reg address hi
                                        DIO_ModBusMessage[3]= add_lo;     //0x00;   //reg address low
                                        DIO_ModBusMessage[4]= reg_val_hi; //0x00;   // no. of  reg hi
                                        DIO_ModBusMessage[5]= reg_val_lo; //0x01;   // no. of reg  lo
                                        DIO_GetCRC16(DIO_ModBusMessage,6);
                                        DIO_ModBusMessage[6]= DIO_CRCHi;  // 0x89;  //CRC_16 hi
                                        DIO_ModBusMessage[7]= DIO_CRCLo;  //0xDD;  //CRC_16 lo
                                        DIO_ModTxCount = 1;
                                        DIO_ModTxLen = 8 ;
                                        DIO_DIRN_MAX487(1);
                                        UART_SendByte((LPC_UART_TypeDef *)LPC_UART0,DIO_ModBusMessage[0]);

                        break;

                    case 0x06:
                                        DIO_ModBusMessage[0]= 0x01;           //address
                                        DIO_ModBusMessage[1]= 0x06;           // Function code
                                        DIO_ModBusMessage[2]= add_hi;         //0x00;  //reg address hi
                                        DIO_ModBusMessage[3]= add_lo;         //0x40;   //reg address low
                                        DIO_ModBusMessage[4]= reg_val_hi; //0x00;   // reg val  hi
                                        DIO_ModBusMessage[5]= reg_val_lo; // DIO_modarray_wr[0];//0x03;  //reg val  lo
                                        DIO_GetCRC16(DIO_ModBusMessage,6);
                                        DIO_ModBusMessage[6]= DIO_CRCHi;  // 0x89;  //CRC_16 hi
                                        DIO_ModBusMessage[7]= DIO_CRCLo;  //0xDD;  //CRC_16 lo
                                        DIO_ModTxCount = 1;
                                        DIO_ModTxLen = 8;
                                        DIO_DIRN_MAX487(1);
                                        UART_SendByte((LPC_UART_TypeDef *)LPC_UART0,DIO_ModBusMessage[0]);
                    break;
                    case 0x10:
                                        DIO_ModBusMessage[0]= 0x02;           //address
                                        DIO_ModBusMessage[1]= 0x10;           // Function code
                                        DIO_ModBusMessage[2]= 0x21;//21 add_hi;         //0x00;  //reg address hi
                                        DIO_ModBusMessage[3]= 0x34;//35add_lo;         //0x40;   //reg address low
                                        DIO_ModBusMessage[4]= 0x00;//Qty_reg_hi; //0x00;   // reg val  hi
                                        DIO_ModBusMessage[5]= 0x03;//Qty_reg_lo; // DIO_modarray_wr[0];//0x03;  //reg val  lo
                                        DIO_ModBusMessage[6]= 0x06;//byte_count*2;

                                        DIO_ModBusMessage[7]= 0x00;//reg_val_hi;
                                        DIO_ModBusMessage[8]= 0x08;//reg_val_lo;
                                        DIO_ModBusMessage[9]= 0x00;
                                        DIO_ModBusMessage[10]= 0x09;
                                        DIO_ModBusMessage[11]= 0x00;
                                        DIO_ModBusMessage[12]= 0x10;

                                        DIO_GetCRC16(DIO_ModBusMessage,13);
                                        DIO_ModBusMessage[13]= DIO_CRCHi;  // 0x89;  //CRC_16 hi
                                        DIO_ModBusMessage[14]= DIO_CRCLo;  //0xDD;  //CRC_16 lo
                                        DIO_ModTxCount = 1;
                                        DIO_ModTxLen = 15;
                                        DIO_DIRN_MAX487(1);
                                        UART_SendByte((LPC_UART_TypeDef *)LPC_UART0,DIO_ModBusMessage[0]);
                                        //Rx_byte_count = 7;
                    break;
                    default:
                        break;

            }

}

*/

void DIO_ComposeMessageRtu(    unsigned char Slave_Addr,
                                                        unsigned char Futn_Code,
                                                        unsigned char Reg_Add_hi,
                                                        unsigned char Reg_Add_lo,
                                                        unsigned char Reg_val_hi,
                                                        unsigned char Reg_val_lo,
                                                        unsigned char Byte_Count
                                                    )
    {    unsigned char i;
        switch(Futn_Code)
            {
                        case 3:
                                        DIO_ModBusMessage[0]= Slave_Addr;       //address
                                        DIO_ModBusMessage[1]= Futn_Code;       // Function code
                                        DIO_ModBusMessage[2]= Reg_Add_hi;     //0x00;  //reg address hi
                                        DIO_ModBusMessage[3]= Reg_Add_lo;     //0x00;   //reg address low
                                        DIO_ModBusMessage[4]= Reg_val_hi; //0x00;   // no. of  reg hi
                                        DIO_ModBusMessage[5]= Reg_val_lo; //0x01;   // no. of reg  lo
                                        DIO_GetCRC16(DIO_ModBusMessage,6);
                                        DIO_ModBusMessage[6]= DIO_CRCHi;  // 0x89;  //CRC_16 hi
                                        DIO_ModBusMessage[7]= DIO_CRCLo;  //0xDD;  //CRC_16 lo
                                        DIO_ModTxCount = 1;
                                        tx_over = 0;//msk 19-jun-2026
                                        DIO_ModBusIdx = -1; //msk 19-jun-2026
                                        DIO_ModTxLen = 8 ;
                                        DIO_DIRN_MAX487(1);
                                        UART_SendByte((LPC_UART_TypeDef *)LPC_UART0,DIO_ModBusMessage[0]);

                        break;

                    case 0x06:
                                        DIO_ModBusMessage[0]= Slave_Addr;           //address
                                        DIO_ModBusMessage[1]= Futn_Code;           // Function code
                                        DIO_ModBusMessage[2]=    Reg_Add_hi;         //0x00;  //reg address hi
                                        DIO_ModBusMessage[3]= Reg_Add_lo;         //0x40;   //reg address low
                                        DIO_ModBusMessage[4]= Reg_val_hi; //0x00;   // reg val  hi
                                        DIO_ModBusMessage[5]= Reg_val_lo; // DIO_modarray_wr[0];//0x03;  //reg val  lo
                                        DIO_GetCRC16(DIO_ModBusMessage,6);
                                        DIO_ModBusMessage[6]= DIO_CRCHi;  // 0x89;  //CRC_16 hi
                                        DIO_ModBusMessage[7]= DIO_CRCLo;  //0xDD;  //CRC_16 lo
                                        DIO_ModTxCount = 1;
                                        tx_over = 0;//msk 19-jun-2026
                                        DIO_ModBusIdx = -1; //msk 19-jun-2026
                                        DIO_ModTxLen = 8;
                                        DIO_DIRN_MAX487(1);
                                        UART_SendByte((LPC_UART_TypeDef *)LPC_UART0,DIO_ModBusMessage[0]);
                    break;
                    case 0x10:
                                        DIO_ModBusMessage[0]= Slave_Addr;           //address
                                        DIO_ModBusMessage[1]= Futn_Code;           // Function code
                                        DIO_ModBusMessage[2]= Reg_Add_hi;//21 add_hi;         //0x00;  //reg address hi
                                        DIO_ModBusMessage[3]= Reg_Add_lo;//35add_lo;         //0x40;   //reg address low
                                        DIO_ModBusMessage[4]= Reg_val_hi;//Qty_reg_hi; //0x00;   // reg val  hi
                                        DIO_ModBusMessage[5]= Reg_val_lo;//Qty_reg_lo; // DIO_modarray_wr[0];//0x03;  //reg val  lo

                                        // Bounds check: DIO_ModBusMessage is 34 bytes, need room for header(7)+data+crc(2)
                                        if ((Byte_Count*2) > 25) Byte_Count = 12;  // Max 24 data bytes (12*2)

                                        DIO_ModBusMessage[6]= Byte_Count*2;//byte_count*2;

                                        for    (i=7;i<((Byte_Count*2)+7);i++)
                                            {
                                                DIO_ModBusMessage[i]=Mod_Data[i-7];
                                            }
                                        DIO_GetCRC16(DIO_ModBusMessage,(DIO_ModBusMessage[6]+7));
                                        DIO_ModBusMessage[i]= DIO_CRCHi;  // 0x89;  //CRC_16 hi
                                        DIO_ModBusMessage[i+1]= DIO_CRCLo;  //0xDD;  //CRC_16 lo
                                        DIO_ModTxCount = 1;
                                        DIO_ModTxLen = i+2;
                                        tx_over = 0;//msk 19-jun-2026
                                        DIO_ModBusIdx = -1; //msk 19-jun-2026
                                        DIO_DIRN_MAX487(1);
                                        UART_SendByte((LPC_UART_TypeDef *)LPC_UART0,DIO_ModBusMessage[0]);

                    break;
                    default:
                        break;

            }

    }

void DIO_modbus(void)
{
    /*
if((DIO_Module == 1)||(DIO_Module == 2))
    {
        if(Dev_ID == 1)
            {
                switch(DIO_status)
                    {
                    case 1 :  //send comman
                                    if((read_ok == 0)&&(write_ok == 1))
                                        {
                                            DIO_ComposeMessageRtu(0x01,0x03,0x00,0x00,0x00,0x01,0x00);  //read command
                                            Rx_byte_count = 6;
                                        }
                                    if((read_ok == 1)&&(write_ok == 0))
                                        {
                                            DIO_ComposeMessageRtu(0x01,0x06,0x00,0x40,0x00,0x06,0x00);  //write command
                                            Rx_byte_count = 7;
                                        }
                                    DIO_status = 2;
                    break ;

                    case 2 : //check for tx over or not
                                    if( tx_over == 1 )
                                     {
                                            DIO_status = 3;
                                            tx_over = 0;
                                     }
                        break;

                    case 3: //Rx msg check
                                    if( DIO_ModBusIdx == Rx_byte_count )
                                        {
                                            DIO_GetCRC16(DIO_ModBusMessage,DIO_ModBusIdx-1);  //calculate CRC for received message
                                            if( ( DIO_ModBusMessage[DIO_ModBusIdx-1] == DIO_CRCHi ) && ( DIO_ModBusMessage[DIO_ModBusIdx] == DIO_CRCLo ) )
                                                {
                                                    if(DIO_ModBusMessage[1]==0x03)
                                                        {
                                                            DIO_modarray_rd[0]= DIO_ModBusMessage[4];
                                                            read_ok = 1;
                                                            write_ok = 0;
                                                        }
                                                    if(DIO_ModBusMessage[1]==0x06)
                                                        {
                                                            DIO_modarray_wr[0]= DIO_ModBusMessage[5];
                                                            read_ok =  0;
                                                            write_ok = 1;

                                                            if((Inv==1)||(Inv==2)||(Inv==3))
                                                                Dev_ID = 2;

                                                        }

                                                    DIO_status = 1;
                                                    Rx_Err_Counter = 0;
                                                    DIO_Err_Gen = 0;
                                                    DIO_DIRN_MAX487(1);
                                                    DIO_ModBusIdx = -1;
                                                    if(Error_No == 1)
                                                    Error_No = 0;
                                                }
                                            else
                                                {
                                                    Rx_Err_Counter++;
                                                    if(    ( Rx_Err_Counter % 10 ) == 1 )
                                                        {
                                                            DIO_Err_Gen = DIO_Err_Gen+1;
                                                            DIO_status = 1;
                                                            DIO_DIRN_MAX487(1);
                                                            DIO_ModBusIdx = -1;

                                                            if((Inv==1)||(Inv==2)||(Inv==3))
                                                                Dev_ID = 2;

                                                            if(DIO_Err_Gen > 50)
                                                                {
                                                                    Rx_Err_Counter=0;
                                                                    DIO_Err_Gen=0;
                                                                    Error_No = 1;
                                                                }

                                                        }
                                                }
                                        }
                                    else
                                        {
                                            Rx_Err_Counter++;
                                            if(    ( Rx_Err_Counter % 10 ) == 1 )
                                                {
                                                    DIO_Err_Gen = DIO_Err_Gen+1;
                                                    DIO_status = 1;
                                                    DIO_DIRN_MAX487(1);
                                                    DIO_ModBusIdx = -1;

                                                    if((Inv==1)||(Inv==2)||(Inv==3))
                                                        Dev_ID = 2;

                                                    if(DIO_Err_Gen > 50)
                                                        {
                                                            Rx_Err_Counter=0;
                                                            DIO_Err_Gen=0;
                                                            Error_No = 1;
                                                        }

                                                }
                                        }
                        break;
                    default:
                        break;

                    }
                }
        }
    else if(((Inv==1)||(Inv==2)||(Inv==3))&&(Dev_ID ==1))
        {
            Dev_ID = 2;
        }

        */
     if(((Inv==1)||(Inv==2)||(Inv==3)||(Inv==4)||(Inv==5)||(Inv==6)||(Inv==7)||(Inv==8))&&(Dev_ID ==1))
        {
            Dev_ID = 2;
        }
}

/*
30/12/2018
     if(((Inv==1)||(Inv==2)||(Inv==3)||(Inv==4))&&(Dev_ID ==1))
Inv==4 added for Yaskawa - J1000;

*/
