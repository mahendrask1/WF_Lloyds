/*Menu.c*/
#include "parameters.h"    //#include "menu.h"
#include "main_routine.h"
#include "OLED_2_7_12864.h"
#include "stdio.h"
#include <stdlib.h>
#include "LPC17xx.h"
#include "cmsis_os.h"
#include "Board_Joystick.h"
#include "keys.h"
#include "lpc_types.h"
#include "lpc17xx_rtc.h"
#include "rtc_app.h"
#include "functions.h" /* msk29-may-26: ResetInvCommAfterOfflineMenu */
#include "modbus_dio.h"
#include "modbus.h"
#include "plc.h"
//#include "usb_app.h"
#include <time.h>
#include <string.h>

void Clear_single_Line(char, char);
char FWBLOCK[5];
unsigned char status_no, start_count_flag, first_key, second_key, key_check_count;
uint16_t Online_menu_parameters[15];
int pass_check;
char sub_display_cursor;
char menu_counter,Sub_Menu_no;
char menu_status, display_line1, display_line5;
char start_display_pointer, end_display_pointer,display_cursor,display_cursor_temp,display_cursor_prev;
char sub_menu_status;
char temp12;
char *print_menu;

extern int ass_counter, prog_counter;
extern char* P_String;
extern char A_String[35];
extern volatile char flag;
extern char *clear_string;// ="                             ";
extern   uint32_t    Key_Input;
extern uint16_t New_menu_parameters[105];
extern char plc_inst[200][7];
uint16_t new_pass;
extern uint16_t scan_count;
char const *pp;
char parameter_options_temp, no_of_digits,no_of_dec;
extern int bytes_written;
extern RTC_TIME_Type rtc_read,rtc_write1;
char *pass_str[3][15]={ {"Password Old:\0"},{"Password New:\0"},{"Enter Password:\0"}};
char *setpoint_string[3][14]={{"Setpoint:-"},{"Setpoint Int .\0"},{"Setpoint Man ..\0"}};
char Check_Start ;
void Check_password(void);
void Enter_setpoint(void);
extern char io_status;
extern char Start_Stop_status;

char Submenu_flag ;
char submenu_Staus_flag ;
extern char key_vlaue ;
char Oled_flag_Temp ;
extern char *unitstri[2][5];

long VARI_online=0;
char new_string_online[15]={0};
char print_new_string_online[15]={0};
char old_string_online[15]={0};
char *new_string_print_online;
char str_len1_online=0,str_len2_online=0;
unsigned char j_online=0,k_online=0,j2_online=0;
unsigned char str_loop_online=0;
signed char diff_online=0;
char dec_flag_online=0;

char check_flag;
char Sel_para_flag,Opt_para_flag ,Temp_par_val=0;
extern char Oled_flag_Temp ;
extern int temp_count1,temp_count2;
char *clear_string_temp ="                               ";
extern unsigned int Totaliser1_Display,Totaliser2_Display,Totaliser1_Display_Old,Totaliser2_Display_Old;
extern double Totaliser1,Totaliser2;
extern double Totaliser_Update ,Totaliser1_Update,Totaliser2_Update;
char total_para_no;

extern char calib_string[20];
extern char *calib_string_ptr ;
extern unsigned char no_menu_par[12];
extern char no_of_menus;
unsigned char par_dec_pt,par_type ;

unsigned char download_menu_par[2900];

extern unsigned int par_count;
extern  uint32_t UART1Count ;
int32_t cnt_to_wrap_1;
extern unsigned long Present_Count,Present_Count_ch2,Count_DL,Count_DL_ch2,Span_Count,Span_Count_ch2,zero_load,zero_load_ch2,full_load,full_load_ch2;
unsigned int Capacity_1,val_of_par;
unsigned char Dwnd_flag,Dwnd_Flag_En,Dwnd_Error;
/*********************************************/

sub_menu_struct Online_menu[5][14] =
{
    {    //0 Performance Parameters
        {'E',41,34},        //E4.1    Int Time
        {'E',41,35},        //E4.1    Prop Gain
        {'E',41,36},        //E4.1    Dev Limit
        {'E',41,37},        //E4.1    Dev Time
        {'E',41,38},        //E4.1    Dead Band
        {'E',41,82},        //E4.1    Min Pulse Time (in Secs) For Pre Hopper
        {'O',2, 55},        //O3        Regulation type WF/Drive
        {'E',41,56},        //E4.0    Reg Tracking
        {'E',41,49},        //E4.0    BL Limit
        {'E',41,50},        //E4.0    PF Delay
        {'E',41,51},        //E4.0    PF_StartSet
        {'E',41,54},        //E4.0    PF Limit
        {'E',54,83},        //E5.4  Correction Factor
        {'F',255,255}
    },
    {    //1 Set Points
        {'E',40,39},        //E4.0    Setpoint Int
        {'E',40,40},        //E4.0    Setpoint Man
        {'F',255,255}
    },
    {    //3 IO Monitor
        {'F',255,255}
    },
};

sub_menu_struct sub_menu1[12][26] =
{
    {    //0 System Parameters
        {'O',2,2},          //O2        Units_Cap,Kg/h,t/h
        {'O',4,3},          //O5        Decimal Point
        {'E',40,4},         //E4.0    Capacity
        {'O',4,5},          //O3        Totaliser1 Decimal point
        {'O',2,6},          //O2        Units_Total1,Kg,t
        {'O',4,7},          //O3        Totaliser2 Decimal point
        {'O',2,8},          //O2        Units_Total2,Kg,t
        {'O',2,91},         //O2      Tacho i/p Tacho / Inv
        {'O',2,57},         //O2        Fixed Freouency,No,Yes
        {'E',40,9},         //E4.0     Tacho Freq
        {'E',41,10},        //E4.1     Max Load
        {'E',41,11},        //E4.1     Min Load
        {'E',20,41},        //E2.0    Date
        {'E',20,42},        //E2.0    Month
        {'E',40,43},        //E4.0    Year
        {'E',20,44},        //E2.0    Hour
        {'E',20,45},        //E2.0    Minute
        {'E',20,46},        //E2.0    Second
        {'E',40,12},        //E4.0    Password
        {'F',255,255}
    },

    {    //1 Configuration
        {'O',2,1},          //O2    PF select Feeding Mode
        {'O',2,47},         //O2    Pre-Hopper select
        {'E',41,13},        //E4.1    Rev Time
        {'E',41,14},        //E4.1    Zero Limit
        {'E',41,15},        //E4.1    Belt Track
        {'E',41,16},        //E4.1    Run Time
        {'O',3,17},         //O3    Pulse1
        {'O',4,18},         //O4    Pulse Width
        {'O',2,19},         //O2    Pulse2
        {'O',3,73},         //O3    AINO Value
        {'O',3,74},         //O3    AO NO Value
        {'O',2,48},         //O2    PI Value
        {'O',7,20},         //O7    AO1 Value
        {'O',7,21},         //O7    AO2 Value
        {'O',7,72},         //O7    AO3 Value
        {'E',41,22},        //E4.1    Load Limit1
        {'E',41,23},        //E4.1    Load Limit2
        {'E',41,24},        //E4.1    Load Delay
        {'O',2,25},         //O2    Clear Total2
        {'O',2,27},         //O2 Mode Select
        {'O',7,66},         //O7    A/D Conversion
        {'O',5,67},         //O5    Filter Order
        {'E',31,68},        //E3.1    Cut off Freq
        {'O',9,69},         //O9 LC_Filter
        {'O',6,0},          //O5 Display_Filter
        {'F',255,255}
    },
    {    //2 Hopper Parameters
        {'E',50,60},        //E5.0    Hopper Capacity
        {'E',20,61},        //E2.0    No of revolutions for auto calibration
        {'O',5,75},         //O5      Filter Order
        {'E',31,76},        //E3.1    Cut off Freq
        {'O',9,77},         //O9 LC_Filter
        {'E',41,78},        //E4.1    Limit Low
        {'E',41,79},        //E4.1    Limit Nominal
        {'E',41,80},        //E4.1    Limit Max
        {'E',41,81},        //E4.1    Limit Delay
        {'F',255,255}
    },
    {    //3 Communication
        {'O',9,70},         //O2    Inverter
        {'O',9,71},         //O2    Inverter
        {'O',4,29},         //O3    SIO (PLC)_Interface
        {'O',3,30},         //O5    SIO Baud
        {'O',3,31},         //O3    SIO Setup Parity
        {'E',20,32},        //E2.0    SIO Address
        {'E',21,33},        //E2.1    Monitor time
        {'E',30,85},        //E3.0    IP Address1
        {'E',30,86},        //E3.0    IP Address2
        {'E',30,87},        //E3.0    IP Address3
        {'E',30,88},        //E3.0    IP Address4
        {'E',20,89},        //E2.0    Profibus Node Address
        {'O',10,90},        //O5    Profibus Baud
        {'F',255,255}
    },
    {    //4 Loadcell Calibration
        {'F',255,255}
    },
    {    //5 AI Calibration
        {'F',255,255}
    },
    {    //6 AO Calibration
        {'F',255,255}
    },
    {    //7 PLC Program
        {'F',255,255}
    },
    {    //8 Performance Parameters
        {'E',41,34},        //E4.1    Int Time
        {'E',41,35},        //E4.1    Prop Gain
        {'E',41,36},        //E4.1    Dev Limit
        {'E',41,37},        //E4.1    Dev Time
        {'E',41,38},        //E4.1    Dead Band
        {'E',41,82},        //E4.1    Min Pulse Time Pre Hopper
        {'O',2,55},         //O3    Regulation
        {'E',41,56},        //E4.0    Reg Track
        {'E',41,49},        //E4.0    BL Limit
        {'E',41,50},        //E4.0    PF Delay
        {'E',41,51},        //E4.0    PF_StartSet
        {'E',41,54},        //E4.0    Setpoint Limit
        {'E',54,83},        //E5.4  Correction Factor
        {'F',255,255}
    },
    {    //9 Setpoint
        {'F',255,255}
    },
    {    //10 Sync
        {'F',255,255}
    },
    {    //11 IO Monitor
        {'F',255,255}
    },
};

// msk main opening menu here
const main_menu_struct Main_menu1[Main_Menu_Max]=
{
    {'M',0,"Main Menu:"},
    {'M',0,"1 System Par"},
    {'M',0,"2 Configuration "},
    {'M',0,"3 Hopper Par"},
    {'M',0,"4 Communication"},
    {'M',0,"5 Loadcell Cal"},
    {'M',0,"6 AI Cal"},
    {'M',0,"7 AO Cal"},
    {'M',0,"8 PLC Program "},
    {'M',0,"9 Performance"},
    {'M',0,"10 Setpoint"},
    {'M',0,"11 Sync"},
    {'M',0,"12 IO Monitor -"},
    {'M',0,"13 SYSTEM -"},
    {'F',255,"9 FF"}
};

