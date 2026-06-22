/*parameters.h*/
#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "menu.h"
#include "parameters.h"
#include "stdbool.h"
#include <stdint.h>

/*adc.c*/
void adc_cs(int on);
long adc_read(void);
long adc_read_value(void);
void init_adc_cs5532(void);
void adc_spi_fault_recovery(void); //msk 26-may-2026
extern volatile uint8_t adc_spi_recovery_count; //msk 26-may-2026
void read_adc_conversion(void);
/***************************************/
/*adc12bit*/
void adc_ltc1288_cs(int on);
void adc_ltc1288_CLK(char on);
void adc_ltc1288_DOUT(char on);
char adc_ltc1288_DIN(void);
int adc_ltc1288_read_bit_by_bit(void);
int adc_ltc1288_read(void);
int adc_ltc1288_read_value(void);
void Read_adc_ltc1288(void);
void AI_adc_avg_cal(void);
/***************************************/

/**************Calibration**************/
void AI_calib_display(void);
void AI_Calibration(void);
void AO_calib_display(void );
void AO_Calibration(void);
void calib_display(void );
void Calibration (void);
void calib_init_display(void);
/****************************************/
/* dac.c*/
void dac1_cs(int on);
void dac_CLK(char on);
void dac_DOUT(char on);
void init_dac(void);
void dac1_write(unsigned int dac1_Count);

/**************eeprom.c******************/
void eeprom_cs(int on);
void eeprom_read_block(int start_address, unsigned char *ram_address, int no_of_bytes);
void eeprom_write_block(int start_address, char *ram_address, int no_of_bytes, int no_of_bytes_written,int par_size);
void eeprom_write_array(unsigned int , unsigned char * , unsigned int );
void read_ass_eeprom(void);
void read_calib_val(void);
void read_AI_calib_val(void);
char read_eeprom_status(void);
void read_par_eeprom(void);
void read_plc_edit_eeprom(void);
void read_report_time(void);
void read_totalizer_eeprom(void);

void save_ass_eeprom(void);
void save_calib_val(void);
void save_AI_calib_val(void);
void save_AO_calib_val(void);
void save_par_eeprom(void);
void save_plc_edit_eeprom(void);

void save_report_time(void);
void save_totalizer_eeprom(void);
/********************************/

/********************filter.c*************/
void bessel_fil_cal(void);
void bsf_filter_calculation(void);
void Calculation(void);
void filter_calculation(void);
void filter_para_calc(void);
/*****************************************/

/******************initialize.c***********/

void after_menu_initialize(void);
void main_initialize_routine(void);

/*****************************************/

/* Digital_Input_Actions.c */
void Digital_Input_Action(void);

/* Auto Calibration.c */
void Auto_Calibration(void);
void After_AutoCAL(void);

/****************io_control.c*************/
void Cancel_Tare(void);
void Digital_Input_Routine(void);
void Digital_Output_Routine(void);
void IO_Control_Routine(void);
void Start_Loop(void);
void Stop_Loop(void);
/*****************************************/

/****************Key_control.c************/
void key_action (void) ;
/*****************************************/
/****************Keys.c*******************/
void key_check_routine(void);
void Key_Convert(void);
void Key_Decode(void );
void keypad_init(void);
void latch_control(char on);
void LED_Routine(void);
void toggle_start_stop(void);
/*****************************************/
/****************menu.c********************/
void Check_password(void);
void Cursor_Location(char line_no);

/*****************************************/

void UART_PrintExp(void);
void rtc_config(void);

/**************oled**********************/
void PRINT_ONLINE(void);
/****************************************/

/**************UI/menu/helpers***********/
void Print_main_menu(main_menu_struct Main_menu[Main_Menu_Max],
                     sub_menu_struct sub_menu[12][26],
                     char const sub_menu_options[12][26][12][20],
                     int menu_type);

void Online_Sub_Menu(main_menu_struct Main_menu[Main_Menu_Max],
                     char Sub_Menu_no1,
                     sub_menu_struct sub_menu[5][14],
                     char const sub_menu_options[5][14][10][20],
                     char Sub_menu_line_no);
/****************************************/

/**************online/monitoring*********/
void IO_Monitor_Online(void);
void Clear_LCD_Temp(void);
void Clear_LCD_RAM_Half_Online(void);
void Clear_LCD_RAM_Half(void);
void display_line_calculate_Modbus(void);
/****************************************/

