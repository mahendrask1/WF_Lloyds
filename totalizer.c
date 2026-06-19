//totalizer.c//

#include "lpc_types.h"
#include "lpc17xx_rtc.h"
#include "main_routine.h"
#include "string.h"
#include "stdio.h"

#include "LPC17xx.h" // Device header
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

extern uint16_t New_menu_parameters[105];
extern unsigned int Actual_Feedrate ,FDR_Total;
extern double Totaliser1,Totaliser2;

unsigned int Totaliser1_Multiplier1,Totaliser1_Multiplier2,Totaliser1_Divisor1, Totaliser1_Divisor2,
                         Totaliser2_Multiplier1,Totaliser2_Multiplier2,Totaliser2_Divisor1, Totaliser2_Divisor2;

extern unsigned int Totaliser1_Display,Totaliser2_Display,Totaliser1_Display_Old,Totaliser2_Display_Old,HOP_Toatl_Start,Hopper_val;

double Totaliser_Update ,Totaliser1_Update,Totaliser2_Update;
char Total_Cnt;

unsigned char Status_Pulse,Pulse_Count,Pulse_Count_Value,Output_Relay1,Output_Relay2;
extern char Start_Stop_status;

extern uint16_t scan_count;
extern uint32_t Key_Input;
char Totaliser_clear_string[20];
char *Totaliser_clear_ptr = Totaliser_clear_string;
extern uint32_t totalizer_parameters[2];
extern unsigned char ZERO_STATUS;

char total_status;

extern char Oled_flag,Rows_clear,ACAL_Status;
extern uint8_t ram_values[RAM_SIZE];        //values in RAM
extern char lcd_flag;
double S1_Start,Delta_S1;

void Init_Totaliser()
{
  Totaliser1_Multiplier1 = 1;
    Totaliser1_Multiplier2 = 1;
    Totaliser1_Divisor1 = 1;
    Totaliser1_Divisor2 = 1;

    if((Units_Capacity == 0)&&(Units_Totaliser1 == 1)) //0 = kg/h 1 = t/h
        {
            Totaliser1_Divisor1 = 1000;
        }

            if((Units_Capacity == 1)&&(Units_Totaliser1 == 0)) //Capacity : 0 = kg/h, 1 = t/h  ; Totalizer 0 = kg, 1 = t;
                {
                    Totaliser1_Multiplier1 = 1000;
                }

                if(Dec_Point_Capacity == Dec_Pt_Totaliser1)
                    {
                        Totaliser1_Divisor1 = Totaliser1_Multiplier1 = 1;
                    }
                    //Dec_Point_Capacity=2;//msk
            if(Dec_Point_Capacity > Dec_Pt_Totaliser1)
                {
                    for(Total_Cnt=0;Total_Cnt<(Dec_Point_Capacity - Dec_Pt_Totaliser1);Total_Cnt++)
                    {
                        Totaliser1_Divisor2 = 10 * Totaliser1_Divisor2;
                    }
                }

    if(Dec_Pt_Totaliser1 > Dec_Point_Capacity )
        {
            for(Total_Cnt=0;Total_Cnt<(Dec_Pt_Totaliser1 - Dec_Point_Capacity ); Total_Cnt++)
                {
                    Totaliser1_Multiplier2  = 10 * Totaliser1_Multiplier2;
                }
        }

    Totaliser2_Multiplier1 = 1;
    Totaliser2_Multiplier2 = 1;
    Totaliser2_Divisor1 = 1;
    Totaliser2_Divisor2 = 1;

    if((Units_Capacity == 0)&&(Units_Totaliser2 == 1)) //Capacity : 0 = kg/h, 1 = t/h  ; Totalizer 0 = kg, 1 = t;
        {
            Totaliser2_Divisor1 = 1000;
        }

            if((Units_Capacity == 1)&&(Units_Totaliser2 == 0)) //Capacity : 0 = kg/h, 1 = t/h  ; Totalizer 0 = kg, 1 = t;
                {

                    Totaliser2_Multiplier1 = 1000;
                }

                if(Dec_Point_Capacity == Dec_Pt_Totaliser2)
                    {
                        Totaliser2_Divisor1 = Totaliser2_Multiplier1 =  1;
                    }

         if(Dec_Point_Capacity > Dec_Pt_Totaliser2)
            {
                for(Total_Cnt=0;Total_Cnt<(Dec_Point_Capacity - Dec_Pt_Totaliser2);Total_Cnt++)
                {
                    Totaliser2_Divisor2 = 10 * Totaliser2_Divisor2  ;
                }
            }

    if(Dec_Pt_Totaliser2 > Dec_Point_Capacity )
        {
            for(Total_Cnt=0;Total_Cnt<(Dec_Pt_Totaliser2 - Dec_Point_Capacity );Total_Cnt++)
                {
                    Totaliser2_Multiplier2  = 10 * Totaliser2_Multiplier2;
                }
        }
    /*********************************************************************/

        Totaliser1 = RTC_ReadGPREG(LPC_RTC,0);          //read totaliser values from Registres powered by Rtc clock
        Totaliser2 = RTC_ReadGPREG(LPC_RTC,1);            //read totaliser values from Registres powered by Rtc clock

        Totaliser1_Display = Totaliser1 ;
        Totaliser2_Display = Totaliser2 ;
        Totaliser1_Display_Old = Totaliser1_Display ;
        Totaliser2_Display_Old = Totaliser2_Display;
        Status_Pulse = OFF;
        Output_Relay1 = OFF;
        Output_Relay2 = OFF;
            if(Pulse_Width == 0)
            {
                Pulse_Count_Value = 1;
            }
                if(Pulse_Width == 1)
                {
                    Pulse_Count_Value = 2;
                }
                    if(Pulse_Width == 2)
                        {
                            Pulse_Count_Value = 5;
                        }
                            if(Pulse_Width == 3)
                                {
                                    Pulse_Count_Value = 10;
                                }
                        Pulse_Count = Pulse_Count_Value;

}
void Totaliser()  //every 10ms
{
//    Totaliser2=0;
//  Totaliser1=0;
if(Start_Stop_status == START )
    {
    if( (ZERO_STATUS >0)  && (ZERO_STATUS < 7)  ) //if Zeroing is in progress exit routine
        {
            return;
        }

            Totaliser_Update = (double)Actual_Feedrate / 360000;

            Totaliser1_Update = (Totaliser_Update * Totaliser1_Multiplier1) / Totaliser1_Divisor1;
            Totaliser1_Update = (Totaliser1_Update * Totaliser1_Multiplier2) / Totaliser1_Divisor2;
            Totaliser1 = Totaliser1 + Totaliser1_Update;
            Totaliser1_Display = Totaliser1;
            if((S1_Start!=Totaliser1_Display)&&(ACAL_Status==2))
                {
                    Delta_S1 =     Totaliser1_Display - S1_Start ;
                    FDR_Total = FDR_Total + Delta_S1;
                }

        S1_Start = Totaliser1_Display ;

            Totaliser2_Update =(Totaliser_Update * Totaliser2_Multiplier1) / Totaliser2_Divisor1;
            Totaliser2_Update = (Totaliser2_Update * Totaliser2_Multiplier2) / Totaliser2_Divisor2;
            Totaliser2 = Totaliser2 + Totaliser2_Update;
            Totaliser2_Display = Totaliser2;

             RTC_WriteGPREG(LPC_RTC,0,Totaliser1); //write totaliser values into Registres powered by Rtc clock
                RTC_WriteGPREG(LPC_RTC,1,Totaliser2); //write totaliser values into Registres powered by Rtc clock

        return;
    }
}