const main_menu_struct Main_menu2[Main_Menu_Max]=
{
    {'M',0,"Main Menu:"},
    {'M',0,"System Par"},
    {'M',0,"Configuration"},
    {'M',0,"Hopper Par"},
    {'M',0,"Communication"},
    {'M',0,"Loadcell Cal"},
    {'M',0,"AI Cal"},
    {'M',0,"AO Cal"},
    {'M',0,"PLC Program"},
    {'M',0,"Performance"},
    {'M',0,"Setpoint"},
    {'M',0,"Sync"},
    {'M',0,"IO Monitor"},
    {'F',255,"9 FF"}
};

const main_menu_struct Online_Main_menu[Main_Menu_Max]=
{
    {'M',0,"Main Menu:"},
    {'M',0,"1 Performance"},
    {'M',0,"2 Setpoint"},
    {'M',0,"3 IO Monitor"},
    {'F',255,"9 FF"}
};

char const Online_menu_options[5][14][10][20]=
{
    {    //0 Performance Parameter
        {{"Int Time\0"}},
        {{"Prop Gain\0"}},
        {{"DEV Limit\0"}},
        {{"DEV Time\0"}},
        {{"Dead Band\0"}},
        {{"Min_Pulse\0"}},
        {{"Regulation\0"},{"WF_Set\0"},{"Drive_Set\0"}},
        {{"Reg Tracking\0"}},
        {{"BL Limit\0"}},
        {{"PF Delay\0"}},
        {{"PF StartSet\0"}},
        {{"PF Limit\0"}},
        {{"Cor Factor\0"}}
    },
    {    //1 Setpoints
        {{"Setpoint Int >\0"}},
        {{"Setpoint Man >.\0"}}
    },
    {    //2 IO Monitor
        {{"IO Monitor\0"}}
    },
};

// msk submenu texts here
char const sub_menu_options1[13][26][12][20]=
{
    {    // 0 System Paramters
        {{"Units,Capacit\0"},{"kg/h\0"},{"t/h\0"}},
        {{"Dec pt,Capacity\0"},{"0000\0"},{"000.0\0"},{"00.00\0"},{"0.000\0"}},
        {{"Capacity\0"}},
        {{"Dec pt,Tot 1\0"},{"000000\0"},{"00000.0\0"},{"0000.00\0"},{"000.000\0"}},
        {{"Units,Total1\0"},{"kg\0"},{"t\0"}},
        {{"Dec pt,Total2\0"},{"000000\0"},{"00000.0\0"},{"0000.00\0"},{"000.000\0"}},
        {{"Units,Total2\0"},{"kg\0"},{"t\0"}},
        {{"Tacho ip\0"},{"Tacho\0"},{"Inv\0"}},
        {{"Fixed_Freq\0"},{"No\0"},{"Yes\0"}},
        {{"Tacho Freq\0"}},
        {{"Max Load\0"}},
        {{"Min Load\0"}},
        {{"Date"}},
        {{"Month"}},
        {{"Year"}},
        {{"Hour"}},
        {{"Minute"}},
        {{"Second"}},
        {{"Password"}}
    },
    {    // 1 Configuration
        {{"Prefeeder\0"},{"No\0"},{"Yes\0"}},
                    {{"PreHopper\0"},{"No\0"},{"Yes\0"}},
                    {{"Rev Time\0"}},                                                                            //[0][3][-]
                    {{"Zero Limit\0"}},                                                                      //[0][1][-]
                    {{"Belt Track\0"}},
          {{"Run Time\0"}},
                    {{"Pulse 1\0"},{"Off\0"},{"S1\0"}, {"S2\0"}},
                    {{"Pulse Width\0"},{"50ms\0"},{"100ms\0"},{"250ms\0"}, {"500ms\0"}},
                    {{"Pulse 2\0"},{"Off\0"},{"ON\0"}},
                    {{"AI Nos\0"},{"1\0"},{"2\0"}, {"3\0"}},
                    {{"AO Nos\0"},{"1\0"},{"2\0"}, {"3\0"}},

                    {{"PI\0"},{"Analog\0"},{"Relay\0"}},
                    //{{"AI2 Value\0"},{"FCG Pot\0"}},

                    {{"AO1 Value\0"},{"tph\0"},{"BB\0"},{"Tacho\0"},{"WFsp\0"},{"PFsp\0"},{"Hop_Levl\0"},{"Valve_Postn\0"} },
                    {{"AO2 Value\0"},{"tph\0"},{"BB\0"},{"Tacho\0"},{"WFsp\0"},{"PFsp\0"},{"Hop_Levl\0"},{"Valve_Postn\0"} },
                    {{"AO3 Value\0"},{"tph\0"},{"BB\0"},{"Tacho\0"},{"WFsp\0"},{"PFsp\0"},{"Hop_Levl\0"},{"Valve_Postn\0"} },

                    {{"Load Limit1\0"}},
                    {{"Load Limit2\0"}},
                    {{"Load Delay\0"}},
          {{"Clear Total2\0"},{"NO\0"},{"Yes\0"}},
                    {{"Mode Select"},{"Int\0"},{"Ext\0"}},
                    {{"A/D Conver\0"}, {"6.25\0"}, {"12.5\0"}, {"25\0"}, {"50\0"}, {"100\0"}, {"200\0"}, {"400\0"}},
                    {{"Filter Order\0"},{"0\0"},{"1\0"},{"2\0"},{"3\0"},{"4\0"}},
                    {{"Cut Off Freq\0"}},
                    {{"LC_Filter\0"},{"0\0"},{"1\0"},{"2\0"},{"3\0"},{"4\0"},{"5\0"},{"6\0"},{"7\0"},{"8\0"}},
                    {{"Display_Filter\0"},{"0\0"},{"1\0"},{"2\0"},{"3\0"},{"4\0"},{"5\0"}}
                    //{{"\0"},{"OFF\0"},{"4I/4O\0"},{"8I/8O\0"},

                },
            {      // 2 Hopper Parmeters
                {{"Hopper Capacity\0"}},
                {{"No Rev\0"}},
                {{"Filter Order\0"},{"0\0"},{"1\0"},{"2\0"},{"3\0"},{"4\0"}},
                {{"Cut Off Freq\0"}},
                {{"LC_Filter\0"},{"0\0"},{"1\0"},{"2\0"},{"3\0"},{"4\0"},{"5\0"},{"6\0"},{"7\0"},{"8\0"}},
                {{"Limit_Low\0"}},
                {{"limit Nominal\0"}},
                {{"Limit_Max"}},
                {{"Limit_Delay\0"},}

            },

            {      // 3 Communication
                    {{"Inv1\0"},{"OFF\0"},{"ATV312\0"},{"PF4M\0"},{"ACS550\0"},{"J1000\0"},{"J7 Mini\0"},{"ACS380\0"},{"ACS560\0"},{"ACS580\0"}},
                    {{"Inv2\0"},{"OFF\0"},{"ATV312-\0"},{"PF4M\0"},{"ACS550\0"},{"J1000\0"},{"J7 Mini\0"},{"ACS380\0"},{"ACS560\0"},{"ACS580\0"}},
//                    {{"DIO Module\0"},{"OFF\0"},{"4Ip/4Op\0"},{"8Ip/8Op\0"}},
                    {{"SIO Interface\0"},{"Off\0"},{"Modbus-RTU\0"},{"Modbus-TCP\0"},{"Profibus-DP\0"}},
                    {{"SIO Baud rate\0"},{"19200\0"},{"38400\0"},{"57600\0"}},
                    {{"SIO Setup\0"},{"8-1-Np\0"},{"8-1-Ep\0"},{"8-1-Op\0"}},
                    {{"SIO Address\0"}},
                    {{"SIO Monitor\0"}},
                    {{"IP Address1\0"}},
                    {{"IP Address2\0"}},
                    {{"IP Address3\0"}},
                    {{"IP Address4\0"}},
                    {{"Node Address\0"}},
                    {{"Profibus Baud \0"},{"9.6Kbps\0"},{"19.2Kbps\0"},{"45.45Kbps\0"},{"93.75Kbps\0"},{"187.5Kbps\0"},{"500Kbps\0"},{"1.5Mbps\0"},{"3Mbps\0"},{"6Mbps\0"},{"12Mbps\0"}}

            },
            {        // 4 Loadcell Calibration
                "Loadcell Calibration"
      },
            {        // 5 AI Calibration
                "AI Calibration"
      },
            {   // 6 AO Calibration
                "AO Calibration"
      },
            {        // 7 PLC Program
                "AI Calibration"
      },
            {   // 8 Performance Parameter
                {{"Int Time\0"}},
                {{"Prop Gain\0"}},
                {{"DEV Limit\0"}},
                {{"DEV Time\0"}},
                {{"Dead Band\0"}},
                {{"Min_Pulse\0"}},
                //PF Parameters
                {{"Regulation\0"},{"WF_Set\0"},{"Drive_Set\0"}},
                {{"Reg Tracking\0"}},
                {{"BL Limit\0"}},
                {{"PF Delay\0"}},
                {{"PF StartSet\0"}},
                {{"PF Limit\0"}},
                {{"Cor Factor\0"}},
            },
            {   // 9 Setpoints
        {{"Setpoint\0"}},
            //    {{"Setpoint Man\0"}}
            },
            {     // 10 Sync
                {{"Sync\0"}},
            },
            {     // 11 IO Monitor
                {{"IO Monitor\0"}},
            },
            {     // 12 SYSTEM -
                {{"System Update\0"}},
            },

        };
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Cursor_Location (Routine to display Cursor pointer)
//Step 1: Clear Cursor Location of all the 4 lines
//Step 2: Print Cursor('>') at the given line_no
//Note: Font size is fixed. i.e. font10
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//-------------------------------------------------------
void Cursor_Location(char line_no)
{
    char *cor[1] = {">"};
    char *blank[1] = {" "};

    Print_String(blank, 1, 0, font10, 0);
    Print_String(blank, 2, 0, font10, 0);
    Print_String(blank, 3, 0, font10, 0);
    Print_String(blank, 4, 0, font10, 0);
    Print_String(cor, line_no, 0, font10, 0);
}
//-------------------------------------------------------

//-------------------------------------------------------
void clear_lcd_temp(void)
{
//  Clear_String(&clear_string_temp,line1,0,font10);
//    Clear_String(&clear_string_temp,line2,0,font10);
//    Clear_String(&clear_string_temp,line3,0,font10);
//    Clear_String(&clear_string_temp,line4,0,font10);
//    Clear_String(&clear_string_temp,line5,0,font10);
//    Clear_String(&clear_string_temp,line6,0,font10);
}
//-------------------------------------------------------