/**************process routines**********/
void Valve_Control(void);
void Zeoring(void);
void Zeroing_Display(void);
void Tacho_Avg_Calculation(void);
void Zero_Tacho_Feed_Buffer(void);
void load_cell_avg_processing(void);
void Totaliser1_Clear(void);
void Modbus_Registers(void);
/****************************************/

/**************inverter/DIO**************/
void Invter_ModCom(void);
void Invter2_ModCom(void);
void Menu_ServiceInvComms(void); /* SIO + VFD/DIO + PLC/control while menu/IO monitor blocks main */
void ResetInvCommState(void); /* msk29-may-26 */
void ClearInvCommFaultFlags(void); /* inv comm fault clear without VFD DI debounce reset */
void ResetInvCommAfterOfflineMenu(void); /* msk29-may-26 */
void ResetVfdDiInputFilter(void); /* debounce I024-I029 after boot or menu */
#define VFD_BOOT_WARMUP_MS    1000U
void VfdBootCommWarmup_ms(unsigned int duration_ms); /* poll VFD before main loop */
void VfdBootSendFaultResetBeforeSuperloop(void); /* FC10 fault reset + stop before main loop */
void VfdDiSanitizeStartMarkers(void); /* clear phantom M011 before PLC scan */
unsigned char VfdDiPlcReady(void);
unsigned char VfdDiLevel024(void);
void InvComm_NotifyOfflineMenuEnter(void); /* msk29-may-26 */
/****************************************/

/**************analog inputs (12-bit)****/
void Read_adc1_ltc1288(void);
void Read_adc2_ltc1288(void);
void AI_adc1_avg_cal(void);
void AI_adc2_avg_cal(void);
/****************************************/

/**************setpoint/display**********/
void Calculate_WF_drive_setpoint_Display(void);
void PF_Drive_Setpoint_Cal(void);
void Update_External_Setpoint(void);
void Calculate_Setpoint_Display(void);
void Gate_Postion_CalCulation(void);
/****************************************/

/**************analog outputs************/
void Anlog_Out2(void);
void Anlog_Out3(void);
/****************************************/

/**************batching.c****************/
void Batching_Routine(void);
/****************************************/
/***************online.c*****************/
void Online_routine(void);
void display_line_calculate(void);
void Online_Menu_Routine(void);
/****************************************/
void Calibration(void);
void IO_Monitor(void);
void plc_menu(void);
//void setpoint_function(void);
//void IO_Status_display(void);
void IP_Status_display(char iotm_no,char iotm1,char iotm2,char iotm3,char iotm4,char iotm5,char iotm6,char iotm7,char iotm8,char iotm9,char iotm10);
void OP_Status_display(char iotm_no,char iotm1,char iotm2,char iotm3,char iotm4,char iotm5,char iotm6,char iotm7,char iotm8,char iotm9,char iotm10);
void MEM_Status_display1(char iotm_no,char iotm1,char iotm2,char iotm3,char iotm4,char iotm5,char iotm6,char iotm7,char iotm8,char iotm9,char iotm10);
void MEM_Status_display2(char iotm_no,char iotm1,char iotm2,char iotm3,char iotm4,char iotm5,char iotm6,char iotm7,char iotm8,char iotm9,char iotm10);
void Timer_Status_display(char iotm_no,char iotm1,char iotm2,char iotm3,char iotm4,char iotm5,char iotm6,char iotm7,char iotm8,char iotm9,char iotm10);
/****************************************/
void Totaliser(void);          //Totaliser Calculation
void Integrate(void);          // Actual Feed Rate Calculation
void Anlog_Out(void);         //Calculation for analog o/p(Dac1 and Dac2 Value calculation)
void Totaliser_Out(void); //Tot5aliser_Out (To display  count of T1 and/or T2 at Remote Location   )
/****************************************/
//---controls.c
void ControlActions(void);
void ApplyFaultResetMarkers(void); /* M018-equivalent fault / inv comm clear */
void VfdBootPostWarmupInit(void);  /* fault clear + sanitize after boot VFD warmup */
void Ext_WF_Controls(void);
bool GetFieldMode(void);
bool GetComMode(void);
bool GetIntMode(void);
bool GetDeIntMode(void);
bool GetVolMode(void);
bool GetGraMode(void);
bool GetDeIntVolMode(void); 
bool GetDeIntGraMode(void);
bool GetIntGraMode(void);
bool GetIntVolMode(void);   
#endif
