/* Online.c     */
#include "LPC17xx.h"                    // Device header
#include "lpc_types.h"
#include "lpc17xx_rtc.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "main_routine.h"
#include "OLED_2_7_12864.h"
#include "keys.h"
#include "parameters.h"    //#include "menu.h"

#include "plc.h"
#include "functions.h"
#include "Errors.h"
/**************************/
#define I00 11  //at this location name is saved in array
#define I01 12  //at this location name is saved in array
#define I02 13  //at this location name is saved in array
#define I03 14  //at this location name is saved in array

#define O00 15  //at this location name is saved in array
#define O01 16  //at this location name is saved in array
#define O02 17  //at this location name is saved in array
#define O03 18  //at this location name is saved in array

#define T00 19  //at this location name is saved in array

#define M00 20  //at this location name is saved in array
#define M01 21  //at this location name is saved in array
#define M02 22  //at this location name is saved in array
#define M03 23  //at this location name is saved in array
#define M04 24  //at this location name is saved in array
#define M05 25  //at this location name is saved in array
#define M06 26  //at this location name is saved in array
#define M07 27  //at this location name is saved in array
#define M08 28  //at this location name is saved in array
#define M09 29  //at this location name is saved in array
/**************************/

void online_display_calculation(void);
void Print_Online_Menu_values(char);
void Calculate_Setpoint_Display(void);

extern char io_status;

char *start_stop_str[2][7]={{"Stop\0"},{"Start\0"}};
char *sign_str[3][1]={{" "},{"-"},{"+"}};
char *Mode_str[4][3]={{"Int"},{"Ext"},{"Man"},{"Rem"}}; // {{"Int"},{"DeI"},{"Man"},{"Auto"}};/*Temp Testing only*/
extern unsigned char Mode_Status,ACAL_Status;
char Mode_slection;
//char Mode=3;  ///temp//
char *temp_val[1][6] = {{"9999+"}};

extern uint16_t scan_count;
extern char lcd_flag;
extern  char *unit_string[3][3];
extern long adc_final_val,Present_Count;
extern char *clear_string;// ="                             ";

extern uint16_t New_menu_parameters[105];

extern unsigned long Scale_Weight_Net,Scale_Weight_Gross;
extern char ram_values[RAM_SIZE];
unsigned char Display_Sign=0,Deviation_Sign,Hop_Load_Sign;
char Start_Stop_status;
char Net_Gross_status;
char Status_No,Error_No;
char *temp_status_ptr;
const char *msg_ptr;
char online_step,online_menu_step;
extern uint32_t Key_Input;
/************************************/
char i5=20,increment=3,Counter1=0,Counter2=1,Counter3=2;
unsigned int  Actual_Feedrate;
extern unsigned int Display_Feedrate;
double Totaliser1,Totaliser2;
extern unsigned int Belt_Speed,Inv_Op_Freq,Inv1_Op_Freq,HOpper_total; //,FDR_Total_try,HOpper_total_try;

unsigned int Totaliser1_Display,
             Totaliser2_Display,
             Totaliser1_Display_Old,
             Totaliser2_Display_Old;

unsigned int External_Setpoint_display,
             External_Setpoint,
             Common_Setpoint,
             Current_Setpoint,
             Deviation,
             PF_Drive_Setpoint_Per,
             WF_Drive_Setpoint_per,
             Hopper_val,
             Gate_Postion,
             HOpper_total,  //HOpper_total_try,
             FDR_Total, FDR_Total_try,
             CM_Difference;

float WF_Drive_Setpoint,PF_Drive_Setpoint,CM_Display;

signed int Belt_Load;/*Actual_Feedrate is Actual feedrate*/

extern unsigned long Count_DL,Span_Count,zero_load;
extern unsigned long Present_Count_ch2,Count_DL_ch2,Span_Count_ch2,zero_load,zero_load_ch2,full_load_ch2;
/***********************************/
extern RTC_TIME_Type rtc_read;

extern unsigned long Count_Limit1, Count_Limit2,
                    Count_Max, Count_Min, Scan_Count_Load_Monitor,
                    Scan_Count_Limit1, Scan_Count_Limit2,
                    Scan_Count_MinLimit, Scan_Count_MaxLimit,
                    scan_count_belt_track, scan_count_belt_track_monitor;

extern unsigned long Count_BeltLoad;
extern unsigned int Count_Tacho,Capacity_1;
extern unsigned char Tacho_Pointer,Tacho_Buffer;

extern unsigned long bsf_adc_final_val;
extern float Tacho_Avg;
extern unsigned int Tacho_Sum;
char display_parametr_line1,display_parametr_line2,display_parametr_line3,display_parametr_line4,display_parameter_line;
char line_display,line1_display,line2_display,line3_display,line4_display;

extern unsigned char ZERO_STATUS;
extern char Check_Start ;
extern int temp_count1,temp_count2;

/*  Online Menu Display variables */
extern char *print_menu;
extern char Oled_flag;
extern main_menu_struct Online_Main_menu[Main_Menu_Max];
extern sub_menu_struct Online_menu[5][14];
extern char const Online_menu_options[5][14][10][20];
extern char* P_String;
extern char A_String[200];

extern char total_status;
extern unsigned long BL_sum,BL_reg[500],BL_avg ;
extern unsigned int Tacho_Counter;
extern volatile uint8_t flag;
extern unsigned char Inv_Op_current,Inv1_Op_current;
extern uint16_t Display_Belt_Speed;

//const char *status_msg[10][20] = {
//    {"                    "},
//    {"St:Starting Batch"},
//    {"St:HF ON"},
//    {"St:Ready to Start"},
//    {"St:CF ON"},
//    {"St:FF ON"},
//    {"St:Add Prop"},
//    {"St:Batch End"},
//    {"St:Discharge ON"},
//    {"St:Rdy fr Disch"}
//};

extern const char *error_msg[18][20],*ACAL_Display[3][16];
unsigned char IO_database_offline[11],IO_Ptr_offline=11,IO_Ptr1_offline,IO_Ptr2_offline,IO_Stat_offline;

unsigned char IO_Disp_L1_offline = 0,IO_Disp_L2_offline =10 ,IO_Disp_L3_offline = 20,IO_Disp_L4_offline = 30,IO_Disp_L5_offline= 40;
unsigned char IO_Par_L1_offline = 11 ,IO_Par_L2_offline = 12 , IO_Par_L3_offline = 13 ,IO_Par_L4_offline = 14 ,IO_Par_L5_offline = 15;
char row_no_offline,Disp_Par_Count;

extern const char *io_str_name[4][13];
extern const char *io_str[30][3];
extern char *Ram_add_no[16][1];
extern unsigned char CM_Negative;

void IO_Monitor(void);

