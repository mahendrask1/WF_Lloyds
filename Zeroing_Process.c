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
extern uint32_t Key_Input;
extern unsigned long calib_val[8];
extern unsigned long Count_Limit1, Count_Limit2, Count_Max, Count_Min;
extern unsigned int Tacho_Counter;
extern unsigned int Belt_Speed;
/* Extern Variables*/
/*New Variables*/
float ZERO_VALUE, ZERO_ERROR_VALUE, ZERO_ERROR_PERSCENT;
unsigned char ZERO_STATUS;
extern char Status_No, Error_No;
unsigned long Belt_REV_COUNT, Belt_Rev_Count_Value;

extern char Oled_flag, Rows_clear;
/*New Variables*/

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
        while (Key_Input == 0xFF) {  //stay here until any key pressed
            key_check_routine();
        }
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
        while (Key_Input == 0xFF) {  //stay here until any key pressed
            key_check_routine();
        }
        if (Key_Input != 0xFF) { //if any key is pressed  then goto online menu
            ZERO_STATUS = 0;
            return;
        }
    }
}

void Zeoring(void)  //calculations of zero value called every 50ms i.e 20th scancount
{
    if (ZERO_STATUS == 0) {
        return;
    }
    if (ZERO_STATUS == 1) {
        Belt_REV_COUNT = ((float)(Rev_Time * Tacho_Freq * 6.1) / (Belt_Speed)); // (Rev_Time * Tacho_Freq * 2 )/ Tacho_Actual
        Belt_Rev_Count_Value = Belt_REV_COUNT;
        ZERO_STATUS = 2;
        ZERO_VALUE = 0;
        Clear_LCD();
    }
    if (ZERO_STATUS == 2) {
        ZERO_VALUE = ZERO_VALUE + ((float)(Present_Count) - (float)(zero_load));

        if (Error_No == 3 /*TACHO_Fault==1*/) {
            ZERO_STATUS = 5;
            return;
        }

        Belt_REV_COUNT = --Belt_REV_COUNT;

        if (Belt_REV_COUNT != 0) {
            return;
        }

        if (Belt_REV_COUNT == 0) {
            ZERO_ERROR_VALUE = (float)((float)ZERO_VALUE / Belt_Rev_Count_Value);
            ZERO_ERROR_PERSCENT = (float)((float)(ZERO_ERROR_VALUE * 100) / (float)(full_load - zero_load));
            if ((unsigned int)((ZERO_ERROR_VALUE - (unsigned int)ZERO_ERROR_VALUE) * 10) > 4) {
                ZERO_ERROR_VALUE = ZERO_ERROR_VALUE + 1;
            }
            if ((unsigned int)((ZERO_ERROR_VALUE - (unsigned int)ZERO_ERROR_VALUE) * 10) <= 4) {
                ZERO_ERROR_VALUE = ZERO_ERROR_VALUE;
            }
        }

        if ((unsigned int)(ZERO_ERROR_PERSCENT * 10) <= Zero_Limit) {  //Zero_Limit/10 because to get decimal point value as entered
            ZERO_STATUS = 3;
        }
        if ((unsigned int)(ZERO_ERROR_PERSCENT * 10) > Zero_Limit) {  //Zero_Limit/10 because to get decimal point value as entered
            ZERO_STATUS = 4;
        }
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
