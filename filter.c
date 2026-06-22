/*        filter.c        */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

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

#include "functions.h"

#define ARYSIZE 10

#define bsf_fc   0            //filter_count fc = adc_read_count
#define bsf_fc_1  1
#define bsf_fc_2  2
unsigned char one_cycle=0,one_cycle_2ch=0;
unsigned char adc_read_count=0,bsf_adc_read_count=0,adc_read_count_2ch=0;
unsigned long adc_final_val,bsf_adc_final_val,adc_final_val_2ch;
extern uint16_t New_menu_parameters[105];
long input[ARYSIZE] = {0};
long output[ARYSIZE] = {0};
long input_2ch[ARYSIZE] = {0};
long output_2ch[ARYSIZE] = {0};
float fil_A,fil_B,fil_C,fil_D,fil_E,fil_A_2ch,fil_B_2ch,fil_C_2ch,fil_D_2ch,fil_E_2ch; //float value co-efficients for filter calc
float fil_X = 0.88895142;     //
float fil_Y = -1.74641132;    //
float fil_Z = 0.77790284;     //for high cutoff 5 low cutoff 1
unsigned long bsf_adc_final_val,bsf_adc_final_val_2ch;
long bsf_input[ARYSIZE] = {0};
long bsf_output[ARYSIZE] = {0};
long bsf_input_2ch[ARYSIZE] = {0};
long bsf_output_2ch[ARYSIZE] = {0};
unsigned char order;
uint16_t sps,cutoff;
float cutoff_1;
float pi=3.14159,alp,alp2,alp3,alp4,alp_2ch,alp2_2ch,alp3_2ch,alp4_2ch;
extern unsigned long rx_data;
//extern unsigned long Present_Count,Count_DL,Span_Count;
extern unsigned long Present_Count,Present_Count_ch2,Count_DL,Count_DL_ch2,Span_Count,Span_Count_ch2,zero_load,zero_load_ch2,full_load,full_load_ch2;
extern unsigned long Calib_Load,BL_avg;
extern char Display_Sign;
/*Analog Out varibles*/
//extern unsigned int Belt_Load;
extern unsigned int  Actual_Feedrate;
extern signed int Belt_Load ;
extern unsigned long Count_Max,Count_Min,Min_Count_Channel1,Min_Count_Channel2,Min_Count_Channel3,Max_Count_Channel1,Max_Count_Channel2,Max_Count_Channel3,zero_load;
extern unsigned long Dac1_SpanCounts,Dac2_SpanCounts,Dac3_SpanCounts;
//extern uint16_t Dac1_ScaleFactor, Dac2_ScaleFactor, Dac3_ScaleFactor; //These are macros in parameters.h
unsigned int Dac1_Val,Dac2_Val,Dac3_Val;
unsigned int Belt_Speed;
extern uint16_t scan_count;
extern float WF_Drive_Setpoint,PF_Drive_Setpoint;
void bessel_fil_cal(void);
void bsf_fil_cal(void);
void calculate_feedrate_display(void);
extern unsigned char ch_id_flag;
extern uint16_t  sec_counter,AI_calb_val[6],AI1_avg;
extern char* P_String;
extern char A_String[35];
extern unsigned int Hop_Per,Gate_Postion;
extern unsigned int  Display_Feedrate;
uint16_t dacop1,dacop2;
extern uint16_t Display_Tacho_Reg[26],Display_Belt_Speed;
//extern unsigned int HOpper_total_try,FDR_Total_try;

/*Function to calculate DAC1 value from input parameter*/
unsigned int Calculate_Dac1_Value(float input_value, unsigned int scaling_factor)
{
    float scaled_input = input_value * (10000.0f / Dac1_ScaleFactor);
    return ((unsigned int)((scaled_input * Dac1_SpanCounts) / scaling_factor) + Min_Count_Channel1);
}

/*Function to calculate DAC2 value from input parameter*/
unsigned int Calculate_Dac2_Value(float input_value, unsigned int scaling_factor)
{
    float scaled_input = input_value * (10000.0f / Dac2_ScaleFactor);
    return ((unsigned int)((scaled_input * Dac2_SpanCounts) / scaling_factor) + Min_Count_Channel2);
}

/*Function to calculate DAC3 value from input parameter*/
unsigned int Calculate_Dac3_Value(float input_value, unsigned int scaling_factor)
{
    float scaled_input = input_value * (10000.0f / Dac3_ScaleFactor);
    return ((unsigned int)((scaled_input * Dac3_SpanCounts) / scaling_factor) + Min_Count_Channel3);
}