/**********************************************************************************
        Online_routine()

|---------------------------|
|Status start/stop            mode|    ->    1st line - Status display - Start/Stop status, Mode status (Int/Ext/Man/Rem)
|para1:        050,01         Kg        |    ->    2nd to 5th lines - Parameters display
|para2:        050,00        Kg        |
|para3:        050,00        Kg        |
|para4:   050,00        Kg      |
|Error                      |    ->  6th line - Status/Alarm text display
|---------------------------|

Algorithm:
    Update Start/Stop status in line1
    Update Net/Gross status in line1
    If status=NET, update Scale_Weight_Net in line2
    If status=GROSS, update Scale_Weight_Gross in line2
    Update Target weight value in line3
    If Error_No !=0, Update Error_display_text as per Error_No in line 4
$    If Error_No =0, Update Status_display_text as per Status_No in line 4

**********************************************************************************/
/***Parameters Displayed in Online Screen
X:0000 t/h     current Feedrate
T1:000000 t    Totaliser1
T2:000000 t    Totaliser2
BL:000.0%      Belt load
Ta:0000 Hz     Tacho Frequency
S_Int:0000 t/h Internal Setpoints
S_Ext:0000 t/h External Setpoints
S_Comn:0000 t/h Commn Setpoint
S_Man:0000 t/h  Manual Setpoints
S_Cur:0000 t/h  current setpoints
Dev:000.0%   Deviation(between setpoint and current Feedrate)
WF_Set:000.0% WF Drive Setpoint
PF_Set:000.0% PF Drive Setpoint
Date:dd/mm/yyyy Date
Time=hh:mm Time
***/
char *online_string[23][10]={
    {"TPH    "},
    {"S1     "},
    {"S2     "},
    {"Load   "},
    {"Tacho  "},
    {"Int_SP "},
    {"Ext_SP "},
    {"Com_SP "},
    {"Man_SP  "},
    {"Cur_SP  "},
    {"Dev              "},
    {"WF_SP "},
    {"PF_SP "},
    {"Hopper"},
    {"Postn "},
    {"HOP_Totl"},
    {"FDR_Totl"},
    {"CM_Diff"},
    {"CM%"},
    {"WFM_Spd"},
    {"WFM_Cur"},
    {"PFM_Spd"},
    {"PFM_Cur"}
};
//----------------------------------------------------
/* Line-1 status text: [START|STOP] [DeInt|Inte|Field|SIO]-[Gra|Vol] [Int|Ext] */
static void Print_Mode_Status_Line1(void)
{
    char mode_line[28];
    const char *run_txt;
    const char *mode_part;
    const char *sel_txt;
    const char *msg_ptr[1];

    run_txt = (Start_Stop_status == START) ? "START" : "STOP";
    sel_txt = (Mode_Select == Int) ? "Int" : "Ext";

    if (GetFieldMode())
        mode_part = "Field";
    else if (GetDeIntGraMode())
        mode_part = "DeInt-Gra";
    else if (GetDeIntVolMode())
        mode_part = "DeInt-Vol";
    else if (GetIntGraMode())
        mode_part = "Inte-Gra";
    else if (GetIntVolMode())
        mode_part = "Inte-Vol";
    else if (GetComMode())
        mode_part = GetGraMode() ? "SIO-Gra" : "SIO-Vol";
    else
    {
        mode_part = "Mode?";
        Operating_Mode = Int_Gra_Dio;   //Interlock Gravimetric Mode
        M052 = 1; // M052 set for gravimetric  mode
        M053 = 1;//Interlock Status (1=Interlocked,0=De-Interlocked)
    }

    sprintf(mode_line, "%s %s %s", run_txt, mode_part, sel_txt);
    msg_ptr[0] = mode_line;
    Print_String2(msg_ptr, line1, 0, font10, 0);
}
//---------------------------------------------------------
void Online_routine()
{
    Mode_slection = Mode_Select ;  //Temp
    //if((Mode_slection == 0)&&(Mode_Status == Manual))
    if((Mode_slection == 0)&&(GetDeIntVolMode()))
        Mode_slection = 2;
    if( (ZERO_STATUS  == 0) && (Check_Start!=20) &&(total_status <1) )  //regular online routine
    {
        if(online_step==0)
        {
            Clear_LCD_RAM_Half_Online();//Clear_LCD_RAM();
            online_step=1;
            if(Start_Stop_status == STOP) //if STOP
                Status_No = 0;

            Print_Mode_Status_Line1(); //msk 22 march 2026  /* START/STOP + mode + Int/Ext in one line */
            Print_String2(online_string[Counter1],line2,0,font10,0);    //diplay parameter line1
            Print_Online_Menu_values(Counter1);
            Print_String2(online_string[Counter2],line3,0,font10,0);    //diplay parameter line2
            Print_Online_Menu_values(Counter2);

            return;
        }
        else if(online_step==1)
        {
            Clear_LCD_RAM_Half();
            online_step=2;

            Print_String2(online_string[Counter3],line4,0,font10,0);    //diplay parameter line3
            Print_Online_Menu_values(Counter3);
            Print_String2(online_string[increment],line5,0,font10,0);    //diplay parameter line4
            Print_Online_Menu_values(increment);
            if(ACAL_Status==0)
            {
                Print_String2(error_msg[Error_No],line6,0,font10,0);                 //error display    LINE5     						 			
                if((M077==1)&&(Error_No==0)&&(Pre_Hopper==1))
                 Print_String2(ACAL_Display[3],line6,0,font10,0);      //Auto cal status display    LINE5     						 			
            }

            if((ACAL_Status>1)&&(ACAL_Status<5)&&(Pre_Hopper==1))
                {
                    if(ACAL_Status==2)
                        Print_String2(ACAL_Display[0],line6,0,font10,0);      //Auto cal status display    LINE5     						 			
                    if((ACAL_Status==3))		
                        Print_String2(ACAL_Display[1],line6,0,font10,0);      //Auto cal status display    LINE5     						 			
                    if(ACAL_Status==4)	
                        Print_String2(ACAL_Display[2],line6,0,font10,0);      //Auto cal status display    LINE5                                                      
                }
            online_display_calculation();  //Calculation of parameters
            online_step=0;
            lcd_flag=1;
        }
    }
    else
        display_line_calculate();  //if online menu routine is in progress
}

/*
void calculate_feedrate_display()
{}
void calculate_beltload_display()
{
Belt_Load = (Present_Count - Count_DL ) * 1000 / (Span_Count);
}
void calculate_totaliser1_display()
{}
void calculate_totaliser2_display()
{}
void calculate_speed_display()
{}
void Calculate_Setpoint_Display()
{}
void Calculate_deviation_Display()
{}
void Calculate_WF_drive_setpoint_Display()
{}
void Calculate_PF_drive_setpoint_Display()
{}
void calculate_date_display()
{}
void calculate_time_display()
{}
*/

