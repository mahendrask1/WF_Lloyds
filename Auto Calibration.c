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
#include "parameters.h"
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

/* Extern Variables*/
extern unsigned int Count_Tacho;
extern uint16_t New_menu_parameters[105];
extern unsigned long Present_Count,Present_Count_ch2,Span_Count_ch2,Span_Count,zero_load_ch2,zero_load,full_load;
extern unsigned long  Count_Max;
extern uint16_t scan_count;
extern uint32_t Key_Input;
extern unsigned long calib_val[8];
extern unsigned long  Count_Limit1,Count_Limit2,Count_Max,Count_Min,Scan_Count_PF_Monitor;
extern unsigned int Tacho_Counter;
extern unsigned int Belt_Speed;
extern char Error_No;
extern unsigned int  External_Setpoint_display,
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

float CM_Dif_value,CM_Percentage;
extern uint8_t ram_values[RAM_SIZE];        //values in RAM

/*New Variables*/
unsigned long Belt_REV_COUNT_ACAL, Belt_Rev_Count_Value_ACAL;
unsigned char ACAL_Status,BIN_PF_Start_Status,CM_Negative;
unsigned int HOP_Toatl_Start,HOP_Toatl_Present,Scan_Count_BIN_PF,Hop_Per;
extern double S1_Start,Delta_S1;
extern float CM_Display;
void Auto_Calibration(void);

void After_AutoCAL()    //To update Values related to Dead load i.e 0%load value
{
//******In auto  calibration span count remains unchanged*****//
    if(CM_Negative == 0 ) //Positive
        {
            zero_load= (zero_load + (unsigned long)(CM_Percentage));  //Count_DL = Zero Load
            full_load= (full_load + (unsigned long)(CM_Percentage));  //Count_DL = Zero Load
        }
    if(CM_Negative==1 ) //Negative
        {
            if(zero_load > CM_Percentage)
                {
                    zero_load =  zero_load - (unsigned long)(CM_Percentage) ; //Count_DL = Zero Load
                    full_load =     full_load - (unsigned long)(CM_Percentage) ;  //Count_DL = Zero Load
                }
        }
    Span_Count = full_load - zero_load; //Span_Count = Full_Loada_Count-Zero_Load_Count     Count_Limit1=(Span_Count* (float)Load_Limit1 / 1000) + zero_load;
    Count_Limit2=    (Span_Count* (float)Load_Limit2 / 1000) + zero_load;
    Count_Max     = (Span_Count* (float)Max_Load / 1000) + zero_load;
    Count_Min     = (Span_Count* (float)Min_Load / 1000) + zero_load;
    calib_val[0] = full_load    ;
    calib_val[2] =  (unsigned long)zero_load    ;
    save_calib_val();

}

