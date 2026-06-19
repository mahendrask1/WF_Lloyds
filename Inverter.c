/* Inverter.c */
#include "stdio.h"
#include "string.h"
#include "modbus.h"
#include "modbus_dio.h"
#include "lpc17xx_pinsel.h"
#include "LPC17xx.h"
#include "GPIO_LPC17xx.h"
#include "lpc17xx_uart.h"
#include "parameters.h"
#include "keys.h"
#include "plc.h"
#include "Errors.h" /* msk29-may-26: ErrorLatch_ClearInvComm */

uint16_t iocomerrcnts = 0;   /* io module comm error counts */
uint16_t iocomcounts = 0;    /* counts increase in Timer0 IRQ every 5 ms */
uint16_t response_time;      /* Timer0 ticks; 1 count = 5 ms */
unsigned char AO_Comm_Fault = 0;

extern unsigned char Rx_byte_count,Inv_Op_current,Inv1_Op_current;
extern unsigned char DIO_ModBusMessage[],DIO_ModData[];
extern unsigned char DIO_CRCHi,DIO_CRCLo;
extern char ram_values[RAM_SIZE];
extern unsigned int Inv_Op_Freq,Inv1_Op_Freq;
extern unsigned int Dac1_Val,Dac2_Val,Dac3_Val;
extern unsigned char tx_over;
extern char DIO_ModBusIdx;
extern uint16_t New_menu_parameters[105];
extern uint16_t AO_cal_Values[6];
extern char Error_No;
extern char Check_Start;
extern float WF_Drive_Setpoint, PF_Drive_Setpoint;
uint16_t Err_Counter_Inv,Err_Counter_Inv2;
int Inv_Sp,Inv_Sp2;
unsigned char Invter_Status,Err_Gen,Dev_ID,Mod_Data[15],INV_Error_Flag,Invter2_Status,Err_Gen2,INV2_Error_Flag;
unsigned char INV_read_ok,INV_write_ok,INV2_read_ok,INV2_write_ok,Start_Stop_Inv,Start_Stop_Inv2,fault_code;

volatile char INV_modarray_rd[10],INV_modarray_wr[2];
extern char Start_Stop_status;
unsigned char Read_count_AtvReg,Read_count_AtvReg_1;
extern int DIO_ModTxLen, DIO_ModTxCount;
void     DIO_DIRN_MAX487(char);
unsigned int DIO_GetCRC16(unsigned char *, int );
extern unsigned char DIO_ModBusMessage[34],DIO_ModData[34];
//extern uint16_t dacop1,dacop2;
extern unsigned long Min_Count_Channel1,Min_Count_Channel2,Min_Count_Channel3,Max_Count_Channel1,Max_Count_Channel2,Max_Count_Channel3,temp_AO_val;
extern unsigned long Dac1_SpanCounts,Dac2_SpanCounts,Dac3_SpanCounts;
extern char calib_string[20];
extern char *calib_string_ptr;// = calib_string;
bool comfail=false;
static unsigned char prev_start_cmd_m066 = 0;
static unsigned char prev_check_start = 0xFF;
static unsigned char inv_commpointer = 0; /* msk29-may-26: bus slot seq reset on comm recovery */

#define INV_RX_TIMEOUT_TICKS  10U  /* 10 x 5 ms Timer0 tick = ~50 ms no-progress timeout msk 19-jun-2026 */

static char inv_rx_last_idx = -2;
static uint16_t inv_rx_progress_time;
static char inv2_rx_last_idx = -2;
static uint16_t inv2_rx_progress_time;

void Print_String(char **string, char start_row, char start_col, char font_size, char underline_no);

static void InvComm_SetComError(unsigned char inv_num)
{
    if (inv_num == 15)
        Error_No = 15;
    else
        Error_No = 14;
}

/* msk29-may-26: clear inv comm counters/flags/latch (menu exit, M066, M018) */
void ResetInvCommState(void)
{
    INV_Error_Flag = 0;
    INV2_Error_Flag = 0;
    Err_Counter_Inv = 0;
    Err_Counter_Inv2 = 0;
    Err_Gen = 0;
    Err_Gen2 = 0;

    INV_read_ok = 0;
    INV_write_ok = 1;
    INV2_read_ok = 0;
    INV2_write_ok = 1;
    Invter_Status = 1;
    Invter2_Status = 1;
    DIO_ModBusIdx = -1;
    inv_commpointer = 0;

    M042 = 0;
    M044 = 0;

    if ((Error_No == 14) || (Error_No == 15))
        Error_No = 0;

    ErrorLatch_ClearInvComm();
}

/* msk29-may-26: called from menu ESC; edge detect fails while menu blocks main loop */
void ResetInvCommAfterOfflineMenu(void)
{
    ResetInvCommState();
    prev_check_start = (unsigned char)Check_Start;
}

/* msk29-may-26: SET offline menu entry; prime prev_check_start for backup edge detect */
void InvComm_NotifyOfflineMenuEnter(void)
{
    prev_check_start = 10;
}

static void ResetInvCommOnStartEdge(void)
{
    unsigned char start_edge = 0;
    unsigned char offline_exit_edge = 0;

    if ((prev_start_cmd_m066 == 0) && (M066 == 1))
        start_edge = 1;
    if ((prev_check_start == 10) && (Check_Start != 10))
        offline_exit_edge = 1;

    if ((start_edge == 1) || (offline_exit_edge == 1))
        ResetInvCommState(); /* msk29-may-26 */

    prev_start_cmd_m066 = M066;
    prev_check_start = (unsigned char)Check_Start;
}

#define AO_ModuleAddVeaveUI2AO 21
#define AO_ModuleAddWINIO2AOMC 15
#define font10     0
#define line1 0
#define line2 1
#define line3 3
#define line4 4
#define line5 5
#define line6 6

void SendQveaveUI2AO(bool calmode)
{
    int dac_op1,dac_op2,dac_op3;

    if(!calmode)        //if not in calmode
    {
        //Map DAC1 value from Min-Max range to 4000-20000 (4mA-20mA)
        dac_op1=Dac1_Val-Min_Count_Channel1;
        dac_op1=(dac_op1 * 16000);
        dac_op1=dac_op1/Dac1_SpanCounts;
        dac_op1+=4000;
        dac_op1=dac_op1&0xffff;
        if (dac_op1 > 21000) dac_op1 = 21000;
        dac_op2 = Dac2_Val - Min_Count_Channel2;
        dac_op2=(dac_op2 * 16000);
        dac_op2=dac_op2/Dac2_SpanCounts;
        dac_op2+=4000;
        dac_op2=dac_op2&0xffff;
        if(dac_op2>21000){dac_op2=21000;}

        //Map DAC3 value from Min-Max range to 4000-20000 (4mA-20mA) - Ready for future Modbus expansion
        dac_op3=Dac3_Val-Min_Count_Channel3;
        dac_op3=(dac_op3 * 16000);
        dac_op3=dac_op3/Dac3_SpanCounts;
        dac_op3+=4000;
        dac_op3=dac_op3&0xffff;
        if(dac_op3>21000){dac_op3=21000;}
    }
    else
    {
        dac_op1=(Dac1_Val * 20000)/3850;
        if(dac_op1>21000){dac_op1=21000;}
        dac_op2=(Dac2_Val * 20000)/3850;
        if(dac_op2>21000){dac_op2=21000;}
        dac_op3=(Dac3_Val * 20000)/3850;
        if (dac_op3 > 21000) dac_op3 = 21000;
    }
    DIO_ModBusMessage[0] = AO_ModuleAddVeaveUI2AO;
    DIO_ModBusMessage[1] = 0x10;
    DIO_ModBusMessage[2] = 0;
    DIO_ModBusMessage[3] = 0;
    DIO_ModBusMessage[4] = 0;
    DIO_ModBusMessage[5] = 2;
    DIO_ModBusMessage[6] = 4;
    DIO_ModBusMessage[7] = (dac_op1 & 0xff00) >> 8;
    DIO_ModBusMessage[8] = dac_op1 & 0x00ff;
    DIO_ModBusMessage[9] = (dac_op2 & 0xff00) >> 8;
    DIO_ModBusMessage[10] = (dac_op2 & 0x00ff);
    DIO_GetCRC16(DIO_ModBusMessage, 11);
    DIO_ModBusMessage[11] = DIO_CRCHi;
    DIO_ModBusMessage[12] = DIO_CRCLo;
    DIO_ModTxCount = 1;
    DIO_ModTxLen = 13;
    DIO_ModBusIdx = -1;
    DIO_DIRN_MAX487(1);
    UART_SendByte((LPC_UART_TypeDef *)LPC_UART0, DIO_ModBusMessage[0]);
}