/**/
void display_line_calculate_Modbus()
{
    switch(Disp_Par_Count)
    {
        case 0:
            if(BL_avg/*Present_Count*/  >= zero_load)  //msk 28 sep 25, >= added earlier, only >
            {
                Display_Sign=Positive;
                Belt_Load = (float)((float)(BL_avg/*Present_Count*/  - zero_load) * 1000 / (Span_Count));
            }
            if(BL_avg  < zero_load )
            {
                Display_Sign=Negative;
                Belt_Load = (float)((float)(zero_load - /*Present_Count*/BL_avg) * 1000 / (Span_Count));
            }
            break;
        case 1:
            if(/*(Tacho_ip    ==0) &&*/ ( Fixed_Frequency == 1 ) && (Start_Stop_status == START ) )  // fixed Frequency Yes = 1 , No = 0 check in menu
            {
                Belt_Speed = Tacho_Freq;
            }
            else
            {
                Tacho_Avg =(float)((float)Tacho_Sum / Tacho_Buffer);
                if((unsigned int)((Tacho_Avg - (unsigned int)Tacho_Avg)*10) > 4 )
                {
                    Belt_Speed = Tacho_Avg +1;
                }
                if((unsigned int)((Tacho_Avg - (unsigned int)Tacho_Avg)*10) <= 4 )
                {
                    Belt_Speed = Tacho_Avg;
                }
            }
            break;
        case 2:
            Calculate_Setpoint_Display();
            break;
        case 3:
            if(Actual_Feedrate > Current_Setpoint)
            {
                Deviation_Sign = Positive;
                Deviation = (((float)( Actual_Feedrate - Current_Setpoint ) * 100 ) / Capacity);
            }
            if(Current_Setpoint > Actual_Feedrate)
            {
                Deviation_Sign = Negative;
                Deviation = (((float)( Current_Setpoint - Actual_Feedrate ) * 100 ) / Capacity);
            }
            break;
        case 4:
            WF_Drive_Setpoint_per = ((float)(WF_Drive_Setpoint * 1000) / 4095);
            break;
        case 5:
            PF_Drive_Setpoint_Per = ((float)(PF_Drive_Setpoint * 1000) / 4095);
            break;
        case 6:
            Gate_Postion_CalCulation();
            break;
        case 7:
            if(Present_Count_ch2>zero_load_ch2)
            {
                Hop_Load_Sign =0;
                Hopper_val= (float)((float)((float)Present_Count_ch2  - (float)zero_load_ch2) * (float)Capacity_1/*Hopper_Capacity*/ / (float)(Span_Count_ch2));//07/02/2018
            }
            else
            {
                Hop_Load_Sign =1;
                Hopper_val= (float)((float)((float)zero_load_ch2- (float)Present_Count_ch2  ) * (float)Capacity_1/*Hopper_Capacity*/ / (float)(Span_Count_ch2));//07/02/2018
            }
            break;
        default:
            break;
    }
}
/**/

void display_line_calculate()
{
    switch(display_parameter_line)
    {
        case 0://calculate_feedrate_display();
            break;
        case 1://calculate_totaliser1_display();
            break;
        case 2://calculate_totaliser2_display();
            break;
        case 3://load_cell_avg_processing();
            if(BL_avg/*Present_Count*/  >= zero_load)        //msk 28-sep-2025  earlier was > , added >= condition
            {
                Display_Sign=Positive;
                //Belt_Load = (float)((float)(5000 - 123) * 752 / (1452));
                Belt_Load = (float)((float)(BL_avg/*Present_Count*/  - zero_load) * 1000 / (Span_Count));
            }
            /*calculate_beltload_display();*/
            if(BL_avg  < zero_load )
            {
                Display_Sign=Negative;
                Belt_Load = (float)((float)(zero_load - /*Present_Count*/BL_avg) * 1000/ (Span_Count));
            }
//            if(BL_avg  ==0)    //msk 28 sep-25 if BL_Avg is zero then belt load is -ve below zero load
//            {
//                Display_Sign=Positive;
//                Belt_Load =0;
//            }
            break;
        case 4:
            if(/*(Tacho_ip    ==0)&&*/( Fixed_Frequency == 1 ) && (Start_Stop_status == START ) )  // fixed Frequency Yes = 1 , No = 0 check in menu
            {
                Belt_Speed = Tacho_Freq;
            }
            else
            {
                Tacho_Avg =(float)((float)Tacho_Sum / Tacho_Buffer);
                if((unsigned int)((Tacho_Avg - (unsigned int)Tacho_Avg)*10) > 4 )
                {
                    Belt_Speed = Tacho_Avg +1;
                }
                if((unsigned int)((Tacho_Avg - (unsigned int)Tacho_Avg)*10) <= 4 )
                {
                    Belt_Speed = Tacho_Avg;
                }
            }
            break;
        case 10:
            if(Actual_Feedrate > Current_Setpoint)
            {
                Deviation_Sign = Positive;
                Deviation = (((float)( Actual_Feedrate - Current_Setpoint ) * 100 ) / Capacity);
            }
            if(Current_Setpoint > Actual_Feedrate)
            {
                Deviation_Sign = Negative;
                Deviation = (((float)( Current_Setpoint - Actual_Feedrate ) * 100 ) / Capacity);
            }
            break;
        case 11:
            WF_Drive_Setpoint_per = ((float)(WF_Drive_Setpoint * 1000) / 4095);
            break;
        case 12:
            PF_Drive_Setpoint_Per = ((float)(PF_Drive_Setpoint * 1000) / 4095);
            break;
        case 13:
            if(Present_Count_ch2>zero_load_ch2)
            {
                Hop_Load_Sign =0;
                Hopper_val= (float)((float)((float)Present_Count_ch2  - (float)zero_load_ch2) * (float)Capacity_1/*Hopper_Capacity*/ / (float)(Span_Count_ch2));//07/02/2018
            }
            else
            {
                Hop_Load_Sign =1;
                Hopper_val= (float)((float)((float)zero_load_ch2- (float)Present_Count_ch2  ) * (float)Capacity_1/*Hopper_Capacity*/ / (float)(Span_Count_ch2));//07/02/2018
            }
            break;
        case 14: Gate_Postion_CalCulation();
            break;
        default:break;
    }
}

void online_display_calculation()
{
    display_parametr_line1=Counter1;
    display_parametr_line2=Counter2;
    display_parametr_line3=Counter3;
    display_parametr_line4=increment;

    if(display_parametr_line1>22)
        display_parametr_line1=0;
    else
        display_line_calculate();
    display_parameter_line=display_parametr_line1;
    line1_display=line_display;

    if(display_parametr_line2>22)
        display_parametr_line2=0;
    else
        display_line_calculate();
    display_parameter_line=display_parametr_line2;
    line2_display=line_display;

    if(display_parametr_line3>22)
        display_parametr_line3=0;
    else
        display_line_calculate();
    display_parameter_line=display_parametr_line3;
    line3_display=line_display;

    if(display_parametr_line4>22)
        display_parametr_line4=0;
    else
        display_line_calculate();
    display_parameter_line=display_parametr_line4;
    line4_display=line_display;
}

