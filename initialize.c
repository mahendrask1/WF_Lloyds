/*        INITIALIZE.c        */
//This routine is initializes all the paramenters at startup

#include "plc.h"
#include "keys.h"
#include "parameters.h" //#include "menu.h"
#include "main_routine.h"
#include "LPC17xx.h"                    // Device header
#include "GPIO_LPC17xx.h"
#include "lpc17xx_rtc.h"
#include "functions.h"
#include "eeprom.h"
#include "adc.h"
#include "dac.h"
#include "RAM512KB_Ext.h"
#include "OLED_2_7_12864.h"

#include "spi.h"
#include "abcc_td.h"
#include "abcc.h"
#include "abcc_sys_adapt.h"
#include "ad_obj.h"
#include "appl_abcc_handler.h"
#include "io_control.h"

char cstr1[20];
char *cstrptr = cstr1;
extern uint16_t AO_cal_Values[6];
void check_defaults(void);
void Load_Wf_defaults(void);
void eeprom_write_array(unsigned int , unsigned char * , unsigned int );
//-----------------------------------------------
void Print_String(char **string, char start_row, char start_col, char font_size, char underline_no);
extern unsigned long Present_Count,Present_Count_ch2,Count_DL_ch2,Span_Count,Span_Count_ch2,zero_load,zero_count_cal,zero_load_ch2,full_load,full_load_ch2;
//Count_DL
unsigned long Calib_Load,Calib_Load_ch2;
extern unsigned long BL_sum,BL_reg[1600],BL_avg;
extern  uint16_t AO_calval[32];
extern unsigned long calib_val[48];
extern uint16_t  AI_calb_val[16];
extern uint16_t AO_val[32];
bool Default_Start_bit=FALSE;
extern double Totaliser_Update ,Totaliser1_Update,Totaliser2_Update;
extern double Totaliser1,Totaliser2;
//extern unsigned int Belt_Speed,Inv_Op_Freq,Inv1_Op_Freq;
extern unsigned char Calib_status;
extern unsigned int Totaliser1_Display,
             Totaliser2_Display,
             Totaliser1_Display_Old,
             Totaliser2_Display_Old;
extern unsigned long Scan_Count_Min_FD;
extern unsigned int External_Setpoint_display,
             External_Setpoint,
             Common_Setpoint,
             Current_Setpoint,
             Deviation,
             PF_Drive_Setpoint_Per,
             WF_Drive_Setpoint_per,
             Hopper_val,
             Gate_Postion,
             HOpper_total,
             FDR_Total,
             CM_Difference;

EXTFUNC APPL_AbccHandlerStatusType APPL_HandleAbcc( void );

unsigned short M0_15_Read,
             M16_31_Read,
             M32_39_Read;

unsigned short Deviation_Comm,
             Belt_Load_Comm;
 extern uint32_t    key_ip,VarB,Key_Input,Key_Input_Bak,Key_status,Key_Press;

extern unsigned int External_Setpoint_display,
             External_Setpoint,
             Common_Setpoint,
             Current_Setpoint,
             Deviation,
             PF_Drive_Setpoint_Per,
             WF_Drive_Setpoint_per,
             Hopper_val,
             Gate_Postion,
             HOpper_total,
             FDR_Total,
             CM_Difference;

extern float WF_Drive_Setpoint,PF_Drive_Setpoint,CM_Display;

void read_AO_calib_val(void);
extern int prog_counter;
extern unsigned long Belt_REV_COUNT_ACAL, Belt_Rev_Count_Value_ACAL;
extern unsigned char ACAL_Status,BIN_PF_Start_Statu,CM_Negative;
extern unsigned int HOP_Toatl_Start,HOP_Toatl_Present,Scan_Count_BIN_PF,Hop_Per,Capacity_1;
extern double Delta_S1;
extern unsigned long scan_count;
extern char online_step;
unsigned char adc_sps_backup;
extern char ram_values[RAM_SIZE];
extern uint16_t New_menu_parameters[105];
extern uint32_t totalizer_parameters[2];
extern unsigned long Empty_Signal_Delay_Timer;
extern unsigned long adc_final_val,bsf_adc_final_val,Present_Count;

char unit_string[2][5]={{"kg/h\0"},{"t/h\0"}},unit_string_t1[2][5]={{"kg\0"},{"t\0"}};
char unit_string1[1][5]={{"%\0"}},unit_string2[2][5]={{"Hz\0"},{"A\0"}};
char *unit_string_ptr;
//extern int bytes_written;
extern char Start_Stop_status;
extern plc_instruction_struct *plc_ptr;
extern unsigned char Status_No;
extern unsigned long Present_Count,Count_DL,Span_Count,Count_DL_ch2,Span_Count_ch2,zero_load_ch2,full_load_ch2;
extern char lcd_flag;
extern char Togl_Led_Srt_Stp,Togl_Led_Zero,Togl_Led_Auto,Togl_Led_Man;
extern char plc_std[5][200][6],plc_inst[200][6],Error_No;
extern uint8_t plc_temp[400];
plc_instruction_struct plc_ass_1[200];