/*Send parameter listing to pc via RS232 */
//-------------------------------------------------------
void Parameter_Upload(void)
{
    unsigned char i,j,m,Line_No;
    return; //msk 03-may-2026 disabled for memory issue
    NVIC_DisableIRQ(TIMER0_IRQn);
    NVIC_DisableIRQ(TIMER1_IRQn);
    NVIC_DisableIRQ(TIMER2_IRQn);
    NVIC_DisableIRQ(UART0_IRQn);
    NVIC_DisableIRQ(UART1_IRQn);
    NVIC_DisableIRQ(UART3_IRQn);
    NVIC_DisableIRQ(SPI_IRQn);
    prog_counter = 0;
    for (Line_No = 0; ((plc_inst[Line_No][2] != '*') && (Line_No <= 200)); Line_No++)
    {
        prog_counter++;
    }
    rtc_read_routine();

    sprintf(A_String,"Parameter List Ver1.H\r\n");
    P_String = A_String;
    m= USBD_CDC_ACM_WriteData(0U, (P_String), strlen(P_String));

    sprintf(A_String,"Date:%d%d/%d%d/%u\r\n",(rtc_read.DOM/10),(rtc_read.DOM % 10 ),((rtc_read.MONTH /10 )),((rtc_read.MONTH %10)),rtc_read.YEAR );
    P_String = A_String;
    m= USBD_CDC_ACM_WriteData(0U, (P_String), strlen(P_String));

    sprintf(A_String,"Time:%d%d:%d%d:%d%d\r\n",((rtc_read.HOUR /10)),((rtc_read.HOUR %10 )),((rtc_read.MIN /10)),((rtc_read.MIN %10)),((rtc_read.SEC /10)),((rtc_read.SEC %10)));
    P_String = A_String;
    m= USBD_CDC_ACM_WriteData(0U, (P_String), strlen(P_String));

    for(i=0;i<12;i++)
        {
            if(i==9)
            {
                sprintf(A_String,"%u~%u~%u~%u~\r\n",10,1,Setpoint_Int,Dec_Point_Capacity);
                P_String = A_String;
                m= USBD_CDC_ACM_WriteData(0U, (P_String), strlen(P_String));

                sprintf(A_String,"%u~%u~%u~%u~\r\n",10,2,Setpoint_Man,Dec_Point_Capacity);
                P_String = A_String;
                m= USBD_CDC_ACM_WriteData(0U, (P_String), strlen(P_String));
            }
            else if( ( i==4 ) ||(i==5) ||(i==6)  ||(i==10) ||(i==11))
            {
                sprintf(A_String,"%u~%u~%u~%u~\r\n",i+1,0,0,0);
                P_String = A_String;
                m= USBD_CDC_ACM_WriteData(0U, (P_String), strlen(P_String));
            }
            else if(i==7)
            {
                sprintf(A_String,"%u~%u~\r\n",i+1,prog_counter);
                P_String = A_String;
                m= USBD_CDC_ACM_WriteData(0U, (P_String), strlen(P_String));
                for(Line_No=0;((plc_inst[Line_No][2]!='*')&&(Line_No<=200));Line_No++)
                {
                    sprintf(A_String,"%u~%c%c%c%c%c%c\r\n",Line_No,plc_inst[Line_No][0],plc_inst[Line_No][1],plc_inst[Line_No][2],plc_inst[Line_No][3],plc_inst[Line_No][4],plc_inst[Line_No][5]);
                    P_String = A_String;
                    m= USBD_CDC_ACM_WriteData(0U, (P_String), strlen(P_String));
                }
                sprintf(A_String,"%u~%c%c%c\r\n",prog_counter,'*','*','*');
                P_String = A_String;
                m= USBD_CDC_ACM_WriteData(0U, (P_String), strlen(P_String));
            }
            else
            {
                for(j=0;j<= no_menu_par[i];j++)
                {
                    if(((sub_menu1[i][j].variable_offset)!=41 )&&((sub_menu1[i][j].variable_offset)!=42)&&((sub_menu1[i][j].variable_offset)!=43)
                            &&((sub_menu1[i][j].variable_offset)!=44 )&&((sub_menu1[i][j].variable_offset)!=45)&&((sub_menu1[i][j].variable_offset)!=46))
                    {
                        val_of_par = New_menu_parameters[(sub_menu1[i][j].variable_offset)];
                        if(((sub_menu1[i][j].variable_offset))==60)
                        {
                            ((uint16_t*)&(val_of_par ))[1]= New_menu_parameters[52] ;
                            ((uint16_t*)&(val_of_par ))[0]= New_menu_parameters[84] ;
                        }
                        if ((sub_menu1[i][j].par_type) == 'E')
                        {
                            par_type = 'E';
                            par_dec_pt = ((sub_menu1[i][j].par_options) % 10);
                        }
                        else
                        {
                            par_dec_pt = 0;
                        }
                        sprintf(A_String,"%u~%u~%u~%u~\r\n",i+1,j+1,val_of_par,par_dec_pt);
                        P_String = A_String;
                        m= USBD_CDC_ACM_WriteData(0U, (P_String), strlen(P_String));
                    }
                }
            }
        }
        sprintf(A_String,"Tare_Count_Ch1:%lu\r\n",zero_load);
        P_String = A_String;
        m= USBD_CDC_ACM_WriteData(0U, (P_String), strlen(P_String));

        sprintf(A_String,"Span_Count_Ch1:%lu\r\n",full_load);
        P_String = A_String;
        m= USBD_CDC_ACM_WriteData(0U, (P_String), strlen(P_String));

        sprintf(A_String,"Tare_Count_Ch2:%lu\r\n",zero_load_ch2);
        P_String = A_String;
        m= USBD_CDC_ACM_WriteData(0U, (P_String), strlen(P_String));

        sprintf(A_String,"Span_Count_Ch2:%lu\r\n",full_load_ch2);
        P_String = A_String;
        m= USBD_CDC_ACM_WriteData(0U, (P_String), strlen(P_String));

        sprintf(A_String,"%c\r\n",'}');
        P_String = A_String;
        m= USBD_CDC_ACM_WriteData(0U, (P_String), strlen(P_String));

        NVIC_EnableIRQ(TIMER0_IRQn);
        NVIC_EnableIRQ(TIMER1_IRQn);
        NVIC_EnableIRQ(TIMER2_IRQn);
        NVIC_EnableIRQ(UART0_IRQn);
        NVIC_EnableIRQ(UART1_IRQn);
        NVIC_EnableIRQ(UART3_IRQn);
        NVIC_EnableIRQ(SPI_IRQn);

    (void)m;
}
//-------------------------------------------------------

