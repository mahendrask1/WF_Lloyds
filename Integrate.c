/*Integrate*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "LPC17xx.h"                    // Device header
#include "lpc17xx_spi.h"
#include "lpc17xx_gpio.h"
#include "GPIO_LPC17xx.h"
#include "lpc17xx_rtc.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_uart.h"
#include "main_routine.h"
#include "OLED_2_7_12864.h"
#include "functions.h"
#include "parameters.h"  //#include "menu.h"
#include "timer.h"
#include "eeprom.h"
#include "adc.h"
#include "dac.h"
#include "spi.h"
#include "plc.h"
#include "io_control.h"
#include "modbus.h"
#include "keys.h"

unsigned int Count_Tacho;

extern unsigned long   Count_Limit1,Count_Limit2,Count_Max,Count_Min,
                                             Scan_Count_Load_Monitor,Scan_Count_Limit1,
                                             Scan_Count_Limit2,Scan_Count_MinLimit,Scan_Count_MaxLimit,
                                             scan_count_belt_track,scan_count_belt_track_monitor;

extern unsigned long Present_Count,zero_load,Span_Count;
extern unsigned int  Actual_Feedrate  ;
float Actual_Feedrate_Calculate,Display_Feedrate_calculate;
extern unsigned int Belt_Speed;
extern char Start_Stop_status;
extern uint16_t New_menu_parameters[105];
extern unsigned char Tacho_Pointer,Tacho_Buffer;

extern float Tacho_Avg;
extern unsigned int Tacho_Sum,Tacho_Register[6];
extern char* P_String;
extern char A_String[200];
extern unsigned int Tacho_Counter;
extern unsigned long BL_avg ;
/*note   Present_Count is Count_Present , zero_load is Count_0% */
unsigned int Display_Feedrate,Feed_buffer,Feed_pointer,Feed_buffer,Feed_reg[1600];
unsigned long Feed_sum;
extern unsigned char Display_Tacho_Buffer,Zero_Tacho_Feed_Buffer_Flag ;
extern uint16_t Display_Belt_Speed ,Display_Tacho_Sum;;
extern unsigned char Display_Tacho_Buffer,Display_Tacho_Pointer,Zero_Tacho_Feed_Buffer_Flag;
extern unsigned long Scan_Count_Min_FD;
void feedrate_display_calculation()
{
        if(BL_avg < Count_Min)
        {
            if(Scan_Count_Min_FD > 0){--Scan_Count_Min_FD;}
            if(Scan_Count_Min_FD == 0)// || (BL_avg == 0))
            {
                Actual_Feedrate = 0;
                Display_Feedrate= 0;
                Scan_Count_Min_FD = 1; //avoid under-flow
                return;
            }
            if(BL_avg <= zero_load)    //-ve load
            {
                Actual_Feedrate = 0;
                Display_Feedrate= 0;
                return;
            }
        }
        else
        {
            Scan_Count_Min_FD = (Scan_Count_Load_Monitor * 100);
        }

    if(( Fixed_Frequency == 1 ) && (Start_Stop_status == START ) )  // fixed Frequency Yes = 1 , No = 0 check in menu
        {
                Belt_Speed = Tacho_Freq;
                Display_Feedrate_calculate  =   ((float)( Belt_Speed ) / ( Tacho_Freq ));
        }
    else
        {
            //Tacho_Avg =(float)((float)Tacho_Sum/Tacho_Buffer);
            Display_Feedrate_calculate =   ((float)(((float)Tacho_Sum/Tacho_Buffer)) / (Tacho_Freq));
        }
    Display_Feedrate_calculate = Display_Feedrate_calculate*(Capacity);
    Display_Feedrate_calculate = (float)((float)Display_Feedrate_calculate/ (float)Span_Count );
    Display_Feedrate = ( Display_Feedrate_calculate* (float)( (float)BL_avg - (float)zero_load ));
    if(  (Correction_Factor > 4999)&&(Correction_Factor < 15001) ) //Correction factor condition added here on 27/12/2017
        {
            Display_Feedrate = Display_Feedrate * Correction_Factor ;
            Display_Feedrate = Display_Feedrate /10000;
        }

    if(Feed_pointer >= Feed_buffer )
        Feed_pointer = 0;

    Feed_sum = ( Feed_sum +    Display_Feedrate - Feed_reg[Feed_pointer] );
    Feed_reg[Feed_pointer] =  Display_Feedrate ;
    Feed_pointer++;
    Display_Feedrate = Feed_sum /Feed_buffer;

}