/* I024..I039: VFD digital inputs (Invter_ModCom). Default 1 = healthy for NC/emergency DIs.
   I024 only: default 0 (start NO open) so SM 011 does not run before first VFD read.
   Prakesh ladder: AI 027 = M010 (DI4 emg NC) — I027 must not default 0. */
static void Init_Vfd_Plc_Input_Image_Defaults(void)
{
    unsigned int k;
    for (k = 24; k < 40; k++) {
        ram_values[k] = 1;
    }
    ram_values[24] = 0; /* I024/DI1 start NO: open until VFD Modbus valid */
}

/*******************************************************************************************/
unsigned long Count_Limit1,
             Count_Limit2,
             Count_Max, Count_Min,
             Scan_Count_Load_Monitor,
             Scan_Count_Min_FD,
             Scan_Count_Limit1,
             Scan_Count_PF_Monitor,
             Scan_Count_Limit2,
             Scan_Count_MinLimit,
             Scan_Count_MaxLimit,
             scan_count_belt_track,
             scan_count_belt_track_monitor,
             scan_count_deviation_monitor,
             scan_count_deviation,
             scan_count_Tacho_monitor,
             scan_count_Tacho,
             scan_count_PF,
             scan_count_PF_Monitor,
             Scan_Count_BIN_PF_Monitor,
             Scan_Count_BIN_PF_Monitor_Reg,
             Scan_Count_Min_Hop,
             Scan_Count_Max_Hop,
             Scan_Count_Nom_Hop,
             Count_Min_Hop,
             Count_Max_Hop,
             Count_Nom_Hop;

unsigned long Count_BeltLoad,Count_Hop_Load;
/*******************************************************************************************/
extern unsigned long Load_Percent,zero_load,full_load;
extern unsigned int Totaliser1_Display, Totaliser2_Display, Hopper_val,
             Gate_Postion,
             HOpper_total,
             FDR_Total,
             CM_Difference;
extern unsigned char Mode_Status;
extern char Auto_Man_status; // , DInt_Int_status;
extern char display_cursor,display_line1;
extern unsigned char tx_over,Rx_byte_count, DIO_status,Invter_Status,Invter2_Status;
extern unsigned char read_ok,write_ok,read_ok_,write_ok_;
extern unsigned char INV_read_ok,INV_write_ok,INV2_read_ok,INV2_write_ok;
extern uint16_t Rx_Err_Counter;
extern char OLED_RAM[128][64];
uint16_t  temp_val_;
const char *IT_LOGO  = "AAG Ultron+";
const char *IT_LOGO1 = "WFMOD 1.7a";
unsigned char Parameter_checkbit[8];
extern unsigned char Dev_ID;
extern const main_menu_struct Main_menu1[Main_Menu_Max];
extern sub_menu_struct sub_menu1[12][26];
unsigned char no_menu_par[12]= {18,24,8,12,0,0,0,0,12,2,0,0};//Add total no of Parmeters used in Menus = {System parameter nos,Configuration Parameter nos,Hopper parameter nos....}
char no_of_menus = 12;//Add Total no of Menus
extern uint16_t Online_menu_parameters[15];
uint16_t OLD_IP_adrs1,OLD_IP_adrs2,OLD_IP_adrs3,OLD_IP_adrs4,OLD_Profibus_adrs,OLD_Profibus_Baud;
extern unsigned char Zero_Tacho_Feed_Buffer_Flag ;
extern uint16_t  Min_adc1_val,Min_adc2_val,Min_adc3_val,Max_adc1_val,Max_adc2_val,Max_adc3_val;