/*Dac Value Calculation For */
void Anlog_Out(void)
{
    signed int BL_Temp;
    int temp1;
    unsigned int Hop_Per_Temp,Gate_Postion_temp;
    init_dac();

    /*  Cal of Dac val   As per AO1 Parameter Selection*/
    switch(AO1_Value)  //Ao1_Para_No is as per parameter selected in AO1 parameter selected in Configuration Menu
        {
            case 0:
                Dac1_Val = Calculate_Dac1_Value((float)Display_Feedrate, Capacity);  /*tph*/

                break;
            case 1: /* Edited by PG Date:18122022 Ver:1.H */
                /* Reason:AO changes to max value i.e. 20mA when load drops below Min_Load or Zero_Load*/
                //if(Present_Count < Count_Min)/*B.L*/
            if(BL_avg < zero_load)
                {
                    Dac1_Val = Min_Count_Channel1;//bug fix from ver1.H 12/03/23 by PG
                    dacop1=4000;    //msk 07-dec-2025
                }
                else
                {
                    BL_Temp = (float)((float)(BL_avg - zero_load ) * 1000 / (Span_Count));
                    Dac1_Val = Calculate_Dac1_Value((float)BL_Temp, 1000); //bug fix from ver1.H 12/03/23 by PG // msk moved this line here

                }
                /* Commented by PG Date:18122022 */
                /* Reason: As we don't want to display/AO negative beltload*/
//                if(Present_Count < zero_load )
//                {
//                    BL_Temp= (float)((float)(zero_load -BL_avg) * 1000/ (Span_Count));
//
//                }
            // msk commented this line and shifted upward 03-nov-2025    Dac1_Val=((float)(BL_Temp*(Max_Count_Channel1-Min_Count_Channel1) / 1000)+Min_Count_Channel1); //bug fix from ver1.H 12/03/23 by PG
                break;
            case 2:                /*Tacho*/
                            if(Fixed_Frequency == 1) // msk added 03-nov-2025
                            {
                                Belt_Speed = Tacho_Freq;
                            }

                            //Dac1_Val= ((float)(Belt_Speed *Dac1_SpanCounts / Tacho_Freq)+Min_Count_Channel1);
                            Dac1_Val = Calculate_Dac1_Value((float)Display_Belt_Speed, Tacho_Freq);//msk 15-dec-25

                         break;
            case 3:
                            Dac1_Val = Calculate_Dac1_Value(WF_Drive_Setpoint, 4095);    /*WFsp*/

                         break;
            case 4:
                            Dac1_Val = Calculate_Dac1_Value((float)PF_Drive_Setpoint, 4095);   /*PFsp*/

                            break;
            case 5:
                            Hop_Per_Temp=(float)((float)((float)Present_Count_ch2  - (float)zero_load_ch2) * 100 / (float)(Span_Count_ch2));

                            Dac1_Val = Calculate_Dac1_Value((float)Hop_Per_Temp, 1500);    /*WFsp*/

                         break;
            case 6:    if(PI==1)
                                    {
                                        if(AI1_avg > AI_calb_val[2])
                                            Gate_Postion_temp=    ((AI1_avg-AI_calb_val[2])*1000)/(AI_calb_val[3]-AI_calb_val[2]);
                                                else
                                                    Gate_Postion_temp= 0;
                                    }
                            Dac1_Val = Calculate_Dac1_Value((float)Gate_Postion, 1000);   /*PFsp*/

                         break;
            default:break;

        }
        if(Dac1_Val >4095)
        {Dac1_Val =Max_Count_Channel1;}//msk added curly braces
//        HOpper_total_try=Dac1_Val;        //for test, to be removed msk 03-nov-2025
        dac1_write(Dac1_Val);//Dac1_Val  instead of constant val /*Temp Solution*/
        PINSEL_Configure();
    }

