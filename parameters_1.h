/*parameters.h*/
#ifndef PARAMETERS_H
#define PARAMETERS_H

#define Main_Menu_Max 15
#define Menu_String_Max_Size 20
#define Sel_par_size 10
#define Sel_string_size 20

/****************************** New_Parameters_For_Weigh_Feeder ********************************************/
#define    Display_Filter                                        *(New_menu_parameters+    0    )
#define    Prefeeder                                                    *(New_menu_parameters+    1    )
#define    Units_Capacity                                        *(New_menu_parameters+    2    )
#define    Dec_Point_Capacity                                *(New_menu_parameters+    3    )
#define    Capacity                                                    *(New_menu_parameters+    4    )     //WF_Capcity
#define    Dec_Pt_Totaliser1                                    *(New_menu_parameters+    5    )
#define    Units_Totaliser1                                    *(New_menu_parameters+    6    )
#define    Dec_Pt_Totaliser2                                    *(New_menu_parameters+    7    )
#define    Units_Totaliser2                                    *(New_menu_parameters+    8    )
#define    Tacho_Freq                                                *(New_menu_parameters+    9    )     //Nominal Tacho Frequency
#define    Max_Load                                                    *(New_menu_parameters+    10    )
#define    Min_Load                                                    *(New_menu_parameters+    11    )
#define    Password                                                    *(New_menu_parameters+    12    )
#define    Rev_Time                                                    *(New_menu_parameters+    13    )
#define    Zero_Limit                                                *(New_menu_parameters+    14    )
#define    Belt_Track                                                *(New_menu_parameters+    15    )
#define    Run_Time                                                    *(New_menu_parameters+    16    )
#define    Pulse1                                                        *(New_menu_parameters+    17    )
#define    Pulse_Width                                                *(New_menu_parameters+    18    )
#define    Pulse2                                                        *(New_menu_parameters+    19    )
#define    AO1_Value                                                    *(New_menu_parameters+    20    )
#define    AO2_Value                                                    *(New_menu_parameters+    21    )
#define    Load_Limit1                                                *(New_menu_parameters+    22    )
#define    Load_Limit2                                                *(New_menu_parameters+    23    )
#define    Load_Delay                                                *(New_menu_parameters+    24    )
#define    Clear_Totaliser2                                    *(New_menu_parameters+    25    )
#define    Load_Filter                                             *(New_menu_parameters+    26    )
#define    Mode_Select                                                *(New_menu_parameters+    27    )
#define    DIO_Module                                                *(New_menu_parameters+    28    )
#define    SIO_Interface                                            *(New_menu_parameters+    29    )
#define    SIO_Baud_Rate                                            *(New_menu_parameters+    30    )
#define    SIO_Setup                                                    *(New_menu_parameters+    31    )
#define    SIO_Address                                                *(New_menu_parameters+    32    )
#define    SIO_Monitor                                                *(New_menu_parameters+    33    )
#define    Int_Time                                                    *(New_menu_parameters+    34    )
#define    Prop_Gain                                                    *(New_menu_parameters+    35    )
#define    Dev_Limit                                                    *(New_menu_parameters+    36    )
#define    Dev_Time                                                    *(New_menu_parameters+    37    )
#define    Dead_Band                                                    *(New_menu_parameters+    38    )
#define    Setpoint_Int                                            *(New_menu_parameters+    39    )
#define    Setpoint_Man                                            *(New_menu_parameters+    40    )
/*                                                                                                                    Date    41
                                                                                                                        Month    42
                                                                                                                        Year    43
                                                                                                                        Hour    44
                                                                                                                        Min      45
                                                                                                                        Sec      46*/