/*Totaliser1 Clear Routine*/
void Totaliser1_Clear()
{

    if(Oled_flag ==3) //wait for clearing lcd
        {

            switch(total_status)
            {
                case 1 :total_status = 2;
                                break;
                case 2 : Totaliser_clear_ptr = "Clear Totaliser1";
                                 Print_String2(&Totaliser_clear_ptr,line1,0,font10,0);
                                 Totaliser_clear_ptr = "->-YES, CL-NO";
                                 Print_String2(&Totaliser_clear_ptr,line2,0,font10,0);
                                 if(lcd_flag==3)
                                 lcd_flag = 1;
                                 break;

                case 3 : Totaliser1_Display = 0;
                                 Totaliser1 = 0;
                                 Totaliser1_Update = 0;
                         total_status = 4;
                                 RTC_WriteGPREG(LPC_RTC,0,Totaliser1); //write totaliser values into Registres powered by Rtc clock
                                 break;
                case 4: total_status = 0;
                                return;
                default:break;

            }
}

//    Clear_LCD();
//    while(1)
//    {
//        super_delay(40);
//        Totaliser_clear_ptr = "Clear Totaliser1";
//        Print_String(&Totaliser_clear_ptr,line2,0,font16,0);
//        Totaliser_clear_ptr = "->-YES, CL-NO";
//        Print_String(&Totaliser_clear_ptr,line3,0,font16,0);
//        super_delay(40);
//        CL_T1_Jump:
//            if(scan_count%20 ==0)
//                key_check_routine();
//                    if(Key_Input==HOR)
//                        {
//                            Clear_LCD();
//                            Totaliser1_Display = 0;
//                            Totaliser1 = 0;
//                            Totaliser1_Update = 0;
//                            //Start_Stop_status =STOP ;
//                            return;
//                        }
//                    if(Key_Input==CL)
//                        {
//                            Clear_LCD();
//                            return;
//                        }
//        goto CL_T1_Jump;
//
//    }
}

/* Totaliser_Out Routine Called Every 50ms*/
void Totaliser_Out()
{
    if(Pulse1  == 0) //Off,S1,S2
        {
            return;
        }

    if(Status_Pulse == ON)
        {
            --Pulse_Count;
            if(Pulse_Count == 0)
                {
                    Output_Relay1 = OFF;
                    O004 = Output_Relay1 ;
                    Pulse_Count = Pulse_Count_Value;
                    Status_Pulse = OFF;
                    if(Pulse2 == 1 )
                        {
                            Output_Relay2 = OFF;
                            O005 = Output_Relay2 ;
                        }
                    return;
                }
        }

    if(Status_Pulse == OFF)
        {
                if(Pulse1  == 1)
                    {
                        if( Totaliser1_Display != Totaliser1_Display_Old)
                            {
                                Output_Relay1 = ON;
                                O004 = Output_Relay1 ;
                                Totaliser1_Display_Old = Totaliser1_Display ;
                                Status_Pulse = ON;
                                if(Pulse2 == 1 )
                                    {
                                        Output_Relay2 = ON;
                                        O005 = Output_Relay2 ;
                                    }
                                return;
                            }
                        else
                            return;
                    }
                if(Pulse1  == 2)
                    {
                        if( Totaliser2_Display != Totaliser2_Display_Old)
                            {
                                Output_Relay1 = ON;
                                O004 = Output_Relay1 ;
                                Totaliser2_Display_Old = Totaliser2_Display ;
                                Status_Pulse = ON;
                                 if(Pulse2 == 1 )
                                    {
                                        Output_Relay2 = ON;
                                        O005 = Output_Relay2 ;

                                    }
                                 return;
                            }
                        else
                            return;
                    }
            }
}

/*
29/03/2018
Totaliser not incrementing due to (float)Totaliser so removed it as float is limiting it if val is > its limit1.2e-38 to 1.2e+38

*/
