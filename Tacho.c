/*Tacho*/

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

unsigned int Tacho_Counter, Inv_Op_Freq, Inv1_Op_Freq;
unsigned char Inv_Op_current, Inv1_Op_current;

extern unsigned int External_Setpoint_display, External_Setpoint, Common_Setpoint,
    Current_Setpoint, Deviation, PF_Drive_Setpoint_Per;
extern unsigned int Actual_Feedrate;
extern float WF_Drive_Setpoint, PF_Drive_Setpoint;
extern uint8_t ram_values[RAM_SIZE];
extern uint16_t New_menu_parameters[105];

unsigned char Mode_Status;
float New_Integral, Old_Integral;
extern char Start_Stop_status;
extern unsigned int Belt_Speed;

unsigned int Deviation_Value;
unsigned char Dev_Sign;

unsigned char Tacho_Pointer, Tacho_Buffer = 5;
float Tacho_Avg;
unsigned int Tacho_Sum, Tacho_Register[6];
float Temp_val_integral;

extern uint16_t AI_Value, AI_avg, AI_calb_val[6];
extern int Inv_Sp;
extern signed int Belt_Load;

char New_Start, PF_Status;

uint16_t Display_Tacho_Reg[26], Display_Belt_Speed;
unsigned int Display_Tacho_Sum;
unsigned char Display_Tacho_Buffer, Display_Tacho_Pointer, Zero_Tacho_Feed_Buffer_Flag;
extern unsigned int Display_Feedrate, Feed_buffer, Feed_pointer, Feed_buffer, Feed_reg[1600];

//-------------------------------------------------
void Tacho_init()
{
    LPC_TIM1->TCR = 1;
    Tacho_Pointer = 0;
    Tacho_Avg = 0;
    Tacho_Sum = 0;
}
//-------------------------------------------------

//-------------------------------------------------
void Tacho_Avg_Calculation()
{
    Tacho_Sum = Tacho_Sum + Tacho_Counter;
    if (Tacho_Sum > Tacho_Register[Tacho_Pointer])
        Tacho_Sum = Tacho_Sum - Tacho_Register[Tacho_Pointer];
    Tacho_Register[Tacho_Pointer] = Tacho_Counter;
    Tacho_Pointer++;
    if (Tacho_Pointer >= Tacho_Buffer)
        Tacho_Pointer = 0;

    if (Start_Stop_status == START)
    {
        Display_Tacho_Sum = Display_Tacho_Sum + Belt_Speed - Display_Tacho_Reg[Display_Tacho_Pointer];
        Display_Tacho_Reg[Display_Tacho_Pointer] = Belt_Speed;
        Display_Tacho_Pointer++;
        if (Display_Tacho_Pointer >= Display_Tacho_Buffer)
            Display_Tacho_Pointer = 0;
        Display_Belt_Speed = Display_Tacho_Sum / Display_Tacho_Buffer;
    }
}
//-------------------------------------------------

//-------------------------------------------------
void Zero_Tacho_Feed_Buffer()
{
    uint16_t i;
    for (i = 0; i <= Feed_buffer; i++)
    {
        if (i <= Display_Tacho_Buffer)
            Display_Tacho_Reg[i] = 0;
        Feed_reg[i] = 0;
    }
    Zero_Tacho_Feed_Buffer_Flag = 1;
}
//-------------------------------------------------

/*
 * Description / Algorithm comment - see PF_Drive_Setpoint_Cal logic below.
 */