void Anlog_Out2(void)
{
    signed int BL_Temp;
    unsigned int Hop_Per_Temp,Gate_Postion_temp;
    init_dac();
/*  Cal of Dac val   As per AO1 and AO2 Parameter Selection*/
    switch(AO2_Value)  //Ao2_Para_No is as per parameter selected in AO2 parameter selected in Configuration Menu
        {
            case 0:
                            Dac2_Val = Calculate_Dac2_Value((float)Display_Feedrate, Capacity);
                         break;
            case 1: /* Edited by PG Date:18122022 Ver:1.H */
                /* Reason: AO changes to max value i.e. 20mA when load drops below Min_Load or Zero_Load*/
            if(BL_avg < zero_load)        //msk changed this 03-nov-2025
                {
                    Dac2_Val = Min_Count_Channel2;
                }
                else
                {
                    BL_Temp= (float)((float)(BL_avg - zero_load ) * 1000 / (Span_Count));
                    Dac2_Val = Calculate_Dac2_Value((float)BL_Temp, 1000);//msk shifted this line here
                }
                /* Commented by PG Date:18122022 */
                /* Reason: As we don't want to display/AO negative beltload*/
//                if(Present_Count < zero_load )
//                {
//                    BL_Temp= (float)((float)(zero_load -BL_avg) * 1000/ (Span_Count));
//
//                }
            //    Dac2_Val=((float)(BL_Temp*(Max_Count_Channel2-Min_Count_Channel2) / 1000)+Min_Count_Channel2);//msk shifted this line up 03-nov-2025
                break;
            case 2:
                            if(Fixed_Frequency == 1) // msk added 03-nov-2025
                            {
                                Belt_Speed = Tacho_Freq;
                            }
                            //Dac2_Val=((float)(Belt_Speed *Dac2_SpanCounts / Tacho_Freq)+Min_Count_Channel2);
                            Dac2_Val = Calculate_Dac2_Value((float)Display_Belt_Speed, Tacho_Freq);//msk 15-dec-25
                         break;
            case 3:
                            //Dac2_Val=((float)(WF_Drive_Setpoint *Dac2_SpanCounts / 4095)+Min_Count_Channel2);
                            Dac2_Val = Calculate_Dac2_Value(WF_Drive_Setpoint, 4095);
                         break;
            case 4:
                            Dac2_Val = Calculate_Dac2_Value((float)PF_Drive_Setpoint, 4095);
                         break;
            case 5:
                            Hop_Per_Temp=(float)((float)((float)Present_Count_ch2  - (float)zero_load_ch2) * 100 / (float)(Span_Count_ch2));

                            Dac2_Val = Calculate_Dac2_Value((float)Hop_Per_Temp, 1500);    /*WFsp*/
                         break;
            case 6:    if(PI==1)
                                    {
                                        if(AI1_avg > AI_calb_val[2])
                                            Gate_Postion_temp=    ((AI1_avg-AI_calb_val[2])*1000)/(AI_calb_val[3]-AI_calb_val[2]);
                                                else
                                                    Gate_Postion_temp= 0;
                                    }
                            Dac2_Val = Calculate_Dac2_Value((float)Gate_Postion, 1000);   /*PFsp*/
                         break;
            default:break;
    }
    if(Dac2_Val >4095)
    {Dac2_Val =Max_Count_Channel2;}    // msk added curly braces
//    FDR_Total_try=Dac2_Val;    //msk added for test , to be removed 03-nov-2025
    dac2_write(Dac2_Val);//Dac2_Val  instead of constant val /*Temp Solution*/

    PINSEL_Configure();
    }

