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
#include "RAM512KB_Ext.h"

extern char Start_Stop_status;
extern unsigned char Mode_Status,Deviation_Sign;
extern uint8_t ram_values[RAM_SIZE];        //values in RAM
unsigned int Cycle_Time_Count,Min_Pulse_Time_Count,On_Time,On_Time_Count,Off_Time,On_Time_Count_temp;
extern uint16_t  AI1_avg,AI_calb_val[6], New_menu_parameters[91];;
extern unsigned int External_Setpoint_display,
                         External_Setpoint,
                         Common_Setpoint,
                         Current_Setpoint,
                         Deviation,
                       PF_Drive_Setpoint_Per,
                         WF_Drive_Setpoint_per,
                         Hopper_val,
                         Gate_Postion,
                         HOpper_total,        //HOpper_total_try,
                         FDR_Total,FDR_Total_Try,
                         CM_Difference,
                         CM_Percentage;
/*
Algorithm:        Added on 22/01/2018

1    If in Local mode,
    If Mxx1=1, O000 (Forward Output) =1, O001 (Reverse Output) = 0
    If Mxx1=0, O000 (Forward Output) =0, O001 (Reverse Output) = 0
    If Mxx2=1, O000 (Forward Output) =0, O001 (Reverse Output) = 1
    If Mxx2=0, O000 (Forward Output) =0, O001 (Reverse Output) = 0

2    If in Stop mode, exit

3    If in Manual Mode,
    i) Dosing Valve Forward Output (O000) will be ON when,
        [(Wm_Setpoint*100/Max_Capacity)  � Dead_Band] > Gate_Position

    ii) Dosing Valve Reverse Output (O001) will be ON when,
        [(Wm_Setpoint*100/Max_Capacity) + Dead_Band] < Gate_Position

    iii) Both Outputs will be OFF when,
         [(Wm_Setpoint*100/Max_Capacity) - Dead_Band] < Gate_Position < [(Wm_Setpoint*100/Max_Capacity) + Dead_Band]

4    If in Auto Mode,
a    Is Cycle_Time_Count>0,
         Yes,
            Is Xd < Dead_Band?
            Yes,    Forward_Output = Off
                Reverse_Output = Off
                Exit
            No,    Decrement Cyle_Time_Count
                If ON_Time_Count >0,                u
                    Decrement ON_Time_Count
                     If Xd is +ve (Actual > Setpoint), �Dosing_Valve_Forward� output (O000) is OFF & �Dosing_Valve_Reverse� output (O001) is ON.
                     If Xd is -ve (Actual < Setpoint), �Dosing_Valve_Forward� output (O000) is ON & �Dosing_Valve_Reverse� output (O001) is OFF.
                    exit
                Else,    Forward_Output = Off
                    Reverse_Output = Off
                    Exit

b    Cycle_Time_Count=0
        Cycle_Time_Count = I_Time / Scan_Count (If routine called every 5 msec & I_Time = 003,0 secs, then Cycle_Time_Count = 3.0 * 1000 / 5 = 600)
        Min_Pulse_Time_Count = Min_Pulse_Time / Scan_Count
        Calculate On_time  [ON_Time   = (P1_Gain) * (Xd) * (I1_time) ]
        On_Time_Count = ON_Time_Count_temp + On_Time / Scan_Count
        If On_Time_Count < Min_Pulse_Time_Count, ON_Time_Count_temp = ON_Time_Count
                    ON_Time_Count = 0
                    Exit

Add in exisitng routine,
Start_Loop:                //Called once during START
    Cycle_Time_Count = 0
    ON_Time_Count_Temp=0

    Assigned Markers
    M021 = Dosing_Valve_Forward;
    M022 = Dosing_Valve_Reverse;
    O000 = Forward Output;
    O001 = Reverse Output;

*/

void Gate_Postion_CalCulation()
{
    if(PI==1)
        {
            if(AI1_avg > AI_calb_val[2])
                Gate_Postion=    ((AI1_avg-AI_calb_val[2])*1000)/(AI_calb_val[3]-AI_calb_val[2]);
                    else
                        Gate_Postion = 0;
        }
}