void Print_Online_Menu_values(char Counter_no) //
{
    // msk added for test 03-nov-2025 to be deleted
//    FDR_Total=FDR_Total_try;                    // msk added for test 03-nov-2025 to be deleted
//    HOpper_total=HOpper_total_try;        // msk added for test 03-nov-2025 to be deleted
    // msk added for test 03-nov-2025 to be deleted

    if(Counter_no==Counter1)  //Line1
        switch(Counter_no)
        {
        case 0:
            if(Dec_Point_Capacity==0)//9999
            {
                if(Display_Feedrate<9999)
                    Print_Variable3(Display_Feedrate,line2,25,font10,0,Dec_Point_Capacity,Units_Capacity,'X');
                if(Display_Feedrate>=9999)
                {
                    Display_Feedrate=9999;
                    Print_Variable3(Display_Feedrate,line2,24,font10,0,0,Units_Capacity,'X');
                    Print_String2(sign_str[2],line2,38,font10,0);
                }
            }
            if(Dec_Point_Capacity==1)//99990
            {
                if(Display_Feedrate<99990)
                    Print_Variable3(Display_Feedrate,line2,25,font10,0,Dec_Point_Capacity,Units_Capacity,'X');
                if(Display_Feedrate>=99990)
                {
                    Display_Feedrate=9999;
                    Print_Variable3(Display_Feedrate,line2,24,font10,0,0,Units_Capacity,'X');
                    Print_String2(sign_str[2],line2,38,font10,0);
                }
            }
            if(Dec_Point_Capacity==2)//999900
            {
                if(Display_Feedrate<999900)
                    Print_Variable3(Display_Feedrate,line2,25,font10,0,Dec_Point_Capacity,Units_Capacity,'X');
                if(Display_Feedrate>=999900)
                {
                    Display_Feedrate=9999;
                    Print_Variable3(Display_Feedrate,line2,24,font10,0,0,Units_Capacity,'X');
                    Print_String2(sign_str[2],line2,38,font10,0);
                }
            }
            if(Dec_Point_Capacity==3)
            {
                if(Display_Feedrate<9999000)
                    Print_Variable3(Display_Feedrate,line2,25,font10,0,Dec_Point_Capacity,Units_Capacity,'X');
                if(Display_Feedrate>=9999000)
                {
                    Display_Feedrate=9999;
                    Print_Variable3(Display_Feedrate,line2,24,font10,0,0,Units_Capacity,'X');
                    Print_String2(sign_str[2],line2,38,font10,0);
                }
            }
            break;
        case 1:
            Print_Variable3(Totaliser1_Display,line2,20,font10,0,Dec_Pt_Totaliser1,Units_Totaliser1,'1');
            break;
        case 2:
            Print_Variable3(Totaliser2_Display,line2,20,font10,0,Dec_Pt_Totaliser2,Units_Totaliser2,'2');
            break;
        case 3:
            if(Belt_Load < 9999)
            {
                Print_String2(sign_str[Display_Sign],line2,24,font10,0);
                Print_Variable3(Belt_Load,line2,27,font10,0,1,0,'0');
            }
            if((Belt_Load >= 9999) && (Display_Sign == Positive))
            {
                Belt_Load=9999;
                Print_Variable3(Belt_Load,line2,24,font10,0,1,0,'0');
                Print_String2(sign_str[2],line2,38,font10,0);
            }
            break;
        case 4:
            Print_Variable3(Display_Belt_Speed,line2,27,font10,0,0,0,'3');
            break;
        case 5:
            Print_Variable2(Setpoint_Int,line2,27,font10,0,Dec_Point_Capacity,Units_Capacity);
            break;
        case 6:
            Print_Variable2(External_Setpoint,line2,27,font10,0,Dec_Point_Capacity,Units_Capacity);
            break;
        case 7:
            Print_Variable2(Common_Setpoint,line2,27,font10,0,Dec_Point_Capacity,Units_Capacity);
            break;
        case 8:
            Print_Variable2(Setpoint_Man,line2,27,font10,0,Dec_Point_Capacity,Units_Capacity);
            break;
        case 9:
            Print_Variable2(Current_Setpoint,line2,27,font10,0,Dec_Point_Capacity,Units_Capacity);
            break;
        case 10:
            if(Deviation < 9999)
            {
                Print_String2(sign_str[Deviation_Sign],line2,24,font10,0);
                Print_Variable3(Deviation,line2,27,font10,0,1,0,'0');
            }
            if(Deviation >=9999)
            {
                Deviation=9999;
                Print_Variable3(Deviation,line2,24,font10,0,1,0,'0');
                Print_String2(sign_str[2],line2,38,font10,0);
            }
            break;
        case 11:
            Print_Variable3(WF_Drive_Setpoint_per,line2,27,font10,0,1,0,'0');
            break;
        case 12:
            Print_Variable3(PF_Drive_Setpoint_Per,line2,27,font10,0,1,0,'0');
            break;
        case 13:
            Print_Variable3(Hopper_val,line2,27,font10,0,Dec_Pt_Totaliser1,Units_Totaliser1,'1');
            Print_String2(sign_str[Hop_Load_Sign],line2,24,font10,0);
            break;
        case 14:
            Print_Variable3(Gate_Postion,line2,27,font10,0,1,0,'0');
            break;
        case 15:
            Print_Variable3(HOpper_total,line2,27,font10,0,Dec_Pt_Totaliser1,Units_Totaliser1,'1');
            break;
        case 16:
            Print_Variable3(FDR_Total,line2,27,font10,0,Dec_Pt_Totaliser1,Units_Totaliser1,'1');
            break;
        case 17:
            Print_Variable3(CM_Difference,line2,27,font10,0,Dec_Pt_Totaliser1,Units_Totaliser1,'1');
            break;
        case 18:
            Print_Variable3(CM_Display,line2,27,font10,0,1,0,'0');
            Print_String2(sign_str[CM_Negative],line2,24,font10,0);
            break;
        case 19:
            Print_Variable3(Inv_Op_Freq,line2,27,font10,0,0,0,'3');
            break;
        case 20:
            Print_Variable3(Inv_Op_current,line2,27,font10,0,1,2,'4');
            break;
        case 21:
            Print_Variable3(Inv1_Op_Freq,line2,27,font10,0,0,0,'3');
            break;
        case 22:
            Print_Variable3(Inv1_Op_current,line2,27,font10,0,1,2,'4');
            break;
        default:
            break;
        }
    if(Counter_no==Counter2)  //Line2
        switch(Counter_no)
        {
        case 0:
            if(Dec_Point_Capacity==0)//9999
            {
                if(Display_Feedrate<9999)
                    Print_Variable3(Display_Feedrate,line3,25,font10,0,Dec_Point_Capacity,Units_Capacity,'X');
                if(Display_Feedrate>=9999)
                {
                    Display_Feedrate=9999;
                    Print_Variable3(Display_Feedrate,line3,24,font10,0,0,Units_Capacity,'X');
                    Print_String2(sign_str[2],line3,38,font10,0);
                }
            }
            if(Dec_Point_Capacity==1)//99990
            {
                if(Display_Feedrate<99990)
                    Print_Variable3(Display_Feedrate,line3,25,font10,0,Dec_Point_Capacity,Units_Capacity,'X');
                if(Display_Feedrate>=99990)
                {
                    Display_Feedrate=9999;
                    Print_Variable3(Display_Feedrate,line3,24,font10,0,0,Units_Capacity,'X');
                    Print_String2(sign_str[2],line3,38,font10,0);
                }
            }
            if(Dec_Point_Capacity==2)//999900
            {
                if(Display_Feedrate<999900)
                    Print_Variable3(Display_Feedrate,line3,25,font10,0,Dec_Point_Capacity,Units_Capacity,'X');
                if(Display_Feedrate>=999900)
                {
                    Display_Feedrate=9999;
                    Print_Variable3(Display_Feedrate,line3,24,font10,0,0,Units_Capacity,'X');
                    Print_String2(sign_str[2],line3,38,font10,0);
                }
            }
            if(Dec_Point_Capacity==3)
            {
                if(Display_Feedrate<9999000)
                    Print_Variable3(Display_Feedrate,line3,25,font10,0,Dec_Point_Capacity,Units_Capacity,'X');
                if(Display_Feedrate>=9999000)
                {
                    Display_Feedrate=9999;
                    Print_Variable3(Display_Feedrate,line3,24,font10,0,0,Units_Capacity,'X');
                    Print_String2(sign_str[2],line3,38,font10,0);
                }
            }
            break;
        case 1:
            Print_Variable3(Totaliser1_Display,line3,20,font10,0,Dec_Pt_Totaliser1,Units_Totaliser1,'1');
            break;
        case 2:
            Print_Variable3(Totaliser2_Display,line3,20,font10,0,Dec_Pt_Totaliser2,Units_Totaliser2,'2');
            break;
        case 3:
            if(Belt_Load < 9999)
            {
                Print_String2(sign_str[Display_Sign],line3,24,font10,0);
                Print_Variable3(Belt_Load,line3,27,font10,0,1,0,'0');
            }
            if((Belt_Load >= 9999) && (Display_Sign == Positive))
            {
                Belt_Load=9999;
                Print_Variable3(Belt_Load,line3,24,font10,0,1,0,'0');
                Print_String2(sign_str[2],line3,38,font10,0);
            }
            break;
        case 4:
            Print_Variable3(Display_Belt_Speed,line3,27,font10,0,0,0,'3');
            break;
        case 5:
            Print_Variable2(Setpoint_Int,line3,27,font10,0,Dec_Point_Capacity,Units_Capacity);
            break;
        case 6:
            Print_Variable2(External_Setpoint,line3,27,font10,0,Dec_Point_Capacity,Units_Capacity);
            break;
        case 7:
            Print_Variable2(Common_Setpoint,line3,27,font10,0,Dec_Point_Capacity,Units_Capacity);
            break;
        case 8:
            Print_Variable2(Setpoint_Man,line3,27,font10,0,Dec_Point_Capacity,Units_Capacity);
            break;
        case 9:
            Print_Variable2(Current_Setpoint,line3,27,font10,0,Dec_Point_Capacity,Units_Capacity);
            break;
        case 10:
            if(Deviation < 9999)
            {
                Print_String2(sign_str[Deviation_Sign],line3,24,font10,0);
                Print_Variable3(Deviation,line3,27,font10,0,1,0,'0');
            }
            if(Deviation >=9999)
            {
                Deviation=9999;
                Print_Variable3(Deviation,line3,24,font10,0,1,0,'0');
                Print_String2(sign_str[2],line3,38,font10,0);
            }
            break;
        case 11:
            Print_Variable3(WF_Drive_Setpoint_per,line3,27,font10,0,1,0,'0');
            break;
        case 12:
            Print_Variable3(PF_Drive_Setpoint_Per,line3,27,font10,0,1,0,'0');
            break;
        case 13:
            Print_Variable3(Hopper_val,line3,27,font10,0,Dec_Pt_Totaliser1,Units_Totaliser1,'1');
            Print_String2(sign_str[Hop_Load_Sign],line3,24,font10,0);
            break;
        case 14:
            Print_Variable3(Gate_Postion,line3,27,font10,0,1,0,'0');
            break;
        case 15:
            Print_Variable3(HOpper_total,line3,27,font10,0,Dec_Pt_Totaliser1,Units_Totaliser1,'1');
            break;
        case 16:
            Print_Variable3(FDR_Total,line3,27,font10,0,Dec_Pt_Totaliser1,Units_Totaliser1,'1');
            break;
        case 17:
            Print_Variable3(CM_Difference,line3,27,font10,0,Dec_Pt_Totaliser1,Units_Totaliser1,'1');
            break;
        case 18:
            Print_Variable3(CM_Display,line3,27,font10,0,1,0,'0');
            Print_String2(sign_str[CM_Negative],line3,24,font10,0);
            break;
        case 19:
            Print_Variable3(Inv_Op_Freq,line3,27,font10,0,0,0,'3');
            break;
        case 20:
            Print_Variable3(Inv_Op_current,line3,27,font10,0,1,2,'4');
            break;
        case 21:
            Print_Variable3(Inv1_Op_Freq,line3,27,font10,0,0,0,'3');
            break;
        case 22:
            Print_Variable3(Inv1_Op_current,line3,27,font10,0,1,2,'4');
            break;
        default:
            break;
        }

    if(Counter_no==Counter3)  //Line3
        switch(Counter_no)
        {
        case 0:
            if(Dec_Point_Capacity==0)//9999
            {
                if(Display_Feedrate<9999)
                    Print_Variable3(Display_Feedrate,line4,25,font10,0,Dec_Point_Capacity,Units_Capacity,'X');
                if(Display_Feedrate>=9999)
                {
                    Display_Feedrate=9999;
                    Print_Variable3(Display_Feedrate,line4,24,font10,0,0,Units_Capacity,'X');
                    Print_String2(sign_str[2],line4,38,font10,0);
                }
            }
            if(Dec_Point_Capacity==1)//99990
            {
                if(Display_Feedrate<99990)
                    Print_Variable3(Display_Feedrate,line4,25,font10,0,Dec_Point_Capacity,Units_Capacity,'X');
                if(Display_Feedrate>=99990)
                {
                    Display_Feedrate=9999;
                    Print_Variable3(Display_Feedrate,line4,24,font10,0,0,Units_Capacity,'X');
                    Print_String2(sign_str[2],line4,38,font10,0);
                }
            }
            if(Dec_Point_Capacity==2)//999900
            {
                if(Display_Feedrate<999900)
                    Print_Variable3(Display_Feedrate,line4,25,font10,0,Dec_Point_Capacity,Units_Capacity,'X');
                if(Display_Feedrate>=999900)
                {
                    Display_Feedrate=9999;
                    Print_Variable3(Display_Feedrate,line4,24,font10,0,0,Units_Capacity,'X');
                    Print_String2(sign_str[2],line4,38,font10,0);
                }
            }
            if(Dec_Point_Capacity==3)
            {
                if(Display_Feedrate<9999000)
                    Print_Variable3(Display_Feedrate,line4,25,font10,0,Dec_Point_Capacity,Units_Capacity,'X');
                if(Display_Feedrate>=9999000)
                {
                    Display_Feedrate=9999;
                    Print_Variable3(Display_Feedrate,line4,24,font10,0,0,Units_Capacity,'X');
                    Print_String2(sign_str[2],line4,38,font10,0);
                }
            }
            break;
        case 1:
            Print_Variable3(Totaliser1_Display,line4,20,font10,0,Dec_Pt_Totaliser1,Units_Totaliser1,'1');
            break;
        case 2:
            Print_Variable3(Totaliser2_Display,line4,20,font10,0,Dec_Pt_Totaliser2,Units_Totaliser2,'2');
            break;
        case 3:
            if(Belt_Load < 9999)
            {
                Print_String2(sign_str[Display_Sign],line4,24,font10,0);
                Print_Variable3(Belt_Load,line4,27,font10,0,1,0,'0');
            }
            if((Belt_Load >= 9999) && (Display_Sign == Positive))
            {
                Belt_Load=9999;
                Print_Variable3(Belt_Load,line4,24,font10,0,1,0,'0');
                Print_String2(sign_str[2],line4,38,font10,0);
            }
            break;
        case 4:
            Print_Variable3(Display_Belt_Speed,line4,27,font10,0,0,0,'3');
            break;
        case 5:
            Print_Variable2(Setpoint_Int,line4,27,font10,0,Dec_Point_Capacity,Units_Capacity);
            break;
        case 6:
            Print_Variable2(External_Setpoint,line4,27,font10,0,Dec_Point_Capacity,Units_Capacity);
            break;
        case 7:
            Print_Variable2(Common_Setpoint,line4,27,font10,0,Dec_Point_Capacity,Units_Capacity);
            break;
        case 8:
            Print_Variable2(Setpoint_Man,line4,27,font10,0,Dec_Point_Capacity,Units_Capacity);
            break;
        case 9:
            Print_Variable2(Current_Setpoint,line4,27,font10,0,Dec_Point_Capacity,Units_Capacity);
            break;
        case 10:
            if(Deviation < 9999)
            {
                Print_String2(sign_str[Deviation_Sign],line4,24,font10,0);
                Print_Variable3(Deviation,line4,27,font10,0,1,0,'0');
            }
            if(Deviation >=9999)
            {
                Deviation=9999;
                Print_Variable3(Deviation,line4,24,font10,0,1,0,'0');
                Print_String2(sign_str[2],line4,38,font10,0);
            }
            break;
        case 11:
            Print_Variable3(WF_Drive_Setpoint_per,line4,27,font10,0,1,0,'0');
            break;
        case 12:
            Print_Variable3(PF_Drive_Setpoint_Per,line4,27,font10,0,1,0,'0');
            break;
        case 13:
            Print_Variable3(Hopper_val,line4,27,font10,0,Dec_Pt_Totaliser1,Units_Totaliser1,'1');
            Print_String2(sign_str[Hop_Load_Sign],line4,24,font10,0);
            break;
        case 14:
            Print_Variable3(Gate_Postion,line4,27,font10,0,1,0,'0');
            break;
        case 15:
            Print_Variable3(HOpper_total,line4,27,font10,0,Dec_Pt_Totaliser1,Units_Totaliser1,'1');
            break;
        case 16:
            Print_Variable3(FDR_Total,line4,27,font10,0,Dec_Pt_Totaliser1,Units_Totaliser1,'1');
            break;
        case 17:
            Print_Variable3(CM_Difference,line4,27,font10,0,Dec_Pt_Totaliser1,Units_Totaliser1,'1');
            break;
        case 18:
            Print_Variable3(CM_Display,line4,27,font10,0,1,0,'0');
            Print_String2(sign_str[CM_Negative],line4,24,font10,0);
            break;
        case 19:
            Print_Variable3(Inv_Op_Freq,line4,27,font10,0,0,0,'3');
            break;
        case 20:
            Print_Variable3(Inv_Op_current,line4,27,font10,0,1,2,'4');
            break;
        case 21:
            Print_Variable3(Inv1_Op_Freq,line4,27,font10,0,0,0,'3');
            break;
        case 22:
            Print_Variable3(Inv1_Op_current,line4,27,font10,0,1,2,'4');
            break;
        default:
            break;
        }

    if(Counter_no==increment)  //Line4
        switch(Counter_no)
        {
        case 0:
            if(Dec_Point_Capacity==0)//9999
            {
                if(Display_Feedrate<9999)
                    Print_Variable3(Display_Feedrate,line5,25,font10,0,Dec_Point_Capacity,Units_Capacity,'X');
                if(Display_Feedrate>=9999)
                {
                    Display_Feedrate=9999;
                    Print_Variable3(Display_Feedrate,line5,24,font10,0,0,Units_Capacity,'X');
                    Print_String2(sign_str[2],line5,38,font10,0);
                }
            }
            if(Dec_Point_Capacity==1)//99990
            {
                if(Display_Feedrate<99990)
                    Print_Variable3(Display_Feedrate,line5,25,font10,0,Dec_Point_Capacity,Units_Capacity,'X');
                if(Display_Feedrate>=99990)
                {
                    Display_Feedrate=9999;
                    Print_Variable3(Display_Feedrate,line5,24,font10,0,0,Units_Capacity,'X');
                    Print_String2(sign_str[2],line5,38,font10,0);
                }
            }
            if(Dec_Point_Capacity==2)//999900
            {
                if(Display_Feedrate<999900)
                    Print_Variable3(Display_Feedrate,line5,25,font10,0,Dec_Point_Capacity,Units_Capacity,'X');
                if(Display_Feedrate>=999900)
                {
                    Display_Feedrate=9999;
                    Print_Variable3(Display_Feedrate,line5,24,font10,0,0,Units_Capacity,'X');
                    Print_String2(sign_str[2],line5,38,font10,0);
                }
            }
            if(Dec_Point_Capacity==3)
            {
                if(Display_Feedrate<9999000)
                    Print_Variable3(Display_Feedrate,line5,25,font10,0,Dec_Point_Capacity,Units_Capacity,'X');
                if(Display_Feedrate>=9999000)
                {
                    Display_Feedrate=9999;
                    Print_Variable3(Display_Feedrate,line5,24,font10,0,0,Units_Capacity,'X');
                    Print_String2(sign_str[2],line5,38,font10,0);
                }
            }
            break;
        case 1:
            Print_Variable3(Totaliser1_Display,line5,20,font10,0,Dec_Pt_Totaliser1,Units_Totaliser1,'1');
            break;
        case 2:
            Print_Variable3(Totaliser2_Display,line5,20,font10,0,Dec_Pt_Totaliser2,Units_Totaliser2,'2');
            break;
        case 3:
            if(Belt_Load < 9999)
            {
                Print_String2(sign_str[Display_Sign],line5,24,font10,0);
                Print_Variable3(Belt_Load,line5,27,font10,0,1,0,'0');
            }
            if((Belt_Load >= 9999) && (Display_Sign == Positive))
            {
                Belt_Load=9999;
                Print_Variable3(Belt_Load,line5,24,font10,0,1,0,'0');
                Print_String2(sign_str[2],line5,38,font10,0);
            }
            break;
        case 4:
            Print_Variable3(Display_Belt_Speed,line5,27,font10,0,0,0,'3');
            break;
        case 5:
            Print_Variable2(Setpoint_Int,line5,27,font10,0,Dec_Point_Capacity,Units_Capacity);
            break;
        case 6:
            Print_Variable2(External_Setpoint,line5,27,font10,0,Dec_Point_Capacity,Units_Capacity);
            break;
        case 7:
            Print_Variable2(Common_Setpoint,line5,27,font10,0,Dec_Point_Capacity,Units_Capacity);
            break;
        case 8:
            Print_Variable2(Setpoint_Man,line5,27,font10,0,Dec_Point_Capacity,Units_Capacity);
            break;
        case 9:
            Print_Variable2(Current_Setpoint,line5,27,font10,0,Dec_Point_Capacity,Units_Capacity);
            break;
        case 10:
            if(Deviation < 9999)
            {
                Print_String2(sign_str[Deviation_Sign],line5,24,font10,0);
                Print_Variable3(Deviation,line5,27,font10,0,1,0,'0');
            }
            if(Deviation >=9999)
            {
                Deviation=9999;
                Print_Variable3(Deviation,line5,24,font10,0,1,0,'0');
                Print_String2(sign_str[2],line5,38,font10,0);
            }
            break;
        case 11:
            Print_Variable3(WF_Drive_Setpoint_per,line5,27,font10,0,1,0,'0');
            break;
        case 12:
            Print_Variable3(PF_Drive_Setpoint_Per,line5,27,font10,0,1,0,'0');
            break;
        case 13:
            Print_Variable3(Hopper_val,line5,27,font10,0,Dec_Pt_Totaliser1,Units_Totaliser1,'1');
            Print_String2(sign_str[Hop_Load_Sign],line5,24,font10,0);
            break;
        case 14:
            Print_Variable3(Gate_Postion,line5,27,font10,0,1,0,'0');
            break;
        case 15:
            Print_Variable3(HOpper_total,line5,27,font10,0,Dec_Pt_Totaliser1,Units_Totaliser1,'1');
            break;
        case 16:
            Print_Variable3(FDR_Total,line5,27,font10,0,Dec_Pt_Totaliser1,Units_Totaliser1,'1');
            break;
        case 17:
            Print_Variable3(CM_Difference,line5,27,font10,0,Dec_Pt_Totaliser1,Units_Totaliser1,'1');
            break;
        case 18:
            Print_Variable3(CM_Display,line5,27,font10,0,1,0,'0');
            Print_String2(sign_str[CM_Negative],line5,24,font10,0);
            break;
        case 19:
            Print_Variable3(Inv_Op_Freq,line5,27,font10,0,0,0,'3');
            break;
        case 20:
            Print_Variable3(Inv_Op_current,line5,27,font10,0,1,2,'4');
            break;
        case 21:
            Print_Variable3(Inv1_Op_Freq,line5,27,font10,0,0,0,'3');
            break;
        case 22:
            Print_Variable3(Inv1_Op_current,line5,27,font10,0,1,2,'4');
            break;
        default:
            break;
        }
}