void Anlog_Out3(void)
{
    signed int BL_Temp;
    unsigned int Hop_Per_Temp,Gate_Postion_temp;
    init_dac();
/*  Cal of Dac val   As per AO1 and AO2 Parameter Selection*/
    switch(AO3_Value)  //Ao2_Para_No is as per parameter selected in AO2 parameter selected in Configuration Menu
        {
            case 0:
                Dac3_Val = Calculate_Dac3_Value((float)Display_Feedrate, Capacity);
                break;
            case 1: /* Edited by PG Date:18122022 Ver:1.H */
                /* Reason:AO changes to max value i.e. 20mA when load drops below Min_Load or Zero_Load*/
            if(BL_avg < zero_load)        //msk changed this 03-nov-2025
                {
                    Dac3_Val = Min_Count_Channel3;
                }
                else
                {
                    BL_Temp= (float)((float)(BL_avg - zero_load ) * 1000 / (Span_Count));
                    Dac3_Val = Calculate_Dac3_Value((float)BL_Temp, 1000);//bug fix from ver1.H 12/03/23 by PG//msk shifted this line here 03-nov-2025
                }
                /* Commented by PG Date:18122022 */
                /* Reason: As we don't want to display/AO negative beltload*/
//                if(Present_Count < zero_load )
//                {
//                    BL_Temp= (float)((float)(zero_load -BL_avg) * 1000/ (Span_Count));
//
//                }
                //Dac3_Val=((float)(BL_Temp*(Max_Count_Channel3-Min_Count_Channel3) / 1000)+Min_Count_Channel3);//bug fix from ver1.H 12/03/23 by PG// msk shifted this line up
                break;
            case 2:
                            if(Fixed_Frequency == 1) // msk added 03-nov-2025
                            {
                                Belt_Speed = Tacho_Freq;
                            }
                            Dac3_Val = Calculate_Dac3_Value((float)Belt_Speed, Tacho_Freq);
                         break;
            case 3:
                            Dac3_Val = Calculate_Dac3_Value(WF_Drive_Setpoint, 4095);
                         break;
            case 4:
                            Dac3_Val = Calculate_Dac3_Value((float)PF_Drive_Setpoint, 4095);
                         break;
            case 5:
                            Hop_Per_Temp=(float)((float)((float)Present_Count_ch2  - (float)zero_load_ch2) * 100 / (float)(Span_Count_ch2));

                            Dac3_Val = Calculate_Dac3_Value((float)Hop_Per_Temp, 1500);    /*Hop_Level*/
                         break;
            case 6:    if(PI==1)
                                    {
                                        if(AI1_avg > AI_calb_val[2])
                                            Gate_Postion_temp=    ((AI1_avg-AI_calb_val[2])*1000)/(AI_calb_val[3]-AI_calb_val[2]);
                                                else
                                                    Gate_Postion_temp= 0;
                                    }
                            Dac3_Val = Calculate_Dac3_Value((float)Gate_Postion_temp, 1000);   /*Dosing Gate Position*/
                         break;
            default:break;
    }
        if(Dac3_Val >4095)
        Dac3_Val = Max_Count_Channel3;
    dac3_write(Dac3_Val);//Dac2_Val  instead of constant val /*Temp Solution*/

    PINSEL_Configure();
    }

/***************************************************************************************
                        Bessel Filter calculation
Bessel filter calculation
-raw values stored in input array
-bessel filter parameters like filter step, filter co-efficients are calculated
in enod software and converted to 32 bit floating point value and entered through Modbus
-To accept new filter values, make filter enabel 1 through modbus
Some pre calculated values are aso under
1) 2nd order filter, 400 sps, cutoff freq = 10hz
        fil_A=0.0054;
        fil_B=-322.228;
        fil_C=141.061;
2) 3rd order filter, 200 sps, cutoff freq = 20hz
        fil_A=0.0152;
        fil_B=-110.555;
        fil_C=67.254;
        fil_D=-14.451;
3) 3rd order filter, 400 sps, cutoff freq = 10hz
        fil_A=0.0004;
        fil_B=-6552.297;
        fil_C=5769.49;
        fil_D=-1700.09;
4) 4th order filter, 200 sps, cutoff freq = 10hz
        fil_A=0.000377;
        fil_B=-8137.501;
        fil_C=9505.377;
        fil_D=-4994.95;
        fil_E=995.146;
5) 4th order filter, 400 sps, cutoff freq = 10hz
        fil_A=2.99e-5;
        fil_B=-117934.38;
        fil_C=156267.62;
        fil_D=-92304.936;
        fil_E=20504.97;
6) 4th order filter, 200sps, cutoff freq = 6hz
        fil_A=0.00005905;
        fil_B=-58087.38;
        fil_C=75061.52;
        fil_D=-43297.037;
        fil_E=9403.934;
***************************************************************************************/