//-------------------------------------------------
void PF_Drive_Setpoint_Cal()
{
    if (Prefeeder == 0)
        return;

    //if (Mode_Status == Local)
    if (GetFieldMode())
    {
        PF_Drive_Setpoint = 0;
        PF_Drive_Setpoint_Per = 0;
        return;
    }

    if (Start_Stop_status == 1)
    {
        PF_Drive_Setpoint = 0;
        PF_Drive_Setpoint_Per = 0;
        M067 = 0;
        return;
    }

    if (Start_Stop_status == 0)
    {
        M067 = 1;
        return;
    }

    //if (Mode_Status == Manual)
    if (GetDeIntVolMode())
    {
        PF_Drive_Setpoint = (((float)(Current_Setpoint * 4095) / Capacity) * (((float)PF_StartSet / 1000)));
        PF_Drive_Setpoint_Per = ((float)(PF_Drive_Setpoint * 1000) / 4095);
        if (PF_Drive_Setpoint_Per > PF_Limit)
        {
            PF_Drive_Setpoint = ((float)PF_Limit * 4095) / 1000;
            PF_Drive_Setpoint_Per = ((float)(PF_Drive_Setpoint * 1000) / 4095);
        }
        M067 = 1;
        return;
    }

    //if ((Mode_Status == Interlock) || (Mode_Status == De_Interlock))
    if ((GetIntMode()) || (GetDeIntMode()))
    {
        if (New_Start == 1)
        {
            New_Start = 0;
            if (Belt_Load < BL_Limit)
            {
                PF_Status = 1;
                PF_Drive_Setpoint = (((float)(Current_Setpoint * 4095) / Capacity) * (((float)PF_StartSet / 1000)));
                PF_Drive_Setpoint_Per = ((float)(PF_Drive_Setpoint * 1000) / 4095);
                if (PF_Drive_Setpoint_Per > PF_Limit)
                {
                    PF_Drive_Setpoint = ((float)PF_Limit * 4095) / 1000;
                    PF_Drive_Setpoint_Per = ((float)(PF_Drive_Setpoint * 1000) / 4095);
                }
                M067 = 1;
                return;
            }
            return;
        }

        if (PF_Status == 0)
        {
            switch (Regulation)
            {
                case 0:
                    PF_Drive_Setpoint = (((float)(Current_Setpoint * 4095) / Capacity) * (((float)Reg_Tracking / 1000)));
                    PF_Drive_Setpoint_Per = ((float)(PF_Drive_Setpoint * 1000) / 4095);
                    if (PF_Drive_Setpoint_Per > PF_Limit)
                    {
                        PF_Drive_Setpoint = ((float)PF_Limit * 4095) / 1000;
                        PF_Drive_Setpoint_Per = ((float)(PF_Drive_Setpoint * 1000) / 4095);
                    }
                    M067 = 1;
                    break;
                case 1:
                    PF_Drive_Setpoint = (Inv_Sp * ((float)Reg_Tracking / 1000));
                    PF_Drive_Setpoint_Per = ((float)(PF_Drive_Setpoint * 1000) / 4095);
                    if (PF_Drive_Setpoint_Per > PF_Limit)
                    {
                        PF_Drive_Setpoint = ((float)PF_Limit * 4095) / 1000;
                        PF_Drive_Setpoint_Per = ((float)(PF_Drive_Setpoint * 1000) / 4095);
                    }
                    M067 = 1;
                    break;
                default:
                    break;
            }
        }
    }
}
//-------------------------------------------------