void SendQWINIO2AOMC(bool calmode)
{
    int dac_op1,dac_op2,dac_op3;

    if(!calmode)        //if not in calmode
    {
        //Map DAC1 value from Min-Max range to 4000-20000 (4mA-20mA)
        dac_op1=Dac1_Val-Min_Count_Channel1;
        dac_op1=(dac_op1 * 4095);//20ma at 4095
        dac_op1=dac_op1/Dac1_SpanCounts;
        dac_op1+=0;  //4ma at 0
        dac_op1=dac_op1&0xffff;
        if(dac_op1>4095){dac_op1=4095;}

        //Map DAC2 value from Min-Max range to 4000-20000 (4mA-20mA)
        dac_op2=Dac2_Val-Min_Count_Channel2;
        dac_op2=(dac_op2 * 4095);
        dac_op2=dac_op2/Dac2_SpanCounts;
        dac_op2+=0;
        dac_op2=dac_op2&0xffff;
        if(dac_op2>4095){dac_op2=4095;}

        //Map DAC3 value from Min-Max range to 4000-20000 (4mA-20mA) - Ready for future Modbus expansion
        dac_op3=Dac3_Val-Min_Count_Channel3;
        dac_op3=(dac_op3 * 4095);
        dac_op3=dac_op3/Dac3_SpanCounts;
        dac_op3+=0;
        dac_op3=dac_op3&0xffff;
        if(dac_op3>4095){dac_op3=4095;}
    }
    else
    {
        dac_op1=(Dac1_Val * 4095)/3850;
        if(dac_op1>4095){dac_op1=4095;}
        dac_op2=(Dac2_Val * 4095)/3850;
        if(dac_op2>4095){dac_op2=4095;}
        dac_op3=(Dac3_Val * 4095)/3850;
        if(dac_op3>4095){dac_op3=4095;}
    }
    DIO_ModBusMessage[0] = AO_ModuleAddWINIO2AOMC;
    DIO_ModBusMessage[1] = 0x10;
    DIO_ModBusMessage[2] = 0;
    DIO_ModBusMessage[3] = 4;
    DIO_ModBusMessage[4] = 0;
    DIO_ModBusMessage[5] = 3;
    DIO_ModBusMessage[6] = 6;
    DIO_ModBusMessage[7] = (dac_op1 & 0xff00) >> 8;
    DIO_ModBusMessage[8] = dac_op1 & 0x00ff;
    DIO_ModBusMessage[9] = (dac_op2 & 0xff00) >> 8;
    DIO_ModBusMessage[10] = (dac_op2 & 0x00ff);
    DIO_ModBusMessage[11] = (dac_op3 & 0xff00) >> 8;
    DIO_ModBusMessage[12] = (dac_op3 & 0x00ff);
    DIO_GetCRC16(DIO_ModBusMessage, 13);
    DIO_ModBusMessage[13] = DIO_CRCHi;
    DIO_ModBusMessage[14] = DIO_CRCLo;
    DIO_ModTxCount = 1;
    DIO_ModTxLen = 15;
    DIO_ModBusIdx = -1;
    DIO_DIRN_MAX487(1);
    UART_SendByte((LPC_UART_TypeDef *)LPC_UART0, DIO_ModBusMessage[0]);
}
void sendAOquery(bool calmodein)
{
    if (AO_Type == 0)
        {SendQveaveUI2AO(calmodein);}   
    else if (AO_Type == 1)
        {SendQWINIO2AOMC(calmodein);}   
}
//-------------------------------------------------------
unsigned char rec_Aioresponse(void)
{
    unsigned char res = 0;
    unsigned char expected_addr;

    comfail = false;
    if (AO_Type == 0)
        expected_addr = AO_ModuleAddVeaveUI2AO;
    else if (AO_Type == 1)
        expected_addr = AO_ModuleAddWINIO2AOMC;
    else
        return 0;

    if ((DIO_ModBusIdx > 6) && (DIO_ModBusIdx < 127))
    {
        if (DIO_ModBusMessage[0] != expected_addr)
            return 0;

        res = 1;
        comfail = true;
        DIO_GetCRC16(DIO_ModBusMessage, DIO_ModBusIdx - 1);
        if ((DIO_ModBusMessage[DIO_ModBusIdx - 1] == DIO_CRCHi) &&
            (DIO_ModBusMessage[DIO_ModBusIdx] == DIO_CRCLo))
        {
            comfail = false;
        }
    }
    return res;
}
//-------------------------------------------------------
bool DIO_Comm(void)
{
    return true;
}
bool AIO_Comm(void)
{
#define MAXAOERR 25
    bool iocommover = true;
    static unsigned char iocomsts=0;
    unsigned char resp;
    int i;
if((AO_Type !=0)&&(AO_Type !=1))
{
    iocomsts = 0;
    iocomerrcnts = 0;
    return true;
}
//if AO_Type is not 0 or 1, its not AO module and we dont 
// need to send AO query
    switch (iocomsts)
    {
        case 0:
            //for (i = 0; i < 2000; i++) {;}
            sendAOquery(false);
            iocomcounts = 0;
            response_time = 0;
            comfail = false;
            iocommover = false;
            iocomsts = 1;
            break;
        case 1:
            resp = rec_Aioresponse();
            if (resp == 0)
            {
                if (response_time > 30) /* 30 x 5 ms = ~150 ms AO response timeout */
                {
                    comfail = true;
                    if (iocomerrcnts++ > MAXAOERR)
                        iocomerrcnts = MAXAOERR;
                    iocomcounts = 0;
                    iocommover = true;
                    iocomsts = 0;
                }
                else
                {
                    iocommover = false;
                }
            }
            else if (resp == 1)
            {
                iocommover = true;
                if (comfail == false)
                {
                    iocomerrcnts = 0;
                    AO_Comm_Fault = 0;
                }
                else if (iocomerrcnts++ > MAXAOERR)
                    iocomerrcnts = MAXAOERR;
                iocomcounts = 0;
                iocomsts = 0;
                DIO_ModBusIdx = -1;
                tx_over = 0;
            }
            if (iocomerrcnts >= MAXAOERR)
            {
                iocomsts = 0;
                iocomerrcnts = 0;
                AO_Comm_Fault = 1;
                Error_No = 17;
            }
            break;
    }
    return iocommover;
}
//-------------------------------------------------
// aocomm code
/* 

                if(commpointer == 0)
                {
                    commpointer=1;
                    // msk added a delay of one loop time to ensure the 
                    // previous tx/rx queue is flushed
                    for (spin_i = 0; spin_i < 350; spin_i++) {;}// spin delay after commpointer 0->1
                    break;
                }// if (commpointer == 0)
                if (commpointer == 1)
                {
                    if (AIO_Comm() == false) { break; }// yield until AIO comm slot done
                    else { commpointer++; break; }// AIO_Comm ok, advance commpointer
                }// if (commpointer == 1)
                if(commpointer == 2)
                {
                    commpointer=3;
                    // msk added a delay of one loop time to ensure the 
                    // previous tx/rx queue is flushed
                    for (spin_i = 0; spin_i < 350; spin_i++) {;}// spin delay after commpointer 2->3
                    break;
                }// if (commpointer == 2)
                else if (commpointer == 3)
                {
                    if (DIO_Comm() == false) { break; }// yield until DIO comm slot done
                    else { commpointer++; break; }// DIO_Comm ok, advance commpointer
                }// else if (commpointer == 3)
                else if (commpointer == 4)
                {
                    commpointer = 5;
                    // msk added a delay of one loop time to ensure the
                    // previous tx/rx queue is flushed
                    for (spin_i = 0; spin_i < 350; spin_i++) {;}// spin delay after commpointer 4->5
                    break;
                }// else if (commpointer == 4)
                else if (commpointer < 20)
                    {commpointer++;}// else if (commpointer < 20)
                else
                    {
                        commpointer = 0; 
                        //break;
                    }// else final commpointer path

*/
//------------------------------------------------------------------------
/* AO/DIO bus slots before VFD; return 1 if this scan stops before VFD TX */
static unsigned char Invter_RunAoDioSlot(int spin_i)
{
    if (inv_commpointer == 0)
    {
        if (AIO_Comm() == false)
            return 1;
        inv_commpointer++;
        for (spin_i = 0; spin_i < 350; spin_i++) {;} /* RS485 bus settle after AO */
        return 1;
    }
    if (inv_commpointer == 1)
    {
        if (DIO_Comm() == false)
            return 1;
        inv_commpointer++;
        for (spin_i = 0; spin_i < 350; spin_i++) {;} /* RS485 bus settle after DIO slot */
        return 1;
    }
    return 0;
}