//-----------------------------------------------
//-----------------------------------------------
void Load_Wf_defaults(void)
{
    Prefeeder = 0;
    Units_Capacity = 1;
    Dec_Point_Capacity = 1;
    Capacity = 5000;
    Dec_Pt_Totaliser1 = 1;
    Units_Totaliser1 = 1;
    Dec_Pt_Totaliser2 = 1;
    Units_Totaliser2 = 1;
    Tacho_Freq = 1000;

    Max_Load = 1100;
    Min_Load = 50;
    Password = 0;

    Rev_Time = 1200;
    Zero_Limit = 500;
    Belt_Track = 100;
    Run_Time = 500;

    Pulse1 = 0;
    Pulse_Width = 3;
    Pulse2 = 0;

    AO1_Value = 3;
    AO2_Value = 3;
    Load_Limit1 = 2000;
    Load_Limit2 = 2000;
    Load_Delay = 100;
    Clear_Totaliser2 = 1;
    Load_Filter = 0;
    Mode_Select = 0;

    DIO_Module = 0;
    SIO_Interface = 0;
    SIO_Baud_Rate = 0;
    SIO_Setup = 0;
    SIO_Address = 1;
    SIO_Monitor = 0;

    IP_Addr1 = 192;
    IP_Addr2 = 168;
    IP_Addr3 = 1;
    IP_Addr4 = 100;

    Profibus_Node_Addr = 1;
    Profibus_Baudrate = 6;

    Int_Time = 1800;
    Prop_Gain = 700;
    Dev_Limit = 50;
    Dev_Time = 100;
    Dead_Band = 0;

    Setpoint_Int = 3000;
    Setpoint_Man = 5000;

    BL_Limit = 0;
    PF_Delay = 0;
    PF_StartSet = 0;

    PF_Limit = 0;
    Regulation = 0;
    Reg_Tracking = 0;
    Tacho_ip = 0;
    Fixed_Frequency = 1;

    AD_Conversion_Rate = 3;
    Filter_Order = 0;
    Cut_Off_Freq = 200;
    LC_Filter = 2;
    Display_Filter = 0;

    Inv = 0;
    Inv_1 = 0;

    AO3_Value = 0;
    AI_Nos = 0;
    AO_Nos = 0;

    Pre_Hopper = 0;
    PI = 0;

    BL_Band = 0;

    Hopper_Capacity = 1000;
    No_Rev = 2;
    Filter__Order = 3;
    Cut__Off_Freq = 100;
    LC__Filter = 2;
    Limit_Low = 1000;
    Limit_Nominal = 2000;
    Limit_Max = 1000;
    Limit_Delay = 20;
    Min_Pulse = 10;
    Correction_Factor = 10000;

    Totaliser1_Display = 0;
    Totaliser1 = 0;
    Totaliser1_Update = 0;
    RTC_WriteGPREG(LPC_RTC, 0, Totaliser1);

    Totaliser2_Display = 0;
    Totaliser2 = 0;
    Totaliser2_Update = 0;
    RTC_WriteGPREG(LPC_RTC, 1, Totaliser2);

    AO_calz1 = 750;
    AO_cals1 = 3750;
    AO_calz2 = 750;
    AO_cals2 = 3750;
    AO_calz3 = 750;
    AO_cals3 = 3750;

    AI_calb_val[0] = Min_adc1_val = 750;
    AI_calb_val[1] = Max_adc1_val = 3750;

    AI_calb_val[2] = Min_adc2_val = 750;
    AI_calb_val[3] = Max_adc2_val = 3750;

    AI_calb_val[4] = Min_adc3_val = 750;
    AI_calb_val[5] = Max_adc3_val = 3750;

    zero_load = zero_count_cal = 50000;
    calib_val[0] = full_load = 350000;
    calib_val[1] = Span_Count = 300000;
    calib_val[2] = (unsigned long)zero_count_cal;
    calib_val[6] = BL_avg = 350000;
    calib_val[7] = Calib_Load = 1000;
}

//-----------------------------------------------
void check_defaults(void)
{
    /* Called after loading defaults and saving to EEPROM; add any post-default init here. */
}

//-----------------------------------------------
void Parameter_init_checkbit(void)
{
    eeprom_read_block(4000, (unsigned char*)Parameter_checkbit, 1);
    super_delay(40);
    eeprom_read_block(4000, (unsigned char*)Parameter_checkbit, 1);

    if((Parameter_checkbit[0] != 0xCD) || (Default_Start_bit == TRUE))
    {
        Clear_LCD();
        cstrptr = "Loading...     ";
        Print_String(&cstrptr, line3, 0, font16, 0);
        cstrptr = "DEFAULT ..   ";
        Print_String(&cstrptr, line4, 0, font16, 0);
        Load_Wf_defaults();
        Parameter_checkbit[0] = 0xCD;
        eeprom_write_array(4000, (unsigned char*)Parameter_checkbit, 1);
        super_delay(30);
        eeprom_write_array(0, (unsigned char*)New_menu_parameters, 324);
        super_delay(30);
        save_AI_calib_val();
        super_delay(30);
        save_AO_calib_val();
        super_delay(40);
        save_calib_val();
        super_delay(40);
        save_par_eeprom();
        super_delay(30);
        check_defaults();
        super_delay(30);
        Default_Start_bit = FALSE;
        super_delay(40);
    }
}

//void menu_details()
//{
//    char i_ptr,i_ptr1,no_of_menu_par = 0;
//
//        for(i_ptr=0;(Main_menu1[i_ptr].mm_type != 'F');i_ptr++)
//            {
//                no_of_menus++;
//            }
//            no_of_menus = no_of_menus - 2;
//
//            for(i_ptr = 0;(i_ptr <= no_of_menus);i_ptr++)
//                {
//                    for(i_ptr1=0;(sub_menu1[i_ptr][i_ptr1].par_type != 'F');i_ptr1++)
//                        {
//                            no_of_menu_par++;
//                        }
//                            if(no_of_menu_par==0)
//                                no_of_menu_par=1;
//                            no_menu_par[i_ptr]= no_of_menu_par - 1;
//                            no_of_menu_par = 0;
//
//                }
//}

