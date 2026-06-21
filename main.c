/*----------------------------------------------------------------------------
 * Name:    MAIN.C
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Date : 06/04/2022  main.c / Digital_Input_Actions.c L232,251,238,257
 * Bug: Interlock & START — alarms did not stop controller
 * Fix: Check M044 (general fault) on start paths
 *
 * Date : 17/04/2022  initialize.c L300,301,327
 * Bug: All DO ON ~2 s during init
 * Fix: Init OLED display after digital inputs
 *
 * Date : 07/08/2022  Filter.c L108,165,223 — AO scale 1500 -> 1000
 * Date : 09/10/2022  Integrate.c L123 — feed rate below MIN_LOAD
 * Date : 18/12/2022  Filter.c — AO at min/zero load
 * Date : 12/03/2022  Filter.c — AO1/AO3 belt load (ver 1.H.1)
 *----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "cmsis_os.h"
#include "LPC17xx.h"
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
#include "modbus_dio.h"
#include "keys.h"
#include "Errors.h"
#include "abcc_app.h"
#include "RAM512KB_Ext.h"

extern unsigned int External_Setpoint_display,
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

extern float WF_Drive_Setpoint, PF_Drive_Setpoint, CM_Display;
extern uint16_t Min_adc1_val, Min_adc2_val, Min_adc3_val, Max_adc1_val, Max_adc2_val, Max_adc3_val;

extern bool Default_Start_bit;
extern unsigned char Status_No, Zero_Tacho_Feed_Buffer_Flag;
extern plc_instruction_struct *plc_ptr;
extern char i5, Disp_Par_Count, Start_Stop_status;
extern char ram_values[RAM_SIZE];
extern long adc_final_val, bsf_adc_final_val;
extern char *clear_string;
extern char counter, Error_No;
extern uint16_t scan_count;
extern uint32_t Key_Input, Key_Input_Bak;
extern unsigned long rx_data;
extern uint16_t sps, cutoff;
extern uint16_t New_menu_parameters[105];
extern char *P_String;
extern char A_String[35];
extern unsigned long Present_Count, Count_DL, Span_Count;
extern unsigned long Calib_Load;
extern float fil_A, fil_B, fil_C, fil_D, fil_E;
extern unsigned char order;
extern float cutoff_1;
extern int count;
extern volatile char modarray_rd[MAX_LENGTH], modarray_wr[MAX_LENGTH];
extern uint8_t plc_temp[400];
extern const main_menu_struct Main_menu1[Main_Menu_Max];
extern sub_menu_struct sub_menu2[10][20];
extern sub_menu_struct sub_menu1[12][26];
extern char const sub_menu_options1[13][26][12][20];
extern unsigned int Tacho_Counter, Count_Tacho;
extern unsigned int Totaliser1_Display, Totaliser2_Display, Common_Setpoint;
extern float ZERO_VALUE, ZERO_ERROR_VALUE, ZERO_ERROR_PERSCENT;
extern unsigned char ZERO_STATUS;
extern char Check_Start;
extern char const Online_menu_options[5][14][10][20];
extern sub_menu_struct Online_menu[5][14];
extern main_menu_struct Online_Main_menu[Main_Menu_Max];
extern char Rows_clear, Oled_flag;
extern char display_cursor;
extern char IO_Online_flag;
extern uint8_t Modbus_Monitor_Time, Mod_Monitoring_Flag;
extern char total_status;
extern signed int Belt_Load;
extern unsigned long zero_load;
extern int DIO_ModTxLen, DIO_ModTxCount;
extern char tx_status_1, rx_status_1;
extern uint16_t adc_count, AI_calb_val[6], AO_val[6];
extern char Ram_display_status;
extern int Ram_adres;
extern char *Ram_pointer;
extern char Ram_status;
extern unsigned char cc, dd;
extern unsigned char ACAL_Status;
extern bool CurTripStatus, CurErrorStatus;

char lcd_flag;
int lcd_count_disp;
volatile uint8_t flag;
uint16_t sec_counter = 0;
uint64_t timer_count1, timer_count2;
char menu_pointer;
unsigned int temp_count1, temp_count2, temp_count3;

void ControlActions(void);

int main(void)
{
    main_initialize_routine();
    ABCC_Init();
    VfdBootCommWarmup_ms(VFD_BOOT_WARMUP_MS);
    VfdBootPostWarmupInit();

    loop_forever:
    {
        if (flag == 1)
        {
            temp_count1 = scan_count;
            flag = 0;

            if (lcd_flag != 0)
            {
                if ((Check_Start != 20) && (total_status == 0))
                    PRINT_ONLINE();

                if (((Check_Start == 20) && (Oled_flag == 3)) ||
                    ((total_status > 0) && (Oled_flag == 3)))
                    PRINT_ONLINE();

                if ((IO_Online_flag == 1) && (Default_Start_bit == false))
                    IO_Monitor_Online();
            }

            if (((ZERO_STATUS == 0) && (Check_Start == 20)) ||
                ((ZERO_STATUS == 0) && (total_status > 0)) ||
                (ZERO_STATUS == 0))
            {
                if (Oled_flag < 3)
                    Clear_LCD_Temp();
            }

            if (((SIO_Interface == 2) || (SIO_Interface == 3)) &&
                (Default_Start_bit == false))
            {
                ABCC_main();
            }

            if (Default_Start_bit == false)
            {
                read_adc_conversion();
                load_cell_avg_processing();
                Read_adc_ltc1288();
                AI_adc_avg_cal();
            }

            if (((scan_count % 2) == 1) && (Default_Start_bit == false))
            {
                if (SIO_Interface == 1)
                    modbus();
                DIO_modbus();
                Invter_ModCom();
                Invter2_ModCom();
            }

            if (((scan_count % 2) == 0) && (Default_Start_bit == false))
            {
                IO_Control_Routine();
                VfdDiSanitizeStartMarkers();
                plc_execute();
                ControlActions(); /* Digital_Input_Action() disabled 17-mar-2026 */
                Valve_Control();
            }

            if (((scan_count % 2) == 0) && (Default_Start_bit == false))
            {
                Integrate();
                Totaliser();
                if ((ACAL_Status > 0) && (ACAL_Status < 3))
                    Auto_Calibration();
            }

            if (((scan_count % 3) == 1) && (Default_Start_bit == false))
            {
                if ((ZERO_STATUS > 0) && (ZERO_STATUS < 3))
                    Zeoring();
            }

            if (((scan_count % 4) == 0) && (Default_Start_bit == false))
                load_cell_avg_processing();

            if ((scan_count == 10) && (Default_Start_bit == false))
                Tacho_Avg_Calculation();

            if ((scan_count % 10) == 0)
            {
                key_check_routine();
                key_action();
                LED_Routine();
                Totaliser_Out();
            }

            if ((scan_count % 15) == 0)
            {
                if ((total_status > 0) && (total_status < 5))
                    Totaliser1_Clear();

                if ((ZERO_STATUS == 0) && (Check_Start == 20))
                    Online_Menu_Routine();
            }

            if ((scan_count % 20) == 0)
                Modbus_Monitoring_Time();

            if ((scan_count == 25) || (scan_count == 50) || (scan_count == 100) ||
                (scan_count == 125) || (scan_count == 150) ||
                (scan_count == 175) || (scan_count == 200))
            {
                if (Default_Start_bit == false)
                {
                    if ((ZERO_STATUS > 1) && (ZERO_STATUS < 6))
                        Zeroing_Display();
                    else
                    {
                        Read_adc_ltc1288();
                        AI_adc_avg_cal();
                    }
                }
            }

            if (AI_Nos > 0)
            {
                if ((scan_count == 23) || (scan_count == 47) ||
                    (scan_count == 71) || (scan_count == 123) ||
                    (scan_count == 143) || (scan_count == 171) ||
                    (scan_count == 183))
                {
                    Read_adc1_ltc1288();
                    AI_adc1_avg_cal();
                }
            }

            if (AI_Nos > 1)
            {
                if ((scan_count == 21) || (scan_count == 43) ||
                    (scan_count == 73) || (scan_count == 121) ||
                    (scan_count == 144) || (scan_count == 172) ||
                    (scan_count == 191))
                {
                    Read_adc2_ltc1288();
                    AI_adc2_avg_cal();
                }
            }

            if ((scan_count % 27) == 0)
            {
                Anlog_Out();
                Calculate_WF_drive_setpoint_Display();
                PF_Drive_Setpoint_Cal();
            }

            if (AO_Nos > 0)
            {
                if ((scan_count % 26) == 0)
                    Anlog_Out2();
            }

            if (AO_Nos > 1)
            {
                if ((scan_count % 28) == 0)
                    Anlog_Out3();
            }

            if ((scan_count % 90) == 0)
                Display_Error();

            if ((scan_count == 141) || (scan_count == 169))
            {
                if (ZERO_STATUS == 0)
                {
                    i5 = 10;
                    Online_routine();
                }
            }

            if ((scan_count % 95) == 0)
            {
                display_line_calculate_Modbus();
                Disp_Par_Count++;
                if (Disp_Par_Count == 8)
                    Disp_Par_Count = 0;
            }

            if ((Start_Stop_status == STOP) && (Zero_Tacho_Feed_Buffer_Flag == 2))
                Zero_Tacho_Feed_Buffer();

            if (SIO_Interface > 0)
                Modbus_Registers();

            temp_count2 = scan_count;

            if (temp_count2 > temp_count1)
            {
                temp_count3 = temp_count2 - temp_count1;
                temp_count2 = 0;
                temp_count1 = 0;
            }

            if (temp_count2 < temp_count1)
            {
                temp_count3 = (temp_count2 + 200) - temp_count1;
                temp_count2 = 0;
                temp_count1 = 0;
            }

            if (temp_count3 > 0)
                temp_count3 = 0;
        }

        goto loop_forever;
    }
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