/* After long if/else/switch runs, each closing brace is tagged:  }// <what this } closes>  */
void Invter_ModCom(void)
{
    unsigned char CW_add_hi, CW_add_lw, SW_add_hi, SW_add_lw, Reg_Read_Count;
    int spin_i; /* ARMCC 5 (C90): no 'for (int i = ...)' in for-init */
    ResetInvCommOnStartEdge();

    /* WF VFD OFF (Inv==0): still run AO on shared RS485 when AO module fitted */
    if (Inv == 0)
    {
        if ((AO_Type == 0) || (AO_Type == 1))
        {
            if (Invter_RunAoDioSlot(spin_i))
            {
                /* DIO slot leaves pointer at 2; wrap for AO-only (no VFD gap 2..14) */
                if (inv_commpointer >= 2)
                    inv_commpointer = 0;
                return;
            }
            inv_commpointer = 0;
        }
        else
            inv_commpointer = 0;

        if ((DIO_Module == 1) || (DIO_Module == 2) ||
            ((Inv_1 == 1) || (Inv_1 == 2) || (Inv_1 == 3) || (Inv_1 == 4) ||
             (Inv_1 == 5) || (Inv_1 == 6) || (Inv_1 == 7) || (Inv_1 == 8)))
            Dev_ID = 3;
        return;
    }

    if ((Inv == 1) || (Inv == 2) || (Inv == 3) || (Inv == 4) ||
        (Inv == 5) || (Inv == 6) || (Inv == 7) || (Inv == 8))
    {
        if (Dev_ID == 2)
        {
            switch (Invter_Status)
            {
            case 1:
            {
				if (Invter_RunAoDioSlot(spin_i))
                    break;
                else if (inv_commpointer < 15)
                    {inv_commpointer++;}
                else
                {
                    inv_commpointer = 0;
                    break; /* gap complete; start AO on next scan, no extra VFD */
                }

                if (M066 == 1)
                {
                    switch (Inv)
                {
                    case 1 :
                        Start_Stop_Inv = 15;//Atv31
                        if(Read_count_AtvReg==0)
                        {
                            SW_add_hi = 0x0C;
                            SW_add_lw = 0x81;
                            Reg_Read_Count= 0x04;
                            Rx_byte_count = 12;
                        }// if(Read_count_AtvReg==0)
                        else
                        {
                            SW_add_hi = 0x14;
                            SW_add_lw = 0x78;
                            Reg_Read_Count= 0x01;
                            Rx_byte_count = 6;
                        }// else Read_count_AtvReg ATV31
                        CW_add_hi = 0x21;
                        CW_add_lw = 0x35;
                        fault_code = 0x28;break;
                    case 2 :
                        Start_Stop_Inv = 2;//PF4M
                        SW_add_hi = 0x21;
                        SW_add_lw = 0x00;
                        CW_add_hi = 0x20;
                        CW_add_lw = 0x00;
                        fault_code = 0x8c;
                        Reg_Read_Count= 0x0d;     //Prev=0x01;new 0x0d added on 16/01/2018
                        Rx_byte_count = 30;break; //Prev=6;new 30 added on    16/01/2018
                    case 6 ://ACS380
                    case 7 ://ACS560
                    case 8 ://ACS580
                    case 3 :
                        Start_Stop_Inv = 0x7f;//63;//ACS550
                        SW_add_hi = 0x00;
                        SW_add_lw = 0x03;
                        CW_add_hi = 0x00;
                        CW_add_lw = 0x00;
                        fault_code = 0x38;
                        Reg_Read_Count= 0x06;
                        Rx_byte_count = 16;break;
                    case 5://J7
                    case 4:
                        Start_Stop_Inv = 1;//J1000
                        SW_add_hi = 0x00;
                        SW_add_lw = 0x20;
                        CW_add_hi = 0x00;
                        CW_add_lw = 0x01;
                        fault_code = 0x28;
                        if(Inv==5)
                            fault_code = 0x08;
                        Reg_Read_Count= 0x0C;
                        Rx_byte_count = 28;break;
                    default: break;
                    }// switch (Inv), M066==1
                }// if (M066 == 1)
                if (M066 == 0)
                {
                    switch (Inv)
                    {
                    case 1 :
                        if(M008==1)
                        Start_Stop_Inv = 6;
                        if(Read_count_AtvReg==0)
                        {
                            SW_add_hi = 0x0C;
                            SW_add_lw = 0x81;
                            Reg_Read_Count= 0x04;
                            Rx_byte_count = 12;
                        }// if(Read_count_AtvReg==0) M066==0
                        else
                        {
                            SW_add_hi = 0x14;
                            SW_add_lw = 0x78;
                            Reg_Read_Count= 0x01;
                            Rx_byte_count = 6;//6( (1*2) + 4 = 6)
                        }// else Read_count_AtvReg M066==0
                        CW_add_hi = 0x21;
                        CW_add_lw = 0x35;
                        fault_code = 0x28;

                        break;

                    case 2 :
                        if(M008==1)
                        Start_Stop_Inv = 1;
                        SW_add_hi = 0x21;
                        SW_add_lw = 0x00;
                        CW_add_hi = 0x20;
                        CW_add_lw = 0x00;
                        fault_code = 0x8c;
                        Reg_Read_Count= 0x0d;     //Prev=0x01;new 0x0d added on 16/01/2018
                        Rx_byte_count = 30;break; //Prev=6;new 30( (13*2) + 4 = 30) added on    16/01/2018
                    case 6 ://ACS380
                    case 7 ://ACS560
                    case 8 ://ACS580
                    case 3 :
                        if(M008==1)
                        Start_Stop_Inv = 0x76;//6;
                        SW_add_hi = 0x00;
                        SW_add_lw = 0x03;
                        CW_add_hi = 0x00;
                        CW_add_lw = 0x00;
                        fault_code = 0x38;
                        Reg_Read_Count= 0x06;
                        Rx_byte_count = 16;break;//16( (6*2) + 4 = 16)
                    case 5 :
                    case 4 :
                        if(M008==1)
                        Start_Stop_Inv = 0;
                        SW_add_hi = 0x00;
                        SW_add_lw = 0x20;
                        CW_add_hi = 0x00;
                        CW_add_lw = 0x01;
                        fault_code = 0x28;
                        if(Inv==5)
                            fault_code = 0x08;

                        Reg_Read_Count= 0x0C;
                        Rx_byte_count = 28;break;//28( (12*2) + 4 = 28)

                    default: break;
                    }// switch (Inv), M066==0
                }// if (M066 == 0)
                if ((INV_read_ok == 0) && (INV_write_ok == 1))
                {
                    DIO_ComposeMessageRtu(0x02, 0x03, SW_add_hi, SW_add_lw, 0x00, Reg_Read_Count, 0x00);
                }// if INV_read_ok==0 && INV_write_ok==1
                if ((INV_read_ok == 1) && (INV_write_ok == 0))
                {
                    if ((Inv == 3) || (Inv == 6) || (Inv == 7) || (Inv == 8))
                    {
                        Inv_Sp = (((unsigned int)(WF_Drive_Setpoint) * 100) / 4095);
                        Inv_Sp = Inv_Sp * 100;
                    }// if ACS family Inv for setpoint
                    else
                        Inv_Sp = (((unsigned int)(WF_Drive_Setpoint) * 500) / 4095);// else Inv setpoint scaling
                    if (INV_Error_Flag == 1)
                    {
                        switch (Inv)
                        {
                        case 1:
                            Mod_Data[0] = 0x00;
                            Mod_Data[1] = 0x86;
                            break;
                        case 2:
                            Mod_Data[0] = 0x00;
                            Mod_Data[1] = 0x08;
                            break;
                        case 6:
                        case 7:
                        case 8:
                            Mod_Data[0] = 0x04;
                            Mod_Data[1] = 0xFF;
                            break;
                        case 3:
                            Mod_Data[0] = 0x04;
                            Mod_Data[1] = 0x86;
                            break;
                        case 5:
                        case 4:
                            Mod_Data[0] = 0x00;
                            Mod_Data[1] = 0x08;
                            break;
                        default: break;
                        }// switch (Inv) Mod_Data when INV_Error_Flag==1
                    }// if (INV_Error_Flag == 1)
                    else
                    {
                        switch (Inv)
                        {
                        case 1:
                            Mod_Data[0] = 0x00;
                            break;
                        case 2:
                            Mod_Data[0] = 0x00;
                            break;
                        case 6:
                        case 7:
                        case 8:
                        case 3:
                            Mod_Data[0] = 0x04;
                            break;
                        case 5:
                        case 4:
                            Mod_Data[0] = 0x00;
                            break;
                        default: break;
                        }// switch (Inv) Mod_Data else branch (no error flag)
                        Mod_Data[1] = Start_Stop_Inv;
                    }// else (INV_Error_Flag == 0)
                    if (Inv == 3)
                    {
                        Mod_Data[4] = (Inv_Sp & 0xff00) >> 8;
                        Mod_Data[5] = (Inv_Sp & 0x00ff);
                        DIO_ComposeMessageRtu(0x02, 0x10, CW_add_hi, CW_add_lw, 0x00, 0x03, 0x06);
                    }// if (Inv == 3) write holding regs layout
                    else
                    {
                        Mod_Data[2] = (Inv_Sp & 0xff00) >> 8;
                        Mod_Data[3] = (Inv_Sp & 0x00ff);
                        DIO_ComposeMessageRtu(0x02, 0x10, CW_add_hi, CW_add_lw, 0x00, 0x02, 0x02);
                    }// else Inv write holding regs layout
                    Rx_byte_count = 7;
                }// if (INV_read_ok == 1) && (INV_write_ok == 0)
                Invter_Status = 2;
                break;
            }// case 1 Invter_Status
            case 2:
            {
                if (tx_over == 1)
                {
                    Invter_Status = 3;
                    tx_over = 0;
                    inv_rx_last_idx = -2;
                    inv_rx_progress_time = response_time;
                }// if (tx_over == 1)
                break;
            }// case 2 Invter_Status
            case 3:
            {
                if (DIO_ModBusIdx == Rx_byte_count)
                {
                    DIO_GetCRC16(DIO_ModBusMessage, DIO_ModBusIdx - 1);
                    if ((DIO_ModBusMessage[DIO_ModBusIdx - 1] == DIO_CRCHi) &&
                        (DIO_ModBusMessage[DIO_ModBusIdx] == DIO_CRCLo))
                    {
                        if(DIO_ModBusMessage[1]==0x03)
                        {
                            INV_modarray_rd[1]= DIO_ModBusMessage[3];                    //Higher Byte of status word 4850: 8 to 15 bits
                            INV_modarray_rd[0]= DIO_ModBusMessage[4];                    //Lower Byte of status word  4850: 0 to 7 bits
                            if((Inv==1) &&(Read_count_AtvReg==0))                            //ATV32
                            {
                                INV_modarray_rd[3] = DIO_ModBusMessage[5];        //Higher Byte of 4851: 8 to 15 bits op frequency in hz
                                INV_modarray_rd[2] = DIO_ModBusMessage[6];        //Lower Byte of  4851: 0 to 7 bits op frequency in hz
                                //INV_modarray_rd[7] = DIO_ModBusMessage[10];        //Higher Byte of 4851: 8 to 15 bits op frequency in hz
                                INV_modarray_rd[6] = DIO_ModBusMessage[10];        //Lower Byte of  4851: 0 to 7 bits op frequency in hz
                            }// if Inv==1 && Read_count_AtvReg==0
                            INV_modarray_rd[5] = DIO_ModBusMessage[11];                //Higher Byte
                            INV_modarray_rd[4] = DIO_ModBusMessage[12];                //Lower Byte
                            if((Inv==3)||(Inv==6)||(Inv==7)||(Inv==8))//ACS550
                            {
                                if(Inv==3)
                                {
                                    INV_modarray_rd[3]= DIO_ModBusMessage[5];            //Higher Byte of 5310 : 8 to 15 bits
                                    INV_modarray_rd[2]= DIO_ModBusMessage[6];            //Lower Byte of 5310 : 0 to 7 bits Digital ips from 1 to 3
                                    INV_modarray_rd[5]= DIO_ModBusMessage[7];            //Higher Byte of 5311: 8 to 15 bits
                                    INV_modarray_rd[4]= DIO_ModBusMessage[8];            //Lower Byte of 5311: 0 to 7 bits Digital ips from 4 to 6
                                    INV_modarray_rd[7]= DIO_ModBusMessage[11];        //Higher Byte of 5313: 8 to 15 bits op frequency in hz
                                    INV_modarray_rd[6]= DIO_ModBusMessage[12];        //Lower Byte of 5313: 0 to 7 bits op frequency in hz
                                    //INV_modarray_rd[9]= DIO_ModBusMessage[13];        //Higher Byte of 5314: 8 to 15 bits op frequency in hz
                                    INV_modarray_rd[8]= DIO_ModBusMessage[14];        //Lower Byte of 5314: 0 to 7 bits op Current in mA
                                }// if Inv==3 (ACS unpack branch A)
                                else
                                {
                                    INV_modarray_rd[3]= DIO_ModBusMessage[5];            //Higher Byte : 8 to 15 bits
                                    INV_modarray_rd[2]= DIO_ModBusMessage[6];            //Lower Byte  : 0 to 7 bits Digital ips from 1 to 6
                                    INV_modarray_rd[5]= DIO_ModBusMessage[7];            //Higher Byte : 8 to 15 bits
                                    INV_modarray_rd[4]= DIO_ModBusMessage[8];            //Lower Byte  : 0 to 7 bits Digital OP /* RO0
                                    INV_modarray_rd[7]= DIO_ModBusMessage[9];          //Higher Byte : 8 to 15 bits op frequency in hz
                                    INV_modarray_rd[6]= DIO_ModBusMessage[10];        //Lower Byte  : 0 to 7 bits op frequency in hz
                                    INV_modarray_rd[8]= DIO_ModBusMessage[14];        //Lower Byte  : 0 to 7 bits op Current in mA
                                }// else ACS unpack (non ACS550 legacy Inv)
                            }// if ACS family Inv unpack
                            if(Inv==2)//PF4M
                            {
                                INV_modarray_rd[3]= DIO_ModBusMessage[9];            //Higher Byte of 4851: 8 to 15 bits op frequency in hz
                                INV_modarray_rd[2]= DIO_ModBusMessage[10];        ///Lower Byte of 4851: 0 to 7 bits op frequency in hz
                            }// if Inv==2 PF4M
                            if((Inv==4)||(Inv==5))//Yaskawa J1000 & J7 Mini
                            {
                                INV_modarray_rd[3]= DIO_ModBusMessage[9];            //Higher Byte of 0X0024: 8 to 15 bits op frequency in hz
                                INV_modarray_rd[2]= DIO_ModBusMessage[10];        //Lower Byte of 0X0024: 0 to 7 bits op frequency in hz

                                if(Inv==5)
                                {
                                    INV_modarray_rd[7]= DIO_ModBusMessage[17];        //Higher Byte of 0X0027: 8 to 15 bits op current in 10/1A
                                    INV_modarray_rd[6]= DIO_ModBusMessage[18];        //Lower Byte of 0X0027: 0 to 7 bits op Current in in 10/1A
                                }// if Inv==5 J7 current
                                if(Inv==4)
                                {
                                    INV_modarray_rd[7]= DIO_ModBusMessage[15];        //Higher Byte of 0X0027: 8 to 15 bits op current in 10/1A
                                    INV_modarray_rd[6]= DIO_ModBusMessage[16];        //Lower Byte of 0X0027: 0 to 7 bits op Current in in 10/1A
                                }// if Inv==4 J1000 current
                                INV_modarray_rd[5]= DIO_ModBusMessage[25];        //Higher Byte of 0x002B: 5 to 15 bits reseverd
                                INV_modarray_rd[4]= DIO_ModBusMessage[26];        //Lower Byte of 0x002B: Digital ips from 1 to 5
                            }// if Inv==4 || Inv==5 Yaskawa

                            if(INV_modarray_rd[0] == fault_code )    //if fault
                            {
                                INV_Error_Flag = 1;
                                M042 = 1;
                                M044 = 1;
                                Err_Counter_Inv++;
                                if((Err_Counter_Inv % 10) == 1)
                                {
                                    Err_Gen = Err_Gen + 1;
                                    Invter_Status = 1;
                                    DIO_DIRN_MAX487(1);
                                    DIO_ModBusIdx = -1;
                                    //if((DIO_Module == 1)||(DIO_Module == 2))
                                    //Dev_ID = 1;
                                    if((DIO_Module == 1)||(DIO_Module == 2) || ((Inv_1==1)||(Inv_1==2)||(Inv_1==3)||(Inv_1==4)||(Inv_1==5)||(Inv_1==6)||(Inv_1==7)||(Inv_1==8)))
                                    {
                                        Dev_ID = 3;
                                        if(Err_Gen>50)
                                        {
                                            Err_Counter_Inv = 0;
                                            InvComm_SetComError(14); /* msk29-may-26 */
                                            Err_Gen = 0;
                                        }// if Err_Gen > 50
                                    }// if DIO_Module / prefeeder Inv path
                                }// if Err_Counter_Inv % 10 == 1
                            }// if INV_modarray_rd[0] == fault_code
                            else
                            {// not fault_code on status word
                                switch (Inv)
                                {
                                case 1:  /* ATv31 Altivar */
                                    if (Read_count_AtvReg == 1)
                                    {
                                        I024 = (INV_modarray_rd[0] & 0x01);
                                        I025 = (INV_modarray_rd[0] & 0x02) >> 1;
                                        I026 = (INV_modarray_rd[0] & 0x04) >> 2;
                                        I027 = (INV_modarray_rd[0] & 0x08) >> 3;
                                        I028 = (INV_modarray_rd[0] & 0x10) >> 4;
                                        I029 = (INV_modarray_rd[0] & 0x20) >> 5;
                                    }// ATV31 Read_count status bits
                                    else if (Read_count_AtvReg == 0)
                                    {
                                        Inv_Op_Freq = INV_modarray_rd[3];
                                        Inv_Op_Freq = (Inv_Op_Freq << 8) | INV_modarray_rd[2];
                                        Inv_Op_current = INV_modarray_rd[6];
                                    }// ATV31 freq/current
                                    break;
                                case 2:  /* PF4M Allen Bradly */
                                    I024 = (INV_modarray_rd[1] & 0x10) >> 4;
                                    I025 = (INV_modarray_rd[1] & 0x20) >> 5;
                                    I026 = (INV_modarray_rd[0] & 0x01);
                                    I027 = (INV_modarray_rd[0] & 0x02) >> 1;
                                    Inv_Op_Freq = (INV_modarray_rd[3] << 8) | INV_modarray_rd[2];
                                    Inv_Op_current = INV_modarray_rd[4];
                                    break;
                                case 6:
                                case 7:
                                case 8:
                                    I024 = (INV_modarray_rd[2] & 0x01);
                                    I025 = (INV_modarray_rd[2] & 0x02) >> 1;
                                    I026 = (INV_modarray_rd[2] & 0x04) >> 2;
                                    I027 = (INV_modarray_rd[2] & 0x08) >> 3;
                                    I028 = (INV_modarray_rd[2] & 0x10) >> 4;
                                    I029 = (INV_modarray_rd[2] & 0x20) >> 5;
                                    Inv_Op_Freq = (INV_modarray_rd[7] << 8) | INV_modarray_rd[6];
                                    Inv_Op_Freq = (int)((float)Inv_Op_Freq / 10);
                                    Inv_Op_current = INV_modarray_rd[8];
                                    break;
                                case 3:  /* ACS550 ABB */
                                    I024 = (INV_modarray_rd[2] & 0x04) >> 2;
                                    I025 = (INV_modarray_rd[2] & 0x02) >> 1;
                                    I026 = (INV_modarray_rd[2] & 0x01);
                                    I027 = (INV_modarray_rd[4] & 0x04) >> 2;
                                    I028 = (INV_modarray_rd[4] & 0x02) >> 1;
                                    I029 = (INV_modarray_rd[4] & 0x01);
                                    Inv_Op_Freq = (INV_modarray_rd[7] << 8) | INV_modarray_rd[6];
                                    Inv_Op_current = INV_modarray_rd[8];
                                    break;
                                case 5:  /* J7 Mini */
                                case 4:  /* J1000 Yaskawa */
                                    I024 = (INV_modarray_rd[4] & 0x01);
                                    I025 = (INV_modarray_rd[4] & 0x02) >> 1;
                                    I026 = (INV_modarray_rd[4] & 0x04) >> 2;
                                    I027 = (INV_modarray_rd[4] & 0x08) >> 3;
                                    I028 = (INV_modarray_rd[4] & 0x10) >> 4;
                                    if (Inv == 4)
                                        I029 = (INV_modarray_rd[4] & 0x20) >> 5;
                                    Inv_Op_current = INV_modarray_rd[6];
                                    Inv_Op_Freq = (INV_modarray_rd[3] << 8) | INV_modarray_rd[2];
                                    break;
                                default:
                                    break;
                                }// switch (Inv) decode process data fc 0x03
                                INV_Error_Flag = 0;
                            }// else not fault_code
                            INV_read_ok = 1;
                            INV_write_ok = 0;
                            Read_count_AtvReg = Read_count_AtvReg + 1;
                            if (Read_count_AtvReg > 1)
                                Read_count_AtvReg = 0;
                        }// if DIO_ModBusMessage[1] == 0x03 read response handler
                        if (DIO_ModBusMessage[1] == 0x10)
                        {
                            INV_modarray_wr[0] = DIO_ModBusMessage[5];
                            INV_read_ok = 0;
                            INV_write_ok = 1;
                            if ((DIO_Module == 1) || (DIO_Module == 2) ||
                                ((Inv_1 == 1) || (Inv_1 == 2) || (Inv_1 == 3) || (Inv_1 == 4) ||
                                 (Inv_1 == 5) || (Inv_1 == 6) || (Inv_1 == 7) || (Inv_1 == 8)))
                                Dev_ID = 3;// if second bus / INV1 present route Dev_ID 3 (fc 10 path)
                        }// if DIO_ModBusMessage[1] == 0x10 write ACK handler
                        Invter_Status = 1;
                        DIO_DIRN_MAX487(1);
                        DIO_ModBusIdx = -1;
                        Err_Counter_Inv = 0;
                        Err_Gen = 0;
                    }// if (...) DIO_ModBusMessage CRC HI/LO match (inside Rx complete)
                    else
                    {// else CRC mismatch while byte count matched Rx_byte_count
                        Err_Counter_Inv++;
                        if (Err_Counter_Inv % 10 == 1)
                        {
                            Err_Gen = Err_Gen + 1;
                            Invter_Status = 1;
                            DIO_DIRN_MAX487(1);
                            DIO_ModBusIdx = -1;
                            if ((DIO_Module == 1) || (DIO_Module == 2) ||
                                ((Inv_1 == 1) || (Inv_1 == 2) || (Inv_1 == 3) || (Inv_1 == 4) ||
                                 (Inv_1 == 5) || (Inv_1 == 6) || (Inv_1 == 7) || (Inv_1 == 8)))
                                {Dev_ID = 3;}// reroute Dev_ID CRC-mismatch backoff path
                            if (Err_Gen > 50)
                            {
                                Err_Counter_Inv = 0;
                                InvComm_SetComError(14);
                                Err_Gen = 0;
                            }// if Err_Gen > 50 (crc bad path)
                        }// if Err_Counter_Inv % 10 == 1 (crc bad path)
                    }// end else of if CRC matches expected (DIO_CRCHi/CRClo)
                }// end if (DIO_ModBusIdx == Rx_byte_count)
                else
                {// else DIO_ModBusIdx != Rx_byte_count (still receiving)
                    if (DIO_ModBusIdx != inv_rx_last_idx)
                    {
                        inv_rx_last_idx = DIO_ModBusIdx;
                        inv_rx_progress_time = response_time;
                    }
                    else if ((uint16_t)(response_time - inv_rx_progress_time) > INV_RX_TIMEOUT_TICKS)
                    {
                        Err_Gen = Err_Gen + 1;
                        Invter_Status = 1;
                        DIO_DIRN_MAX487(1);
                        DIO_ModBusIdx = -1;
                        inv_rx_last_idx = -2;
                        inv_rx_progress_time = response_time;
                        if ((DIO_Module == 1) || (DIO_Module == 2) ||
                            ((Inv_1 == 1) || (Inv_1 == 2) || (Inv_1 == 3) || (Inv_1 == 4) ||
                             (Inv_1 == 5) || (Inv_1 == 6) || (Inv_1 == 7) || (Inv_1 == 8)))
                            Dev_ID = 3;
                        if (Err_Gen > 50)
                        {
                            Err_Counter_Inv = 0;
                            InvComm_SetComError(14);
                            Err_Gen = 0;
                        }// if Err_Gen > 50 (partial frame timeout path)
                    }// no progress for INV_RX_TIMEOUT_TICKS x 5 ms
                }// else branch of if(DIO_ModBusIdx==Rx_byte_count) -- frame still assembling
                    
                break;
            }// case 3 Invter_Status
            default:
            {
                break;
            }// default Invter_Status

        }// switch Invter_Status
    }// if Dev_ID == 2
    else if ((DIO_Module == 1) || (DIO_Module == 2) ||
             ((Inv_1 == 1) || (Inv_1 == 2) || (Inv_1 == 3) || (Inv_1 == 4) ||
              (Inv_1 == 5) || (Inv_1 == 6) || (Inv_1 == 7) || (Inv_1 == 8)))
        Dev_ID = 3;// else-if route Dev_ID 3 via DIO/INV1 combo
    }// if Inv type supported
}// void Invter_ModCom