//-----------------------------------------------------------
void check_key_input_default(void)
{
    static int defstage = 0;
    Default_Start_bit = FALSE;
    while(defstage < 5)
    {
        int i;
        for(i = 0; i < 50000; i++)
            Key_Input = NO_KEY;
        key_check_routine();
        key_check_routine();
        switch(defstage)
        {
            case 0:
                if(Key_Input == HOR)
                {
                    cstrptr = "              ";
                    Print_String(&cstrptr, line1, 0, font16, 0);
                    cstrptr = "DEF 1         ";
                    Print_String(&cstrptr, line1, 0, font16, 0);
                    defstage = 1;
                }
                else
                {
                    Default_Start_bit = FALSE;
                    defstage = 5;
                }
                break;
            case 1:
                if(Key_Input == K_INC)
                {
                    cstrptr = "              ";
                    Print_String(&cstrptr, line1, 0, font16, 0);
                    cstrptr = "DEF 2         ";
                    Print_String(&cstrptr, line2, 0, font16, 0);
                    defstage = 2;
                }
                else
                    defstage = 1;
                break;
            case 2:
                if(Key_Input == K_DEC)
                {
                    cstrptr = "              ";
                    Print_String(&cstrptr, line1, 0, font16, 0);
                    cstrptr = "DEF 3         ";
                    Print_String(&cstrptr, line3, 0, font16, 0);
                    cstrptr = "LOAD DEFAULT    ";
                    Print_String(&cstrptr, line4, 0, font16, 0);
                    Default_Start_bit = TRUE;
                    defstage = 5;
                }
                else
                    defstage = 2;
                break;
            default:
                defstage = 5;
                break;
        }
    }
}
//-----------------------------------------------------------
void main_initialize_routine(void)
{
    int i;
    int i1, j1;

    LPC_SC->PCONP |= (1 << 15);
    LPC_SC->PCONP |= (1 << 24);

    
    GPIO_SetDir(1, 20, 1);
    GPIO_SetDir(1, 22, 1);
    GPIO_SetDir(1, 23, 1);
    GPIO_SetDir(1, 24, 1);
    GPIO_SetDir(1, 25, 1);
    GPIO_SetDir(1, 26, 1);
    GPIO_SetDir(0, 10, 1);
    GPIO_SetDir(0, 26, 1);
    DO0_Init();
    Digital_Output_Routine();

    GPIO_SetDir(1, 28, 0);
    GPIO_SetDir(1, 29, 0);
    GPIO_SetDir(0, 28, 0);
    GPIO_SetDir(1, 31, 0);
    GPIO_SetDir(1, 21, 0);
    GPIO_SetDir(1, 27, 0);
    PINSEL_Configure_IO();
    GPIO_SetDir(0, 4, 0);
    GPIO_SetDir(0, 5, 0);
    GPIO_SetDir(0, 19, 0);
    GPIO_SetDir(0, 20, 0);
    GPIO_SetDir(0, 21, 0);
    GPIO_SetDir(0, 23, 0);
    GPIO_SetDir(0, 24, 0);
    GPIO_SetDir(0, 27, 0);

    GPIO_SetDir(0, 0, 1);
    GPIO_SetDir(0, 1, 0);

    for(i1 = 0; i1 < 64; i1++)
        for(j1 = 0; j1 < 128; j1++)
            OLED_RAM[i1][j1] = 0;
    OLED_12864_Init();
    Clear_LCD();

    Print_String(&IT_LOGO, line2, 10, font16, 0);
    Print_String(&IT_LOGO1, line3, 10, font16, 0);
    super_delay(80);

    keypad_init();

    GPIO_SetDir(eeprom_cs_port, eeprom_cs_pin, 1);
    eeprom_cs(1);

    GPIO_SetDir(adc_cs_port, adc_cs_pin, 1);
    adc_cs(1);

    GPIO_SetDir(dac_1_cs_port, dac_1_cs_pin, 1);
    dac1_cs(1);

    GPIO_SetDir(dac_2_cs_port, dac_2_cs_pin, 1);
    dac2_cs(1);

    GPIO_SetDir(dac_3_cs_port, dac_3_cs_pin, 1);
    dac3_cs(1);

    GPIO_SetDir(adc_ltc1288_cs_port, adc_ltc1288_cs_pin, 1);
    adc_ltc1288_cs(1);

    GPIO_SetDir(adc1_DOUT_port, adc1_DOUT_pin, 0);
    GPIO_SetDir(adc1_ltc1288_cs_port, adc1_ltc1288_cs_pin, 1);
    adc1_ltc1288_cs(1);

    GPIO_SetDir(adc2_DOUT_port, adc2_DOUT_pin, 0);
    GPIO_SetDir(adc2_ltc1288_cs_port, adc1_ltc1288_cs_pin, 1);
    adc2_ltc1288_cs(1);

    GPIO_SetDir(Ram__cs_port, Ram__cs_pin, 1);
    ram_cs(1);
    SPI_Initialize();

    check_key_input_default();
    Parameter_init_checkbit();

    read_par_eeprom();
    ((uint16_t*)&(Capacity_1))[1] = New_menu_parameters[52];
    ((uint16_t*)&(Capacity_1))[0] = New_menu_parameters[84];
    super_delay(30);
    read_AO_calib_val();
    read_AI_calib_val();
    read_calib_val();
    Status_No = 0;
    //  Mode_Status = Manual;
    //DInt_Int_status = De_Interlock;
    //Auto_Man_status = STOP;
    Start_Stop_status = STOP;

//-------------------------------------New Control Logic-------------------------------------
    /*if(Mode_Select == Int)
    {
        Operating_Mode=DeInt_Vol_Dio;// Internal means panel mode, volumetric
        M053 = 0;//: Interlock Status (1=Interlocked,0=De-Interlocked)*(verified correct)
        M015 = 1;//: Int/De-Interlock Mode Select (1-De-Interlock, 0-Int)*(verified correct)
        M052 = 0;//: Gravimetric/vol Mode Status (1=Gravimetric,0=Volumetric)*(verified correct)
        M051 = 0;//: Local Mode Status (1=Local,0=Remote)*(verified correct)
    }
    else if(Mode_Select == Ext)
    {
        Operating_Mode=Int_Gra_Dio;// External means PLC/DCS mode, gravimetric
        M053 = 1;//: Interlock Status (1=Interlocked,0=De-Interlocked)*(verified correct)
        M015 = 0;//: Int/De-Interlock Mode Select (1-De-Interlock, 0-Int)*(verified correct)
        M052 = 1;//: Gravimetric/vol Mode Status (1=Gravimetric,0=Volumetric)*(verified correct)
        M051 = 0;//: Local Mode Status (1=Local,0=Remote)*(verified correct)
    }*/
//-------------------------------------
    Zero_Tacho_Feed_Buffer_Flag = 1;
    filter_para_calc();
    Span_Count = full_load - zero_load;
    Count_Limit1 = ((Span_Count) * (float)Load_Limit1 / 1000) + zero_load;
    Count_Limit2 = ((Span_Count) * (float)Load_Limit2 / 1000) + zero_load;
    Count_Max = ((Span_Count) * (float)Max_Load / 1000) + zero_load;
    Count_Min = (((Span_Count) * (float)Min_Load / 1000) + zero_load);

    Scan_Count_Load_Monitor = (float)(Load_Delay * 100) / (333);
    Scan_Count_Limit1 = Scan_Count_Load_Monitor;
    Scan_Count_MaxLimit = Scan_Count_Load_Monitor;
    Scan_Count_MinLimit = Scan_Count_Load_Monitor;
    Scan_Count_Min_FD = Scan_Count_Load_Monitor;
    scan_count_PF_Monitor = (float)(PF_Delay * 100) / (333);
    scan_count_PF = scan_count_PF_Monitor;

    Scan_Count_BIN_PF_Monitor = (float)(Limit_Delay * 100) / (10000);
    Scan_Count_BIN_PF = Scan_Count_BIN_PF_Monitor;
    Count_Min_Hop = ((Span_Count_ch2) * (float)Limit_Low / 1000) + zero_load_ch2;
    Count_Max_Hop = ((Span_Count_ch2) * (float)Limit_Max / 1000) + zero_load_ch2;
    Count_Nom_Hop = (((Span_Count_ch2) * (float)Limit_Nominal / 1000) + zero_load_ch2);

    Scan_Count_BIN_PF_Monitor_Reg = (float)(Limit_Delay * 100) / (333);
    Scan_Count_Min_Hop = Scan_Count_BIN_PF_Monitor_Reg;
    Scan_Count_Max_Hop = Scan_Count_BIN_PF_Monitor_Reg;
    Scan_Count_Nom_Hop = Scan_Count_BIN_PF_Monitor_Reg;

    M019 = 1; // WF Drive Fault  if M019 = 0
    M020 = 1; // PF Drive Fault  if M020 = 0    
    M003 = 0; // Clear Totaliser1 Clear Marker  
    M010 = 1; // Emergency Fault if M010 = 0
    M044 = 0; // General Fault
    M040 = 0; // Tacho Fault
    
    M056 = 0;
    M057 = 0;
    M058 = 0;
    M059 = 0;
    M060 = 0;
    M041 = 0;
    M044 = 0;
    M010 = 1;
    M019 = 1;
    M015 = 0;
    M080 = 0;
    M081 = 0;
    M082 = 0;
    M043 = 0;
    M040 = 0;
    M046 = 0;
    M042 = 0;       //msk marker reversal 21-mar-2026   
    M044 = 0;       //msk marker reversal 21-mar-2026
    M045 = 0;       //msk marker reversal 21-mar-2026
    M046 = 0;       //msk marker reversal 21-mar-2026
    M047 = 0;       //msk marker reversal 21-mar-2026
    M048 = 0;       //msk marker reversal 21-mar-2026
    M049 = 0;       //msk marker reversal 21-mar-2026
    M050 = 0;       //msk marker reversal 21-mar-2026

    Init_Totaliser();
    Tacho_init();

    scan_count_belt_track = scan_count_belt_track_monitor;
    scan_count_deviation_monitor = ((float)(Dev_Time * 1000) / 3333);
    scan_count_deviation = scan_count_deviation_monitor;

    scan_count_Tacho_monitor = ((float)(Run_Time * 1000) / 3333);
    scan_count_Tacho = scan_count_Tacho_monitor;

    DIO_status = 1;
    read_ok = 0;
    write_ok = 1;
    Invter_Status = 1;
    INV_read_ok = 0;
    INV_write_ok = 1;
    Invter2_Status = 1;
    INV2_read_ok = 0;
    INV2_write_ok = 1;
    Dev_ID = 1;
    lcd_flag = 0;
    adc_sps_backup = AD_Conversion_Rate;

    Togl_Led_Srt_Stp = 0;
    Togl_Led_Zero = 0;
    Togl_Led_Auto = 0;
    Togl_Led_Man = 0;

    read_ass_eeprom();
    for(i = 0; plc_temp[i * 2] != 255 && i < 200; i++)
    {
        plc_ass_1[i].inst = plc_temp[i * 2];
        plc_ass_1[i].addr = plc_temp[(i * 2) + 1];
    }
    plc_ass_1[i].inst = 255;
    plc_ass_1[i].addr = 255;
    plc_ptr = plc_ass_1;
//-------------------msk 19-mar-2026---------------------------------
Operating_Mode = Int_Gra_Dio;   //Interlock Gravimetric Mode
M052 = 1; // M052 set for gravimetric  mode
M053 = 1;//Interlock Status (1=Interlocked,0=De-Interlocked)

Start_Stop_status = STOP;
M066 = 0;//: WF-Start/Stop (1=start,0=stop)*(verified correct)
M002 = 0;//: Remote (modbus command) (1=remote on,0=remote off)*(verified correct)
M068 = 1;//: System Healthy (1=Healthy,0=Fault) not used anywhere in sw
M011 = 0;//: Local start WF(1-start)*(verified correct)
M012 = 1;//: Local stop WF(1-stop)*(verified correct)
M002 = 0;//: Remote (modbus command) (1=remote on,0=remote off)*(verified correct)
M017 = 1;//: Downstream interlock (1=healthy); avoid false fault from BSS 0 before VFD DI valid
//-------------------------------------
    read_plc_edit_eeprom();

    Init_Vfd_Plc_Input_Image_Defaults();
    ResetVfdDiInputFilter();

    Temp_Intialisation();
    serial_init();
    DIO_serial_init();
    rtc_config();
    Counter1_init();
    init_adc_cs5532();
    for(temp_val_ = 0; temp_val_ <= 100; temp_val_++)
    {
        read_adc_conversion();
        super_delay(25);
    }
    BL_init_calculation();
    feedrate_display_Init_calculation();
    Timer0_init();

    Online_menu_parameters[0] = New_menu_parameters[34];
    Online_menu_parameters[1] = New_menu_parameters[35];
    Online_menu_parameters[2] = New_menu_parameters[36];
    Online_menu_parameters[3] = New_menu_parameters[37];
    Online_menu_parameters[4] = New_menu_parameters[38];
    Online_menu_parameters[5] = New_menu_parameters[82];
    Online_menu_parameters[6] = New_menu_parameters[55];
    Online_menu_parameters[7] = New_menu_parameters[56];
    Online_menu_parameters[8] = New_menu_parameters[49];
    Online_menu_parameters[9] = New_menu_parameters[50];
    Online_menu_parameters[10] = New_menu_parameters[51];
    Online_menu_parameters[11] = New_menu_parameters[54];
    Online_menu_parameters[12] = New_menu_parameters[83];
    Online_menu_parameters[13] = New_menu_parameters[39];
    Online_menu_parameters[14] = New_menu_parameters[40];

    OLD_IP_adrs1 = IP_Addr1;
    OLD_IP_adrs2 = IP_Addr2;
    OLD_IP_adrs3 = IP_Addr3;
    OLD_IP_adrs4 = IP_Addr4;

    OLD_Profibus_adrs = Profibus_Node_Addr;
    OLD_Profibus_Baud = Profibus_Baudrate;

    Calib_status = 0;
}

