/*        IO_CONTORL.c        */

#include "plc.h"
#include "parameters.h"  //#include "menu.h"
#include "keys.h"
#include "io_control.h"
#include "main_routine.h"
#include "LPC17xx.h"                    // Device header
#include "lpc17xx_gpio.h"
#include "GPIO_LPC17xx.h"
#include "functions.h"
#include "OLED_2_7_12864.h"

#define latch_port_DO0 2      // Latch for DIO 1st card
#define latch_pin_DO0 11            // Latch for DIO 1st card

#define latch_port_DO1 2            // Latch for DIO 2nd card
#define latch_pin_DO1 8                // Latch for DIO 2nd card

extern char ram_values[RAM_SIZE];
extern uint16_t New_menu_parameters[105];
extern unsigned long Count_DL, adc_final_val,bsf_adc_final_val;
extern char Start_Stop_status,Error_No;
extern unsigned char  Status_No;
//extern char Auto_Man_status;
extern char TIMER_ENABLED;
extern int temp_count1,temp_count2; //temp variable
extern volatile char DIO_modarray_rd[2],DIO_modarray_wr[2];
const char *io_str[20][3] = {    {"."},
    {"I00"},{"I01"},{"I02"},{"I03"},{"O00"},{"O01"},{"O02"},{"O03"},
    {"M00"},{"M01"},{"M02"},{"M03"},{"M04"},{"M05"},{"M06"},{"M07"},{"M08"},{"M09"},{"T00"}
};

const char *io_str_name[5][13] = {
    {"Input_Status"},
    {"Output_Status"},
    {"Memory_Status"},
    {"Timer_Status"},
    {"IOTM_Status"}
};

void latch_control_DO0(char on)
{
    GPIO_PinWrite (latch_port_DO0,latch_pin_DO0, on);
}

//void latch_control_DO1(char on)
//{
//  GPIO_PinWrite (latch_port_DO1,latch_pin_DO1, on);
//}

void DO0_Init()
{
    GPIO_SetDir(latch_port_DO0,latch_pin_DO0,1);        //Initiallize Latch
    GPIO_PinWrite(latch_port_DO0,latch_pin_DO0,1);  //Turn on Latch

    GPIO_PinWrite(2,0,!O012); //D001    active low
    GPIO_PinWrite(2,1,!O013); //D002    active low
    GPIO_PinWrite(2,2,!O014); //D003    active low
    GPIO_PinWrite(2,3,!O015); //D004    active low

    GPIO_PinWrite(latch_port_DO0,latch_pin_DO0,0);  //Turn off Latch

    GPIO_SetDir(latch_port_DO1,latch_pin_DO1,1);        //Initiallize Latch
    GPIO_PinWrite(latch_port_DO1,latch_pin_DO1,1);  //Turn on Latch

    GPIO_PinWrite(2,0,!O008); //D001    active low
    GPIO_PinWrite(2,1,!O009); //D002    active low
    GPIO_PinWrite(2,2,!O010); //D003    active low
    GPIO_PinWrite(2,3,!O011); //D004    active low

    GPIO_PinWrite(latch_port_DO1,latch_pin_DO1,0);  //Turn off Latch

}

/**********************************************************************************
    Digital_Input_Routine()

    Algorithm:
Digital_Input:
    If P1.28 = 0, I000=0, else I000=1
    If P1.29 = 0, I001=0, else I001=1
    If P1.30 = 0, I002=0, else I002=1
    If P1.31 = 0, I003=0, else I003=1
  If P0.29 = 0, I004=0, else I004=1
    If P0.30 = 0, I005=0, else I005=1
    Return
    **************************************

    ********************************************/
void Digital_Input_Routine(void)
{
    I000 = (GPIO_PinRead(1,28));     //DI0
    I001 = (GPIO_PinRead(1,29));    //DI1
    I002 = (GPIO_PinRead(0,28));    //DI2
    I003 = (GPIO_PinRead(1,31));    //DI3
    I004 = (GPIO_PinRead(1,21));    //DI4
    I005 = (GPIO_PinRead(1,27));    //DI5

    I012 = (GPIO_PinRead(0,4));     //DI12
    I013 = (GPIO_PinRead(0,5));        //DI13
    I014 = (GPIO_PinRead(0,19));    //DI14
    I015 = (GPIO_PinRead(0,20));    //DI15

    I008 = (GPIO_PinRead(0,21));    //DI8
    I009 = (GPIO_PinRead(0,23));    //DI9
    I010 = (GPIO_PinRead(0,24));    //DI10
    I011 = (GPIO_PinRead(0,27));    //DI11

    return;
}

/**********************************************************************************
    Digital_Output_Routine()

    Algorithm:
Digital_Output:
    If O000=0, P1.20=0, else P1.20=1  //D001
    ...
    Return
    **********************************************************************************/
void Digital_Output_Routine(void)
{
    int i;
    GPIO_PinWrite(1,20,!O000); //D001    active low
    GPIO_PinWrite(1,22,!O001); //D002    active low
    GPIO_PinWrite(1,23,!O002); //D003    active low
    GPIO_PinWrite(1,24,!O003); //D004    active low
    GPIO_PinWrite(1,25,!O004); //DOO5    active low
    GPIO_PinWrite(1,26,!O005); //DOO6    active low
    GPIO_PinWrite(0,10,!O006); //DOO7    active low
    GPIO_PinWrite(0,26,!O007); //DOO8    active low

    GPIO_PinWrite(2,0,!O008); //D001    active low
    GPIO_PinWrite(2,1,!O009); //D002    active low
    GPIO_PinWrite(2,2,!O010); //D003    active low
    GPIO_PinWrite(2,3,!O011); //D004    active low
    for(i=0;i<200;i++){;}
    GPIO_PinWrite(latch_port_DO1,latch_pin_DO1,1);  //Turn on Latch
    for(i=0;i<200;i++){;}
    GPIO_PinWrite(latch_port_DO1,latch_pin_DO1,0);  //Turn off Latch
    for(i=0;i<200;i++){;}
    GPIO_PinWrite(2,0,!O012); //D001    active low
    GPIO_PinWrite(2,1,!O013); //D002    active low
    GPIO_PinWrite(2,2,!O014); //D003    active low
    GPIO_PinWrite(2,3,!O015); //D004    active low
    for(i=0;i<200;i++){;}
    GPIO_PinWrite(latch_port_DO0,latch_pin_DO0,1);  //Turn on Latch
    for(i=0;i<200;i++){;}
    __nop();__nop();__nop();__nop();__nop();__nop();__nop();
    GPIO_PinWrite(latch_port_DO0,latch_pin_DO0,0);  //Turn off Latch
    for(i=0;i<200;i++){;}

    return;
}
/**********************************************************************************
    IO_Control_Routine()

    Algorithm:
    Call Digital_Input
    ...
    Return
    **********************************************************************************/
void IO_Control_Routine(void)
{
    Digital_Input_Routine();
    Digital_Output_Routine();
    return;
}

/*
31/03/2018
//Previous was X7A=DIO12..15 & X7B=DIO08..11; Changed DI & DO to X7A=DIO08..11;& X7A=DIO12..15 ;
*/