#define    Pre_Hopper                                                *(New_menu_parameters+    47    )
#define    PI                                                              *(New_menu_parameters+    48    )
#define    BL_Limit                                                    *(New_menu_parameters+    49    )
#define    PF_Delay                                                    *(New_menu_parameters+    50    )
#define    PF_StartSet                                                *(New_menu_parameters+    51    )
/*                                                                            Capacity hopper high word 52 */
#define    BL_Band                                                  *(New_menu_parameters+    53    )
#define    PF_Limit                                                    *(New_menu_parameters+    54    )
#define    Regulation                                            *(New_menu_parameters+    55    )
#define    Reg_Tracking                                            *(New_menu_parameters+    56    )
#define Fixed_Frequency                   *(New_menu_parameters+    57    )
#define    Hopper_Capacity                                        *(New_menu_parameters+    60    )
#define    No_Rev                                                      *(New_menu_parameters+    61    )
#define    AD_Conversion_Rate                                *(New_menu_parameters+    66    )
#define    Filter_Order                                            *(New_menu_parameters+    67    )
#define    Cut_Off_Freq                                            *(New_menu_parameters+    68    )
#define LC_Filter                         *(New_menu_parameters+    69    )
#define Inv                                      *(New_menu_parameters+    70    )
#define Inv_1                                  *(New_menu_parameters+    71    )
#define    AO3_Value                                                    *(New_menu_parameters+    72    )
#define    AI_Nos                                                        *(New_menu_parameters+    73    )
#define    AO_Nos                                                        *(New_menu_parameters+    74    )
#define    Filter__Order                                            *(New_menu_parameters+    75    )
#define    Cut__Off_Freq                                          *(New_menu_parameters+    76    )
#define    LC__Filter                                                *(New_menu_parameters+    77    )
#define    Limit_Low                                                *(New_menu_parameters+    78    )
#define    Limit_Nominal                                          *(New_menu_parameters+    79    )
#define    Limit_Max                                                  *(New_menu_parameters+    80    )
#define    Limit_Delay                                        *(New_menu_parameters+    81    )
#define    Min_Pulse                                                  *(New_menu_parameters+    82    )
#define    Correction_Factor                                    *(New_menu_parameters+    83    )
/*                                                                             Capacity hopper low word 84 */
#define    IP_Addr1                                                *(New_menu_parameters+    85    )//added on 23/05/2018
#define    IP_Addr2                                                  *(New_menu_parameters+    86    )
#define    IP_Addr3                                                  *(New_menu_parameters+    87    )
#define    IP_Addr4                                                  *(New_menu_parameters+    88    )
#define    Profibus_Node_Addr                                *(New_menu_parameters+    89    )
#define    Profibus_Baudrate                                    *(New_menu_parameters+    90    )
#define    Tacho_ip                                                    *(New_menu_parameters+    91    ) //added on 28/05/2018
//#define AI_calval                                                    *(New_menu_parameters+    92    )    //6 values
#define free1                *(New_menu_parameters+    93    )    //6 values
#define free2                *(New_menu_parameters+    94    )    //6 values
#define free3                *(New_menu_parameters+    95    )    //6 values
#define free4                *(New_menu_parameters+    96    )    //6 values
#define free5                *(New_menu_parameters+    97    )    //6 values
#define free6                *(New_menu_parameters+    98    )    //6 values
#define free7                *(New_menu_parameters+    99    )    //6 values
#define free8                *(New_menu_parameters+    100    )    //6 values
#define free9                *(New_menu_parameters+    101    )    //6 values
#define free10                *(New_menu_parameters+    102    )    //6 values
#define free11                *(New_menu_parameters+    103    )    //6 values

#define nextval                                                        *(New_menu_parameters+    104)    //after 6 int16 values

unsigned short AO_cal_Values[6];

#define AO_calz1    AO_cal_Values[0]//                                                *(New_menu_parameters+    98    )    //after 6 int16 values
#define AO_cals1    AO_cal_Values[1]//                                                *(New_menu_parameters+    99    )    //after 6 int16 values

#define AO_calz2    AO_cal_Values[2]//                                                *(New_menu_parameters+    100    )    //after 6 int16 values
#define AO_cals2    AO_cal_Values[3]//                                                *(New_menu_parameters+    101    )    //after 6 int16 values

#define AO_calz3    AO_cal_Values[4]//                                                *(New_menu_parameters+    102    )    //after 6 int16 values
#define AO_cals3    AO_cal_Values[5]//                                                *(New_menu_parameters+    103    )    //after 6 int16 values

#define Positive 0
#define Negative 1
#define Yes 0
#define No     1
#define Mode0 0
#define Mode1    1
#define OFF 0
#define ON 1

typedef struct  {
    char mm_type; //mm for main menu
    int mm_no;
    char *mm_name[Menu_String_Max_Size];

}main_menu_struct;

typedef struct  {
    char mm_type; //mm for main menu
    int mm_no;
    char *mm_name[Menu_String_Max_Size];

}main_menu_2_struct;

typedef struct  {
    char par_type;
    int par_options;
    int variable_offset;

}sub_menu_struct;

#endif

/*
28/05/2018
Added Tacho ip parameter (if tacho got damaged then set Tacho ip = Inv so Inv output Freq is used as speed for calcculation )

23/05/2018
#define    IP_Addr1                                                *(New_menu_parameters+    85    )
#define    IP_Addr2                                                  *(New_menu_parameters+    86    )
#define    IP_Addr3                                                  *(New_menu_parameters+    87    )
#define    IP_Addr4                                                  *(New_menu_parameters+    88    )

#define    Profibus_Node_Addr                                *(New_menu_parameters+    89    )
#define    Profibus_Baudrate                                    *(New_menu_parameters+    90    )
*/