void bessel_fil_cal_2ch(void)
{
unsigned char fc,fc_1,fc_2,fc_3,fc_4,filter_step=2;
static unsigned char fil_step_old;
fc = adc_read_count_2ch;            //filter_count fc = adc_read_count
fc_1 = adc_read_count_2ch-1;
fc_2 = adc_read_count_2ch-2;
fc_3 = adc_read_count_2ch-3;
fc_4 = adc_read_count_2ch-4;

    if(fil_E_2ch == 0 && fil_D_2ch == 0)      //calculate filter step to implement appropriate filtering formula
        filter_step = 2;
    else if(fil_E_2ch == 0)
        filter_step = 3;
    else
        filter_step=4;

        if(fil_step_old != filter_step && one_cycle_2ch == 1)    //if filter step changed & buffer filled once, only then reset filter_count and one_cycle
            {adc_read_count_2ch = 0; one_cycle_2ch = 0;}

        if(fc>=4 || one_cycle_2ch == 1)
            {
            switch(filter_step)
            {
            case 2:

                if(fc==0)
                {fc_1 = (ARYSIZE-1); fc_2 = (ARYSIZE-2);}
                if(fc==1)
                {fc_2 = (ARYSIZE-1);}
                output_2ch[fc] = (fil_A_2ch*(input_2ch[fc]+(2*input_2ch[fc_1])+input_2ch[fc_2]-(fil_B_2ch*output_2ch[fc_1])-(fil_C*output_2ch[fc_2])));
                break;

            case 3:
                if(fc==0)
                {fc_1 = (ARYSIZE-1); fc_2 = (ARYSIZE-2);fc_3 = (ARYSIZE-3);}
                if(fc==1)
                {fc_2 = (ARYSIZE-1);fc_3 = (ARYSIZE-2);}
                if(fc==2)
                {fc_3 = (ARYSIZE-1);}

                output_2ch[fc] = (fil_A_2ch*(input_2ch[fc]+(3*input_2ch[fc_1])+(3*input_2ch[fc_2])+input_2ch[fc_3]-(fil_B_2ch*output_2ch[fc_1])-(fil_C_2ch*output_2ch[fc_2])-(fil_D_2ch*output_2ch[fc_3])));
                break;

            case 4:

                if(fc==0)
                {fc_1 = (ARYSIZE-1); fc_2 = (ARYSIZE-2);fc_3 = (ARYSIZE-3);fc_4 = (ARYSIZE-4);}
                if(fc==1)
                {fc_2 = (ARYSIZE-1);fc_3 = (ARYSIZE-2);fc_4 = (ARYSIZE-3);}
                if(fc==2)
                {fc_3 = (ARYSIZE-1);fc_4 = (ARYSIZE-2);}
                if(fc==3)
                {fc_4 = (ARYSIZE-1);}

                output_2ch[fc] = (fil_A_2ch*(input_2ch[fc]+(4*input_2ch[fc_1])+(6*input_2ch[fc_2])+(4*input_2ch[fc_3])+input_2ch[fc_4]-(fil_B_2ch*output_2ch[fc_1])-(fil_C_2ch*output_2ch[fc_2])-(fil_D_2ch*output_2ch[fc_3])-(fil_E_2ch*output_2ch[fc_4])));
                break;

            default:    output_2ch[fc] = input_2ch[fc];        //no filter selected
                break;
            }

            fil_step_old = filter_step;
            }
        else
            {
                output_2ch[0] = 0;
                output_2ch[1] = 0;
                output_2ch[2] = 0;
                output_2ch[3] = 0;
            }

        fil_step_old = filter_step;
}

/***************************************************************************************
                        Bessel Filter calculation
Bessel filter calculation
-raw values stored in input array
-bessel filter parameters like filter step, filter co-efficients are calculated
in enod software and converted to 32 bit floating point value and entered through Modbus
-To accept new filter values, make filter enabel 1 through modbus
Some pre calculated values are aso under
1) 2nd order filter, 400 sps, cutoff freq = 10hz
        fil_A=0.0054;
        fil_B=-322.228;
        fil_C=141.061;
2) 3rd order filter, 200 sps, cutoff freq = 20hz
        fil_A=0.0152;
        fil_B=-110.555;
        fil_C=67.254;
        fil_D=-14.451;
3) 3rd order filter, 400 sps, cutoff freq = 10hz
        fil_A=0.0004;
        fil_B=-6552.297;
        fil_C=5769.49;
        fil_D=-1700.09;
4) 4th order filter, 200 sps, cutoff freq = 10hz
        fil_A=0.000377;
        fil_B=-8137.501;
        fil_C=9505.377;
        fil_D=-4994.95;
        fil_E=995.146;
5) 4th order filter, 400 sps, cutoff freq = 10hz
        fil_A=2.99e-5;
        fil_B=-117934.38;
        fil_C=156267.62;
        fil_D=-92304.936;
        fil_E=20504.97;
6) 4th order filter, 200sps, cutoff freq = 6hz
        fil_A=0.00005905;
        fil_B=-58087.38;
        fil_C=75061.52;
        fil_D=-43297.037;
        fil_E=9403.934;
***************************************************************************************/

