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

/* Extern Variables*/
extern char calib_string[20];
extern char *calib_string_ptr;
extern char *clear_string;

extern unsigned int Count_Tacho;
extern uint16_t New_menu_parameters[105];
extern unsigned long Present_Count, Count_DL, Span_Count, zero_load, full_load;
extern unsigned long Count_Max;
extern uint16_t scan_count;
extern uint16_t sec_counter;
extern uint32_t Key_Input;
extern unsigned long calib_val[8];
extern unsigned long Count_Limit1, Count_Limit2, Count_Max, Count_Min;
extern unsigned int Tacho_Counter;
extern unsigned int Belt_Speed;
extern volatile uint8_t flag;
/* Extern Variables*/

/*New Variables*/
float ZERO_VALUE, ZERO_ERROR_VALUE, ZERO_ERROR_PERSCENT;
unsigned char ZERO_STATUS;
extern char Status_No, Error_No;

extern char Oled_flag, Rows_clear;

#define ZERO_SCAN_TICKS_PER_SEC  200U /* msk 22-06-2026: Timer0 5 ms tick; 200 ticks = 1 s */

static uint32_t zero_start_total_ticks;
static uint32_t zero_duration_ticks;
static unsigned long zero_sample_count;

/*New Variables*/

/* msk 22-06-2026: monotonic 5 ms time base from sec_counter + scan_count (not response_time) */
static uint32_t Zeroing_GetTotalScanTicks(void)
{
    return ((uint32_t)sec_counter * (uint32_t)ZERO_SCAN_TICKS_PER_SEC) + (uint32_t)scan_count;
}

static void Zeroing_FinishCapture(void)
{
    if (zero_sample_count == 0)
        zero_sample_count = 1;

    ZERO_ERROR_VALUE = (float)((float)ZERO_VALUE / (float)zero_sample_count);
    ZERO_ERROR_PERSCENT = (float)((float)(ZERO_ERROR_VALUE * 100) / (float)(full_load - zero_load));
    if ((unsigned int)((ZERO_ERROR_VALUE - (unsigned int)ZERO_ERROR_VALUE) * 10) > 4) {
        ZERO_ERROR_VALUE = ZERO_ERROR_VALUE + 1;
    }

    if ((unsigned int)(ZERO_ERROR_PERSCENT * 10) <= Zero_Limit) {
        ZERO_STATUS = 3;
    }
    if ((unsigned int)(ZERO_ERROR_PERSCENT * 10) > Zero_Limit) {
        ZERO_STATUS = 4;
    }
}

/* msk 22-06-2026: service VFD/DIO/PLC on 5 ms tick while zero result screen blocks main */
static void Zeroing_PollCommsAndKeys(void)
{
    if (flag == 0)
        return;
    flag = 0;
    Menu_ServiceInvComms();
    key_check_routine();
}

static void Zeroing_WaitForKey(void)
{
    while (Key_Input == 0xFF) {
        Zeroing_PollCommsAndKeys();
    }
}

void Zeoring(void);

void After_Zeroing(void)    //To update Values related to Dead load i.e 0%load value
{
    Span_Count = full_load - zero_load; //Span_Count = Full_Loada_Count-Zero_Load_Count
    Count_Limit1 = (Span_Count * (float)Load_Limit1 / 1000) + zero_load;
    Count_Limit2 = (Span_Count * (float)Load_Limit2 / 1000) + zero_load;
    Count_Max = (Span_Count * (float)Max_Load / 1000) + zero_load;
    Count_Min = ((Span_Count * (float)Min_Load / 1000) + zero_load);
    calib_val[0] = full_load;
    calib_val[2] = (unsigned long)zero_load;//Count_DL;
    save_calib_val();
}

void Zero_Display_Update(char status_no)
{
    switch (status_no) {
    case 2:
        Clear_String(&clear_string, line4, 0, font10);
        sprintf(calib_string, "%.f ", ZERO_VALUE);          //Zero Value without  digits after decimal point
        calib_string_ptr = calib_string;
        Print_String(&calib_string_ptr, line4, 5, font10, 0);
        break;
    case 3:
        sprintf(calib_string, "%.2f", ZERO_ERROR_PERSCENT); //ZERO_ERROR_PERSCENT with 2 digits after decimal point
        calib_string_ptr = (calib_string);
        Print_String(&calib_string_ptr, line4, 28, font10, 0);
        break;
    case 4:
        Clear_String(&clear_string, line2, 28, font10);
        sprintf(calib_string, "%.2f ", ZERO_ERROR_PERSCENT); //ZERO_ERROR_PERSCENT
        calib_string_ptr = calib_string;
        Print_String(&calib_string_ptr, line2, 28, font10, 0);
        break;
    default:
        break;
    }
    /*      //Zero value*/
}