void IO_adrs_data_Identify_offline(char data_,char Row_no,char col_no)
{
    switch(((char)(data_)))
    {
        case 0: Print_String(io_str[0],Row_no,col_no,font10,0); break;
        case 1: Print_String(Ram_add_no[0],Row_no,col_no,font10,0); break;
        case 2: Print_String(Ram_add_no[1],Row_no,col_no,font10,0); break;
        case 3: Print_String(Ram_add_no[2],Row_no,col_no,font10,0); break;
        case 4: Print_String(Ram_add_no[3],Row_no,col_no,font10,0); break;
        case 5: Print_String(Ram_add_no[4],Row_no,col_no,font10,0); break;
        case 6: Print_String(Ram_add_no[5],Row_no,col_no,font10,0); break;
        case 7: Print_String(Ram_add_no[6],Row_no,col_no,font10,0); break;
        case 8: Print_String(Ram_add_no[7],Row_no,col_no,font10,0); break;
        case 9: Print_String(Ram_add_no[8],Row_no,col_no,font10,0); break;
        case 10: Print_String(Ram_add_no[9],Row_no,col_no,font10,0); break;
        case 11: Print_String(io_str[1],Row_no,col_no,font10,0); break;
        case 12: Print_String(io_str[2],Row_no,col_no,font10,0); break;
        case 13: Print_String(io_str[3],Row_no,col_no,font10,0); break;
        case 14: Print_String(io_str[4],Row_no,col_no,font10,0); break;
        case 15: Print_String(io_str[5],Row_no,col_no,font10,0); break;
        case 16: Print_String(io_str[6],Row_no,col_no,font10,0); break;
        case 17: Print_String(io_str[7],Row_no,col_no,font10,0); break;
        case 18: Print_String(io_str[8],Row_no,col_no,font10,0); break;
        case 19: Print_String(io_str[9],Row_no,col_no,font10,0); break;
        case 20: Print_String(io_str[10],Row_no,col_no,font10,0); break;
        case 21: Print_String(io_str[11],Row_no,col_no,font10,0); break;
        case 22: Print_String(io_str[12],Row_no,col_no,font10,0); break;
        case 23: Print_String(io_str[13],Row_no,col_no,font10,0); break;
        case 24: Print_String(io_str[14],Row_no,col_no,font10,0); break;
        case 25: Print_String(io_str[15],Row_no,col_no,font10,0); break;
        case 26: Print_String(io_str[16],Row_no,col_no,font10,0); break;
        case 27: Print_String(io_str[17],Row_no,col_no,font10,0); break;
        case 28: Print_String(io_str[18],Row_no,col_no,font10,0); break;
        case 29: Print_String(io_str[19],Row_no,col_no,font10,0); break;
        case 30: Print_String(io_str_name[4],0,col_no,font10,0); break;
        default: break;
    }
}