void Invter2_ModCom(void)
{
    unsigned char CW_add_hi, CW_add_lw, SW_add_hi, SW_add_lw, Reg_Read_Count;

    if ((Inv_1 == 1) || (Inv_1 == 2) || (Inv_1 == 3) || (Inv_1 == 4) ||
        (Inv_1 == 5) || (Inv_1 == 6) || (Inv_1 == 7) || (Inv_1 == 8))
    {
        if (Dev_ID == 3)
        {
            switch (Invter2_Status)
            {
            case 1:
                if (M066 == 1)
                {
                    switch (Inv_1)
                    {
                    case 1:
                        Start_Stop_Inv = 15;
                        if (Read_count_AtvReg_1 == 0)
                        {
                            SW_add_hi = 0x0C;
                            SW_add_lw = 0x81;
                            Reg_Read_Count = 0x04;
                            Rx_byte_count = 12;
                        }// if Read_count_AtvReg_1==0 (INV2 case1 M066==1)
                        else
                        {
                            SW_add_hi = 0x14;
                            SW_add_lw = 0x78;
                            Reg_Read_Count = 0x01;
                            Rx_byte_count = 6;
                        }// else Read_count_AtvReg_1 ATV31 INV2
                        CW_add_hi = 0x21;
                        CW_add_lw = 0x35;
                        fault_code = 0x28;
                        break;
                    case 2:
                        Start_Stop_Inv = 2;
                        SW_add_hi = 0x21;
                        SW_add_lw = 0x00;
                        CW_add_hi = 0x20;
                        CW_add_lw = 0x00;
                        fault_code = 0x8c;
                        Reg_Read_Count = 0x0d;
                        Rx_byte_count = 30;
                        break;
                    case 6:
                    case 7:
                    case 8:
                    case 3:
                        Start_Stop_Inv = 0x7f;
                        SW_add_hi = 0x00;
                        SW_add_lw = 0x03;
                        CW_add_hi = 0x00;
                        CW_add_lw = 0x01;
                        fault_code = 0x38;
                        Reg_Read_Count = 0x06;
                        Rx_byte_count = 16;
                        break;
                    case 5:
                    case 4:
                        Start_Stop_Inv = 1;
                        SW_add_hi = 0x00;
                        SW_add_lw = 0x20;
                        CW_add_hi = 0x00;
                        CW_add_lw = 0x01;
                        fault_code = 0x28;
                        if (Inv == 5)
                            fault_code = 0x08;
                        Reg_Read_Count = 0x0C;
                        Rx_byte_count = 28;
                        break;
                    default: break;
                    }// switch (Inv_1) M066==1 branch
                }// if M066==1 INV2 case1
                if (M066 == 0)
                {
                    switch (Inv_1)
                    {
                    case 1:
                        if (M008 == 1) Start_Stop_Inv = 6;
                        if (Read_count_AtvReg_1 == 0)
                        {
                            SW_add_hi = 0x0C;
                            SW_add_lw = 0x81;
                            Reg_Read_Count = 0x04;
                            Rx_byte_count = 12;
                        }// if Read_count_AtvReg_1==0 INV2 M066==0
                        else
                        {
                            SW_add_hi = 0x14;
                            SW_add_lw = 0x78;
                            Reg_Read_Count = 0x01;
                            Rx_byte_count = 6;
                        }// else Read_count_AtvReg_1 INV2 M066==0
                        CW_add_hi = 0x21;
                        CW_add_lw = 0x35;
                        fault_code = 0x28;
                        break;
                    case 2:
                        if (M008 == 1) Start_Stop_Inv = 1;
                        SW_add_hi = 0x21;
                        SW_add_lw = 0x00;
                        CW_add_hi = 0x20;
                        CW_add_lw = 0x00;
                        fault_code = 0x8c;
                        Reg_Read_Count = 0x0d;
                        Rx_byte_count = 30;
                        break;
                    case 6:
                    case 7:
                    case 8:
                    case 3:
                        if (M008 == 1) Start_Stop_Inv = 0x76;
                        SW_add_hi = 0x00;
                        SW_add_lw = 0x03;
                        CW_add_hi = 0x00;
                        CW_add_lw = 0x01;
                        fault_code = 0x38;
                        Reg_Read_Count = 0x06;
                        Rx_byte_count = 16;
                        break;
                    case 5:
                    case 4:
                        if (M008 == 1) Start_Stop_Inv = 0;
                        SW_add_hi = 0x00;
                        SW_add_lw = 0x20;
                        CW_add_hi = 0x00;
                        CW_add_lw = 0x01;
                        fault_code = 0x28;
                        if (Inv == 5) fault_code = 0x08;
                        Reg_Read_Count = 0x0C;
                        Rx_byte_count = 28;
                        break;
                    default: break;
                    }// switch (Inv_1) M066==0 branch
                }// if M066==0 INV2 case1
                if ((INV2_read_ok == 0) && (INV2_write_ok == 1))
                {
                    DIO_ComposeMessageRtu(0x03, 0x03, SW_add_hi, SW_add_lw, 0x00, Reg_Read_Count, 0x00);
                }// if INV2 read phase compose 0x03
                if ((INV2_read_ok == 1) && (INV2_write_ok == 0))
                {
                    if (Prefeeder == 1)
                    {
                        if ((Inv_1 == 3) || (Inv_1 == 6) || (Inv_1 == 7) || (Inv_1 == 8))
                        {
                            Inv_Sp = (((unsigned int)(PF_Drive_Setpoint) * 100) / 4095);
                            Inv_Sp = Inv_Sp * 100;
                        }// ACS-style PF setpoint scaling
                        else
                            Inv_Sp = (((unsigned int)(PF_Drive_Setpoint) * 500) / 4095);// else PF setpoint norm
                    }// if Prefeeder==1
                    if (Prefeeder == 0)
                    {
                        if ((Inv_1 == 3) || (Inv_1 == 6) || (Inv_1 == 7) || (Inv_1 == 8))
                        {
                            Inv_Sp = (((unsigned int)(WF_Drive_Setpoint) * 100) / 4095);
                            Inv_Sp = Inv_Sp * 100;
                        }// ACS-style WF setpoint scaling
                        else
                            Inv_Sp = (((unsigned int)(WF_Drive_Setpoint) * 500) / 4095);// else WF setpoint norm
                    }// if Prefeeder==0
                    if (INV2_Error_Flag == 1)
                    {
                        switch (Inv_1)
                        {
                        case 1: Mod_Data[0] = 0x00; Mod_Data[1] = 0x86; break;
                        case 2: Mod_Data[0] = 0x00; Mod_Data[1] = 0x08; break;
                        case 6:
                        case 7:
                        case 8: Mod_Data[0] = 0x04; Mod_Data[1] = 0xFF; break;
                        case 3: Mod_Data[0] = 0x04; Mod_Data[1] = 0x86; break;
                        case 5:
                        case 4: Mod_Data[0] = 0x00; Mod_Data[1] = 0x08; break;
                        default: break;
                        }// switch (Inv_1) Mod_Data INV2 err flag set
                    }// if INV2_Error_Flag==1 (Mod_Data faults)
                    else
                    {
                        switch (Inv_1)
                        {
                        case 1: Mod_Data[0] = 0x00; break;
                        case 2: Mod_Data[0] = 0x00; break;
                        case 6:
                        case 7:
                        case 8:
                        case 3: Mod_Data[0] = 0x04; break;
                        case 5:
                        case 4: Mod_Data[0] = 0x00; break;
                        default: break;
                        }// switch (Inv_1) Mod_Data INV2 ok path
                        Mod_Data[1] = Start_Stop_Inv;
                    }// else INV2_Error_Flag==0
                    if (Inv_1 == 3)
                    {
                        Mod_Data[4] = (Inv_Sp & 0xff00) >> 8;
                        Mod_Data[5] = (Inv_Sp & 0x00ff);
                        DIO_ComposeMessageRtu(0x03, 0x10, CW_add_hi, CW_add_lw, 0x00, 0x03, 0x06);
                    }// if Inv_1==3 write PDU layout INV2
                    else
                    {
                        Mod_Data[2] = (Inv_Sp & 0xff00) >> 8;
                        Mod_Data[3] = (Inv_Sp & 0x00ff);
                        DIO_ComposeMessageRtu(0x03, 0x10, CW_add_hi, CW_add_lw, 0x00, 0x02, 0x02);
                    }// else INV2 write PDU layout
                    Rx_byte_count = 7;
                }// if INV2_read_ok && write phase
                Invter2_Status = 2;
                break;
            case 2:
                if (tx_over == 1)
                {
                    Invter2_Status = 3;
                    tx_over = 0;
                    inv2_rx_last_idx = -2;
                    inv2_rx_progress_time = response_time;
                }// if tx_over INV2 wait TX done
                break;
            case 3:// --- INV2 Rx parse (parallel logic to Invter_ModCom case 3) ---
                if (DIO_ModBusIdx == Rx_byte_count)
                {
                    DIO_GetCRC16(DIO_ModBusMessage, DIO_ModBusIdx - 1);
                    if ((DIO_ModBusMessage[DIO_ModBusIdx - 1] == DIO_CRCHi) &&
                        (DIO_ModBusMessage[DIO_ModBusIdx] == DIO_CRCLo))
                    {
                        if (DIO_ModBusMessage[1] == 0x03)
                        {
                            INV_modarray_rd[1] = DIO_ModBusMessage[3];
                            INV_modarray_rd[0] = DIO_ModBusMessage[4];
                            if ((Inv_1 == 1) && (Read_count_AtvReg_1 == 0))
                            {
                                INV_modarray_rd[3] = DIO_ModBusMessage[5];
                                INV_modarray_rd[2] = DIO_ModBusMessage[6];
                                INV_modarray_rd[6] = DIO_ModBusMessage[10];
                            }// Inv_1==1 ATV reg alternate read INV2 fc03
                            INV_modarray_rd[5] = DIO_ModBusMessage[11];
                            INV_modarray_rd[4] = DIO_ModBusMessage[12];
                            if ((Inv_1 == 3) || (Inv_1 == 6) || (Inv_1 == 7) || (Inv_1 == 8))
                            {
                                if (Inv_1 == 3)
                                {
                                    INV_modarray_rd[3] = DIO_ModBusMessage[5];
                                    INV_modarray_rd[2] = DIO_ModBusMessage[6];
                                    INV_modarray_rd[5] = DIO_ModBusMessage[7];
                                    INV_modarray_rd[4] = DIO_ModBusMessage[8];
                                    INV_modarray_rd[7] = DIO_ModBusMessage[11];
                                    INV_modarray_rd[6] = DIO_ModBusMessage[12];
                                    INV_modarray_rd[8] = DIO_ModBusMessage[14];
                                }// Inv_1==3 ACS INV2 unpack
                                else
                                {
                                    INV_modarray_rd[3] = DIO_ModBusMessage[5];
                                    INV_modarray_rd[2] = DIO_ModBusMessage[6];
                                    INV_modarray_rd[5] = DIO_ModBusMessage[7];
                                    INV_modarray_rd[4] = DIO_ModBusMessage[8];
                                    INV_modarray_rd[7] = DIO_ModBusMessage[9];
                                    INV_modarray_rd[6] = DIO_ModBusMessage[10];
                                    INV_modarray_rd[8] = DIO_ModBusMessage[14];
                                }// else ACS family unpack INV2 fc03
                            }// Inv_1 ACS378/560/580 branch fc03
                            if (Inv_1 == 2)
                            {
                                INV_modarray_rd[3] = DIO_ModBusMessage[9];
                                INV_modarray_rd[2] = DIO_ModBusMessage[10];
                            }// Inv_1==2 PF4M fc03 unpack
                            if ((Inv_1 == 4) || (Inv_1 == 5))
                            {
                                INV_modarray_rd[3] = DIO_ModBusMessage[9];
                                INV_modarray_rd[2] = DIO_ModBusMessage[10];
                                if (Inv_1 == 5)
                                {
                                    INV_modarray_rd[7] = DIO_ModBusMessage[17];
                                    INV_modarray_rd[6] = DIO_ModBusMessage[18];
                                }// Inv_1==5 J7 current INV2 fc03
                                if (Inv_1 == 4)
                                {
                                    INV_modarray_rd[7] = DIO_ModBusMessage[15];
                                    INV_modarray_rd[6] = DIO_ModBusMessage[16];
                                }// Inv_1==4 J1000 current INV2 fc03
                                INV_modarray_rd[5] = DIO_ModBusMessage[25];
                                INV_modarray_rd[4] = DIO_ModBusMessage[26];
                            }// Inv_1 Yaskawa J1000/J7 fc03 unpack
                            if (INV_modarray_rd[0] == fault_code)
                            {
                                INV2_Error_Flag = 1;
                                Err_Counter_Inv2++;
                                if ((Err_Counter_Inv2 % 10) == 1)
                                {
                                    Err_Gen2 = Err_Gen2 + 1;
                                    Invter2_Status = 1;
                                    DIO_DIRN_MAX487(1);
                                    DIO_ModBusIdx = -1;
                                    if ((DIO_Module == 1) || (DIO_Module == 2) ||
                                        ((Inv == 1) || (Inv == 2) || (Inv == 3) || (Inv == 4) ||
                                         (Inv == 5) || (Inv == 6) || (Inv == 7) || (Inv == 8)))
                                        Dev_ID = 1;
                                    if (Err_Gen2 > 50)
                                    {
                                        Err_Counter_Inv2 = 0;
                                        InvComm_SetComError(15);
                                        Err_Gen2 = 0;
                                    }// Err_Gen2>50 INV2 fault path
                                }// Err_Counter_Inv2 % 10==1 INV2 fault
                            }// if INV_modarray_rd[0]==fault_code INV2
                            else
                            {
                                switch (Inv_1)
                                {
                                case 1:  /* ATv31 Altivar */
                                    if (Read_count_AtvReg_1 == 1)
                                    {
                                        I032 = (INV_modarray_rd[0] & 0x01);
                                        I033 = (INV_modarray_rd[0] & 0x02) >> 1;
                                        I034 = (INV_modarray_rd[0] & 0x04) >> 2;
                                        I035 = (INV_modarray_rd[0] & 0x08) >> 3;
                                        I036 = (INV_modarray_rd[0] & 0x10) >> 4;
                                        I037 = (INV_modarray_rd[0] & 0x20) >> 5;
                                    }// ATV31 INV2 Rd status bits alternate
                                    else if (Read_count_AtvReg_1 == 0)
                                    {
                                        Inv1_Op_Freq = (INV_modarray_rd[3] << 8) | INV_modarray_rd[2];
                                        Inv1_Op_current = INV_modarray_rd[6];
                                    }// ATV31 INV2 freq/current
                                    break;
                                case 2:  /* PF4M */
                                    I032 = (INV_modarray_rd[1] & 0x10) >> 4;
                                    I033 = (INV_modarray_rd[1] & 0x20) >> 5;
                                    I034 = (INV_modarray_rd[0] & 0x01);
                                    I035 = (INV_modarray_rd[0] & 0x02) >> 1;
                                    Inv1_Op_Freq = (INV_modarray_rd[3] << 8) | INV_modarray_rd[2];
                                    Inv1_Op_current = INV_modarray_rd[4];
                                    break;
                                case 6:
                                case 7:
                                case 8:
                                    I032 = (INV_modarray_rd[2] & 0x01);
                                    I033 = (INV_modarray_rd[2] & 0x02) >> 1;
                                    I034 = (INV_modarray_rd[2] & 0x04) >> 2;
                                    I035 = (INV_modarray_rd[2] & 0x08) >> 3;
                                    I036 = (INV_modarray_rd[2] & 0x10) >> 4;
                                    I037 = (INV_modarray_rd[2] & 0x20) >> 5;
                                    Inv1_Op_Freq = (INV_modarray_rd[7] << 8) | INV_modarray_rd[6];
                                    Inv1_Op_Freq = (int)((float)Inv1_Op_Freq / 10);
                                    Inv1_Op_current = INV_modarray_rd[8];
                                    break;
                                case 3:  /* ACS550 */
                                    I032 = (INV_modarray_rd[2] & 0x04) >> 2;
                                    I033 = (INV_modarray_rd[2] & 0x02) >> 1;
                                    I034 = (INV_modarray_rd[2] & 0x01);
                                    I035 = (INV_modarray_rd[4] & 0x04) >> 2;
                                    I036 = (INV_modarray_rd[4] & 0x02) >> 1;
                                    I037 = (INV_modarray_rd[4] & 0x01);
                                    Inv1_Op_Freq = (INV_modarray_rd[7] << 8) | INV_modarray_rd[6];
                                    Inv1_Op_current = INV_modarray_rd[8];
                                    break;
                                case 5:  /* J7 Mini */
                                case 4:  /* J1000 */
                                    I032 = (INV_modarray_rd[4] & 0x01);
                                    I033 = (INV_modarray_rd[4] & 0x02) >> 1;
                                    I034 = (INV_modarray_rd[4] & 0x04) >> 2;
                                    I035 = (INV_modarray_rd[4] & 0x08) >> 3;
                                    I036 = (INV_modarray_rd[4] & 0x10) >> 4;
                                    if (Inv_1 == 4)
                                        I037 = (INV_modarray_rd[4] & 0x20) >> 5;
                                    Inv1_Op_current = INV_modarray_rd[6];
                                    Inv1_Op_Freq = (INV_modarray_rd[3] << 8) | INV_modarray_rd[2];
                                    break;
                                default: break;
                                }// switch (Inv_1) decode fc03 INV2 OK path
                                INV2_Error_Flag = 0;
                            }// else not fault INV2 fc03 decode
                            INV2_read_ok = 1;
                            INV2_write_ok = 0;
                            Read_count_AtvReg_1 = Read_count_AtvReg_1 + 1;
                            if (Read_count_AtvReg_1 > 1)
                                Read_count_AtvReg_1 = 0;
                        }// if fc==0x03 inside CRC-ok INV2
                        if (DIO_ModBusMessage[1] == 0x10)
                        {
                            INV_modarray_wr[0] = DIO_ModBusMessage[5];
                            INV2_read_ok = 0;
                            INV2_write_ok = 1;
                            if ((DIO_Module == 1) || (DIO_Module == 2) ||
                                ((Inv == 1) || (Inv == 2) || (Inv == 3) || (Inv == 4) ||
                                 (Inv == 5) || (Inv == 6) || (Inv == 7) || (Inv == 8)))
                                Dev_ID = 1;// reroute Dev main INV INV2 ACK fc10 path
                        }// if fc==0x10 INV2 ACK
                        Invter2_Status = 1;
                        DIO_DIRN_MAX487(1);
                        DIO_ModBusIdx = -1;
                        Err_Counter_Inv2 = 0;
                        Err_Gen2 = 0;
                    }// if CRC HI/LO match (inside ModBusIdx==Rx)
                    else
                    {// CRC bad while Rx count matched INV2
                        Err_Counter_Inv2++;
                        if ((Err_Counter_Inv2 % 10) == 1)
                        {
                            Err_Gen2 = Err_Gen2 + 1;
                            Invter2_Status = 1;
                            DIO_DIRN_MAX487(1);
                            DIO_ModBusIdx = -1;
                            if ((DIO_Module == 1) || (DIO_Module == 2) ||
                                ((Inv == 1) || (Inv == 2) || (Inv == 3) || (Inv == 4) ||
                                 (Inv == 5) || (Inv == 6) || (Inv == 7) || (Inv == 8)))
                                Dev_ID = 1;
                            if (Err_Gen2 > 50)
                            {
                                Err_Counter_Inv2 = 0;
                                InvComm_SetComError(15);
                                Err_Gen2 = 0;
                            }// Err_Gen2>50 INV2 CRC-bad backoff
                        }// Err %10 INV2 CRC-bad (Rx_byte_count matched)
                    }// end else CRC mismatch INV2 (still inside if DIO_ModBusIdx==Rx_byte_count)
                }// end if (DIO_ModBusIdx == Rx_byte_count) INV2
                else
                {// DIO_ModBusIdx != Rx_byte_count INV2 (still assembling frame)
                    if (DIO_ModBusIdx != inv2_rx_last_idx)
                    {
                        inv2_rx_last_idx = DIO_ModBusIdx;
                        inv2_rx_progress_time = response_time;
                    }
                    else if ((uint16_t)(response_time - inv2_rx_progress_time) > INV_RX_TIMEOUT_TICKS)
                    {
                        Err_Gen2 = Err_Gen2 + 1;
                        Invter2_Status = 1;
                        DIO_DIRN_MAX487(1);
                        DIO_ModBusIdx = -1;
                        inv2_rx_last_idx = -2;
                        inv2_rx_progress_time = response_time;
                        if ((DIO_Module == 1) || (DIO_Module == 2) ||
                            ((Inv == 1) || (Inv == 2) || (Inv == 3) || (Inv == 4) ||
                             (Inv == 5) || (Inv == 6) || (Inv == 7) || (Inv == 8)))
                            Dev_ID = 1;// reroute DEV main INV assembling timeout
                        if (Err_Gen2 > 50)
                        {
                            Err_Counter_Inv2 = 0;
                            InvComm_SetComError(15);
                            Err_Gen2 = 0;
                        }// Err_Gen2>50 assembling-frame path INV2
                    }// no progress for INV_RX_TIMEOUT_TICKS x 5 ms
                }// else Rx_byte_count not reached INV2
                break;
            default:
                break;
            }// switch (Invter2_Status)
        }// if Dev_ID == 3
    }// if Inv_1 is supported drive type
    else if ((DIO_Module == 1) || (DIO_Module == 2) ||
             ((Inv == 1) || (Inv == 2) || (Inv == 3) || (Inv == 4) ||
              (Inv == 5) || (Inv == 6) || (Inv == 7) || (Inv == 8)))
        Dev_ID = 1;// else-if: hand off to main INV modbus path
}// void Invter2_ModCom

/*
 * 20.3.2018 M019 = WF Drive Fault, M020 = PF Drive Fault
 * 07/04/2018 Stop command disabled in local mode
 * 30/05/2018 Inv_Op_Freq: ACS550=4008, PF4m=8451
 * 29/08/2018 ATV312: Read_count for status/IO alternate read
 * 29/11/2018 Yaskawa J1000: CW=0x0001, SW=0x0020, Freq=0x0024
 */