void after_menu_initialize(void)
{
    char i;

    CM_Negative = 0;
    ACAL_Status = 0;
    CM_Difference = 0;
    FDR_Total = 0;
    HOpper_total = 0;
    Delta_S1 = 0;
    HOP_Toatl_Start = 0;
    M076 = 0;
    M078 = 0;

    //Mode_Status = Manual;
    Operating_Mode = DeInt_Vol_Dio;
    //DInt_Int_status = De_Interlock;
    //Auto_Man_status = STOP;
    Start_Stop_status = STOP;
    Zero_Tacho_Feed_Buffer_Flag = 1;
    display_line1 = 1;
    eeprom_write_array(0, (unsigned char *)New_menu_parameters, 324);
    Status_No = 0;
    filter_para_calc();
    read_calib_val();
    serial_init();
    super_delay(30);

    Span_Count = full_load - zero_load;
    Count_Limit1 = ((Span_Count) * (float)Load_Limit1 / 1000) + zero_load;
    Count_Limit2 = ((Span_Count) * (float)Load_Limit2 / 1000) + zero_load;
    Count_Max = ((Span_Count) * (float)Max_Load / 1000) + zero_load;
    Count_Min = (((Span_Count) * (float)Min_Load / 1000) + zero_load);

    Scan_Count_Load_Monitor = (float)(Load_Delay * 100) / (333);
    Scan_Count_Limit1 = Scan_Count_Load_Monitor;
    Scan_Count_MaxLimit = Scan_Count_Load_Monitor;
    Scan_Count_MinLimit = Scan_Count_Load_Monitor;
    scan_count_PF_Monitor = (PF_Delay * 100) / (333);
    scan_count_PF = scan_count_PF_Monitor;

    scan_count_belt_track_monitor = (float)(Belt_Track * 1000) / (3333);
    scan_count_belt_track = scan_count_belt_track_monitor;

    Init_Totaliser();
    Tacho_init();

    Scan_Count_BIN_PF_Monitor = (float)(Limit_Delay * 100) / (10000);
    Scan_Count_BIN_PF = Scan_Count_BIN_PF_Monitor;
    Count_Min_Hop = ((Span_Count_ch2) * (float)Limit_Low / 1000) + zero_load_ch2;
    Count_Max_Hop = ((Span_Count_ch2) * (float)Limit_Max / 1000) + zero_load_ch2;
    Count_Nom_Hop = (((Span_Count_ch2) * (float)Limit_Nominal / 1000) + zero_load_ch2);
    Scan_Count_BIN_PF_Monitor_Reg = (float)(Limit_Delay * 100) / (333);
    Scan_Count_Min_Hop = Scan_Count_BIN_PF_Monitor_Reg;
    Scan_Count_Max_Hop = Scan_Count_BIN_PF_Monitor_Reg;
    Scan_Count_Nom_Hop = Scan_Count_BIN_PF_Monitor_Reg;

    scan_count_deviation_monitor = ((float)(Dev_Time * 1000) / 3333);
    scan_count_deviation = scan_count_deviation_monitor;

    scan_count_Tacho_monitor = ((float)(Run_Time * 1000) / 3333);
    scan_count_Tacho = scan_count_Tacho_monitor;
    BL_init_calculation();
    feedrate_display_Init_calculation();
    init_adc_cs5532();

    Togl_Led_Srt_Stp = 0;
    Togl_Led_Zero = 0;
    Togl_Led_Auto = 0;
    Togl_Led_Man = 0;

    scan_count = 0;

    read_ass_eeprom();
    for(i = 0; plc_temp[i * 2] != 255 && i < 200; i++)
    {
        plc_ass_1[i].inst = plc_temp[i * 2];
        plc_ass_1[i].addr = plc_temp[(i * 2) + 1];
    }
    plc_ass_1[i].inst = 255;
    plc_ass_1[i].addr = 255;
    plc_ptr = plc_ass_1;

    Clear_LCD_RAM();

    Online_menu_parameters[0] = New_menu_parameters[34];
    Online_menu_parameters[1] = New_menu_parameters[35];
    Online_menu_parameters[2] = New_menu_parameters[36];
    Online_menu_parameters[3] = New_menu_parameters[37];
    Online_menu_parameters[4] = New_menu_parameters[38];
    Online_menu_parameters[5] = New_menu_parameters[82];
    Online_menu_parameters[6] = New_menu_parameters[55];
    Online_menu_parameters[7] = New_menu_parameters[56];
    Online_menu_parameters[8] = New_menu_parameters[49];
    Online_menu_parameters[9] = New_menu_parameters[50];
    Online_menu_parameters[10] = New_menu_parameters[51];
    Online_menu_parameters[11] = New_menu_parameters[54];
    Online_menu_parameters[12] = New_menu_parameters[83];
    Online_menu_parameters[13] = New_menu_parameters[39];
    Online_menu_parameters[14] = New_menu_parameters[40];
    serial_init();
    if(SIO_Interface == 2)
    {
        if((OLD_IP_adrs1!=IP_Addr1)||(OLD_IP_adrs2!=IP_Addr2)||(OLD_IP_adrs3!=IP_Addr3)||(OLD_IP_adrs4!=IP_Addr4))
        {
            if((IP_Addr1 < 224) && (IP_Addr2 < 256) && (IP_Addr3 < 256) && (IP_Addr4 < 255))
            {
                if(OLD_IP_adrs1 != IP_Addr1)
                    APPL_SetAddress(IP_Addr1);
                else if(OLD_IP_adrs2 != IP_Addr2)
                    APPL_SetAddress(IP_Addr2);
                else if(OLD_IP_adrs3 != IP_Addr3)
                    APPL_SetAddress(IP_Addr3);
                else if(OLD_IP_adrs4 != IP_Addr4)
                    APPL_SetAddress(IP_Addr4);
                APPL_RestartAbcc();
            }
        }
    }

    if(SIO_Interface == 3)
    {
        if(OLD_Profibus_adrs != Profibus_Node_Addr)
        {
            if((Profibus_Node_Addr < 100) && (Profibus_Node_Addr > 0))
            {
                APPL_SetAddress(Profibus_Node_Addr);
                APPL_RestartAbcc();
            }
        }
        if(OLD_Profibus_Baud != Profibus_Baudrate)
        {
            APPL_SetBaudrate(Profibus_Baudrate);
            APPL_RestartAbcc();
        }
    }

//    Hopper_val=500;
//Gate_Postion=200;
//HOpper_total=300;
//FDR_Total=600;
//CM_Difference=700;
//CM_Display=100;
//PF_Drive_Setpoint_Per=450;

    M010 = 1;
    M017 = 1;
    M019 = 1;
    M020 = 1;
    Init_Vfd_Plc_Input_Image_Defaults();
    ResetVfdDiInputFilter();
}