//-------------------------------------------------------
void Parameter_Download(void)
{
    uint32_t ptr_counter = 49, Temp_val_par,No_Of_Lines;
    unsigned char adress_count = 0,i = 0,j = 0,m,col_no,row_no;
    return; //msk 03-may-2026 disabled for memory issue 
    Dwnd_flag = 1;
    NVIC_DisableIRQ(TIMER0_IRQn);
    NVIC_DisableIRQ(TIMER1_IRQn);
    NVIC_DisableIRQ(TIMER2_IRQn);
    NVIC_DisableIRQ(UART0_IRQn);
    NVIC_DisableIRQ(UART1_IRQn);
    NVIC_DisableIRQ(UART3_IRQn);
    NVIC_DisableIRQ(SPI_IRQn);

//            sprintf(A_String,"Send File\n \r");
//            P_String = A_String;
//            UART_Send((LPC_UART_TypeDef *)LPC_UART2, P_String, strlen(P_String), BLOCKING);

    while (Dwnd_flag == 0);

    if ((download_menu_par[0] == 'P') && (download_menu_par[18] == '1') && (download_menu_par[19] == '.') && (download_menu_par[20] == 'E') && (par_count < 2683))
    {
        while (ptr_counter < par_count)
        {
            switch (adress_count)
            {
                case 0:
                    i = (download_menu_par[ptr_counter] - 0x30);
                    ptr_counter++;
                    if (download_menu_par[ptr_counter] == '~')
                    {
                        adress_count++;
                    }
                    else
                    {
                        i = ((i << 4) | (download_menu_par[ptr_counter] - 0x30));
                        ptr_counter++;
                        if (download_menu_par[ptr_counter] == '~')
                        {
                            adress_count++;
                        }
                    }

                    i = ((((i & 0xf0) >> 4) * 10) + (((i & 0x0f) >> 0) * 1));
                    i = i - 1;

                    if ((i == 4) || (i == 5) || (i == 6) || (i == 7) || (i == 10) || (i == 11))
                    {
                        if (i == 11)
                        {
                            ptr_counter = ptr_counter + 8;
                        }
                        else if (i == 7)
                        {
                            ptr_counter++;
                            No_Of_Lines = (download_menu_par[ptr_counter] - 0x30);
                            ptr_counter++;
                            if (download_menu_par[ptr_counter] != '~')
                            {
                                No_Of_Lines = ((No_Of_Lines << 4) | (download_menu_par[ptr_counter] - 0x30));
                                ptr_counter++;
                                if (download_menu_par[ptr_counter] != '~')
                                {
                                    No_Of_Lines = ((No_Of_Lines << 4) | (download_menu_par[ptr_counter] - 0x30));
                                    ptr_counter++;
                                }
                            }
                            No_Of_Lines = ((((No_Of_Lines & 0x0f00) >> 8) * 100) + (((No_Of_Lines & 0x00f0) >> 4) * 10) + (((No_Of_Lines & 0x000f) >> 0) * 1));
                            No_Of_Lines = No_Of_Lines - 1;
                            ptr_counter = ptr_counter - 3;
                            ptr_counter = ptr_counter + 4;
                            for (row_no = 0; row_no < No_Of_Lines + 1; row_no++)
                            {
                                if (row_no > 9)
                                {
                                    ptr_counter = ptr_counter + 1;
                                }
                                if (row_no > 99)
                                {
                                    ptr_counter = ptr_counter + 1;
                                }
                                ptr_counter = ptr_counter + 2;
                                for (col_no = 0; col_no < 6; col_no++)
                                {
                                    plc_inst[row_no][col_no] = download_menu_par[ptr_counter];
                                    ptr_counter++;
                                }
                            }
                            if (row_no > 9)
                            {
                                ptr_counter = ptr_counter + 1;
                            }
                            if (row_no > 99)
                            {
                                ptr_counter = ptr_counter + 1;
                            }
                            ptr_counter = ptr_counter + 2;
                            plc_inst[row_no][0] = download_menu_par[ptr_counter];
                            ptr_counter = ptr_counter + 1;
                            plc_inst[row_no][1] = download_menu_par[ptr_counter];
                            ptr_counter = ptr_counter + 1;
                            plc_inst[row_no][2] = download_menu_par[ptr_counter];
                            ptr_counter = ptr_counter + 1;
                            plc_inst[row_no][3] = 0;
                            plc_inst[row_no][4] = 0;
                            plc_inst[row_no][5] = 0;
                        }
                        else
                        {
                            ptr_counter = ptr_counter + 7;
                        }
                        adress_count = 0;
                    }
                    break;
                case 1:
                    ptr_counter++;
                    if (download_menu_par[ptr_counter] == '~')
                    {
                        adress_count++;
                    }
                    else
                    {
                        j = (download_menu_par[ptr_counter] - 0x30);
                        ptr_counter++;
                        if (download_menu_par[ptr_counter] == '~')
                        {
                            adress_count++;
                        }
                        else
                        {
                            j = ((j << 4) | (download_menu_par[ptr_counter] - 0x30));
                            ptr_counter++;
                            if (download_menu_par[ptr_counter] == '~')
                            {
                                adress_count++;
                            }
                        }
                        j = ((((j & 0xf0) >> 4) * 10) + (((j & 0x0f) >> 0) * 1));
                        j = j - 1;
                    }
                    break;
                case 2:
                    ptr_counter++;
                    if (download_menu_par[ptr_counter] == '~')
                    {
                        adress_count++;
                    }
                    else
                    {
                        Temp_val_par = download_menu_par[ptr_counter] - 0x30;
                        ptr_counter++;
                        if (download_menu_par[ptr_counter] == '~')
                        {
                            adress_count++;
                        }
                        else
                        {
                            Temp_val_par = ((Temp_val_par << 4) | (download_menu_par[ptr_counter] - 0x30));
                            ptr_counter++;
                            if (download_menu_par[ptr_counter] == '~')
                            {
                                adress_count++;
                            }
                            else
                            {
                                Temp_val_par = ((Temp_val_par << 4) | (download_menu_par[ptr_counter] - 0x30));
                                ptr_counter++;
                                if (download_menu_par[ptr_counter] == '~')
                                {
                                    adress_count++;
                                }
                                else
                                {
                                    Temp_val_par = ((Temp_val_par << 4) | (download_menu_par[ptr_counter] - 0x30));
                                    ptr_counter++;
                                    if (download_menu_par[ptr_counter] == '~')
                                    {
                                        adress_count++;
                                    }
                                    else
                                    {
                                        Temp_val_par = ((Temp_val_par << 4) | (download_menu_par[ptr_counter] - 0x30));
                                        ptr_counter++;
                                        if (download_menu_par[ptr_counter] == '~')
                                        {
                                            adress_count++;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    break;
                case 3:
                    ptr_counter++;
                    if (download_menu_par[ptr_counter] == '~')
                    {
                        adress_count++;
                    }
                    break;

                case 4:
                    ptr_counter++;
                    if (download_menu_par[ptr_counter] == '~')
                    {
                        adress_count = 0;
                    }
                    if (i == 9)
                    {
                        Temp_val_par = ((((Temp_val_par & 0xf000) >> 12) * 1000) + (((Temp_val_par & 0x0f00) >> 8) * 100) + (((Temp_val_par & 0x00f0) >> 4) * 10) + (((Temp_val_par & 0x000f) >> 0) * 1));
                        if (j == 0)
                        {
                            Setpoint_Int = (Temp_val_par);
                        }
                        if (j == 1)
                        {
                            Setpoint_Man = (Temp_val_par);
                        }
                        adress_count = 0;
                    }
                    else
                    {
                        Temp_val_par = ((((Temp_val_par & 0xf0000) >> 16) * 10000) + (((Temp_val_par & 0xf000) >> 12) * 1000) + (((Temp_val_par & 0x0f00) >> 8) * 100) + (((Temp_val_par & 0x00f0) >> 4) * 10) + (((Temp_val_par & 0x000f) >> 0) * 1));
                        New_menu_parameters[(sub_menu1[i][j].variable_offset)] = (Temp_val_par);
                        if ((sub_menu1[i][j].variable_offset) == 60)
                        {
                            Capacity_1 = Temp_val_par;
                            New_menu_parameters[52] = ((uint16_t*)&(Capacity_1))[1];
                            New_menu_parameters[84] = ((uint16_t*)&(Capacity_1))[0];
                        }
                        adress_count = 0;
                    }
                    break;

                default:
                    break;
            }
        }

        New_menu_parameters[41] = ((((download_menu_par[26] - 0x30) * 10) + ((download_menu_par[27] - 0x30) * 1)) & 0x00ff);
        New_menu_parameters[42] = ((((download_menu_par[29] - 0x30) * 10) + ((download_menu_par[30] - 0x30) * 1)) & 0x00ff);
        New_menu_parameters[43] = ((((download_menu_par[32] - 0x30) * 1000) + ((download_menu_par[33] - 0x30) * 100) + ((download_menu_par[34] - 0x30) * 10) + ((download_menu_par[35] - 0x30) * 1)) & 0xffff);
        New_menu_parameters[44] = ((((download_menu_par[41] - 0x30) * 10) + ((download_menu_par[42] - 0x30) * 1)) & 0x00ff);
        New_menu_parameters[45] = ((((download_menu_par[44] - 0x30) * 10) + ((download_menu_par[45] - 0x30) * 1)) & 0x00ff);
        New_menu_parameters[46] = ((((download_menu_par[47] - 0x30) * 10) + ((download_menu_par[48] - 0x30) * 1)) & 0x00ff);

        rtc_read.DOM = New_menu_parameters[41];
        rtc_read.MONTH = New_menu_parameters[42];
        rtc_read.YEAR = New_menu_parameters[43];
        rtc_read.HOUR = New_menu_parameters[44];
        rtc_read.MIN = New_menu_parameters[45];
        rtc_read.SEC = New_menu_parameters[46];

        rtc_write_routine(rtc_read);
        save_plc_edit_eeprom();

        for (par_count = 0; par_count < 770; par_count++)
        {
            download_menu_par[par_count] = 0;
        }
        Dwnd_Error = 0;
        Dwnd_flag = 0;
        par_count = 0;
        ptr_counter = 0;
        cnt_to_wrap_1 = 0;
        sprintf(A_String, "Downloaded\r\n ");
        P_String = A_String;
        j = 0;
        m = USBD_CDC_ACM_WriteData(0U, (P_String), strlen(P_String));
    }
    else
    {
        for (par_count = 0; par_count < 770; par_count++)
        {
            download_menu_par[par_count] = 0;
        }
        Dwnd_Error = 1;
        Dwnd_flag = 0;
        par_count = 0;
        ptr_counter = 0;
        cnt_to_wrap_1 = 0;
        sprintf(A_String, "Error not in proper Format\r\n ");
        P_String = A_String;
        m = USBD_CDC_ACM_WriteData(0U, (P_String), strlen(P_String));
    }
    NVIC_EnableIRQ(TIMER0_IRQn);
    NVIC_EnableIRQ(TIMER1_IRQn);
    NVIC_EnableIRQ(TIMER2_IRQn);
    NVIC_EnableIRQ(UART0_IRQn);
    NVIC_EnableIRQ(UART1_IRQn);
    NVIC_EnableIRQ(UART3_IRQn);
    NVIC_EnableIRQ(SPI_IRQn);
    Dwnd_flag = 0;
    (void)m;
}
//-------------------------------------------------------
void Firmware_Update_Menu()
{
    char sys_sts;
    char fwrite[5]={"FWR  "};
    sys_sts=1;
    Key_Input=NO_KEY;
    Clear_LCD();
    super_delay(20);
    calib_string_ptr = "Firmware Upgrade";
    Print_String(&calib_string_ptr,line1,0,font16,0);
    calib_string_ptr = "1 Upgrade <>";
    Print_String(&calib_string_ptr,line3,0,font10,0);
    calib_string_ptr = "2 Exit";
    Print_String(&calib_string_ptr,line4,0,font10,0);
    calib_string_ptr = "Connect Pendrive ";
    Print_String(&calib_string_ptr,line5,0,font10,0);
    calib_string_ptr = "Return - Exit";
    Print_String(&calib_string_ptr,line6,0,font10,0);
    sys_sts=1;
    key_check_routine(); //check for key press
        eeprom_read_block(8180,(unsigned char *)&FWBLOCK[0],3);
    sys_jump1:
    if(flag==1)//flag becomes 1 after every 5 ms (Timer0 tick)
    {
        flag=0;
        Key_Input=NO_KEY;
        if(scan_count%25 ==0)  //every 35th/20th scan
        {
            key_check_routine(); //check for key press
        }

        if(    Key_Input==NO_KEY)
        {
            goto sys_jump3;              //display calibration routine
        }
        if((Key_Input==K_DEC)&&(sys_sts==1))                         // //Chanel 1<> selection  display
        {
            sys_sts=2;
            Key_Input=NO_KEY;
            Clear_LCD();
            super_delay(20);
            calib_string_ptr = "Firmware Upgrade";
            Print_String(&calib_string_ptr,line1,0,font16,0);
            calib_string_ptr = "1 Upgrade   ";
            Print_String(&calib_string_ptr,line3,0,font10,0);
            calib_string_ptr = "2 Exit <>";
            Print_String(&calib_string_ptr,line4,0,font10,0);
            calib_string_ptr = "CLEAR - Exit";
            Print_String(&calib_string_ptr,line6,0,font10,0);
            key_check_routine(); //check for key press
        }
        else if((Key_Input==K_INC)&&(sys_sts==2))                         // //Chanel 1<> selection  display
        {
            sys_sts=1;
            Key_Input=NO_KEY;
            Clear_LCD();
            super_delay(20);
            calib_string_ptr = "Firmware Upgrade";
            Print_String(&calib_string_ptr,line1,0,font16,0);
            calib_string_ptr = "1 Upgrade <> ";
            Print_String(&calib_string_ptr,line3,0,font10,0);
            calib_string_ptr = "2 Exit   ";
            Print_String(&calib_string_ptr,line4,0,font10,0);
            calib_string_ptr = "CLEAR - Exit";
            Print_String(&calib_string_ptr,line6,0,font10,0);
            key_check_routine(); //check for key press
        }
        else if((Key_Input==HOR)&&(sys_sts==1))              // //Chanel 2<> selection display
        {
            sys_sts = 3;
            Key_Input=NO_KEY;
            Clear_LCD();
            super_delay(20);
            calib_string_ptr = "Connect PENDRIVE";
            Print_String(&calib_string_ptr,line2,0,font10,0);
            calib_string_ptr = "THEN TURN OFF";
            Print_String(&calib_string_ptr,line3,0,font10,0);
            calib_string_ptr = "........";
            Print_String(&calib_string_ptr,line4,0,font10,0);
            eeprom_write_array(8180,(unsigned char *)&fwrite[0],3);
            super_delay(20);
            ////Parameter_Upload();
            super_delay(60);
            super_delay(60);

            return;
        }
        else if((Key_Input==HOR)&&(sys_sts==2))              // //Chanel 2<> selection display
        {
            sys_sts = 4;
            Key_Input=NO_KEY;
            Clear_LCD();
            super_delay(20);
            calib_string_ptr = "exiting..";
            Print_String(&calib_string_ptr,line2,0,font16,0);
            calib_string_ptr = "no update";
            Print_String(&calib_string_ptr,line3,0,font16,0);
            calib_string_ptr = "........";
            Print_String(&calib_string_ptr,line4,0,font16,0);
            super_delay(20);
            //Parameter_Download();
            super_delay(60);
            Clear_LCD();
            calib_string_ptr = "Return - Exit";
            Print_String(&calib_string_ptr,line6,0,font16,0);
            return;
        }

            if(Key_Input==CL)
            {
                return;
            }
            super_delay(20);
    }

    if (scan_count >= 200)
    {
        scan_count = 0;
    }

sys_jump3:
    goto sys_jump1;
}
//-------------------------------------------------------
void AO_Module_Select()
{
    unsigned char i;
    unsigned char ao_type = AO_Type;
    const char* ao_names[] = {
        "1> UI0-2AO(VEAVE)",
        "2> WIN-IO-2AOMC   WittelIB",
        "3> Internal       (Unavailable)"
    };
    const unsigned char ao_type_count = 3;
    bool running = true;
    char line3str[18] = {0};
    char line4str[18] = {0};
    const char* name;
    Clear_LCD();
    super_delay(20);
    while(running)
    {
        Key_Input = NO_KEY;
        //Clear_LCD();
        calib_string_ptr = "AO_MODULE SELECT";
        Print_String(&calib_string_ptr, line1, 0, font10, 0);
        calib_string_ptr = "INC key to select";
        Print_String(&calib_string_ptr, line2, 0, font10, 0);
        super_delay(10);
        name = ao_names[ao_type];
        if(strlen(name) <= 17) {
            calib_string_ptr = (char*)name;
            Print_String(&calib_string_ptr, line3, 0, font10, 0);
        }
        else
        {
            memset(line3str, 0, sizeof(line3str));
            memset(line4str, 0, sizeof(line4str));
            memcpy(line3str, name, 17);
            line3str[17] = '\0';
            strncpy(line4str, name+17, 17);
            line4str[17] = '\0';
            calib_string_ptr = line3str;
            Print_String(&calib_string_ptr, line3, 0, font10, 0);
            calib_string_ptr = line4str;
            Print_String(&calib_string_ptr, line4, 0, font10, 0);
        }
        super_delay(30);
        key_check_routine();
        if(Key_Input == K_INC)
        {
            for(i=0;i<18;i++)
            {
                line3str[i]=' ';
                line4str[i]=' ';
            }

            calib_string_ptr = line3str;
            Print_String(&calib_string_ptr, line3, 0, font10, 0);
            calib_string_ptr = line4str;
            Print_String(&calib_string_ptr, line4, 0, font10, 0);
            Clear_LCD();
            ao_type = (ao_type + 1) % ao_type_count;
        }
        else if(Key_Input == HOR) {
            AO_Type = ao_type;
            save_par_eeprom();
            break;
        }
        else if(Key_Input == CL) {
            break;
        }
    }
}
//-------------------------------------------------------
void AI_Module_Select()
{
    bool flag=true;
    Clear_LCD();
    super_delay(20);
    while(flag==true)
    {
        Key_Input=NO_KEY;
        super_delay(30);
        key_check_routine();
        calib_string_ptr = "AI_MODULE SELECT";
        Print_String(&calib_string_ptr,line1,0,font10,0);
        if(Key_Input==CL)
        {
            break;
        }
        super_delay(20);
    }
}
//-------------------------------------------------------
void DIO_Module_Select()
{
    bool flag=true;
    Clear_LCD();
    super_delay(20);
    while(flag==true)
    {
        Key_Input=NO_KEY;
        super_delay(30);
        key_check_routine();
        calib_string_ptr = "DIO_MODULE SELECT";
        Print_String(&calib_string_ptr,line1,0,font10,0);
        if(Key_Input==CL)
        {
            break;
        }
        super_delay(20);
    }
}
//-------------------------------------------------------
void System_Update_Menu()
{

    Clear_LCD();
    super_delay(20);
Menu_Start:
    Key_Input=NO_KEY;
    calib_string_ptr = "Press the Key.";
    Print_String(&calib_string_ptr,line1,0,font10,0);
    calib_string_ptr = "1> Firmware Upgrade";
    Print_String(&calib_string_ptr,line2,0,font10,0);
    calib_string_ptr = "2> AO Module     ";
    Print_String(&calib_string_ptr,line3,0,font10,0);
    calib_string_ptr = "3> AI Module     ";
    Print_String(&calib_string_ptr,line4,0,font10,0);
    calib_string_ptr = "4> DIO Module    ";
    Print_String(&calib_string_ptr,line5,0,font10,0);
    calib_string_ptr = "CLEAR:  - Exit   ";
    Print_String(&calib_string_ptr,line6,0,font10,0);
    //key_check_routine(); //check for key press
        //eeprom_read_block(8180,&FWBLOCK[0],3);
    Menu_Jump1:
    if(flag==1)//flag becomes 1 after every 5 ms (Timer0 tick)
    {
        flag=0;
        Key_Input=NO_KEY;
        super_delay(10);
        //if(scan_count%25 ==0)  //every 35th/20th scan
        //{
            key_check_routine(); //check for key press
        //}

        if(    Key_Input==NO_KEY)
        {
            goto Menu_Jump1;              //display calibration routine
        }
        if(Key_Input==Key1)  //key '1" pressed                       // //Chanel 1<> selection  display
        {
            Clear_LCD();
            Firmware_Update_Menu();
        }
        else if(Key_Input==Key2)
        {
            Clear_LCD();
            AO_Module_Select();
            Clear_LCD();
            goto Menu_Start;
        }
        else if(Key_Input==Key3)
        {
                        Clear_LCD();
            AI_Module_Select();
            Clear_LCD();
            goto Menu_Start;
        }
        else if(Key_Input==Key4)
        {
                        Clear_LCD();
            DIO_Module_Select();
            Clear_LCD();
            goto Menu_Start;
        }
        else if(Key_Input==CL)
        {
            Clear_LCD();
            Key_Input=NO_KEY;
            return;
        }
        super_delay(20);
    }
    if (scan_count >= 200)
    {
        scan_count = 0;
    }
    goto Menu_Start;
}
//-------------------------------------------------------

//-------------------------------------------------------
void Sync(void)
{
char Sync_status;

    Sync_status = 1;
         Key_Input=NO_KEY;
            Clear_LCD();
            super_delay(20);
            calib_string_ptr = "Sync";
            Print_String(&calib_string_ptr,line1,0,font16,0);
            calib_string_ptr = "1 Upload <>";
            Print_String(&calib_string_ptr,line3,0,font10,0);
            calib_string_ptr = "2 Download";
            Print_String(&calib_string_ptr,line4,0,font10,0);
            calib_string_ptr = "Return - Exit";
            Print_String(&calib_string_ptr,line6,0,font10,0);

            super_delay(20);

 Sync_jump: // keep executing loop until 5 ms Timer0 interrupt routine

    if(flag==1)//flag becomes 1 after every 5 ms (Timer0 tick)
                    {
                                    flag=0;
                                        Key_Input=NO_KEY;

                                        if (scan_count % 25 == 0)
                                        {
                                            key_check_routine();
                                        }

                                            if (Key_Input == NO_KEY)
                                            {
                                                goto Sync_jump3;
                                            }

                                                        if((Key_Input==K_DEC)&&(Sync_status==1))                         // //Chanel 1<> selection  display
                                                                {
                                                                    Sync_status=2;
                                                                    Key_Input=NO_KEY;
                                                                        Clear_LCD();
                                                                    super_delay(20);
                                                                    calib_string_ptr = "10 Sync";
                                                                    Print_String(&calib_string_ptr,line1,0,font16,0);
                                                                    calib_string_ptr = "1 Upload";
                                                                    Print_String(&calib_string_ptr,line3,0,font10,0);
                                                                    calib_string_ptr = "2 Download <>";
                                                                    Print_String(&calib_string_ptr,line4,0,font10,0);
                                                                    calib_string_ptr = "Return - Exit";
                                                                        Print_String(&calib_string_ptr,line6,0,font10,0);
                                                                    super_delay(20);

                                                                }
                                                                else if((Key_Input==K_INC)&&(Sync_status==2))              // //Chanel 2<> selection display
                                                                    {
                                                                        Sync_status=1;
                                                                        Key_Input=NO_KEY;
                                                                        Clear_LCD();
                                                                        super_delay(20);
                                                                        calib_string_ptr = "10 Sync";
                                                                        Print_String(&calib_string_ptr,line1,0,font16,0);
                                                                        calib_string_ptr = "1 Upload <>";
                                                                        Print_String(&calib_string_ptr,line3,0,font10,0);
                                                                        calib_string_ptr = "2 Download";
                                                                        Print_String(&calib_string_ptr,line4,0,font10,0);
                                                                        calib_string_ptr = "Return - Exit";
                                                                        Print_String(&calib_string_ptr,line6,0,font10,0);
                                                                        super_delay(20);

                                                                    }
                                                                else if((Key_Input==HOR)&&(Sync_status==1))              // //Chanel 2<> selection display
                                                                    {
                                                                        Sync_status = 3;
                                                                        Key_Input=NO_KEY;
                                                                        Clear_LCD();
                                                                        super_delay(20);
                                                                        calib_string_ptr = "Uploading data";//msk 03-may-2026
                                                                        calib_string_ptr = "Not Available ";//msk 03-may-2026
                                                                        Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                        calib_string_ptr = "Disabled.......";
                                                                        Print_String(&calib_string_ptr,line3,0,font16,0);
                                                                        calib_string_ptr = "........";
                                                                        Print_String(&calib_string_ptr,line4,0,font16,0);
                                                                        super_delay(20);
                                                                        //Parameter_Upload();   //msk 03-may-2026 disabled for memory issue
                                                                        super_delay(60);  
                                                                        Clear_LCD();
                                                                        calib_string_ptr = "Uploaded ";
                                                                        Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                        calib_string_ptr = "Not available...";  //msk 03-may-2026
                                                                        Print_String(&calib_string_ptr,line3,0,font16,0);

                                                                        calib_string_ptr = "Return - Exit";
                                                                        Print_String(&calib_string_ptr,line6,0,font16,0);
                                                                    }
                                                                else if((Key_Input==HOR)&&(Sync_status==2))              // //Chanel 2<> selection display
                                                                    {
                                                                        Sync_status = 4;
                                                                        Key_Input=NO_KEY;
                                                                        Clear_LCD();
                                                                        super_delay(20);
                                                                        calib_string_ptr = "Downloading Disabled";
                                                                        Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                        calib_string_ptr = "Not available..."; //msk 03-may-2026
                                                                        Print_String(&calib_string_ptr,line3,0,font16,0);
                                                                        calib_string_ptr = "........";
                                                                        Print_String(&calib_string_ptr,line4,0,font16,0);
                                                                        super_delay(20);
                                                                       // Parameter_Download(); //msk 03-may-2026 disabled for memory issue
                                                                        super_delay(60);
                                                                        Clear_LCD();
                                                                           if(Dwnd_Error==0)
                                                                            {
                                                                                calib_string_ptr = "Download ";
                                                                                Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                                calib_string_ptr = "disabled...";   
                                                                                Print_String(&calib_string_ptr,line3,0,font16,0);
                                                                            }
                                                                        else
                                                                            {
                                                                                calib_string_ptr = "Err Not Cor Fmt";
                                                                                Print_String(&calib_string_ptr,line2,0,font16,0);
//                                                                                calib_string_ptr = "Successfully";
//                                                                                Print_String(&calib_string_ptr,line3,0,font16,0);

                                                                            }
                                                                        calib_string_ptr = "Return - Exit";
                                                                        Print_String(&calib_string_ptr,line6,0,font16,0);

                                                                    }

                                                                if(Key_Input==ESC)
                                                                     {

                                                                            {
                                                                                return;
                                                                            }

                                                                     }
                                                    }

    if (scan_count >= 200)
    {
        scan_count = 0;
    }

Sync_jump3:
    goto Sync_jump;
}
//-------------------------------------------------------

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Display Main Menu Routine
//Step 1: Clear LCD
//Step 2: Print MENU Heading on Line0
//Step 3: Print String at print_line1 address on line1
//Step 4: Print String at print_line1+1 address on line2
//Step 5: Print String at print_line1+2 address on line3
//Step 6: Print String at print_line1+3 address on line4
//Step 7: Print Notice string on line5
//Step 8: display cursor = (display_cursor - print_line1 + 1)
//Step 9: Call cursor routine
//Note: Font size is fixed. i.e. font10
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//-------------------------------------------------------
void display_main_menu_routine(const main_menu_struct Main_menu[Main_Menu_Max], char print_line1, char display_cursor)
{
    unsigned int disp_cursor;

    Clear_LCD();
    print_menu = *(Main_menu[0].mm_name);
    Print_String(&print_menu, line1, 0, font10, 0);
    print_menu = *(Main_menu[print_line1].mm_name);
    Print_String(&print_menu, line2, 5, font10, 0);
    print_menu = *(Main_menu[print_line1 + 1].mm_name);
    Print_String(&print_menu, line3, 5, font10, 0);
    print_menu = *(Main_menu[print_line1 + 2].mm_name);
    Print_String(&print_menu, line4, 5, font10, 0);
    print_menu = *(Main_menu[print_line1 + 3].mm_name);
    Print_String(&print_menu, line5, 5, font10, 0);
    print_menu = *(Main_menu[print_line1 + 4].mm_name);
    Print_String(&print_menu, line6, 5, font10, 0);

    disp_cursor = display_cursor - print_line1 + 1;
    Cursor_Location(disp_cursor);
    return;
}
//-------------------------------------------------------

//-------------------------------------------------------
void Sub_Menu_Routine(main_menu_struct Main_menu[Main_Menu_Max], char Sub_Menu_no1, sub_menu_struct sub_menu[12][26], char const sub_menu_options[12][26][12][20], int menu_type)
{
    char *no_string[4] = {"/"};
    char total_par;
    char *print_sub_menu;
    unsigned long par_val=0;
    int Sub_Menu_no=Sub_Menu_no1;
    new_pass=0;
    sub_menu_status = 0;
    sub_jumpxx:

    Menu_ServiceInvComms();
    if (scan_count % 40 == 0)
    {
        key_check_routine();

        if (sub_menu_status != 0)
        {
            goto sub_jump1;
        }

    sub_menu_status = 1;
    sub_display_cursor = 0;

    Clear_LCD();

    for (sub_display_cursor = 0; ((sub_menu[Sub_Menu_no - 1][sub_display_cursor + 1].par_type) != 'F'); sub_display_cursor++)
    {
    }
    total_par = sub_display_cursor + 1;
    sub_display_cursor = 0;

    print_sub_menu = *(Main_menu2[Sub_Menu_no].mm_name);
    Print_String(&print_sub_menu, line1, 0, font16, 0);

sub_jump_next_parameter:
    Clear_String(&clear_string, line2, 0, font16);
    print_sub_menu = (char *)(sub_menu_options[Sub_Menu_no - 1][sub_display_cursor][0]);
    Print_String(&print_sub_menu, line2, 5, font16, 0);
    Clear_String(&clear_string, line6, 0, font16);
    Print_Variable(sub_display_cursor + 1, line6, 45, font10, 0);
    Print_String(no_string, line6, 52, font10, 0);
    Print_Variable(total_par, line6, 56, font10, 0);

    if ((sub_menu[Sub_Menu_no - 1][sub_display_cursor].par_type) == 'O')
    {
        par_val = 1 + New_menu_parameters[(sub_menu[Sub_Menu_no - 1][sub_display_cursor].variable_offset)];
        if (sub_menu[Sub_Menu_no - 1][sub_display_cursor].par_options < par_val)
        {
            par_val = 1;
        }
        {
                                Clear_String(&clear_string, line3, 0,font16);
                                print_sub_menu =(char *)(sub_menu_options[Sub_Menu_no-1][sub_display_cursor][par_val]);
                                Print_String(&print_sub_menu,line3,10,font16,0);

                     }
                goto sub_jumpxx;
    }
else    if((sub_menu[Sub_Menu_no-1][sub_display_cursor].par_type) =='E')    //ENTRY PARAMTER
 {
                par_val = New_menu_parameters[(sub_menu[Sub_Menu_no-1][sub_display_cursor].variable_offset)];    //as at offset '0' Paramter Name is saved
                if((sub_menu[Sub_Menu_no-1][sub_display_cursor].variable_offset)==60)//Hopper Capacity
                    {
                            ((uint16_t*)&(par_val))[1]= New_menu_parameters[52] ;
                            ((uint16_t*)&(par_val))[0]= New_menu_parameters[84] ;
                        //Hopper Capacity
                    }
                Clear_String(&clear_string, line3, 0,font16);

                parameter_options_temp = (sub_menu[Sub_Menu_no-1][sub_display_cursor].par_options);
                no_of_digits =     parameter_options_temp /10;
                no_of_dec = parameter_options_temp %10;

    roll_entry:
                        //RTC
                        rtc_read_routine();
                    if ((sub_menu[Sub_Menu_no - 1][sub_display_cursor].variable_offset) == 41)
                    {
                        par_val = rtc_read.DOM;
                    }
                    else if ((sub_menu[Sub_Menu_no - 1][sub_display_cursor].variable_offset) == 42)
                    {
                        par_val = rtc_read.MONTH;
                    }
                    else if ((sub_menu[Sub_Menu_no - 1][sub_display_cursor].variable_offset) == 43)
                    {
                        par_val = rtc_read.YEAR;
                    }
                    else if ((sub_menu[Sub_Menu_no - 1][sub_display_cursor].variable_offset) == 44)
                    {
                        par_val = rtc_read.HOUR;
                    }
                    else if ((sub_menu[Sub_Menu_no - 1][sub_display_cursor].variable_offset) == 45)
                    {
                        par_val = rtc_read.MIN;
                    }
                    else if ((sub_menu[Sub_Menu_no - 1][sub_display_cursor].variable_offset) == 46)
                    {
                        par_val = rtc_read.SEC;
                    }

                        if((sub_menu[Sub_Menu_no-1][sub_display_cursor].variable_offset) == 12)  //Password Entry
                                {
                                    Clear_String(&clear_string,line2,30,font16);
                                    Print_String(pass_str[0],line2,5,font16,0);
                                    new_pass =    Enter_Variable2(new_pass,line3,20,font16,4,0);
                                        if(new_pass == par_val || new_pass == 7870)    //7870 is master a
                                            {
                                                Clear_String(&clear_string,line2,30,font16);
                                                Print_String(pass_str[1],line2,5,font16,0);
                                                par_val =    Enter_Variable2(0000,line3,20,font16,4,0);
                                                new_pass=0;
                                            }
                                }

                        else
                         par_val =    Enter_Variable2(par_val,line3,10,font16,no_of_digits,no_of_dec);
                                    {
                                        //RTC
                                        if((sub_menu[Sub_Menu_no-1][sub_display_cursor].variable_offset) == 41)//date
                                            rtc_read.DOM = par_val ;
                                        else if((sub_menu[Sub_Menu_no-1][sub_display_cursor].variable_offset) == 42)//Month
                                            rtc_read.MONTH = par_val ;
                                        else if((sub_menu[Sub_Menu_no-1][sub_display_cursor].variable_offset) == 43)//year
                                            rtc_read.YEAR = par_val ;
                                        else if((sub_menu[Sub_Menu_no-1][sub_display_cursor].variable_offset) == 44)//Hour
                                            rtc_read.HOUR = par_val ;
                                        else if((sub_menu[Sub_Menu_no-1][sub_display_cursor].variable_offset) == 45)//Minutes
                                            rtc_read.MIN = par_val ;
                                        else if((sub_menu[Sub_Menu_no-1][sub_display_cursor].variable_offset) == 46)//Sec
                                            rtc_read.SEC = par_val ;
                                        rtc_write_routine(rtc_read);
                                    }

                    if (Key_Input != K_DEC && Key_Input != ESC && Key_Input != K_INC)
                    {
                        goto roll_entry;
                    }

                    New_menu_parameters[(sub_menu[Sub_Menu_no-1][sub_display_cursor].variable_offset)] = par_val;
                    if((sub_menu[Sub_Menu_no-1][sub_display_cursor].variable_offset)==60)
                        {
                                New_menu_parameters[52] =((uint16_t*)&(par_val))[1];
                                New_menu_parameters[84] = ((uint16_t*)&(par_val))[0];
                        }
                    if(Key_Input == ESC)
                        {
                            Key_Input = K_DEC;
                            return;
                        }
                    goto sub_jump1;

 }

      sub_jump1:
                                if (Key_Input != K_DEC) //if NOT VERTICAL go and check for HOR key
                                     goto sub_jump2;
                                Key_Input  = NO_KEY;
                                if((sub_menu[Sub_Menu_no-1][sub_display_cursor+1].par_type) != 'F' )
                                    {
                                        sub_display_cursor++;
                                        if (sub_display_cursor > total_par)
                                        {
                                            sub_display_cursor = total_par - 1;
                                        }
                                    }
                                else
                                        sub_display_cursor = total_par-1;

                                goto sub_jump_next_parameter;

        sub_jump2:
                                if (Key_Input != K_INC) //if NOT VERTICAL go and check for HOR key
                                     goto sub_jump3;
                                Key_Input  = NO_KEY;
                                if(((sub_menu[Sub_Menu_no-1][sub_display_cursor+1].par_type) != 'F' )    )
                                    {
                                        if ((sub_display_cursor < total_par - 1) && (sub_display_cursor > 0))
                                        {
                                            sub_display_cursor--;
                                        }
//                                        if(sub_display_cursor>total_par)
//                                            sub_display_cursor = total_par-1;
                                    }
                                else
                                        sub_display_cursor = total_par-2;

                                goto sub_jump_next_parameter;

        sub_jump3:     if(Key_Input != HOR)        //if not HORIZONTAL go and check for ESC key
                                    goto sub_jump4;
                Key_Input  = NO_KEY;
                                if (par_val < (sub_menu[Sub_Menu_no - 1][sub_display_cursor].par_options))
                                {
                                    par_val++;
                                }
                                else
                                    par_val=1;

                                New_menu_parameters[(sub_menu[Sub_Menu_no-1][sub_display_cursor].variable_offset)]    = par_val - 1;

                                Clear_String(&clear_string, line3, 0,font16);
                                print_sub_menu =(char *)(sub_menu_options[Sub_Menu_no-1][sub_display_cursor][par_val]);
                                Print_String(&print_sub_menu,line3,10,font16,0);

                                goto sub_jumpxx;

        sub_jump4:
                                if(Key_Input != ESC)        //if not ESC
                                goto sub_jumpxx;

                                return;
}

goto sub_jumpxx;

}
//-------------------------------------------------------

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Print Main Menu
/*    Algorithm
JUMPXX: goto JUMPXX
                Call Key_check Routine
                if Menu_Status !=0, goto JUMP1
                Menu_status=1                                    //1st time entry in Menu routine
                direction = 0                                    //direction variable for cursor direction UP/DOWN
                Display_Cursor=1                            //Address of 1st string
                Save Display_Cursor as Display_Line1    //Save address of 1st String
                Save Display_Line1+3 as display_line5    //Save address of 4th String
                call Display_Main_Menu_routine
jump1:    If Key_Input != Ver, goto JUMP2
                if Display_Cursor < display_line5
                if direction ==0, Increment Display_Cursor, goto JUMP3
                if Display_Cursor == display_line5,
                if(Main_menu[Display_Cursor+1].mm_type == 'F'),direction = 1 ,go to JUMP6        //when last line, make direction=1 for UP cursor
                Inc Display_Line1                        //Next line exists!
                Inc display_line5
                Inc Display_Cursor

jump6:    If Display_Cursor > Display_Line1, Dec Display_Cursor, goto JUMP3            //Next line does not exist!
                If Display_Cursor=Display_Line1,
                if Display_Line1 ==1            //Display already on 1st line,
                if direction ==1, Inc Display_Cursor, goto JUMP3    //when 1st line, make direction=0 for DOWN cursor
                Dec Display_Line1
                Dec display_line5
                Dec Display_Cursor
                goto JUMP3

jump2:  If Key_Input != Hor,  goto JUMPXX
                menu_status=2
                Sub_Menu_no=Display_cursor
                Call Sub_Menu_Routine
                goto JUMPXX

jump3:    Call Display_Main_Menu_routine
                goto JUMPXX

*/
//Note: Font size is fixed. i.e. font10
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
/* Offline/parameter menus and IO monitor block main(); keep all RS485/Modbus alive. */
void Menu_ServiceInvComms(void)
{
    if ((scan_count % 2) == 1)
    {
        if (SIO_Interface == 1)
            modbus();
        DIO_modbus();
        Invter_ModCom();
        Invter2_ModCom();
    }
    else
    {
        VfdDiSanitizeStartMarkers();
        plc_execute();
        ControlActions();
        Modbus_Registers();
    }
}

//-------------------------------------------------------
void Enter_setpoint(void)
{
    char *no_string[2][2] = {{"1/2"}, {"2/2"}};

    Clear_LCD();
    while (Key_Input != ESC)
    {
        if (temp12 == 0)
        {
            Clear_String(&clear_string, line6, 0, font16);
            Print_String(no_string[0], line6, 52, font10, 0);
            Print_String(setpoint_string[0], line1, 0, font16, 0);
            Print_String(setpoint_string[1], line2, 2, font16, 0);
            Setpoint_Int = Enter_Variable3(Setpoint_Int, line3, 20, font16, 4, Dec_Point_Capacity);
        }
        if (temp12 == 1)
        {
            Clear_String(&clear_string, line6, 0, font16);
            Print_String(no_string[1], line6, 52, font10, 0);
            Print_String(setpoint_string[2], line2, 2, font16, 0);
            Setpoint_Man = Enter_Variable3(Setpoint_Man, line3, 20, font16, 4, Dec_Point_Capacity);
            Clear_String(&clear_string, line2, 0, font16);
        }
    }
    save_par_eeprom();
    super_delay(30);
}
//-------------------------------------------------------

//-------------------------------------------------------
void Print_main_menu(main_menu_struct Main_menu[Main_Menu_Max], sub_menu_struct sub_menu[12][26], char const sub_menu_options[12][26][12][20], int menu_type)
{
    int Sub_Menu_no;
    menu_status = 0;
jumpxx2:
jumpxx:
    if (flag == 0)
    {
        goto jumpxx2;
    }
    else
    {
        flag = 0;
    }

    Menu_ServiceInvComms();
    key_check_routine();
    if (menu_status != 0)
    {
        goto jump1;
    }

    menu_status = 1;
    display_cursor = 1;
    if (Main_menu[display_cursor].mm_type != 'F')
    {
        display_line1 = display_cursor;
        display_line5 = display_cursor + 4;
        display_main_menu_routine(Main_menu, display_line1, display_cursor);
    }
jump1:
    if ((Key_Input != K_DEC) && (Key_Input != K_INC))
    {
        goto jump2;
    }

    if ((Key_Input == K_DEC) || (Key_Input == K_INC))
    {
        if (Key_Input == K_DEC)
        {
            if (display_cursor < display_line5)
            {
                display_cursor++;
                goto jump3;
            }
            if ((display_cursor == display_line5) && (Main_menu[display_cursor + 1].mm_type == 'F'))
            {
                goto jump3;
            }
            display_line1++;
            display_line5++;
            display_cursor++;
            goto jump3;
        }

        if (Key_Input == K_INC)
        {
            if (display_cursor > display_line1)
            {
                display_cursor--;
                goto jump3;
            }
            if ((display_cursor == display_line1) && (display_line1 == 1))
            {
                goto jump3;
            }
            display_cursor--;
            display_line1--;
            display_line5--;
            goto jump3;
        }
    }
jump3:
    display_main_menu_routine(Main_menu, display_line1, display_cursor);
    goto jumpxx;

jump2:
    if (Key_Input == HOR)
    {
        Sub_Menu_no = display_cursor;
    }
    if (Key_Input != HOR)
    {
        if (Key_Input > 9)
        {
            goto jump4;
        }
    }

    if (Key_Input <= 9)
    {
        switch (status_no)
        {
            case 0:
                start_count_flag = 1;
                display_cursor_temp = Key_Input;
                display_cursor_prev = display_cursor;
                status_no = 1;
                break;
            case 1:
                if ((display_cursor_temp == 1) && (Key_Input == 0))
                {
                    display_cursor_temp = 10;
                }
                if ((display_cursor_temp == 1) && (Key_Input == 1))
                {
                    display_cursor_temp = 11;
                    status_no = 0;
                }
                if ((display_cursor_temp < 9))
                {
                    display_cursor = display_cursor_prev;
                    display_cursor_temp = display_cursor_prev;
                    start_count_flag = 0;
                    key_check_count = 0;
                    status_no = 0;
                }
                break;
            default:
                break;
        }
        goto jump4;
    }
back123:
    if (menu_type == 1)
    {
        if ((Sub_Menu_no > 0))
        {
            Check_password();
        }

        if (pass_check == 5)
        {
            if ((Sub_Menu_no == 5) && (Check_Start == 10))
            {
                calib_init_display();
            }
            else if ((Sub_Menu_no == 6) && (Check_Start == 10))
            {
                AI_Calibration();
            }
            else if ((Sub_Menu_no == 7) && (Check_Start == 10))
            {
                AO_Calibration();
            }
            else if ((Sub_Menu_no == 8) && (Check_Start == 10))
            {
                plc_menu();
            }
            else if (((Sub_Menu_no == 10) && (Check_Start == 10)) || ((Sub_Menu_no == 2) && (Check_Start == 20)))
            {
                Enter_setpoint();
            }
            else if (((Sub_Menu_no == 12) && (Check_Start == 10)) || ((Sub_Menu_no == 3) && (Check_Start == 20)))
            {
                IO_Monitor();
            }
            else if ((Sub_Menu_no == 11) && (Check_Start == 10))
            {
                Sync();
            }
            else if ((Sub_Menu_no == 3) && (Check_Start == 10))
            {
                if ((Pre_Hopper) == 1)
                {
                    Sub_Menu_Routine(Main_menu, Sub_Menu_no, sub_menu, sub_menu_options, menu_type);
                }
            }
            else if ((Sub_Menu_no == 13) && (Check_Start == 10))
            {
                System_Update_Menu();
            }
            else
            {
                Sub_Menu_Routine(Main_menu, Sub_Menu_no, sub_menu, sub_menu_options, menu_type);
            }
        }
    }
    else
    {
        goto back123;
    }
    display_main_menu_routine(Main_menu, display_line1, display_cursor);
    super_delay(50);
    goto jumpxx;

jump4:
    if (Key_Input == ESC)
    {
        if (Clear_Totaliser2 == 1)
        {
            Totaliser2_Display = 0;
            Totaliser2 = 0;
            Totaliser2_Update = 0;
            RTC_WriteGPREG(LPC_RTC, 1, Totaliser2);
            Clear_Totaliser2 = 0;
        }
        ((uint16_t*)&(Capacity_1))[1] = New_menu_parameters[52];
        ((uint16_t*)&(Capacity_1))[0] = New_menu_parameters[84];

        after_menu_initialize();
        sub_display_cursor = 0;
        Check_Start = 0;
        ResetInvCommAfterOfflineMenu(); /* msk29-may-26: false Inv com fault after settings menu */
        return;
    }
    if (start_count_flag == 1)
    {
        key_check_count++;
        if (key_check_count == 200)
        {
            display_cursor = display_cursor_temp;

            if ((display_cursor > 0))
            {
                display_line1 = display_cursor;
                if ((display_line1 > 8))
                {
                    display_line1 = 7;
                }
                display_line5 = display_line1 + 4;
                Sub_Menu_no = display_cursor;
                start_count_flag = 0;
                key_check_count = 0;
                status_no = 0;
                goto back123;
            }
            start_count_flag = 0;
            key_check_count = 0;
            status_no = 0;
        }
    }

    goto jumpxx;

    return;
}

        void Check_password(void)
{
    Clear_LCD();
    Print_String(pass_str[2],line2,2,font16,0);
    pass_check =    Enter_Variable2(0000,line3,20,font16,4,0);
    if (pass_check == Password || pass_check == 7870)
    {
        pass_check = 5;
    }
    else
        pass_check=0;
}

/*********************************************************/
long Enter_online_Variable(unsigned long variable, unsigned char start_row, unsigned char start_col, unsigned char font_size,
                                                     unsigned char no_of_digit, unsigned char decimal_pt)
{
    //long VARI=0;
    //char new_string[15]={0};
    //char print_new_string[15]={0};
    //char old_string[15]={0};
    //char *new_string_print;
    //char str_len1=0,str_len2=0;
    //unsigned char j=0,k=0,j2=0;
    //unsigned char str_loop=0;
    //signed char diff=0;
    //char dec_flag=0;
/*************************************************************************************************/

    if (check_flag == 0)
    {
        if (decimal_pt == 9)
        {
            decimal_pt = Dec_Point_Capacity;
        }

        new_string_print_online = new_string_online;
        sprintf(old_string_online, "%lu", variable);
        str_len1_online = strlen(old_string_online);
        diff_online = no_of_digit - str_len1_online;

        if (diff_online < 0)
        {
            variable = 0x0;
            sprintf(old_string_online, "%lu", variable);
            str_len1_online = strlen(old_string_online);
            diff_online = no_of_digit - str_len1_online;
        }

        if (diff_online > 0)
        {
            for (j_online = 0; j_online < diff_online; j_online++)
            {
                new_string_online[j_online] = '0';
            }
            if (variable < 10)
            {
                new_string_online[j_online] = variable + 48;
                new_string_online[j_online + 1] = '\0';
            }
            else
            {
                strcat(new_string_online, old_string_online);
            }
        }
        if (diff_online == 0)
        {
            strcpy(new_string_online, old_string_online);
        }

        new_string_print_online = print_new_string_online;
        str_len2_online = strlen(new_string_online);
        strcpy(print_new_string_online, new_string_online);
        str_loop_online = str_len2_online - decimal_pt;

        for (k_online = str_loop_online; k_online < (str_len2_online) + 1; k_online++)
        {
            print_new_string_online[k_online + 1] = new_string_online[k_online];
        }
        print_new_string_online[str_loop_online] = '.';

        check_flag = 1;
        key_vlaue = NO_KEY;
    }

/************************************************************************************************************************/
    if (check_flag == 1)
    {
        if ((j2_online <= (str_len2_online)))
        {
            if (key_vlaue <= 9)
            {
                new_string_print_online[j_online] = key_vlaue + 48;
                new_string_online[j2_online - 1] = new_string_print_online[j_online];
                key_vlaue = NO_KEY;
            }

            j_online = j2_online;
            if (print_new_string_online[j_online] == '.')
            {
                dec_flag_online = 1;
            }

            if (dec_flag_online == 1)
            {
                j_online++;
                if (j_online > (str_len2_online))
                {
                    j2_online = 0;
                    j_online = 0;
                    dec_flag_online = 0;
                }
            }
            Clear_single_Line(30, 40);
            Print_String2(&new_string_print_online, start_row, start_col, font_size, j_online + 1);
        }
        if (j2_online > str_len2_online)
        {
            j2_online = 1;
        }

exit_and_return:
        VARI_online = atoi(new_string_online);
        return (VARI_online);
    }
    // Defensive fallback (prevents undefined return if state is unexpected)
    return (long)variable;
}
/*********************************************************/
//-------------------------------------------------------
void Check_password_online(void)
{
    Print_String(pass_str[2],line2,2,font16,0);
    pass_check =    Enter_online_Variable(pass_check,line3,20,font16,4,0);
    if (pass_check == Password || pass_check == 7870)
    {
        pass_check = 5;
    }
    else
    {
        pass_check = 0;
    }
}
//-------------------------------------------------------

//-------------------------------------------------------
void Online_Sub_Menu(main_menu_struct Main_menu[Main_Menu_Max], char Sub_Menu_no1, sub_menu_struct sub_menu[5][14], char const sub_menu_options[5][14][10][20], char Sub_menu_line_no)
{
    char temp_i;
    char *print_sub_menu ;
    char total_par ;
    long par_val=0;
    unsigned char Sub_Menu_no= Sub_Menu_no1;
    char *no_string[1] = {"/"};
    const char *no_strg[13][3] = {{"1"},{"2"},{"3"},{"4"},{"5"},{"6"},{"7"},{"8"},{"9"},{"10"},{"11"},{"12"},{"13"}};

    new_pass = 0;
    switch (Sub_menu_line_no)
    {
        case 3:
            Print_String2(&(*(Main_menu[Sub_Menu_no].mm_name)), line2, 0, font10, 0);
            submenu_Staus_flag = 1;
            break;
        case 4:
            if (submenu_Staus_flag == 1)
            {
                switch (Sub_Menu_no)
                {
                    case 1:
                        if (sub_display_cursor > 12)
                        {
                            sub_display_cursor = 12;
                        }
                        if (sub_display_cursor < 1)
                        {
                            sub_display_cursor = 0;
                        }
                        total_para_no = 12;
                        break;
                    case 2:
                        if (sub_display_cursor > 1)
                        {
                            sub_display_cursor = 1;
                        }
                        if (sub_display_cursor < 1)
                        {
                            sub_display_cursor = 0;
                        }
                        total_para_no = 1;
                        break;
                    case 4:
                        if (sub_display_cursor > 9)
                        {
                            sub_display_cursor = 9;
                        }
                        if (sub_display_cursor < 1)
                        {
                            sub_display_cursor = 0;
                        }
                        total_para_no = 9;
                        break;
                }
                Clear_single_Line(20, 30);
                print_sub_menu = (char *)(sub_menu_options[Sub_Menu_no - 1][sub_display_cursor][0]);
                Print_String2(&print_sub_menu, line3, 10, font10, 0);
                submenu_Staus_flag = 2;
            }
            break;
        case 5:
            if (submenu_Staus_flag == 2)
            {
                if ((sub_menu[Sub_Menu_no - 1][sub_display_cursor].par_type) == 'E')
                {
                    if (Opt_para_flag == 0)
                    {
                        for (temp_i = 0; temp_i <= 15; temp_i++)
                        {
                            new_string_online[temp_i] = 0;
                            print_new_string_online[temp_i] = 0;
                            old_string_online[temp_i] = 0;
                        }

                        VARI_online = 0;
                        new_string_print_online = 0;
                        str_len1_online = 0, str_len2_online = 0;
                        j_online = 0, k_online = 0, j2_online = 0;
                        str_loop_online = 0;
                        diff_online = 0;
                        dec_flag_online = 0;

                        switch ((sub_menu[Sub_Menu_no - 1][sub_display_cursor].variable_offset))
                        {
                            case 34: par_val = Online_menu_parameters[0]; break;
                            case 35: par_val = Online_menu_parameters[1]; break;
                            case 36: par_val = Online_menu_parameters[2]; break;
                            case 37: par_val = Online_menu_parameters[3]; break;
                            case 38: par_val = Online_menu_parameters[4]; break;
                            case 82: par_val = Online_menu_parameters[5]; break;
                            case 55: par_val = Online_menu_parameters[6]; break;
                            case 56: par_val = Online_menu_parameters[7]; break;
                            case 49: par_val = Online_menu_parameters[8]; break;
                            case 50: par_val = Online_menu_parameters[9]; break;
                            case 51: par_val = Online_menu_parameters[10]; break;
                            case 54: par_val = Online_menu_parameters[11]; break;
                            case 83: par_val = Online_menu_parameters[12]; break;
                            case 39: par_val = Online_menu_parameters[13]; break;
                            case 40: par_val = Online_menu_parameters[14]; break;
                            default: break;
                        }

                        Clear_single_Line(30, 40);

                        parameter_options_temp = (sub_menu[Sub_Menu_no - 1][sub_display_cursor].par_options);
                        no_of_digits = parameter_options_temp / 10;
                        if (Sub_Menu_no == 2)
                        {
                            no_of_dec = Dec_Point_Capacity;
                        }
                        else
                        {
                            no_of_dec = parameter_options_temp % 10;
                        }

                        Opt_para_flag = 1;
                    }
                    if (Opt_para_flag == 1)
                    {
                        par_val = Enter_online_Variable(par_val, line4, 10, font10, no_of_digits, no_of_dec);

                        if (Sub_Menu_no == 2)
                        {
                            Print_String2(unitstri[Units_Capacity], line4, 30, font10, 0);
                        }

                        switch ((sub_menu[Sub_Menu_no - 1][sub_display_cursor].variable_offset))
                        {
                            case 34: Online_menu_parameters[0] = par_val; break;
                            case 35: Online_menu_parameters[1] = par_val; break;
                            case 36: Online_menu_parameters[2] = par_val; break;
                            case 37: Online_menu_parameters[3] = par_val; break;
                            case 38: Online_menu_parameters[4] = par_val; break;
                            case 82: Online_menu_parameters[5] = par_val; break;
                            case 56: Online_menu_parameters[7] = par_val; break;
                            case 49: Online_menu_parameters[8] = par_val; break;
                            case 50: Online_menu_parameters[9] = par_val; break;
                            case 51: Online_menu_parameters[10] = par_val; break;
                            case 54: Online_menu_parameters[11] = par_val; break;
                            case 83: Online_menu_parameters[12] = par_val; break;
                            case 39: Online_menu_parameters[13] = par_val; break;
                            case 40: Online_menu_parameters[14] = par_val; break;
                            default: break;
                        }
                    }
                }
                if ((sub_menu[Sub_Menu_no - 1][sub_display_cursor].par_type) == 'O')
                {
                    if (Sel_para_flag == 0)
                    {
                        Temp_par_val = 1 + Online_menu_parameters[6];
                        Sel_para_flag = 1;
                    }
                    if (Sel_para_flag == 1)
                    {
                        if (Temp_par_val > sub_menu[Sub_Menu_no - 1][sub_display_cursor].par_options)
                        {
                            Temp_par_val = 1;
                        }
                        Sel_para_flag = 2;
                    }
                    Clear_single_Line(30, 40);
                    print_sub_menu = (char *)(sub_menu_options[Sub_Menu_no - 1][sub_display_cursor][Temp_par_val]);
                    Print_String2(&print_sub_menu, line4, 10, font10, 0);

                    if (Sel_para_flag == 2)
                    {
                        Online_menu_parameters[6] = Temp_par_val - 1;
                        Sel_para_flag = 3;
                    }
                }
                Clear_single_Line(50, 60);
                Print_String2(no_strg[sub_display_cursor], line6, 45, font10, 0);
                Print_String2(no_string, line6, 52, font10, 0);
                Print_String2(no_strg[total_para_no], line6, 56, font10, 0);
            }
            break;
        default:
            break;
    }
}

//void Print_Online_Menu(main_menu_struct Main_menu[Main_Menu_Max],sub_menu_struct sub_menu[10][20],char const sub_menu_options[10][20][20][20])
//{
//    /*Main Menu Routine*/
//    switch(display_line1)
//        {
//            case 1:
//                            display_online_menu_routine(Main_menu,1,display_cursor);
//                            display_line1 = 5 ;
//                            break;
//            default:
//                            break;
//
//        }
//    if( Submenu_flag == 1)
//        {
//             switch(display_cursor)
//                {
//                    case 1: Online_Sub_Menu(Main_menu,1,sub_menu,sub_menu_options,1); //
//                                    break;
//                  case 2: Enter_setpoint();
//                                    break;
//                  case 3:    Online_Sub_Menu(Main_menu,3,sub_menu,sub_menu_options,1);
//                                    break;
//                    case 4:    IO_Monitor();   //IO_Monitor
//                                    break;
//                }
//        }
//
//}
//-------------------------------------------------------
void Temp_Intialisation(void)
{
    display_cursor = 1;
    display_line1 = display_cursor;
    display_line5 = display_cursor + 3;
}
//-------------------------------------------------------

/*Changes
Date 30/01/2018
PI parameter added
PI:Analog :PI used from WF SP calculation rotine :4-20mA OP  or INV sp via RS485
     :Relay :PI used from Valve_Control.c & O000 &O001 is fixed for gate control
Date 23/02/2018
Analog OP Selsection
    Hop Level & Gate Postion OP are added for Analog OP selection
Date 07/04/2018
added PLC program upload & download via usb in upload & Download function
PLC program cannot be more than 198 lines version changed to V1.6
Added Inc key function
Date 28/05/2018
Added Tacho ip parameter (if tacho damaged then set Tacho ip = Inv so Inv output Freq is used as speed for internal calcculation )

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

uint16_t New_menu_parameters[91(Changed to 91 from 85)];    //changed to 105 msk 11-oct-2025

Date 28/08/2018
Added ATV312 IO reading

Date 29/11/2018
Added Yaskawa J1000 option in Communication

*/
