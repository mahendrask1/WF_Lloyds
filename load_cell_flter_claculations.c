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

unsigned long BL_sum,BL_reg[1600],BL_avg;
uint16_t BL_pointer,BL_buffer;
extern unsigned long adc_final_val,bsf_adc_final_val;
extern uint16_t New_menu_parameters[105];
extern unsigned long Present_Count,Count_Min;
extern char* P_String;
extern char A_String[200];
extern unsigned int  Display_Feedrate;
extern signed int Belt_Load;
extern unsigned char Calib_status;;
void load_cell_avg_processing()
{
//    if((bsf_adc_final_val < Count_Min) && (Min_Load != 0 ) && (    Calib_status == 0))   //to bypass min load observed when zeroing
//    {
//        uint16_t i;    //msk 28-sep-2025
//        for(i=0;i<BL_buffer;i++)
//        {
//            BL_reg[i] =  0;
//            BL_sum = 0;
//            BL_avg = 0;
//        }
//        return;
//    }
//    else
//    {
        if(BL_pointer >= BL_buffer)
            BL_pointer = 0;
        BL_sum = ( BL_sum +    bsf_adc_final_val - BL_reg[BL_pointer] );
        BL_reg[BL_pointer] =  bsf_adc_final_val ;
        BL_pointer++;
        BL_avg = BL_sum /BL_buffer;
//    }
}

void BL_init_calculation()
{
    uint16_t i;
    switch(LC_Filter)
    {
        case 0 :
            BL_buffer = 1;                //0.002msec
            BL_avg =  bsf_adc_final_val;
            break;
        case 1 :BL_buffer = 200;    //0.018msec
            break;
        case 2 :BL_buffer = 400;  //0.034msec
            break;
        case 3 :BL_buffer = 600;    //0.05msec
            break;
        case 4 :BL_buffer = 800;    //0.065msec
            break;
        case 5 :BL_buffer = 1000;    //0.082msec
            break;
        case 6 :BL_buffer = 1200;    //0.05msec
            break;
        case 7 :BL_buffer = 1400;    //0.065msec
            break;
        case 8 :BL_buffer = 1600;    //0.082msec
            break;
        default:break;
    }
    for(i=0;i<BL_buffer;i++)
    {
        BL_reg[i] =  bsf_adc_final_val;
    }
    BL_sum = bsf_adc_final_val * BL_buffer;
    BL_avg = BL_sum / BL_buffer;
}

/*
Date:15/10/2019
BL Buffer count reduced to 1600 from 2000 */