void IO_Monitor_Offline()
{
    char IO_offline;
    switch(IO_Stat_offline)
    {
        case 0:
            switch(row_no_offline)
            {
                case 0: IO_Ptr2_offline = IO_Disp_L1_offline; IO_Ptr_offline = IO_Par_L1_offline; break;
                case 1: IO_Ptr2_offline = IO_Disp_L2_offline; IO_Ptr_offline = IO_Par_L2_offline; break;
                case 2: IO_Ptr2_offline = IO_Disp_L3_offline; IO_Ptr_offline = IO_Par_L3_offline; break;
                case 3: IO_Ptr2_offline = IO_Disp_L4_offline; IO_Ptr_offline = IO_Par_L4_offline; break;
                case 4: IO_Ptr2_offline = IO_Disp_L5_offline; IO_Ptr_offline = IO_Par_L5_offline; break;
                default: break;
            }
            for(IO_offline=0;IO_offline<10;IO_offline++)
            {
                if(ram_values[ IO_offline + IO_Ptr2_offline ] == 1)
                    IO_database_offline[ IO_offline ] = IO_offline + 1;
                else
                    IO_database_offline[ IO_offline ] = 0;
            }
            IO_Stat_offline = 1;
            break;
        case 1:
            switch(IO_Ptr1_offline)
            {
                case 0: IO_adrs_data_Identify_offline(IO_database_offline[IO_Ptr1_offline],row_no_offline+1,15); break;
                case 1: IO_adrs_data_Identify_offline(IO_database_offline[IO_Ptr1_offline],row_no_offline+1,20); break;
                case 2: IO_adrs_data_Identify_offline(IO_database_offline[IO_Ptr1_offline],row_no_offline+1,25); break;
                case 3: IO_adrs_data_Identify_offline(IO_database_offline[IO_Ptr1_offline],row_no_offline+1,30); break;
                case 4: IO_adrs_data_Identify_offline(IO_database_offline[IO_Ptr1_offline],row_no_offline+1,35); break;
                case 5: IO_adrs_data_Identify_offline(IO_database_offline[IO_Ptr1_offline],row_no_offline+1,40); break;
                case 6: IO_adrs_data_Identify_offline(IO_database_offline[IO_Ptr1_offline],row_no_offline+1,45); break;
                case 7: IO_adrs_data_Identify_offline(IO_database_offline[IO_Ptr1_offline],row_no_offline+1,50); break;
                case 8: IO_adrs_data_Identify_offline(IO_database_offline[IO_Ptr1_offline],row_no_offline+1,55); break;
                case 9: IO_adrs_data_Identify_offline(IO_database_offline[IO_Ptr1_offline],row_no_offline+1,60); break;
                case 10: IO_adrs_data_Identify_offline((IO_Ptr_offline),row_no_offline+1,0); break;
                case 11: IO_adrs_data_Identify_offline(30,0,0); break;
                default: break;
            }
            IO_Ptr1_offline++;
            if(IO_Ptr1_offline > 11)
            {
                IO_Stat_offline = 0;
                IO_Ptr1_offline = 0;
                row_no_offline++;
                if(row_no_offline > 4)
                    row_no_offline = 0;
            }
            break;
        default:
            break;
    }
}