void Zeroing_Display(void)
{
    if (ZERO_STATUS == 2) {
        calib_string_ptr = " Zeroing ";
        Clear_String(&clear_string, line2, 0, font10);
        Print_String(&calib_string_ptr, line2, 0, font10, 0);
        Zero_Display_Update(ZERO_STATUS);
    }
    if (ZERO_STATUS == 3) {
        calib_string_ptr = "Zeroing Ok";
        Clear_String(&clear_string, line2, 0, font10);
        Print_String(&calib_string_ptr, line2, 0, font10, 0);

        calib_string_ptr = "Error =        %";
        Clear_String(&clear_string, line4, 0, font10);
        Print_String(&calib_string_ptr, line4, 0, font10, 0);

        Zero_Display_Update(ZERO_STATUS);
        //After_Zeroing();
        ZERO_STATUS = 6;
    }
    if (ZERO_STATUS == 4) {
        calib_string_ptr = "Error =         %";
        Clear_String(&clear_string, line2, 0, font10);
        Print_String(&calib_string_ptr, line2, 0, font10, 0);

        calib_string_ptr = "Zero corr>Set_Lmt";
        Clear_String(&clear_string, line4, 0, font10);
        Print_String(&calib_string_ptr, line4, 0, font10, 0);

        Zero_Display_Update(ZERO_STATUS);
        ZERO_STATUS = 7;
    }
    if (ZERO_STATUS == 5) {
        calib_string_ptr = " ZEROING Error ";
        Clear_String(&clear_string, line2, 0, font10);
        Print_String(&calib_string_ptr, line2, 0, font10, 0);

        calib_string_ptr = " CORR. ABORTED ";
        Clear_String(&clear_string, line4, 0, font10);
        Print_String(&calib_string_ptr, line4, 0, font10, 0);

        ZERO_STATUS = 7;
    }
    if (ZERO_STATUS == 6) {
        Zeroing_WaitForKey();
        if (Key_Input == CL) { //if any key is pressed  then goto online menu
            ZERO_STATUS = 0;
            return;
        }
        if (Key_Input != 0xFF) { //if any key is pressed  then goto online menu
            ZERO_STATUS = 0;
            if (ZERO_ERROR_VALUE > 0) {//Postive
                (zero_load) = (float)((float)zero_load + (unsigned long)(ZERO_ERROR_VALUE));  //Count_DL = Zero Load
                full_load = full_load + ZERO_ERROR_VALUE;
            }
            if (ZERO_ERROR_VALUE < 0) {//Negative
                if (zero_load > (unsigned long)(ZERO_ERROR_VALUE)) {
                    zero_load = (float)((float)zero_load - (unsigned long)(ZERO_ERROR_VALUE)); //Count_DL = Zero Load
                    full_load = full_load - ZERO_ERROR_VALUE;
                }
            }
            After_Zeroing();
            return;
        }
    }
    if (ZERO_STATUS == 7) {
        Zeroing_WaitForKey();
        if (Key_Input != 0xFF) { //if any key is pressed  then goto online menu
            ZERO_STATUS = 0;
            return;
        }
    }
}

void Zeoring(void)  // dynamic zero; load average each call; duration on Timer0 5 ms ticks
{
    float rev_time_sec;
    float zero_time_sec;
    uint32_t elapsed_ticks;

    if (ZERO_STATUS == 0) {
        return;
    }
    if (ZERO_STATUS == 1) {
        /* msk 22-06-2026: Rev_Time (E4.1 s) at Tacho_Freq; scale wall time by Belt_Speed at zero start */
        if (Belt_Speed == 0) {
            ZERO_STATUS = 5;
            return;
        }
        rev_time_sec = (float)Rev_Time / 10.0f;
        zero_time_sec = rev_time_sec * ((float)Tacho_Freq / (float)Belt_Speed);
        zero_duration_ticks = (uint32_t)(zero_time_sec * (float)ZERO_SCAN_TICKS_PER_SEC + 0.5f);
        if (zero_duration_ticks < 2U)
            zero_duration_ticks = 2U;
        zero_start_total_ticks = Zeroing_GetTotalScanTicks();
        zero_sample_count = 0;
        ZERO_STATUS = 2;
        ZERO_VALUE = 0;
        Clear_LCD();
    }
    if (ZERO_STATUS == 2) {
        ZERO_VALUE = ZERO_VALUE + ((float)(Present_Count) - (float)(zero_load));
        zero_sample_count++;

        if (Error_No == 3 /*TACHO_Fault==1*/) {
            ZERO_STATUS = 5;
            return;
        }

        /* msk 22-06-2026: stop on Timer0 5 ms ticks (sec_counter*200 + scan_count), not Zeoring call count */
        elapsed_ticks = Zeroing_GetTotalScanTicks() - zero_start_total_ticks;
        if (elapsed_ticks < zero_duration_ticks)
            return;

        Zeroing_FinishCapture();
    }
    if (ZERO_STATUS == 3) {
        //                                                    if(ZERO_ERROR_VALUE > 0 )
        //                                                        {
        //                                                            (zero_load)= (float)( (float)zero_load + (unsigned long)(ZERO_ERROR_VALUE ) );  //Count_DL = Zero Load
        //
        //                                                            return;
        //                                                        }
        //                                                    if(ZERO_ERROR_VALUE < 0 )
        //                                                        {
        //                                                            zero_load = (float)( (float)zero_load - (unsigned long)(ZERO_ERROR_VALUE ) ); //Count_DL = Zero Load
        //
        //                                                            return;
        //                                                        }
    }
}

/*Changes*/
/*
Date 30/1/2018
After Zeroing:
    Span Count calculation after zeroing removed
    full_load is recalculated and is changed by same value as cahnge in zero_load value
*/