void Valve_Control()
{
    if(PI==1)
    {
        if(Start_Stop_status ==STOP)// if STOP
        {
            return;
        }
        //else if(Mode_Status==Local)    // if Mode Status is "Local"
        else if(GetFieldMode())    // if Mode Status is "Local"
        {
            if(M021==1) //If Mxx1=1, O000 (Forward Output) =1, O001 (Reverse Output) = 0
            {
                O000=1;
                O001=0;
            }
            if(M021==0) //If Mxx1=0, O000 (Forward Output) =0, O001 (Reverse Output) = 0
            {
                O000=0;
            }
            if(M022==1)//If Mxx2=1, O000 (Forward Output) =0, O001 (Reverse Output) = 1
            {
                O000=0;
                O001=1;
            }
            if(M022==0)//    If Mxx2=0, O000 (Forward Output) =0, O001 (Reverse Output) = 0
            {
                O001=0;
            }
            return;
        }
        //else if(Mode_Status ==Manual)    // if Mode Status is "Manual"
        else if(GetDeIntVolMode())    // if Mode Status is "Manual"
        {
            Gate_Postion_CalCulation();
            if(Gate_Postion < (((float)((float)(Setpoint_Man*100)/(float)Capacity)*10)-Dead_Band)/*((((Setpoint_Man*100)/Capacity)*10)-Dead_Band)*/  )
                O000 = 1 ;
            else
                O000 = 0;
            if(Gate_Postion > (((float)((float)(Setpoint_Man*100)/(float)Capacity)*10)+Dead_Band)/*((((Setpoint_Man*100)/Capacity)*10)+Dead_Band)  */)
                O001 = 1 ;
            else
                O001 = 0;
            if((Gate_Postion > ( ((float)((float)(Setpoint_Man*100)/(float)Capacity)*10)-Dead_Band)) && ( Gate_Postion < (((float)((float)(Setpoint_Man*100)/(float)Capacity)*10)+Dead_Band) )  )
                {
                    O000 = 0 ;
                    O001 = 0 ;
                }
            return;
        }
        //  else if((Mode_Status == Interlock)|| (Mode_Status == De_Interlock))// if Mode Status is "Auto"
        else if((GetIntMode()) || (GetDeIntMode()))// if Mode Status is "Auto"
        {
            if(Cycle_Time_Count>0)
            {
                if((Deviation*10) < Dead_Band)/*muliplied by 10 to get Dead bands actual value */
                {
                    O000 = 0 ;
                    O001 = 0 ;
                    return;
                }
                else
                {
                    --Cycle_Time_Count;
                    if(On_Time_Count>0)
                    {
                        --On_Time_Count;
                        if(Deviation_Sign ==Positive)
                        {
                            O000 = 0 ;
                            O001 = 1 ;
                        }
                        if(Deviation_Sign ==Negative)
                        {
                            O000 = 1;
                            O001 = 0;
                        }
                        return;
                    }
                    else
                    {
                        O000 = 0;
                        O001 = 0;
                    }
                }
            }
            if(Cycle_Time_Count == 0)
            {
                Cycle_Time_Count = ((Int_Time*100)/10);//Considering Routine called every 10 msec
                Min_Pulse_Time_Count = ((Min_Pulse*100)/10);//Considering Routine called every 10 msec
                On_Time = Prop_Gain*Deviation*Int_Time;
                On_Time_Count= ( ( On_Time_Count_temp )    + ( ( On_Time) / 10000) );//Considering Routine called every 10 msec

                if(On_Time_Count < Min_Pulse_Time_Count)
                {
                    On_Time_Count_temp = On_Time_Count;
                    On_Time_Count = 0;
                }
                return;
            }
        }
    }
}
/*Date :20/07/2019
changed in
Valve_Control()
1:To Use actual values of parameters i have divided them by 10 & As Routine called for every 10 msec its already multiplied by 1000 & divded  by 10
2:To use Dead bands actual value which is used to compare with deviation i have multiplied deviation by 10 instead of dividing dead band
3:Also for calculation of on time formula is OnTime = Prop/100 * Dev/100 * Int_Time; & On Time count =  (OnTime*1000 + On time count temp)/10

*/
