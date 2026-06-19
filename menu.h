/*menu.h*/
#ifndef MENU_H
#define MENU_H

#define Main_Menu_Max 15
#define Menu_String_Max_Size 20
#define Sel_par_size 10
#define Sel_string_size 20

#define BL_Load_Dec_Pt                    *(Decimal_Point+1)
#define Unit                              *(menu_parameters+ 1 )
//#define Capacity                         *(menu_parameters+ 2 )
#define Dec_Point                         *(menu_parameters+ 3 )
#define Least_Count                       *(menu_parameters+ 4 )
#define Motion_Sensitivity                *(menu_parameters+ 5 )
// These macros overlap with the newer New_menu_parameters layout in parameters.h.
// They are intentionally NOT defined here to avoid include-order-dependent
// macro redefinitions (which leads to wrong behavior and compiler warnings).
#define Auto_zero_Range                   *(menu_parameters+ 9 )
#define Powerup_Zero                      *(menu_parameters+ 10 )
#define PushButton_Zero                   *(menu_parameters+ 11 )
//#define Password                         *(menu_parameters+ 12 )
#define No_of_DIO                         *(menu_parameters+ 13 )
#define PLC_Interface                     *(menu_parameters+ 14 )
#define Baud_Rate                         *(menu_parameters+ 15 )
// NOTE: do not define `Parity` macro here.
// It collides with CMSIS `lpc17xx_uart.h` struct member `UART_PARITY_Type Parity`,
// which breaks compilation when menu.h is included before CMSIS headers.
// If you need a parity parameter, use the SIO menu offset directly or define
// it under a non-conflicting name.
#define Node_Address                      *(menu_parameters+ 17 )
#define Monitoring_Time                   *(menu_parameters+ 18 )
#define Print_Format                      *(menu_parameters+ 19 )
#define Feeding_Mode                      *(menu_parameters+ 20 )
#define Jumbo_Bagging                     *(menu_parameters+ 21 )
#define Atuo_Compensate                   *(menu_parameters+ 22 )
#define Auto_Fallback_Comp                *(menu_parameters+ 23 )
#define AFC_Coefficient                   *(menu_parameters+ 24 )
#define AFC_Sampling_Times                *(menu_parameters+ 25 )
#define AFC_Value_Limit                   *(menu_parameters+ 26 )
#define Empty_Limit                       *(menu_parameters+ 27 )
#define Periodic_Zero                     *(menu_parameters+ 28 )
#define F_M_inhibit_time                  *(menu_parameters+ 29 )
#define M_L_inhibit_time                  *(menu_parameters+ 30 )
#define L_Target_inhibit_time             *(menu_parameters+ 31 )
#define Stable_Weight_Hold_Time           *(menu_parameters+ 32 )
#define Stable_Judging_Time              *(menu_parameters+ 33 )
#define Empty_Signal_Delay                *(menu_parameters+ 34 )
#define Addln_Prop_Pulse                  *(menu_parameters+ 35 )
#define Addln_Prop_Pause                  *(menu_parameters+ 36 )
#define Start_Wait_Time                   *(menu_parameters+ 37 )
#define Feed_Blank_Time                   *(menu_parameters+ 38 )
#define Discharge_Monitor                 *(menu_parameters+ 39 )
#define Feed_Monitor                      *(menu_parameters+ 40 )

/****************************** New_Parameters_For_Weigh_Feeder ********************************************/

#define WF_Type                           *(New_menu_parameters+ 1 )
#define Units_Capacity                    *(New_menu_parameters+ 2 )
#define Dec_Point_Capacity                *(New_menu_parameters+ 3 )
#define Capacity                          *(New_menu_parameters+ 4 )
#define Dec_Pt_Totaliser1                 *(New_menu_parameters+ 5 )
#define Units_Totaliser1                  *(New_menu_parameters+ 6 )
#define Dec_Pt_Totaliser2                 *(New_menu_parameters+ 7 )
#define Units_Totaliser2                  *(New_menu_parameters+ 8 )
#define Tacho_Freq                        *(New_menu_parameters+ 9 )
#define Max_Load                          *(New_menu_parameters+ 10 )
#define Min_Load                          *(New_menu_parameters+ 11 )
#define Password                          *(New_menu_parameters+ 12 )

#define Rev_Time                          *(New_menu_parameters+ 13 )
#define Zero_Limit                        *(New_menu_parameters+ 14 )
#define Belt_Track                        *(New_menu_parameters+ 15 )
#define Run_Time                          *(New_menu_parameters+ 16 )
#define Pulse1                            *(New_menu_parameters+ 17 )
#define Pulse_Width                       *(New_menu_parameters+ 18 )
#define Pulse2                            *(New_menu_parameters+ 19 )
#define AO1_Value                         *(New_menu_parameters+ 20 )
#define AO2_Value                         *(New_menu_parameters+ 21 )
#define Load_Limit1                       *(New_menu_parameters+ 22 )
#define Load_Limit2                       *(New_menu_parameters+ 23 )
#define Load_Delay                        *(New_menu_parameters+ 24 )
#define Clear_Totaliser2                  *(New_menu_parameters+ 25 )
#define Load_Filter                       *(New_menu_parameters+ 26 )
#define Mode_Select                       *(New_menu_parameters+ 27 )