void IO_Monitor()
{
    unsigned char excu_count,excu_count1;

    Clear_LCD();
IO_jump1:
    if(flag==1)
    {
        flag = 0;
        IO_Monitor_Offline();

        Key_Input = NO_KEY;

        if(scan_count%10 ==0)
            key_check_routine();

        if(scan_count%20 ==0)
            IO_Control_Routine();

        if(Key_Input == K_DEC )
        {
            excu_count++;
            switch(excu_count)
            {
                case 1:
                    IO_Disp_L1_offline = IO_Disp_L2_offline;
                    IO_Disp_L2_offline = IO_Disp_L3_offline;
                    IO_Disp_L3_offline = IO_Disp_L4_offline;
                    IO_Disp_L4_offline = IO_Disp_L5_offline;
                    IO_Disp_L5_offline = IO_Disp_L5_offline+10;

                    IO_Par_L1_offline = IO_Par_L2_offline;
                    IO_Par_L2_offline = IO_Par_L3_offline;
                    IO_Par_L3_offline = IO_Par_L4_offline;
                    IO_Par_L4_offline = IO_Par_L5_offline;
                    IO_Par_L5_offline = IO_Par_L5_offline+1;

                    if((IO_Disp_L5_offline > 184))
                    {
                        IO_Disp_L5_offline = 0;
                        IO_Par_L5_offline = 11;
                    }
                    IO_Stat_offline = 0;
                    IO_Ptr1_offline = 0;
                    Clear_LCD();
                    break;
                default: break;
            }
            if(excu_count>1)
                excu_count = 0;
            Key_Input = NO_KEY;
            goto IO_jump1;
        }

        if(Key_Input == K_INC )
        {
            excu_count++;
            switch(excu_count)
            {
                case 1:
                    IO_Disp_L5_offline = IO_Disp_L4_offline;
                    IO_Disp_L4_offline = IO_Disp_L3_offline;
                    IO_Disp_L3_offline = IO_Disp_L2_offline;
                    IO_Disp_L2_offline = IO_Disp_L1_offline;
                    IO_Disp_L1_offline = IO_Disp_L1_offline-10;

                    IO_Par_L5_offline = IO_Par_L4_offline;
                    IO_Par_L4_offline = IO_Par_L3_offline;
                    IO_Par_L3_offline = IO_Par_L2_offline;
                    IO_Par_L2_offline = IO_Par_L1_offline;
                    IO_Par_L1_offline = IO_Par_L1_offline-1;

                    if((IO_Disp_L1_offline > 184))
                    {
                        IO_Disp_L1_offline = 180;
                        IO_Par_L1_offline = 29;
                    }
                    Clear_LCD();
                    IO_Stat_offline = 0;
                    IO_Ptr1_offline = 0;
                    break;
                default: break;
            }
            if(excu_count>1)
                excu_count = 0;
            Key_Input = NO_KEY;
            goto IO_jump1;
        }
        if(Key_Input == HOR )
        {
            excu_count1++;
            switch(excu_count1)
            {
                case 1:
                    if( ( IO_Disp_L5_offline >= 0 ) && ( IO_Disp_L5_offline < 40 ) )
                    {
                        IO_Disp_L5_offline = 40;
                        IO_Par_L5_offline = 15;
                        Clear_String(&clear_string,line6,0,font10);
                        Key_Input = NO_KEY;
                        IO_Stat_offline = 0;
                        IO_Ptr1_offline = 0;
                        goto IO_jump1;
                    }
                    if( ( IO_Disp_L5_offline >= 40 ) && (IO_Disp_L5_offline < 80) )
                    {
                        IO_Disp_L5_offline = 80;
                        IO_Par_L5_offline = 19;
                        Clear_String(&clear_string,line6,0,font10);
                        Key_Input = NO_KEY;
                        IO_Stat_offline = 0;
                        IO_Ptr1_offline = 0;
                        goto IO_jump1;
                    }
                    if( ( IO_Disp_L5_offline >= 80 ) && (IO_Disp_L5_offline < 184 ) )
                    {
                        IO_Disp_L5_offline = 184 ;
                        IO_Par_L5_offline = 29;
                        Clear_String(&clear_string,line6,0,font10);
                        Key_Input = NO_KEY;
                        IO_Stat_offline = 0;
                        IO_Ptr1_offline = 0;
                        goto IO_jump1;
                    }
                    if( (IO_Disp_L5_offline >= 184) )
                    {
                        IO_Disp_L5_offline = 0;
                        IO_Par_L5_offline = 11;
                        Clear_String(&clear_string,line6,0,font10);
                        Key_Input = NO_KEY;
                        IO_Stat_offline = 0;
                        IO_Ptr1_offline = 0;
                        goto IO_jump1;
                    }
                    break;
                default:
                    break;
            }
            if(excu_count1>1)
                excu_count1 = 0;
            Key_Input = NO_KEY;
        }

        if(Key_Input == ESC )
        {
            IO_Disp_L1_offline = 0,IO_Disp_L2_offline =10 ,IO_Disp_L3_offline = 20,IO_Disp_L4_offline = 30,IO_Disp_L5_offline= 40;
            IO_Par_L1_offline = 11 ,IO_Par_L2_offline = 12 , IO_Par_L3_offline = 13 ,IO_Par_L4_offline = 14 ,IO_Par_L5_offline = 15;
            IO_Stat_offline = 0;
            IO_Ptr1_offline = 0;
            return;
        }
    }

    goto IO_jump1;
}
/*
date:2/2/2018
Belt Load
    Belt_Load = (BL_Avg-ZeroLoad)*Capacity/span_Count
    value is calculated wrt capacity
date:19/2/2018
Belt_Load
    Belt_Load = (BL_Avg-ZeroLoad)*1000/span_Count
both values are calculated wrt %

*/