/*
06/02/2018
Span Count calculation removed from after menu initialization and main intialization routine as its saved in eeprom and called while power up so no need to recalculate

07/02/2018
((uint16_t*)&(Capacity_1))[1] = New_menu_parameters[52] ;    //Capacity higher 2 bytes
((uint16_t*)&(Capacity_1))[0] = New_menu_parameters[84] ;    //Capacity lower 2 bytes
As capacity is 5 digit so cant be saved in single short int New_Mwnu_Parameters[];limit of short is 65536
so used unsigned int Capacity_1

19/02/2016
Span Count calculation added again after menu initialization and main intialization routine as its saved in eeprom and called while power up so no need to recalculate

23/02/2018
Limit Counts related to Hopper added and used in Display Error Function For Markers ;
Count_Hop_Min
Count_Hop_Max
Count_Hop_Nom
Scan_Count_Hop_Min
Scan_Count_Hop_Max
Scan_Count_Hop_Nom
Scan_Count_Bin_PF Used for Autocalibration
Scan_Count_Bin_PF_Reg used for Regular Limit Markers ;M080-M082
Hop_Levl & Gate_Postn added to all Analog o/p options .
Software version Changed To 1.5.

07/04/2018
Software version Changed To 1.6.
Main thread Stack size incresed from 512 to 800 bytes in RTX_conf_CM.c because it get stuck(stack overflow error observed in debugger mode) if enter into usb after entering  PLC menu  & then enter into Sync function.

30/05/2018
Software version Changed To 1.7
Added Tacho ip parameter in system Parameters.
This parameter will decide source of freq ip to controller;
1st is Tacho sensor & 2nd is Inverter op freq
no_menu_par[],No of parameters inside System parameters is increased by 1 so it becomes 18 from 17
08/08/2018
Software version Changed To 1.8
changed to io control .c

16/08/2018:
unsigned char no_menu_par[12]= {17,23,8,12(changed to 12 from 5),0,0,0,0,12,2,0,0};

New parameters in communication menu:
For Modbus TCP:
                IP_Addr1=192;
                IP_Addr2=168;
                IP_Addr3=1;
                IP_Addr4=100;
For Profibus DP:
                Profibus_Node_Addr = 1;
                Profibus_Baudrate = 6;

uint16_t New_menu_parameters[91(Changed to 91 from 85)];    //changed to 105

28/08/2018
Software version Changed To 1.9
changed to io INV.c

29/09/2018
Software version Changed To 1.A
changed to io INV.c (AYV312)

29/11/2018
Software version Changed To 1.B
changed to io INV.c (Yaskawa-J1000)

11/10/2019
Software version Changed To 1.D
changed to io INV.c & menu.C

Drives Added inside Inv option (Communication)

17-04-2022 by Prince
Software version Changed To 1.F
Bug: all Digital Output turned ON during Initialization for 2seconds
Solution: "Init OLED Display" shifted below "Init Digital Input"
Result: During Initialization All Digital Output turned OFF

*/