//-------------------------------------------------
void Calculate_Setpoint_Display()
{
    //if (Mode_Status == Local)
    if (AI_avg > AI_calb_val[0])
    {
        External_Setpoint = ((unsigned int)((float)(AI_avg - AI_calb_val[0]) * Capacity) / (AI_calb_val[1] - AI_calb_val[0]));
    }
    else
    {
        External_Setpoint = 0;
    }
    if (GetFieldMode())
    {
        Current_Setpoint = 0;
        WF_Drive_Setpoint = 0;
        if ((Inv == 1) || (Inv == 2) || (Inv == 3) || (Inv == 4) || (Inv == 5) || (Inv == 6) || (Inv == 7) || (Inv == 8))
        {
            if ((Inv == 3) || (Inv == 6) || (Inv == 7) || (Inv == 8))
            {
                Inv_Sp = (((unsigned int)(WF_Drive_Setpoint) * (100)) / 4095);
                Inv_Sp = Inv_Sp * 100;
            }
            else
                Inv_Sp = (((unsigned int)(WF_Drive_Setpoint) * (500)) / 4095);
        }
        return;
    }
/*   
//msk 18/03/26  mode selection changed to int/de-int mode, 
//and setpoint assignment is done in controls.c
    if ((Mode_Status == Interlock) && (M002 == 1))
        Mode_Status = Remote;

    if ((Mode_Status == Remote) && (M002 == 1))
        Current_Setpoint = Common_Setpoint;

    if ((Mode_Status == Interlock) && (M002 == 0))
        Current_Setpoint = External_Setpoint;

    if (Mode_Status == De_Interlock)
        Current_Setpoint = Setpoint_Int;

    if (Mode_Status == Manual)
        Current_Setpoint = Setpoint_Man;

    if (Current_Setpoint > Capacity)
        Current_Setpoint = Capacity;
*/

}
//-------------------------------------------------
//-------------------------------------------------
void Calculate_WF_drive_setpoint_Display()
{
    //if (Mode_Status == Local)
    if (GetFieldMode())
    {
        Current_Setpoint = 0;
        WF_Drive_Setpoint = 0;
        if ((Inv == 1) || (Inv == 2) || (Inv == 3) || (Inv == 4) || (Inv == 5) || (Inv == 6) || (Inv == 7) || (Inv == 8))
        {
            if ((Inv == 3) || (Inv == 6) || (Inv == 7) || (Inv == 8))
            {
                Inv_Sp = (((unsigned int)(WF_Drive_Setpoint) * (100)) / 4095);
                Inv_Sp = Inv_Sp * 100;
            }
            else
                Inv_Sp = (((unsigned int)(WF_Drive_Setpoint) * (500)) / 4095);
        }
        return;
    }
/*  
//msk 18/03/26  mode selection changed to int/de-int mode, 
//and setpoint assignment is done in controls.c
    if ((Mode_Status == Interlock) && (M002 == 1))
        Mode_Status = Remote;

    if ((Mode_Status == Remote) && (M002 == 1))
        Current_Setpoint = Common_Setpoint;

    if ((Mode_Status == Interlock) && (M002 == 0))
    {
        if (AI_avg > AI_calb_val[0])
            External_Setpoint = ((unsigned int)((float)(AI_avg - AI_calb_val[0]) * Capacity) / (AI_calb_val[1] - AI_calb_val[0]));
        else
            External_Setpoint = 0;
        Current_Setpoint = External_Setpoint;
    }

    if (Mode_Status == De_Interlock)
        Current_Setpoint = Setpoint_Int;

    if (Mode_Status == Manual)
        Current_Setpoint = Setpoint_Man;

    if (Current_Setpoint > Capacity)
        Current_Setpoint = Capacity;
*/
    if (Start_Stop_status == STOP)
    {
        WF_Drive_Setpoint = 0;
        Old_Integral = 0;
        New_Integral = 0;
        if ((Inv == 1) || (Inv == 2) || (Inv == 3) || (Inv == 4) || (Inv == 5) || (Inv == 6) || (Inv == 7) || (Inv == 8))
        {
            if ((Inv == 3) || (Inv == 6) || (Inv == 7) || (Inv == 8))
            {
                Inv_Sp = (((unsigned int)(WF_Drive_Setpoint) * (100)) / 4095);
                Inv_Sp = Inv_Sp * 100;
            }
            else
                Inv_Sp = (((unsigned int)(WF_Drive_Setpoint) * (500)) / 4095);
        }
        return;
    }
    //if (Mode_Status == Manual)
    if (GetVolMode())
    {   
        WF_Drive_Setpoint = ((float)(Current_Setpoint * 4095) / (Capacity));
        if ((Inv == 1) || (Inv == 2) || (Inv == 3) || (Inv == 4) || (Inv == 5) || (Inv == 6) || (Inv == 7) || (Inv == 8))
        {
            if ((Inv == 3) || (Inv == 6) || (Inv == 7) || (Inv == 8))
            {
                Inv_Sp = (((unsigned int)(WF_Drive_Setpoint) * (100)) / 4095);
                Inv_Sp = Inv_Sp * 100;
            }
            else
                Inv_Sp = (((unsigned int)(WF_Drive_Setpoint) * (500)) / 4095);
        }
        return;
    }

    if (Current_Setpoint > Actual_Feedrate)
    {
        Deviation_Value = (((Current_Setpoint - Actual_Feedrate) * 1000) / (Current_Setpoint));
        Dev_Sign = Positive;
    }
    if (Current_Setpoint < Actual_Feedrate)
    {
        Deviation_Value = (((Actual_Feedrate - Current_Setpoint) * 1000) / (Current_Setpoint));
        Dev_Sign = Negative;
    }

    if ((Int_Time == 0) && (Prop_Gain == 0))
        return;

    if (Deviation_Value < Dead_Band)
        return;

    if (Deviation_Value > Dead_Band)
    {
        Temp_val_integral = (float)((float)Current_Setpoint - (float)Actual_Feedrate);
        Temp_val_integral = (float)(Temp_val_integral * 4095);
        Temp_val_integral = (float)(Temp_val_integral / Capacity);

        if (Dev_Sign == Positive)
            New_Integral = (Old_Integral + ((float)(Temp_val_integral * 10) / Int_Time));
        if (Dev_Sign == Negative)
        {
            New_Integral = (Old_Integral + ((float)(Temp_val_integral * 10) / Int_Time));
            if (New_Integral < 0)
                New_Integral = 0;
        }
        if (New_Integral < 4095)
            Old_Integral = New_Integral;

        if (Dev_Sign == Positive)
            New_Integral = (New_Integral + ((float)(Temp_val_integral * (float)Prop_Gain) / 1000));
        if (Dev_Sign == Negative)
            New_Integral = (New_Integral + ((float)(Temp_val_integral * (float)Prop_Gain) / 1000));

        if (New_Integral < 0)
            New_Integral = 0;
        if (New_Integral > 4095)
            New_Integral = 4095;

        WF_Drive_Setpoint = New_Integral;
        if ((Inv == 1) || (Inv == 2) || (Inv == 3) || (Inv == 4) || (Inv == 5) || (Inv == 6) || (Inv == 7) || (Inv == 8))
        {
            if ((Inv == 3) || (Inv == 6) || (Inv == 7) || (Inv == 8))
            {
                Inv_Sp = (((unsigned int)(WF_Drive_Setpoint) * (100)) / 4095);
                Inv_Sp = Inv_Sp * 100;
            }
            else
                Inv_Sp = (((unsigned int)(WF_Drive_Setpoint) * (500)) / 4095);
        }
    }
}
//-------------------------------------------------