#define DIO_Module                        *(New_menu_parameters+ 28 )
#define SIO_Interface                     *(New_menu_parameters+ 29 )
#define SIO_Baud_Rate                     *(New_menu_parameters+ 30 )
#define SIO_Setup                         *(New_menu_parameters+ 31 )
#define SIO_Address                       *(New_menu_parameters+ 32 )
#define SIO_Monitor                       *(New_menu_parameters+ 33 )

#define Int_Time                          *(New_menu_parameters+ 34 )
#define Prop_Gain                         *(New_menu_parameters+ 35 )
#define Dev_Limit                         *(New_menu_parameters+ 36 )
#define Dev_Time                          *(New_menu_parameters+ 37 )
#define Dead_Band                         *(New_menu_parameters+ 38 )

#define Setpoint_Int                      *(New_menu_parameters+ 39 )
#define Setpoint_Man                      *(New_menu_parameters+ 40 )

#define PF_Int_Time                       *(New_menu_parameters+ 47 )
#define PF_Prop_Gain                      *(New_menu_parameters+ 48 )
#define BL_Limit                          *(New_menu_parameters+ 49 )
#define BL_Delay                          *(New_menu_parameters+ 50 )
#define BL_Setpoint                       *(New_menu_parameters+ 51 )
#define BL_Control                        *(New_menu_parameters+ 52 )
#define BL_Band                           *(New_menu_parameters+ 53 )
#define Setpoint_Limit                    *(New_menu_parameters+ 54 )
#define Reg_Type                          *(New_menu_parameters+ 55 )
#define Reg_Track                         *(New_menu_parameters+ 56 )

/*
Date    41
Month   42
Year    43
Hour    44
Min     45
Sec     46
*/
//#define Recipe_No                        *(menu_parameters+ 47 )
//#define No_of_Batches                    *(menu_parameters+ 48 )
//#define Target_Weight                    *(menu_parameters+ 49 )
//#define Fast_Feed                        *(menu_parameters+ 50 )
//#define Middle_Feed                      *(menu_parameters+ 51 )
//#define Inflight                         *(menu_parameters+ 52 )
//#define Tolerance_Positive               *(menu_parameters+ 53 )
//#define Tolerance_Negative               *(menu_parameters+ 54 )
//#define Span_Correction                  *(menu_parameters+ 55 )
//#define New_Menu_para1                   *(menu_parameters+ 155 )
//#define New_Menu_para2                   *(menu_parameters+ 156 )
/*  menu_parameters+ 56 - menu_parameters + 124 = Recipe */

#define R01_Count                         *(totalizer_parameters+0)
#define R01_Net                           *(totalizer_parameters+1)
#define R02_Count                         *(totalizer_parameters+2)
#define R02_Net                           *(totalizer_parameters+3)
#define R03_Count                         *(totalizer_parameters+4)
#define R03_Net                           *(totalizer_parameters+5)
#define R04_Count                         *(totalizer_parameters+6)
#define R04_Net                           *(totalizer_parameters+7)
#define R05_Count                         *(totalizer_parameters+8)
#define R05_Net                           *(totalizer_parameters+9)
#define R06_Count                         *(totalizer_parameters+10)
#define R06_Net                           *(totalizer_parameters+11)
#define R07_Count                         *(totalizer_parameters+12)
#define R07_Net                           *(totalizer_parameters+13)
#define R08_Count                         *(totalizer_parameters+14)
#define R08_Net                           *(totalizer_parameters+15)
#define R09_Count                         *(totalizer_parameters+16)
#define R09_Net                           *(totalizer_parameters+17)
#define R10_Count                         *(totalizer_parameters+18)
#define R10_Net                           *(totalizer_parameters+19)

/*
#define Recipe_No                         *(menu_parameters+ 47 )
#define No_of_Batches                     *(menu_parameters+ 48 )
#define Target_Weight                     *(menu_parameters+ Recipe_No* 7 +56 )
#define Fast_Feed                         *(menu_parameters+ Recipe_No* 7 +57 )
#define Middle_Feed                       *(menu_parameters+ Recipe_No* 7 +58 )
#define Inflight                          *(menu_parameters+ Recipe_No* 7 +59 )
#define Tolerance_Positive                *(menu_parameters+ Recipe_No* 7 +60 )
#define Tolerance_Negative                *(menu_parameters+ Recipe_No* 7 +61 )
*/
//#define PLC_Prog_no                       *(menu_parameters+ Recipe_No* 7 +62 )

typedef struct  {
    char mm_type; //mm for main menu
    int mm_no;
    char *mm_name[Menu_String_Max_Size];
}main_menu_struct;

typedef struct  {
    char par_type;
    int par_options;
    int variable_offset;
}sub_menu_struct;

void Print_main_menu(main_menu_struct Main_menu[Main_Menu_Max],
                      sub_menu_struct sub_menu[12][26],
                      char const sub_menu_options[12][26][12][20],
                      int menu_type);

#endif