void bessel_fil_cal(void)
{
unsigned char fc,fc_1,fc_2,fc_3,fc_4,filter_step=2;
static unsigned char fil_step_old;
fc = adc_read_count;            //filter_count fc = adc_read_count
fc_1 = adc_read_count-1;
fc_2 = adc_read_count-2;
fc_3 = adc_read_count-3;
fc_4 = adc_read_count-4;

    if(fil_E == 0 && fil_D == 0)      //calculate filter step to implement appropriate filtering formula
        filter_step = 2;
    else if(fil_E == 0)
        filter_step = 3;
    else
        filter_step=4;

        if(fil_step_old != filter_step && one_cycle == 1)    //if filter step changed & buffer filled once, only then reset filter_count and one_cycle
            {adc_read_count = 0; one_cycle = 0;}

        if(fc>=4 || one_cycle == 1)
            {
            switch(filter_step)
            {
            case 2:

                if(fc==0)
                {fc_1 = (ARYSIZE-1); fc_2 = (ARYSIZE-2);}
                if(fc==1)
                {fc_2 = (ARYSIZE-1);}
                output[fc] = (fil_A*(input[fc]+(2*input[fc_1])+input[fc_2]-(fil_B*output[fc_1])-(fil_C*output[fc_2])));
                break;

            case 3:
                if(fc==0)
                {fc_1 = (ARYSIZE-1); fc_2 = (ARYSIZE-2);fc_3 = (ARYSIZE-3);}
                if(fc==1)
                {fc_2 = (ARYSIZE-1);fc_3 = (ARYSIZE-2);}
                if(fc==2)
                {fc_3 = (ARYSIZE-1);}

                output[fc] = (fil_A*(input[fc]+(3*input[fc_1])+(3*input[fc_2])+input[fc_3]-(fil_B*output[fc_1])-(fil_C*output[fc_2])-(fil_D*output[fc_3])));
                break;

            case 4:

                if(fc==0)
                {fc_1 = (ARYSIZE-1); fc_2 = (ARYSIZE-2);fc_3 = (ARYSIZE-3);fc_4 = (ARYSIZE-4);}
                if(fc==1)
                {fc_2 = (ARYSIZE-1);fc_3 = (ARYSIZE-2);fc_4 = (ARYSIZE-3);}
                if(fc==2)
                {fc_3 = (ARYSIZE-1);fc_4 = (ARYSIZE-2);}
                if(fc==3)
                {fc_4 = (ARYSIZE-1);}

                output[fc] = (fil_A*(input[fc]+(4*input[fc_1])+(6*input[fc_2])+(4*input[fc_3])+input[fc_4]-(fil_B*output[fc_1])-(fil_C*output[fc_2])-(fil_D*output[fc_3])-(fil_E*output[fc_4])));
                break;

            default:    output[fc] = input[fc];        //no filter selected
                break;
            }

            fil_step_old = filter_step;
            }
        else
            {
            output[0] = 0;
            output[1] = 0;
            output[2] = 0;
            output[3] = 0;
            }

        fil_step_old = filter_step;
}

/***************************************************************************************
                                Store ADC count
Storing of ADC count in input filter and implementing bessel filter on raw adc counts
***************************************************************************************/
void filter_calculation(void)
{
 if(Pre_Hopper==1)
    {
        if(ch_id_flag==1)
            {
                input_2ch[adc_read_count_2ch] = rx_data;        //0x00 xx xx xx 24 bit data
                bessel_fil_cal_2ch();                        //call bessel filtering
                adc_final_val_2ch=output_2ch[adc_read_count_2ch];
                adc_read_count_2ch++;
                if(adc_read_count_2ch>=ARYSIZE)
                    {
                        adc_read_count_2ch = 0;
                        one_cycle_2ch = 1;  //once array rollsover , filtering starts from input[0], hence this flag
                    }
            }
        else
            {
                input[adc_read_count] = rx_data;        //0x00 xx xx xx 24 bit data
                bessel_fil_cal();                        //call bessel filtering
                adc_final_val=output[adc_read_count];
                adc_read_count++;
                if(adc_read_count>=ARYSIZE)
                    {
                        adc_read_count = 0;
                        one_cycle = 1;  //once array rollsover , filtering starts from input[0], hence this flag
                    }
            }

    }
 else
    {
        input[adc_read_count] = rx_data;        //0x00 xx xx xx 24 bit data
        bessel_fil_cal();                        //call bessel filtering
        adc_final_val=output[adc_read_count]=rx_data;        //msk 16-aug-25
        adc_read_count++;
        if(adc_read_count>=ARYSIZE)
            {
                adc_read_count = 0;
                one_cycle = 1;  //once array rollsover , filtering starts from input[0], hence this flag
            }
    }

}

/***********************Filter Parameter Calculation*******************/
/***********************fil_A,fil_B,fil_C,fil_D,fil_E*******************/