void Auto_Calibration()
{

     switch(ACAL_Status)
        {
         case 0:
             return;
                        break;
         case 1:

         jump1:             Hop_Per=((float)(Present_Count_ch2  - zero_load_ch2)* 1000 / Span_Count_ch2);
                                Hopper_val=((float)(Present_Count_ch2  - zero_load_ch2) * Hopper_Capacity / (Span_Count_ch2));

                                if(Hop_Per < Limit_Nominal )
                                    {
                                        M075 = 1;//Hopper Prefeeder on o/p marker =1.
                                        return;
                                    }
                                if(M075==1)//if hopper pf is on
                                    {
                                        if(Hop_Per > Limit_Max)//Hopper Prefeeder wt is greater than max limit
                                            {
                                                M075 = 0;//Release Hopper Prefeeder Off command;
                                            }
                                    }
                                if(M075==1)//if Bin_Prefeeder is on
                                 {
                                        if(Scan_Count_BIN_PF!=0)//wait till limit dealy
                                            {
                                                --Scan_Count_BIN_PF;
                                                return;
                                            }
                                    }
                                Belt_REV_COUNT_ACAL=Rev_Time*No_Rev*10;

                                CM_Negative = 0;
                                ACAL_Status = 2;
                                CM_Difference = 0;
                                FDR_Total = 0;
                                HOpper_total =0;
                                Delta_S1 = 0;
                                HOP_Toatl_Start = Hopper_val;//                HOP_Toatl_Start =0;
                                M076 = 1;    //Control Measurement on Marker =1
                                M078 = 0;//Correction Limit Exeeded    =0

                        break;
         case 2:
                        Hop_Per=((float)(Present_Count_ch2  - zero_load_ch2) * 1000 / (Span_Count_ch2));
                        Hopper_val=((float)(Present_Count_ch2  - zero_load_ch2) * Hopper_Capacity /(Span_Count_ch2));
                        if( Hop_Per < Limit_Low)
                            {
                                ACAL_Status = 1;
                                goto jump1;
                            }

                            if(BIN_PF_Start_Status == 1    )    //;IF "HOPPER PF START"=ON, ACAL_STATUS=00 (ABORT)
                                {
                                    ACAL_Status = 0;//ACAL Status =0;
                                    M077=1;//Auto calibration aborted
                                }
                                if(HOP_Toatl_Start > Hopper_val)
                                HOpper_total = HOP_Toatl_Start - Hopper_val;

                            if(Error_No!=0)//Any Fault
                                {
                                    ACAL_Status = 0;
                                }
                                --Belt_REV_COUNT_ACAL;
                                if(Belt_REV_COUNT_ACAL!=0)
                                    return;
                                if(Belt_REV_COUNT_ACAL == 0)
                                    {
                                        if(FDR_Total > HOpper_total)
                                            {
                                                CM_Difference = FDR_Total-HOpper_total;
                                                CM_Percentage = (((float)CM_Difference*100)/FDR_Total);
                                                CM_Negative = 0;
                                            }
                                        if(HOpper_total > FDR_Total)
                                            {
                                                CM_Difference = HOpper_total - FDR_Total;
                                                CM_Percentage = (((float)CM_Difference*100)/FDR_Total);
                                                CM_Negative = 1;
                                            }
                                            /**************************************************/
                                            /*this is used to identify 1st digit after decimal point is >4 i.e 5 or not */
                                        if((unsigned int)((CM_Percentage - (unsigned int)(CM_Percentage) )*10) > 4 )
                                            {
                                                CM_Percentage  = (unsigned int)(CM_Percentage)  +1;//if  >4 then increment count by1
                                            }
                                        else//((unsigned int)((CM_Percentage  - (unsigned int)CM_Percentage )*10) <= 4 )
                                            {
                                                CM_Percentage  = (unsigned int)(CM_Percentage) ;//if  <4 then no change
                                            }

                                        CM_Display = ((float)CM_Percentage*Current_Setpoint*10)/Capacity; // This val is used to display on lcd in online screen

                                        if(CM_Percentage*10 <= Zero_Limit)
                                            {
                                                ACAL_Status=3;

                                            }
                                        if(CM_Percentage*10 > Zero_Limit)
                                            {
                                                ACAL_Status=4;
                                                M078=1;//Cor>Zero Limit
                                            }
                                                    M076 =0;//Control Measurentment on =0
                                                    if(ACAL_Status==3)
                                                        {
                                                            /**************************************************/
                                                            /*CM_Percentage below is used in diogital_Input_Actions.c & Key_Control.c if auto calibrationtion val is Accepted */
                                                            //CM_Percentage = (float)(CM_Percentage * Span_Count);
                                                            CM_Percentage =     ((float)CM_Percentage * Span_Count)/100;
                                                            /**************************************************/
                                                        }
                                    }
                        break;

        }
}

/*Changes*/
/*
Date 30/1/2018
After Auto calibration:
    Span Count calculation after Autocalibration removed
    full_load is recalculated and is changed by same value as cahnge in zero_load value
Auto calibration abort condtions:
    If any fault
    If BIN_PF_Start_Status ==1

        Do This:
            Acal_status =0
             M077=1;     //Auto calibration aborted marker on

*/