void feedrate_display_Init_calculation()
{
    switch(Display_Filter)
    {
         case 0 :    Feed_buffer  = 1;     //0.002msec
                            Display_Tacho_Buffer= 1;
             break;
         case 1 : Feed_buffer  = 250;        //0.018msec
                            Display_Tacho_Buffer= 5;
             break;
         case 2 : Feed_buffer  = 500;   //0.034msec
                            Display_Tacho_Buffer= 10;
             break;
         case 3 :Feed_buffer   = 750;        //0.05msec
                         Display_Tacho_Buffer= 15;
             break;
         case 4 :Feed_buffer   = 1000;        //0.065msec
                         Display_Tacho_Buffer= 20;
             break;
         case 5 :Feed_buffer   = 1500;        //0.082msec
                         Display_Tacho_Buffer= 25;
             break;
         default:break;
     }
}

void Integrate()
{
    //if((Present_Count < Count_Min)||(Start_Stop_status==STOP)) //commented to check Display Feedrate 9999+ condition
    if(Start_Stop_status==STOP)
    {
        Actual_Feedrate = 0;
      Display_Feedrate= 0;
        Display_Belt_Speed = 0;
        Display_Tacho_Sum = 0;
        Display_Tacho_Pointer = 0;
        Feed_sum=0;
        Feed_pointer =0;
        goto Exit_Routine;
    }

    if(( Fixed_Frequency == 1 ) && (Start_Stop_status == START ) )  // fixed Frequency Yes = 1 , No = 0 check in menu
    {
        Belt_Speed = Tacho_Freq;
        Actual_Feedrate_Calculate =   ((float)( Belt_Speed ) / ( Tacho_Freq ));
    }
    else
    {
        if( Tacho_Counter==0 )
        {
            char i;
            for(i=0;i<=Tacho_Buffer;i++)
            {
                Tacho_Register[i]=0;}
                Tacho_Sum = 0;
        }
        Tacho_Avg =(float)((float)Tacho_Sum / Tacho_Buffer);
            Actual_Feedrate_Calculate =   ((float)( Tacho_Avg   ) / ( Tacho_Freq ));
    }

    Actual_Feedrate_Calculate =    Actual_Feedrate_Calculate *( Capacity );
    Actual_Feedrate_Calculate = (float)((float)Actual_Feedrate_Calculate / (float)Span_Count );
  Actual_Feedrate = ( Actual_Feedrate_Calculate * ( (float)Present_Count - (float)zero_load ));
  feedrate_display_calculation();

    if((Correction_Factor > 4999)&&(Correction_Factor < 15001) ) //Correction factor condition added here on 27/12/2017; Updated on 06/02/2018
    {
        Actual_Feedrate = Actual_Feedrate * Correction_Factor ;
        Actual_Feedrate = Actual_Feedrate /10000;
    }

    Exit_Routine:
        return;
}
/*
 * Date : 09/10/2022
 * File name: Integrate.c
 * Line no:. 123 //if((Present_Count < Count_Min)||(Start_Stop_status==STOP))
 * Details: Software version changed to 1.G
 * Bug: When Belt Load goes below MIN_LOAD i.e 10%, Display_Feedrate becomes 9999+, AO for TPH becomes 20mA
 * Solution: modified condition that clears paraemter responsible for feed rate calculation
 * Result: THP recovers when belt load recovers, Tacho_Freq as per motor speed, AO is as per TPH
 *
 */