void filter_para_calc(void)
{
    float a,b,c;

    switch(AD_Conversion_Rate)
     {
        case 0: sps = 6.25;
                        break;
        case 1: sps = 12.5;
                        break;
        case 2: sps = 25;
                        break;
        case 3: sps = 50;
                        break;
        case 4: sps = 100;
                        break;
        case 5: sps = 200;
                        break;
        case 6: sps = 400;
                        break;
        default: //sps = 100;//Commented default as it allways took  sps as 100;
                        break;
     }
    order = Filter_Order;
    cutoff = Cut_Off_Freq;

    cutoff_1 = cutoff/10.0;

    alp= sps/(pi*cutoff_1);

        alp2 = alp * alp;

     if( order == 0x03)                //3rd order
         {
             a=2.43288;
             b=2.466212;
    //    alp2= pow(alp,2);
            alp2=alp*alp;
    //    alp3= pow(alp,3);
            alp3=alp2*alp;

             fil_A= 1/(alp3 + a*alp2 + b*alp +1);

             fil_B= -3*alp3 - a*alp2 +b*alp +3;

             fil_C= 3*alp3 - a*alp2 - b*alp +3;

             fil_D= -alp3 + a*alp2 - b*alp +1;

             fil_E=0;

         }

    else if( order == 0x04)        //4th Order
        {
       a=2.1239399;
       b=4.39155;
       c=3.2010858;
//       alp2=pow(alp,2);
        alp2=alp*alp;
//       alp3=pow(alp,3);
        alp3=alp2*alp;
//       alp4=pow(alp,4);
        alp4=alp3*alp;

       fil_A=0.0000054630815;  //0.000059049433;         //0.00071420741;     //0.00002986615;         //0.00037765298; // 0.003859562;                 //1/(alp4 + a*alp3 + b*alp2 + c*alp + 1);

       fil_B=-675649.71;            //-58087.8;                         //-4063.436;                    //-117934.38;            //-8137.5008;    //-587.76426;                    //-4*alp4 - 2*a*alp3 + 2*c*alp + 4;

       fil_C= 936408.52;      //75061.52;                          //4511.5712;                    //156267.62;             //9505.3773;    //532.96753;                        //*alp4 - 2*b*alp2 + 6;

       fil_D= -577513.96;     //-43297.037;                     //-2265.5901;                //-92304.936;            //-4994.9564;   //-225.51432;                        //-4*alp4 + a*alp3 - 2*c*alp + 4;

       fil_E=133724.27;       //9403.9341;                     //433.30138;                    //20504.97;                    //995.14639;    //37.214299;                        //alp4 - a*alp3 + b*alp2 - c*alp + 1;

        }
     else                //2nd Order
        {
             a= 1.732050807568877;                //root(3)

            fil_A = 1/(alp2 + (a*alp) + 1);

            fil_B =    2 - 2*alp2;

            fil_C =    alp2 - (a*alp) + 1;

            fil_D =0;
            fil_E =0;

        }

    order = Filter__Order;
    cutoff = Cut__Off_Freq;

    cutoff_1 = cutoff/10.0;

    alp_2ch= sps/(pi*cutoff_1);

        alp2_2ch = alp_2ch * alp_2ch;

     if( order == 0x03)                //3rd order
         {
             a=2.43288;
             b=2.466212;
    //    alp2= pow(alp,2);
            alp2_2ch=alp_2ch*alp_2ch;
    //    alp3= pow(alp,3);
            alp3_2ch=alp2_2ch*alp_2ch;

             fil_A_2ch= 1/(alp3_2ch + a*alp2_2ch + b*alp_2ch +1);

             fil_B_2ch= -3*alp3_2ch - a*alp2_2ch +b*alp_2ch +3;

             fil_C_2ch= 3*alp3_2ch - a*alp2_2ch - b*alp_2ch +3;

             fil_D_2ch= -alp3_2ch + a*alp2_2ch - b*alp_2ch +1;

             fil_E_2ch=0;

         }

    else if( order == 0x04)        //4th Order
        {
       a=2.1239399;
       b=4.39155;
       c=3.2010858;
//       alp2=pow(alp,2);
        alp2_2ch=alp_2ch*alp_2ch;
//       alp3=pow(alp,3);
        alp3_2ch=alp2_2ch*alp_2ch;
//       alp4=pow(alp,4);
        alp4_2ch=alp3_2ch*alp_2ch;

       fil_A_2ch=0.0000054630815;  //0.000059049433;         //0.00071420741;     //0.00002986615;         //0.00037765298; // 0.003859562;                 //1/(alp4 + a*alp3 + b*alp2 + c*alp + 1);

       fil_B_2ch=-675649.71;            //-58087.8;                         //-4063.436;                    //-117934.38;            //-8137.5008;    //-587.76426;                    //-4*alp4 - 2*a*alp3 + 2*c*alp + 4;

       fil_C_2ch= 936408.52;      //75061.52;                          //4511.5712;                    //156267.62;             //9505.3773;    //532.96753;                        //*alp4 - 2*b*alp2 + 6;

       fil_D_2ch= -577513.96;     //-43297.037;                     //-2265.5901;                //-92304.936;            //-4994.9564;   //-225.51432;                        //-4*alp4 + a*alp3 - 2*c*alp + 4;

       fil_E_2ch=133724.27;       //9403.9341;                     //433.30138;                    //20504.97;                    //995.14639;    //37.214299;                        //alp4 - a*alp3 + b*alp2 - c*alp + 1;

        }
     else                //2nd Order
        {
             a= 1.732050807568877;                //root(3)

            fil_A_2ch = 1/(alp2_2ch + (a*alp_2ch) + 1);

            fil_B_2ch =    2 - 2*alp2_2ch;

            fil_C_2ch =    alp2_2ch - (a*alp_2ch) + 1;

            fil_D_2ch =0;
            fil_E_2ch=0;

        }

}
void bsf_filter_calculation_2ch(void)
{
    int x;
     for(x=2;x>=0;x--)
        bsf_input_2ch[x+1] = bsf_input_2ch[x];

        bsf_input_2ch[bsf_fc]=adc_final_val_2ch;        //take input from previous filter
        bsf_output_2ch[bsf_fc] = fil_X*(bsf_input_2ch[bsf_fc]+bsf_input_2ch[bsf_fc_2]) + fil_Y*(bsf_input_2ch[bsf_fc_1]-bsf_output_2ch[bsf_fc_1])
                                         - fil_Z*bsf_output_2ch[bsf_fc_2];
        //filter calculation formula
        bsf_adc_final_val_2ch = bsf_output_2ch[bsf_fc];        //take output for calculation of weight
            for(x=2;x>=0;x--)
                bsf_output_2ch[x+1] = bsf_output_2ch[x];

}

void bsf_filter_calculation(void)
{
//    long bsf_input_val = adc_final_val;

    int x;

    for(x=2;x>=0;x--)
        bsf_input[x+1] = bsf_input[x];

bsf_input[bsf_fc]=adc_final_val=rx_data;        //take input from previous filter //msk 16-aug-25

bsf_output[bsf_fc] = fil_X*(bsf_input[bsf_fc]+bsf_input[bsf_fc_2]) + fil_Y*(bsf_input[bsf_fc_1]-bsf_output[bsf_fc_1])
                                         - fil_Z*bsf_output[bsf_fc_2];
//filter calculation formula

    bsf_adc_final_val = bsf_output[bsf_fc];        //take output for calculation of weight

    for(x=2;x>=0;x--)
    bsf_output[x+1] = bsf_output[x];

}

/*
date:2/2/2018
Belt Load
    Belt_Load = (BL_Avg-ZeroLoad)*Capacity/span_Count
    value is calculated wrt capacity
date:19/2/2018
Belt_Load
    Belt_Load = (BL_Avg-ZeroLoad)*1000/span_Count
value is calculated wrt %
BL_Temp added in AO,AO1,AO2 calculation for dac val as for BL display Belt_Load variable is used along with BL_avg & in above calculation it is used along with present count so BL_Temp is used insted of Belt_Load
Date 23/02/2018
Analog OP Selsection
    Hop Level & Gate Postion OP are added for Analog OP selection
Date 07/08/2022 By PG
Analog Output Belt load for AO1,AO2 and AO3 @line no.108, 165, 223
  Previous:Dac1_Val=((float)(BL_Temp*(Max_Count_Channel1-Min_Count_Channel1) / 1500)+Min_Count_Channel1);
    Change:Dac1_Val=((float)(BL_Temp*(Max_Count_Channel1-Min_Count_Channel1) / 1000)+Min_Count_Channel1);
    Reason: at 100% Beltload AO is 14.4mA which should be 20mA

 * Date : 18/12/2022
 * File name: Filter.c
 * Line no:. 99, 161, 224
 * Details: Software version Changed To 1.H
 * Bug: AO changes to max value i.e. 20mA when load drops below Min_Load or Zero_Load
 * Solution: modified condition that calculates responsible for DAC calculation
 * Result: AO become 0mA below Min_Load or Zero_Load, Also during recovery DAC value is calculated when BL_avg is greater than Zero_Load
 *
 */
