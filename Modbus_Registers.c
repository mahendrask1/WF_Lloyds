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
#include "Errors.h"

//volatile unsigned char modarray_rd[MAX_LENGTH],recipe[MAX_LENGTH];
extern volatile char DIO_modarray_rd[2],modarray_rd[MAX_LENGTH],modarray_wr[MAX_LENGTH],DIO_modarray_wr[2];;
extern uint16_t New_menu_parameters[105];
extern uint8_t ram_values[RAM_SIZE];
extern unsigned int Common_Setpoint;
extern unsigned int Actual_Feedrate;
int16_t intdeviation;
extern unsigned char Display_Sign,Deviation_Sign;
extern unsigned int External_Setpoint,
                                        Common_Setpoint,
                                        Current_Setpoint,
                                        Deviation,
                                        PF_Drive_Setpoint,
                                        WF_Drive_Setpoint_per,
                                        Belt_Speed,
                                        PF_Drive_Setpoint_Per,
                                        Hopper_val,
                                        Gate_Postion,
                                        HOpper_total,
                                        FDR_Total,
                                        CM_Difference;

extern double Totaliser1,Totaliser2;
extern unsigned int Totaliser1_Display,Totaliser2_Display;
extern unsigned int Common_Setpoint;
extern signed int Belt_Load;
extern float WF_Drive_Setpoint,CM_Display;
extern unsigned short
            M0_15_Read,
            M16_31_Read,
            M32_39_Read;

extern unsigned short
            Deviation_Comm,
            Belt_Load_Comm;

extern unsigned short  ALL_Data[35];
extern unsigned int Inv_Op_Freq,Inv1_Op_Freq;
extern unsigned char Inv_Op_current,Inv1_Op_current;

 void Modbus_Registers()
 {

/********************** Read Address Starts From 8*****************************************/
/*************************** Digital Inputs 0 to 15 ***********************/ //8 : Read Adreess
  modarray_rd[0] =  ( ((unsigned char*)&I000)[0] );
    modarray_rd[0] = (modarray_rd[0] | ( (((unsigned char*)&I001)[0]) << 1 ));
  modarray_rd[0] = (modarray_rd[0] | ( (((unsigned char*)&I002)[0]) << 2 ));
  modarray_rd[0] = (modarray_rd[0] | ( (((unsigned char*)&I003)[0]) << 3 ));
  modarray_rd[0] = (modarray_rd[0] | ( (((unsigned char*)&I004)[0]) << 4 ));
    modarray_rd[0] = (modarray_rd[0] | ( (((unsigned char*)&I005)[0]) << 5 ));
  modarray_rd[0] = (modarray_rd[0] | ( (((unsigned char*)&I006)[0]) << 6 ));
  modarray_rd[0] = (modarray_rd[0] | ( (((unsigned char*)&I007)[0]) << 7 ));

  modarray_rd[1] = ( ( (((unsigned char*)&I008)[0]) << 0 ));
    modarray_rd[1] = (modarray_rd[1] | ( (((unsigned char*)&I009)[0]) << 1 ));
    modarray_rd[1] = (modarray_rd[1] | ( (((unsigned char*)&I010)[0]) << 2 ));
  modarray_rd[1] = (modarray_rd[1] | ( (((unsigned char*)&I011)[0]) << 3 ));
  modarray_rd[1] = (modarray_rd[1] | ( (((unsigned char*)&I012)[0]) << 4 ));
  modarray_rd[1] = (modarray_rd[1] | ( (((unsigned char*)&I013)[0]) << 5 ));
    modarray_rd[1] = (modarray_rd[1] | ( (((unsigned char*)&I014)[0]) << 6 ));
    modarray_rd[1] = (modarray_rd[1] | ( (((unsigned char*)&I015)[0]) << 7 ));

    ALL_Data[0] = ( modarray_rd[1]);
    ALL_Data[0] = ( ALL_Data[0] << 8 | modarray_rd[0]);

    /************************** Digital Inputs 16 to 31***********************/  //9
    modarray_rd[2] =  ( ((unsigned char*)&I016)[0] );
  modarray_rd[2] = (modarray_rd[2] | ( (((unsigned char*)&I017)[0]) << 1 ));
  modarray_rd[2] = (modarray_rd[2] | ( (((unsigned char*)&I018)[0]) << 2 ));
  modarray_rd[2] = (modarray_rd[2] | ( (((unsigned char*)&I019)[0]) << 3 ));
    modarray_rd[2] = (modarray_rd[2] | ( (((unsigned char*)&I020)[0]) << 4 ));
  modarray_rd[2] = (modarray_rd[2] | ( (((unsigned char*)&I021)[0]) << 5 ));
  modarray_rd[2] = (modarray_rd[2] | ( (((unsigned char*)&I022)[0]) << 6 ));
  modarray_rd[2] = (modarray_rd[2] | ( (((unsigned char*)&I023)[0]) << 7 ));

    modarray_rd[3] = ( ( (((unsigned char*)&I024)[0]) << 0 ));
    modarray_rd[3] = (modarray_rd[3] | ( (((unsigned char*)&I025)[0]) << 1 ));
  modarray_rd[3] = (modarray_rd[3] | ( (((unsigned char*)&I026)[0]) << 2 ));
  modarray_rd[3] = (modarray_rd[3] | ( (((unsigned char*)&I027)[0]) << 3 ));
  modarray_rd[3] = (modarray_rd[3] | ( (((unsigned char*)&I028)[0]) << 4 ));
    modarray_rd[3] = (modarray_rd[3] | ( (((unsigned char*)&I029)[0]) << 5 ));
    modarray_rd[3] = (modarray_rd[3] | ( (((unsigned char*)&I030)[0]) << 6 ));
    modarray_rd[3] = (modarray_rd[3] | ( (((unsigned char*)&I031)[0]) << 7 ));

        ALL_Data[1] = (  modarray_rd[3]);
        ALL_Data[1] = ( ALL_Data[1] << 8 | modarray_rd[2]);

        /************************** Digital Inputs 32 to 39***********************/  //10
    modarray_rd[4] =  ( ((unsigned char*)&I032)[0] );
  modarray_rd[4] = (modarray_rd[4] | ( (((unsigned char*)&I033)[0]) << 1 ));
  modarray_rd[4] = (modarray_rd[4] | ( (((unsigned char*)&I034)[0]) << 2 ));
  modarray_rd[4] = (modarray_rd[4] | ( (((unsigned char*)&I035)[0]) << 3 ));
    modarray_rd[4] = (modarray_rd[4] | ( (((unsigned char*)&I036)[0]) << 4 ));
  modarray_rd[4] = (modarray_rd[4] | ( (((unsigned char*)&I037)[0]) << 5 ));
  modarray_rd[4] = (modarray_rd[4] | ( (((unsigned char*)&I038)[0]) << 6 ));
  modarray_rd[4] = (modarray_rd[4] | ( (((unsigned char*)&I039)[0]) << 7 ));

    /*modarray_rd[5] = (modarray_rd[5] | ( (((unsigned char*)&I024)[0]) << 0 ));
    modarray_rd[5] = (modarray_rd[5] | ( (((unsigned char*)&I025)[0]) << 1 ));
  modarray_rd[5] = (modarray_rd[5] | ( (((unsigned char*)&I026)[0]) << 2 ));
  modarray_rd[5] = (modarray_rd[5] | ( (((unsigned char*)&I027)[0]) << 3 ));
  modarray_rd[5] = (modarray_rd[5] | ( (((unsigned char*)&I028)[0]) << 4 ));
    modarray_rd[5] = (modarray_rd[5] | ( (((unsigned char*)&I029)[0]) << 5 ));
    modarray_rd[5] = (modarray_rd[5] | ( (((unsigned char*)&I030)[0]) << 6 ));
    modarray_rd[5] = (modarray_rd[5] | ( (((unsigned char*)&I031)[0]) << 7 ));*/

        ALL_Data[2] = ( modarray_rd[4]);
        //I32_47_Write = ( I32_47_Write << 12 | modarray_rd[3]);

    /************************ Digital Outputs 0 to 15 *************************/ //11
  modarray_rd[6] =  ( ((unsigned char*)&O000)[0] );
    modarray_rd[6] = (modarray_rd[6] | ( (((unsigned char*)&O001)[0]) << 1 ));
  modarray_rd[6] = (modarray_rd[6] | ( (((unsigned char*)&O002)[0]) << 2 ));
  modarray_rd[6] = (modarray_rd[6] | ( (((unsigned char*)&O003)[0]) << 3 ));
  modarray_rd[6] = (modarray_rd[6] | ( (((unsigned char*)&O004)[0]) << 4 ));
    modarray_rd[6] = (modarray_rd[6] | ( (((unsigned char*)&O005)[0]) << 5 ));
  modarray_rd[6] = (modarray_rd[6] | ( (((unsigned char*)&O006)[0]) << 6 ));
  modarray_rd[6] = (modarray_rd[6] | ( (((unsigned char*)&O007)[0]) << 7 ));

  modarray_rd[7] = ( ( (((unsigned char*)&O008)[0]) << 0 ));
    modarray_rd[7] = (modarray_rd[7] | ( (((unsigned char*)&O009)[0]) << 1 ));
    modarray_rd[7] = (modarray_rd[7] | ( (((unsigned char*)&O010)[0]) << 2 ));
  modarray_rd[7] = (modarray_rd[7] | ( (((unsigned char*)&O011)[0]) << 3 ));
  modarray_rd[7] = (modarray_rd[7] | ( (((unsigned char*)&O012)[0]) << 4 ));
  modarray_rd[7] = (modarray_rd[7] | ( (((unsigned char*)&O013)[0]) << 5 ));
    modarray_rd[7] = (modarray_rd[7] | ( (((unsigned char*)&O014)[0]) << 6 ));
    modarray_rd[7] = (modarray_rd[7] | ( (((unsigned char*)&O015)[0]) << 7 ));

        ALL_Data[3] = ( modarray_rd[7]);
        ALL_Data[3] = ( ALL_Data[3] << 8 | modarray_rd[6]);

        /************************ Digital Outputs 16 to 31 *************************/ //12
  modarray_rd[8] =  ( ((unsigned char*)&O016)[0] );
    modarray_rd[8] = (modarray_rd[8] | ( (((unsigned char*)&O017)[0]) << 1 ));
  modarray_rd[8] = (modarray_rd[8] | ( (((unsigned char*)&O018)[0]) << 2 ));
  modarray_rd[8] = (modarray_rd[8] | ( (((unsigned char*)&O019)[0]) << 3 ));
  modarray_rd[8] = (modarray_rd[8] | ( (((unsigned char*)&O020)[0]) << 4 ));
    modarray_rd[8] = (modarray_rd[8] | ( (((unsigned char*)&O021)[0]) << 5 ));
  modarray_rd[8] = (modarray_rd[8] | ( (((unsigned char*)&O022)[0]) << 6 ));
  modarray_rd[8] = (modarray_rd[8] | ( (((unsigned char*)&O023)[0]) << 7 ));

  modarray_rd[9] = ( ( (((unsigned char*)&O024)[0]) << 0 ));
    modarray_rd[9] = (modarray_rd[9] | ( (((unsigned char*)&O025)[0]) << 1 ));
    modarray_rd[9] = (modarray_rd[9] | ( (((unsigned char*)&O026)[0]) << 2 ));
  modarray_rd[9] = (modarray_rd[9] | ( (((unsigned char*)&O027)[0]) << 3 ));
  modarray_rd[9] = (modarray_rd[9] | ( (((unsigned char*)&O028)[0]) << 4 ));
  modarray_rd[9] = (modarray_rd[9] | ( (((unsigned char*)&O029)[0]) << 5 ));
    modarray_rd[9] = (modarray_rd[9] | ( (((unsigned char*)&O030)[0]) << 6 ));
    modarray_rd[9] = (modarray_rd[9] | ( (((unsigned char*)&O031)[0]) << 7 ));

        ALL_Data[4] = ( modarray_rd[9]);
        ALL_Data[4] = ( ALL_Data[4] << 8 | modarray_rd[8]);

    /************************ Memory Bits 00 to 15 *************************/  //13
  modarray_rd[10] =  ( ((unsigned char*)&M000)[0] );
    modarray_rd[10] = (modarray_rd[10] | ( (((unsigned char*)&M001)[0]) << 1 ));
  modarray_rd[10] = (modarray_rd[10] | ( (((unsigned char*)&M002)[0]) << 2 ));
  modarray_rd[10] = (modarray_rd[10] | ( (((unsigned char*)&M003)[0]) << 3 ));
  modarray_rd[10] = (modarray_rd[10] | ( (((unsigned char*)&M004)[0]) << 4 ));
    modarray_rd[10] = (modarray_rd[10] | ( (((unsigned char*)&M005)[0]) << 5 ));
  modarray_rd[10] = (modarray_rd[10] | ( (((unsigned char*)&M006)[0]) << 6 ));
  modarray_rd[10] = (modarray_rd[10] | ( (((unsigned char*)&M007)[0]) << 7 ));

  modarray_rd[11] = (( (((unsigned char*)&M008)[0]) << 0 ));
    modarray_rd[11] = (modarray_rd[11] | ( (((unsigned char*)&M009)[0]) << 1 ));
    modarray_rd[11] = (modarray_rd[11] | ( (((unsigned char*)&M010)[0]) << 2 ));
  modarray_rd[11] = (modarray_rd[11] | ( (((unsigned char*)&M011)[0]) << 3 ));
  modarray_rd[11] = (modarray_rd[11] | ( (((unsigned char*)&M012)[0]) << 4 ));
  modarray_rd[11] = (modarray_rd[11] | ( (((unsigned char*)&M013)[0]) << 5 ));
    modarray_rd[11] = (modarray_rd[11] | ( (((unsigned char*)&M014)[0]) << 6 ));
    modarray_rd[11] = (modarray_rd[11] | ( (((unsigned char*)&M015)[0]) << 7 ));

        ALL_Data[5] = ( modarray_rd[11]);
        ALL_Data[5] = ( ALL_Data[5] << 8  | modarray_rd[10]);

    /************************ Memory Bits 16 to 31 *************************/   //14
  modarray_rd[12] =  (((unsigned char*)&M016)[0]);
    modarray_rd[12] = (modarray_rd[12] | ( (((unsigned char*)&M017)[0]) << 1 ));
  modarray_rd[12] = (modarray_rd[12] | ( (((unsigned char*)&M018)[0]) << 2 ));
  modarray_rd[12] = (modarray_rd[12] | ( (((unsigned char*)&M019)[0]) << 3 ));
  modarray_rd[12] = (modarray_rd[12] | ( (((unsigned char*)&M020)[0]) << 4 ));
    modarray_rd[12] = (modarray_rd[12] | ( (((unsigned char*)&M021)[0]) << 5 ));
  modarray_rd[12] = (modarray_rd[12] | ( (((unsigned char*)&M022)[0]) << 6 ));
  modarray_rd[12] = (modarray_rd[12] | ( (((unsigned char*)&M023)[0]) << 7 ));

  modarray_rd[13] = ( ( (((unsigned char*)&M024)[0]) << 0 ));
    modarray_rd[13] = (modarray_rd[13] | ( (((unsigned char*)&M025)[0]) << 1 ));
    modarray_rd[13] = (modarray_rd[13] | ( (((unsigned char*)&M026)[0]) << 2 ));
  modarray_rd[13] = (modarray_rd[13] | ( (((unsigned char*)&M027)[0]) << 3 ));
  modarray_rd[13] = (modarray_rd[13] | ( (((unsigned char*)&M028)[0]) << 4 ));
  modarray_rd[13] = (modarray_rd[13] | ( (((unsigned char*)&M029)[0]) << 5 ));
    modarray_rd[13] = (modarray_rd[13] | ( (((unsigned char*)&M030)[0]) << 6 ));
    modarray_rd[13] = (modarray_rd[13] | ( (((unsigned char*)&M031)[0]) << 7 ));

        ALL_Data[6] = (modarray_rd[13]);
        ALL_Data[6] = ( ALL_Data[6] << 8 | modarray_rd[12]);

        /************************ Memory Bits 32 to 47 *************************/  //15
  modarray_rd[14] =                     (((unsigned char*)&M032)[0]);
    modarray_rd[14] = (modarray_rd[14] | ( (((unsigned char*)&M033)[0]) << 1 ));
  modarray_rd[14] = (modarray_rd[14] | ( (((unsigned char*)&M034)[0]) << 2 ));
  modarray_rd[14] = (modarray_rd[14] | ( (((unsigned char*)&M035)[0]) << 3 ));
  modarray_rd[14] = (modarray_rd[14] | ( (((unsigned char*)&M036)[0]) << 4 ));
    modarray_rd[14] = (modarray_rd[14] | ( (((unsigned char*)&M037)[0]) << 5 ));
  modarray_rd[14] = (modarray_rd[14] | ( (((unsigned char*)&M038)[0]) << 6 ));
  modarray_rd[14] = (modarray_rd[14] | ( (((unsigned char*)&M039)[0]) << 7 ));

  modarray_rd[15] = ( ( (((unsigned char*)&M040)[0]) << 0 ));
    modarray_rd[15] = (modarray_rd[15] | ( (((unsigned char*)&M041)[0]) << 1 ));
    modarray_rd[15] = (modarray_rd[15] | ( (((unsigned char*)&M042)[0]) << 2 ));
  modarray_rd[15] = (modarray_rd[15] | ( (((unsigned char*)&M043)[0]) << 3 ));
  modarray_rd[15] = (modarray_rd[15] | ( (((unsigned char*)&M044)[0]) << 4 ));
  modarray_rd[15] = (modarray_rd[15] | ( (((unsigned char*)&M045)[0]) << 5 ));
    modarray_rd[15] = (modarray_rd[15] | ( (((unsigned char*)&M046)[0]) << 6 ));
    modarray_rd[15] = (modarray_rd[15] | ( (((unsigned char*)&M047)[0]) << 7 ));

        ALL_Data[7] = ( modarray_rd[15]);
        ALL_Data[7] = ( ALL_Data[7] << 8 | modarray_rd[14]);

    /************************ Memory Bits 48 to 63 *************************/  //16
  modarray_rd[16] =                     (((unsigned char*)&M048)[0]);
    modarray_rd[16] = (modarray_rd[16] | ( (((unsigned char*)&M049)[0]) << 1 ));
  modarray_rd[16] = (modarray_rd[16] | ( (((unsigned char*)&M050)[0]) << 2 ));
  modarray_rd[16] = (modarray_rd[16] | ( (((unsigned char*)&M051)[0]) << 3 ));
  modarray_rd[16] = (modarray_rd[16] | ( (((unsigned char*)&M052)[0]) << 4 ));
    modarray_rd[16] = (modarray_rd[16] | ( (((unsigned char*)&M053)[0]) << 5 ));
  modarray_rd[16] = (modarray_rd[16] | ( (((unsigned char*)&M054)[0]) << 6 ));
  modarray_rd[16] = (modarray_rd[16] | ( (((unsigned char*)&M055)[0]) << 7 ));

  modarray_rd[17] = ( ( (((unsigned char*)&M056)[0]) << 0 ));
    modarray_rd[17] = (modarray_rd[17] | ( (((unsigned char*)&M057)[0]) << 1 ));
    modarray_rd[17] = (modarray_rd[17] | ( (((unsigned char*)&M058)[0]) << 2 ));
  modarray_rd[17] = (modarray_rd[17] | ( (((unsigned char*)&M059)[0]) << 3 ));
  modarray_rd[17] = (modarray_rd[17] | ( (((unsigned char*)&M060)[0]) << 4 ));
  modarray_rd[17] = (modarray_rd[17] | ( (((unsigned char*)&M061)[0]) << 5 ));
    modarray_rd[17] = (modarray_rd[17] | ( (((unsigned char*)&M062)[0]) << 6 ));
    modarray_rd[17] = (modarray_rd[17] | ( (((unsigned char*)&M063)[0]) << 7 ));

        ALL_Data[8] = ( modarray_rd[17]);
        ALL_Data[8] = ( ALL_Data[8] << 8 | modarray_rd[16]);

            /************************ Memory Bits 64 to 79 *************************/  //17
  modarray_rd[18] =                     (((unsigned char*)&M064)[0]);                        //1
    modarray_rd[18] = (modarray_rd[18] | ( (((unsigned char*)&M065)[0]) << 1 ));  //2
  modarray_rd[18] = (modarray_rd[18] | ( (((unsigned char*)&M066)[0]) << 2 ));    //4
  modarray_rd[18] = (modarray_rd[18] | ( (((unsigned char*)&M067)[0]) << 3 ));    //8
  modarray_rd[18] = (modarray_rd[18] | ( (((unsigned char*)&M068)[0]) << 4 ));    //
    modarray_rd[18] = (modarray_rd[18] | ( (((unsigned char*)&M069)[0]) << 5 ));
  modarray_rd[18] = (modarray_rd[18] | ( (((unsigned char*)&M070)[0]) << 6 ));
  modarray_rd[18] = (modarray_rd[18] | ( (((unsigned char*)&M071)[0]) << 7 ));

  modarray_rd[19] = ( ( (((unsigned char*)&M072)[0]) << 0 ));
    modarray_rd[19] = (modarray_rd[19] | ( (((unsigned char*)&M073)[0]) << 1 ));
    modarray_rd[19] = (modarray_rd[19] | ( (((unsigned char*)&M074)[0]) << 2 ));
  modarray_rd[19] = (modarray_rd[19] | ( (((unsigned char*)&M075)[0]) << 3 ));
  modarray_rd[19] = (modarray_rd[19] | ( (((unsigned char*)&M076)[0]) << 4 ));
  modarray_rd[19] = (modarray_rd[19] | ( (((unsigned char*)&M077)[0]) << 5 ));
    modarray_rd[19] = (modarray_rd[19] | ( (((unsigned char*)&M078)[0]) << 6 ));
    modarray_rd[19] = (modarray_rd[19] | ( (((unsigned char*)&M079)[0]) << 7 ));

        ALL_Data[9] = ( modarray_rd[19]);
        ALL_Data[9] = ( ALL_Data[9]  << 8 | modarray_rd[18]);

    /************************ Memory Bits 80 to 95 *************************/      //18
  modarray_rd[20] =                     (((unsigned char*)&M080)[0]);
    modarray_rd[20] = (modarray_rd[20] | ( (((unsigned char*)&M081)[0]) << 1 ));
  modarray_rd[20] = (modarray_rd[20] | ( (((unsigned char*)&M082)[0]) << 2 ));
  modarray_rd[20] = (modarray_rd[20] | ( (((unsigned char*)&M083)[0]) << 3 ));
  modarray_rd[20] = (modarray_rd[20] | ( (((unsigned char*)&M084)[0]) << 4 ));
    modarray_rd[20] = (modarray_rd[20] | ( (((unsigned char*)&M085)[0]) << 5 ));
  modarray_rd[20] = (modarray_rd[20] | ( (((unsigned char*)&M086)[0]) << 6 ));
  modarray_rd[20] = (modarray_rd[20] | ( (((unsigned char*)&M087)[0]) << 7 ));

  modarray_rd[21] = (( (((unsigned char*)&M088)[0]) << 0 ));
    modarray_rd[21] = (modarray_rd[21] | ( (((unsigned char*)&M089)[0]) << 1 ));
    modarray_rd[21] = (modarray_rd[21] | ( (((unsigned char*)&M090)[0]) << 2 ));
  modarray_rd[21] = (modarray_rd[21] | ( (((unsigned char*)&M091)[0]) << 3 ));
  modarray_rd[21] = (modarray_rd[21] | ( (((unsigned char*)&M092)[0]) << 4 ));
  modarray_rd[21] = (modarray_rd[21] | ( (((unsigned char*)&M093)[0]) << 5 ));
    modarray_rd[21] = (modarray_rd[21] | ( (((unsigned char*)&M094)[0]) << 6 ));
    modarray_rd[21] = (modarray_rd[21] | ( (((unsigned char*)&M095)[0]) << 7 ));

        ALL_Data[10]  = ( modarray_rd[21]);
        ALL_Data[10]  = ( ALL_Data[10]  << 8 | modarray_rd[20]);

    /****************************** DIO Modbus ********************************************/
    //DIO_modarray_wr[0] = O006;
    //DIO_modarray_wr[0] = (DIO_modarray_wr[0]| ( (((O007) << 1 ))));
    //DIO_modarray_wr[0] = (DIO_modarray_wr[0]| ( (((O008) << 2 ))));
    //DIO_modarray_wr[0] = (DIO_modarray_wr[0]| ( (((O009) << 3 ))));

/************************* Modbus Output Registers  ******************************************/
modarray_rd[22] = ((unsigned char*)&(Actual_Feedrate))[1];   //Actual Feedrate 19
modarray_rd[23] = ((unsigned char*)&(Actual_Feedrate))[0];     //Actual Feedrate 19

ALL_Data[11]= Actual_Feedrate;

modarray_rd[24] = ((unsigned char*)&(Totaliser1_Display))[3];  //Totaliser2    20
modarray_rd[25] = ((unsigned char*)&(Totaliser1_Display))[2];     //Totaliser2    20
modarray_rd[26] = ((unsigned char*)&(Totaliser1_Display))[1];  //Totaliser2    21
modarray_rd[27] = ((unsigned char*)&(Totaliser1_Display))[0];     //Totaliser2    21

ALL_Data[12]= ((uint16_t*)&(Totaliser1_Display))[1];     //Totaliser2    20;
ALL_Data[13]= ((uint16_t*)&(Totaliser1_Display))[0];     //Totaliser2    20;

modarray_rd[28] = ((unsigned char*)&(Totaliser2_Display))[3];  //Totaliser2    22
modarray_rd[29] = ((unsigned char*)&(Totaliser2_Display))[2];     //Totaliser2    22
modarray_rd[30] = ((unsigned char*)&(Totaliser2_Display))[1];  //Totaliser2    23
modarray_rd[31] = ((unsigned char*)&(Totaliser2_Display))[0];     //Totaliser2    23

ALL_Data[14]= ((uint16_t*)&(Totaliser2_Display))[1];     //Totaliser2    20;
ALL_Data[15]= ((uint16_t*)&(Totaliser2_Display))[0];     //Totaliser2    20;

if(Deviation_Sign == Positive)
    {
        modarray_rd[32] = ((unsigned char*)&(Deviation))[1];                      //Deviation   24 (two's complement)
        modarray_rd[33] = ((unsigned char*)&(Deviation))[0];                        //Deviation   24
    }
if(Deviation_Sign == Negative)
    {
        intdeviation=(int16_t)((int16_t)0x0000-(int16_t)Deviation);
        // intdeviation = -(int16_t)Deviation;
        //modarray_rd[32] = (unsigned char)((intdeviation >> 8) & 0xFF);

        // Mask to get the Low Byte (LSB)
        //modarray_rd[33] = (unsigned char)(intdeviation & 0xFF);
        modarray_rd[32] = ((unsigned char*)&intdeviation)[1]; //Deviation   24 (two's complement)
        modarray_rd[33] = ((unsigned char*)&intdeviation)[0]; //Deviation   24 (two's complement)
    }

modarray_rd[34] = ((unsigned char*)&(Current_Setpoint))[1]; //Current Setpoint 25
modarray_rd[35] = ((unsigned char*)&(Current_Setpoint))[0]; //Current Setpoint 25

if(Display_Sign == Positive)
modarray_rd[36] = ((unsigned char*)&(Belt_Load))[1];                       //Belt Load 26
if(Display_Sign == Negative)
modarray_rd[36] = ((((unsigned char*)&(Belt_Load))[1]) | 0x80);  //Belt Load 26
modarray_rd[37] = ((unsigned char*)&(Belt_Load))[0];                       //Belt Load 26

if((SIO_Interface==2)||(SIO_Interface==3))/*Added by vishal on 21/08/2018 for including -ve sign info of BL & Deviation.Used for modbus TCP and Profibus DP */
    {
        Deviation_Comm =                 modarray_rd[32];
        Deviation_Comm = ( Deviation_Comm << 8 | modarray_rd[33]);
        ALL_Data[16] =Deviation_Comm;
        ALL_Data[17] =Current_Setpoint;
        Belt_Load_Comm = modarray_rd[36];
        Belt_Load_Comm = (Belt_Load_Comm << 8 | modarray_rd[37]);
        ALL_Data[18] =Belt_Load_Comm ;
        }

modarray_rd[38] = ((unsigned char*)&(Belt_Speed))[1]; //Tacho 27
modarray_rd[39] = ((unsigned char*)&(Belt_Speed))[0];    //Tacho 27

    ALL_Data[19]=Belt_Speed;

modarray_rd[40] = ((unsigned char*)&(WF_Drive_Setpoint_per))[1];  //WF_Drive_Setpoint_per 28
modarray_rd[41] = ((unsigned char*)&(WF_Drive_Setpoint_per))[0];    //WF_Drive_Setpoint_per 28

    ALL_Data[20]=WF_Drive_Setpoint_per;

modarray_rd[42] = ((unsigned char*)&(PF_Drive_Setpoint_Per))[1];  //PF_Drive_Setpoint_per 29  Higher byte
modarray_rd[43] = ((unsigned char*)&(PF_Drive_Setpoint_Per))[0];    //PF_Drive_Setpoint_per 29 Lower byte

    ALL_Data[21]=PF_Drive_Setpoint_Per;

/*Hopper,Postn,HopToatl,FDR_Total,CM_Diff,CM%*/
modarray_rd[44] = ((unsigned char*)&(Hopper_val))[2];  //Hopper Load 30  Higher byte
modarray_rd[45] = ((unsigned char*)&(Hopper_val))[1];    //Hopper Load 30 Lower byte
modarray_rd[46] = ((unsigned char*)&(Hopper_val))[0];    //Hopper Load 31 Lower byte

ALL_Data[22]= ((uint16_t*)&(Hopper_val))[1];     //Totaliser2    20;
ALL_Data[23]= ((uint16_t*)&(Hopper_val))[0];     //Totaliser2    20;

modarray_rd[47] = ((unsigned char*)&(Gate_Postion))[1];  //Gate Postn 32  Higher byte
modarray_rd[48] = ((unsigned char*)&(Gate_Postion))[0];    //Gate Postn 32 Lower byte

ALL_Data[24]= Gate_Postion;

modarray_rd[49] = ((unsigned char*)&(HOpper_total))[2];  //Hopper Total 33  Higher byte
modarray_rd[50] = ((unsigned char*)&(HOpper_total))[1];    //Hopper Total 33 Lower byte
modarray_rd[51] = ((unsigned char*)&(HOpper_total))[0];    //Hopper Total 34 Lower byte

ALL_Data[25]= ((uint16_t*)&(HOpper_total))[1];     //Totaliser2    20;
ALL_Data[26]= ((uint16_t*)&(HOpper_total))[0];     //Totaliser2    20;

modarray_rd[52] = ((unsigned char*)&(FDR_Total))[2];  //Feeder Total 35  Higher byte
modarray_rd[53] = ((unsigned char*)&(FDR_Total))[1];    //Feeder Total 35 Lower byte
modarray_rd[54] = ((unsigned char*)&(FDR_Total))[0];    //Feeder Total 36 Lower byte

ALL_Data[27]= ((uint16_t*)&(FDR_Total))[1];     //Totaliser2    20;
ALL_Data[28]= ((uint16_t*)&(FDR_Total))[0];     //Totaliser2    20;

modarray_rd[55] = ((unsigned char*)&(CM_Difference))[1];  //CM Difference 37  Higher byte
modarray_rd[56] = ((unsigned char*)&(CM_Difference))[0];    //CM Difference 37 Lower byte

ALL_Data[29]= CM_Difference;

modarray_rd[57] = ((unsigned char*)&(CM_Display))[1];  //CM% 38  Higher byte
modarray_rd[58] = ((unsigned char*)&(CM_Display))[0];    //CM% 38 Lower byte

ALL_Data[30]= CM_Display;

modarray_rd[59] = ((unsigned char*)&(Inv_Op_Freq))[1];  //Inv_Op_Freq xx  Higher byte
modarray_rd[60] = ((unsigned char*)&(Inv_Op_Freq))[0];    //Inv_Op_Freq xx  Lower byte

ALL_Data[31]= Inv_Op_Freq;

modarray_rd[61] = ((unsigned char*)&(Inv_Op_current))[1];  //Inv_Op_current xx  Higher byte
modarray_rd[62] = ((unsigned char*)&(Inv_Op_current))[0];    //Inv_Op_current xx Lower byte

ALL_Data[32]= Inv_Op_current;

modarray_rd[63] = ((unsigned char*)&(Inv1_Op_Freq))[1];  //Inv1_Op_Freq xx  Higher byte
modarray_rd[64] = ((unsigned char*)&(Inv1_Op_Freq))[0];    //Inv1_Op_Freq xx Lower byte

ALL_Data[33]= Inv1_Op_Freq;

modarray_rd[65] = ((unsigned char*)&(Inv1_Op_current))[1];  //Inv1_Op_current xx  Higher byte
modarray_rd[66] = ((unsigned char*)&(Inv1_Op_current))[0];    //Inv1_Op_current xx Lower byte

ALL_Data[34]= Inv1_Op_current;

{
    uint16_t err_latch_lo = ErrorLatch_GetBitmapLow();   /* bit0->Err1 ... bit15->Err16 */
    uint16_t err_latch_hi = ErrorLatch_GetBitmapHigh();  /* bit0->Err17 */
    modarray_rd[72] = (unsigned char)(err_latch_lo >> 8);  /* FC04 reg 36 */
    modarray_rd[73] = (unsigned char)(err_latch_lo & 0xFF);/* FC04 reg 36 */
    modarray_rd[74] = (unsigned char)(err_latch_hi >> 8);  /* FC04 reg 37 */
    modarray_rd[75] = (unsigned char)(err_latch_hi & 0xFF);/* FC04 reg 37 */
}

/************************************************************************************/

/********************** Write Address Starts From 0*****************************************/

if(SIO_Interface==1)
{((unsigned char*)&Common_Setpoint)[1] = modarray_wr[0];            //communication setpoint at Register 0
 ((unsigned char*)&Common_Setpoint)[0] = modarray_wr[1];
}

//    /*Execute only if Modbus-Tcp or Profibus DP slected */
//    if((SIO_Interface==2)||(SIO_Interface==3))/*add selection parameter for RS485 / Modbus-tcp/Profibus-DP*/
//    {
//        modarray_wr[3] = M0_15_Read & 0x00FF;
//        modarray_wr[4] = (M0_15_Read >> 8) & 0x00FF;
//
//        modarray_wr[5] = M16_31_Read & 0x00FF;
//        modarray_wr[6] = (M16_31_Read >> 8) & 0x00FF;
//
//        modarray_wr[7] = M32_39_Read & 0x00FF;
//    }
//
//
//(((unsigned char*)&M000)[0]) = (modarray_wr[3] & 0x01) ;
//(((unsigned char*)&M001)[0]) = ((modarray_wr[3] >> 1) & 0x01 ) ;
//(((unsigned char*)&M002)[0]) = ((modarray_wr[3] >> 2) & 0x01 ) ;
//(((unsigned char*)&M003)[0]) = ((modarray_wr[3] >> 3) & 0x01 ) ;
//
//(((unsigned char*)&M004)[0]) = ((modarray_wr[3] >> 4) & 0x01 ) ;
//(((unsigned char*)&M005)[0]) = ((modarray_wr[3] >> 5) & 0x01 ) ;
//(((unsigned char*)&M006)[0]) = ((modarray_wr[3] >> 6) & 0x01 ) ;
//(((unsigned char*)&M007)[0]) = ((modarray_wr[3] >> 7) & 0x01 ) ;

///**************************  After ************************************/
//(((unsigned char*)&M008)[0]) = (modarray_wr[4] & 0x01) ;
//(((unsigned char*)&M009)[0]) = ((modarray_wr[4] >> 1) & 0x01 ) ;
//(((unsigned char*)&M010)[0]) = ((modarray_wr[4] >> 2) & 0x01 ) ;
//(((unsigned char*)&M011)[0]) = ((modarray_wr[4] >> 3) & 0x01 ) ;
//
//(((unsigned char*)&M012)[0]) = ((modarray_wr[4] >> 4) & 0x01 ) ;
//(((unsigned char*)&M013)[0]) = ((modarray_wr[4] >> 5) & 0x01 ) ;
//(((unsigned char*)&M014)[0]) = ((modarray_wr[4] >> 6) & 0x01 ) ;
//(((unsigned char*)&M015)[0]) = ((modarray_wr[4] >> 7) & 0x01 ) ;

//(((unsigned char*)&M016)[0]) = (modarray_wr[5] & 0x01) ;
//(((unsigned char*)&M017)[0]) = ((modarray_wr[5] >> 1) & 0x01 ) ;
//(((unsigned char*)&M018)[0]) = ((modarray_wr[5] >> 2) & 0x01 ) ;
//(((unsigned char*)&M019)[0]) = ((modarray_wr[5] >> 3) & 0x01 ) ;

//(((unsigned char*)&M020)[0]) = ((modarray_wr[5] >> 4) & 0x01 ) ;
//(((unsigned char*)&M021)[0]) = ((modarray_wr[5] >> 5) & 0x01 ) ;
//(((unsigned char*)&M022)[0]) = ((modarray_wr[5] >> 6) & 0x01 ) ;
//(((unsigned char*)&M023)[0]) = ((modarray_wr[5] >> 7) & 0x01 ) ;

//(((unsigned char*)&M024)[0]) = (modarray_wr[6] & 0x01) ;
//(((unsigned char*)&M025)[0]) = ((modarray_wr[6] >> 1) & 0x01 ) ;
//(((unsigned char*)&M026)[0]) = ((modarray_wr[6] >> 2) & 0x01 ) ;
//(((unsigned char*)&M027)[0]) = ((modarray_wr[6] >> 3) & 0x01 ) ;
//
//(((unsigned char*)&M028)[0]) = ((modarray_wr[6] >> 4) & 0x01 ) ;
//(((unsigned char*)&M029)[0]) = ((modarray_wr[6] >> 5) & 0x01 ) ;
//(((unsigned char*)&M030)[0]) = ((modarray_wr[6] >> 6) & 0x01 ) ;
//(((unsigned char*)&M031)[0]) = ((modarray_wr[6] >> 7) & 0x01 ) ;

//(((unsigned char*)&M032)[0]) = (modarray_wr[7] & 0x01) ;
//(((unsigned char*)&M033)[0]) = ((modarray_wr[7] >> 1) & 0x01 ) ;
//(((unsigned char*)&M034)[0]) = ((modarray_wr[7] >> 2) & 0x01 ) ;
//(((unsigned char*)&M035)[0]) = ((modarray_wr[7] >> 3) & 0x01 ) ;

//(((unsigned char*)&M036)[0]) = ((modarray_wr[7] >> 4) & 0x01 ) ;
//(((unsigned char*)&M037)[0]) = ((modarray_wr[7] >> 5) & 0x01 ) ;
//(((unsigned char*)&M038)[0]) = ((modarray_wr[7] >> 6) & 0x01 ) ;
//(((unsigned char*)&M039)[0]) = ((modarray_wr[7] >> 7) & 0x01 ) ;
/************************************************************************************/

 }

 /*
 07/02/2018
 Updated hoppee Related values displayed in Online Screen 3
 Added Memory input and output bits;30-38
 Added Inv Digital inputs;10
 Updated adress ALL

 14/08/2018

 Added modbus Tcp & profibus DP varaiables
 :

 Read Access:
             M0_15_Read,
            M16_31_Read,
            M32_39_Read;

 Write Access:
            I0_15_Write,
            I16_31_Write,
            I32_47_Write,
            O0_15_Write,
            O16_31_Write,
            M0_15_Write,
            M16_31_Write,
            M32_47_Write,
            M48_63_Write,
            M64_79_Write,
            M80_95_Write,
            Deviation_Comm,
            Belt_Load_Comm;

 if((SIO_Interface==2)||(SIO_Interface==3))  //add selection parameter for RS485 / Modbus-tcp/Profibus-DP

 */

//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>

//#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
//#include "LPC17xx.h"                    // Device header
//#include "lpc17xx_spi.h"
//#include "lpc17xx_gpio.h"
//#include "GPIO_LPC17xx.h"
//#include "lpc17xx_rtc.h"
//#include "lpc17xx_timer.h"
//#include "lpc17xx_uart.h"
//#include "main_routine.h"
//#include "OLED_2_7_12864.h"
//#include "functions.h"
//#include "parameters.h"  //#include "menu.h"
//#include "timer.h"
//#include "eeprom.h"
//#include "adc.h"
//#include "dac.h"
//#include "spi.h"
//#include "plc.h"
//#include "io_control.h"
//#include "modbus.h"
//#include "keys.h"

////volatile unsigned char modarray_rd[MAX_LENGTH],recipe[MAX_LENGTH];
//extern volatile char DIO_modarray_rd[2],modarray_rd[MAX_LENGTH],modarray_wr[MAX_LENGTH],DIO_modarray_wr[2];;

//extern uint8_t ram_values[RAM_SIZE];
//extern unsigned int Common_Setpoint;
//extern unsigned int Actual_Feedrate;

//extern unsigned char Display_Sign,Deviation_Sign;
//extern unsigned int External_Setpoint,
//                                        Common_Setpoint,
//                                        Current_Setpoint,
//                                        Deviation,
//                                        PF_Drive_Setpoint,
//                                        WF_Drive_Setpoint_per,
//                                        Belt_Speed,
//                                        PF_Drive_Setpoint_Per,
//                                        Hopper_val,
//                                        Gate_Postion,
//                                        HOpper_total,
//                                        FDR_Total,
//                                        CM_Difference;

//extern double Totaliser1,Totaliser2;
//extern unsigned int Totaliser1_Display,Totaliser2_Display;
//extern unsigned int Common_Setpoint;
//extern signed int Belt_Load;
//extern float WF_Drive_Setpoint,CM_Display;

// void Modbus_Registers()
// {
//
///********************** Read Address Starts From 8*****************************************/
///*************************** Digital Inputs 0 to 15 ***********************/ //8 : Read Adreess
//  modarray_rd[0] =  ( ((unsigned char*)&I000)[0] );
//    modarray_rd[0] = (modarray_rd[0] | ( (((unsigned char*)&I001)[0]) << 1 ));
//  modarray_rd[0] = (modarray_rd[0] | ( (((unsigned char*)&I002)[0]) << 2 ));
//  modarray_rd[0] = (modarray_rd[0] | ( (((unsigned char*)&I003)[0]) << 3 ));
//  modarray_rd[0] = (modarray_rd[0] | ( (((unsigned char*)&I004)[0]) << 4 ));
//    modarray_rd[0] = (modarray_rd[0] | ( (((unsigned char*)&I005)[0]) << 5 ));
//  modarray_rd[0] = (modarray_rd[0] | ( (((unsigned char*)&I006)[0]) << 6 ));
//  modarray_rd[0] = (modarray_rd[0] | ( (((unsigned char*)&I007)[0]) << 7 ));
//
//  modarray_rd[1] = (modarray_rd[1] | ( (((unsigned char*)&I008)[0]) << 0 ));
//    modarray_rd[1] = (modarray_rd[1] | ( (((unsigned char*)&I009)[0]) << 1 ));
//    modarray_rd[1] = (modarray_rd[1] | ( (((unsigned char*)&I010)[0]) << 2 ));
//  modarray_rd[1] = (modarray_rd[1] | ( (((unsigned char*)&I011)[0]) << 3 ));
//  modarray_rd[1] = (modarray_rd[1] | ( (((unsigned char*)&I012)[0]) << 4 ));
//  modarray_rd[1] = (modarray_rd[1] | ( (((unsigned char*)&I013)[0]) << 5 ));
//    modarray_rd[1] = (modarray_rd[1] | ( (((unsigned char*)&I014)[0]) << 6 ));
//    modarray_rd[1] = (modarray_rd[1] | ( (((unsigned char*)&I015)[0]) << 7 ));
//
//    /************************** Digital Inputs 16 to 31***********************/  //9
//    modarray_rd[2] =  ( ((unsigned char*)&I016)[0] );
//  modarray_rd[2] = (modarray_rd[2] | ( (((unsigned char*)&I017)[0]) << 1 ));
//  modarray_rd[2] = (modarray_rd[2] | ( (((unsigned char*)&I018)[0]) << 2 ));
//  modarray_rd[2] = (modarray_rd[2] | ( (((unsigned char*)&I019)[0]) << 3 ));
//    modarray_rd[2] = (modarray_rd[2] | ( (((unsigned char*)&I020)[0]) << 4 ));
//  modarray_rd[2] = (modarray_rd[2] | ( (((unsigned char*)&I021)[0]) << 5 ));
//  modarray_rd[2] = (modarray_rd[2] | ( (((unsigned char*)&I022)[0]) << 6 ));
//  modarray_rd[2] = (modarray_rd[2] | ( (((unsigned char*)&I023)[0]) << 7 ));
//
//    modarray_rd[3] = (modarray_rd[3] | ( (((unsigned char*)&I024)[0]) << 0 ));
//    modarray_rd[3] = (modarray_rd[3] | ( (((unsigned char*)&I025)[0]) << 1 ));
//  modarray_rd[3] = (modarray_rd[3] | ( (((unsigned char*)&I026)[0]) << 2 ));
//  modarray_rd[3] = (modarray_rd[3] | ( (((unsigned char*)&I027)[0]) << 3 ));
//  modarray_rd[3] = (modarray_rd[3] | ( (((unsigned char*)&I028)[0]) << 4 ));
//    modarray_rd[3] = (modarray_rd[3] | ( (((unsigned char*)&I029)[0]) << 5 ));
//    modarray_rd[3] = (modarray_rd[3] | ( (((unsigned char*)&I030)[0]) << 6 ));
//    modarray_rd[3] = (modarray_rd[3] | ( (((unsigned char*)&I031)[0]) << 7 ));
//
//        /************************** Digital Inputs 32 to 39***********************/  //10
//    modarray_rd[4] =  ( ((unsigned char*)&I032)[0] );
//  modarray_rd[4] = (modarray_rd[4] | ( (((unsigned char*)&I033)[0]) << 1 ));
//  modarray_rd[4] = (modarray_rd[4] | ( (((unsigned char*)&I034)[0]) << 2 ));
//  modarray_rd[4] = (modarray_rd[4] | ( (((unsigned char*)&I035)[0]) << 3 ));
//    modarray_rd[4] = (modarray_rd[4] | ( (((unsigned char*)&I036)[0]) << 4 ));
//  modarray_rd[4] = (modarray_rd[4] | ( (((unsigned char*)&I037)[0]) << 5 ));
//  modarray_rd[4] = (modarray_rd[4] | ( (((unsigned char*)&I038)[0]) << 6 ));
//  modarray_rd[4] = (modarray_rd[4] | ( (((unsigned char*)&I039)[0]) << 7 ));
//
//    /*modarray_rd[5] = (modarray_rd[5] | ( (((unsigned char*)&I024)[0]) << 0 ));
//    modarray_rd[5] = (modarray_rd[5] | ( (((unsigned char*)&I025)[0]) << 1 ));
//  modarray_rd[5] = (modarray_rd[5] | ( (((unsigned char*)&I026)[0]) << 2 ));
//  modarray_rd[5] = (modarray_rd[5] | ( (((unsigned char*)&I027)[0]) << 3 ));
//  modarray_rd[5] = (modarray_rd[5] | ( (((unsigned char*)&I028)[0]) << 4 ));
//    modarray_rd[5] = (modarray_rd[5] | ( (((unsigned char*)&I029)[0]) << 5 ));
//    modarray_rd[5] = (modarray_rd[5] | ( (((unsigned char*)&I030)[0]) << 6 ));
//    modarray_rd[5] = (modarray_rd[5] | ( (((unsigned char*)&I031)[0]) << 7 ));*/
//    /************************ Digital Outputs 0 to 15 *************************/ //11
//  modarray_rd[6] =  ( ((unsigned char*)&O000)[0] );
//    modarray_rd[6] = (modarray_rd[6] | ( (((unsigned char*)&O001)[0]) << 1 ));
//  modarray_rd[6] = (modarray_rd[6] | ( (((unsigned char*)&O002)[0]) << 2 ));
//  modarray_rd[6] = (modarray_rd[6] | ( (((unsigned char*)&O003)[0]) << 3 ));
//  modarray_rd[6] = (modarray_rd[6] | ( (((unsigned char*)&O004)[0]) << 4 ));
//    modarray_rd[6] = (modarray_rd[6] | ( (((unsigned char*)&O005)[0]) << 5 ));
//  modarray_rd[6] = (modarray_rd[6] | ( (((unsigned char*)&O006)[0]) << 6 ));
//  //modarray_rd[6] = (modarray_rd[6] | ( (((unsigned char*)&O007)[0]) << 7 ));
//
//  modarray_rd[7] = (modarray_rd[7] | ( (((unsigned char*)&O008)[0]) << 0 ));
//    modarray_rd[7] = (modarray_rd[7] | ( (((unsigned char*)&O009)[0]) << 1 ));
//    modarray_rd[7] = (modarray_rd[7] | ( (((unsigned char*)&O010)[0]) << 2 ));
//  modarray_rd[7] = (modarray_rd[7] | ( (((unsigned char*)&O011)[0]) << 3 ));
//  modarray_rd[7] = (modarray_rd[7] | ( (((unsigned char*)&O012)[0]) << 4 ));
//  modarray_rd[7] = (modarray_rd[7] | ( (((unsigned char*)&O013)[0]) << 5 ));
//    modarray_rd[7] = (modarray_rd[7] | ( (((unsigned char*)&O014)[0]) << 6 ));
//    modarray_rd[7] = (modarray_rd[7] | ( (((unsigned char*)&O015)[0]) << 7 ));
//
//        /************************ Digital Outputs 15 to 31 *************************/ //12
//  modarray_rd[8] =  ( ((unsigned char*)&O000)[0] );
//    modarray_rd[8] = (modarray_rd[8] | ( (((unsigned char*)&O016)[0]) << 1 ));
//  modarray_rd[8] = (modarray_rd[8] | ( (((unsigned char*)&O017)[0]) << 2 ));
//  modarray_rd[8] = (modarray_rd[8] | ( (((unsigned char*)&O018)[0]) << 3 ));
//  modarray_rd[8] = (modarray_rd[8] | ( (((unsigned char*)&O019)[0]) << 4 ));
//    modarray_rd[8] = (modarray_rd[8] | ( (((unsigned char*)&O020)[0]) << 5 ));
//  modarray_rd[8] = (modarray_rd[8] | ( (((unsigned char*)&O021)[0]) << 6 ));
//  modarray_rd[6] = (modarray_rd[8] | ( (((unsigned char*)&O022)[0]) << 7 ));
//
//  modarray_rd[9] = (modarray_rd[9] | ( (((unsigned char*)&O023)[0]) << 0 ));
//    modarray_rd[9] = (modarray_rd[9] | ( (((unsigned char*)&O024)[0]) << 1 ));
//    modarray_rd[9] = (modarray_rd[9] | ( (((unsigned char*)&O025)[0]) << 2 ));
//  modarray_rd[9] = (modarray_rd[9] | ( (((unsigned char*)&O026)[0]) << 3 ));
//  modarray_rd[9] = (modarray_rd[9] | ( (((unsigned char*)&O027)[0]) << 4 ));
//  modarray_rd[9] = (modarray_rd[9] | ( (((unsigned char*)&O028)[0]) << 5 ));
//    modarray_rd[9] = (modarray_rd[9] | ( (((unsigned char*)&O029)[0]) << 6 ));
//    modarray_rd[9] = (modarray_rd[9] | ( (((unsigned char*)&O030)[0]) << 7 ));
//    /************************ Memory Bits 00 to 15 *************************/  //13
//  modarray_rd[10] =  ( ((unsigned char*)&M000)[0] );
//    modarray_rd[10] = (modarray_rd[10] | ( (((unsigned char*)&M001)[0]) << 1 ));
//  modarray_rd[10] = (modarray_rd[10] | ( (((unsigned char*)&M002)[0]) << 2 ));
//  modarray_rd[10] = (modarray_rd[10] | ( (((unsigned char*)&M003)[0]) << 3 ));
//  modarray_rd[10] = (modarray_rd[10] | ( (((unsigned char*)&M004)[0]) << 4 ));
//    modarray_rd[10] = (modarray_rd[10] | ( (((unsigned char*)&M005)[0]) << 5 ));
//  modarray_rd[10] = (modarray_rd[10] | ( (((unsigned char*)&M006)[0]) << 6 ));
//  modarray_rd[10] = (modarray_rd[10] | ( (((unsigned char*)&M007)[0]) << 7 ));
//
//  modarray_rd[11] = (modarray_rd[11] | ( (((unsigned char*)&M008)[0]) << 0 ));
//    modarray_rd[11] = (modarray_rd[11] | ( (((unsigned char*)&M009)[0]) << 1 ));
//    modarray_rd[11] = (modarray_rd[11] | ( (((unsigned char*)&M010)[0]) << 2 ));
//  modarray_rd[11] = (modarray_rd[11] | ( (((unsigned char*)&M011)[0]) << 3 ));
//  modarray_rd[11] = (modarray_rd[11] | ( (((unsigned char*)&M012)[0]) << 4 ));
//  modarray_rd[11] = (modarray_rd[11] | ( (((unsigned char*)&M013)[0]) << 5 ));
//    modarray_rd[11] = (modarray_rd[11] | ( (((unsigned char*)&M014)[0]) << 6 ));
//    modarray_rd[11] = (modarray_rd[11] | ( (((unsigned char*)&M015)[0]) << 7 ));
//
//    /************************ Memory Bits 16 to 31 *************************/   //14
//  modarray_rd[12] =  (((unsigned char*)&M016)[0]);
//    modarray_rd[12] = (modarray_rd[12] | ( (((unsigned char*)&M017)[0]) << 1 ));
//  modarray_rd[12] = (modarray_rd[12] | ( (((unsigned char*)&M018)[0]) << 2 ));
//  modarray_rd[12] = (modarray_rd[12] | ( (((unsigned char*)&M019)[0]) << 3 ));
//  modarray_rd[12] = (modarray_rd[12] | ( (((unsigned char*)&M020)[0]) << 4 ));
//    modarray_rd[12] = (modarray_rd[12] | ( (((unsigned char*)&M021)[0]) << 5 ));
//  modarray_rd[12] = (modarray_rd[12] | ( (((unsigned char*)&M022)[0]) << 6 ));
//  modarray_rd[12] = (modarray_rd[12] | ( (((unsigned char*)&M023)[0]) << 7 ));
//
//  modarray_rd[13] = (modarray_rd[13] | ( (((unsigned char*)&M024)[0]) << 0 ));
//    modarray_rd[13] = (modarray_rd[13] | ( (((unsigned char*)&M025)[0]) << 1 ));
//    modarray_rd[13] = (modarray_rd[13] | ( (((unsigned char*)&M026)[0]) << 2 ));
//  modarray_rd[13] = (modarray_rd[13] | ( (((unsigned char*)&M027)[0]) << 3 ));
//  modarray_rd[13] = (modarray_rd[13] | ( (((unsigned char*)&M028)[0]) << 4 ));
//  modarray_rd[13] = (modarray_rd[13] | ( (((unsigned char*)&M029)[0]) << 5 ));
//    modarray_rd[13] = (modarray_rd[13] | ( (((unsigned char*)&M030)[0]) << 6 ));
//    modarray_rd[13] = (modarray_rd[13] | ( (((unsigned char*)&M031)[0]) << 7 ));
//
//        /************************ Memory Bits 32 to 47 *************************/  //15
//  modarray_rd[14] =                     (((unsigned char*)&M032)[0]);
//    modarray_rd[14] = (modarray_rd[14] | ( (((unsigned char*)&M033)[0]) << 1 ));
//  modarray_rd[14] = (modarray_rd[14] | ( (((unsigned char*)&M034)[0]) << 2 ));
//  modarray_rd[14] = (modarray_rd[14] | ( (((unsigned char*)&M035)[0]) << 3 ));
//  modarray_rd[14] = (modarray_rd[14] | ( (((unsigned char*)&M036)[0]) << 4 ));
//    modarray_rd[14] = (modarray_rd[14] | ( (((unsigned char*)&M037)[0]) << 5 ));
//  modarray_rd[14] = (modarray_rd[14] | ( (((unsigned char*)&M038)[0]) << 6 ));
//  modarray_rd[14] = (modarray_rd[14] | ( (((unsigned char*)&M039)[0]) << 7 ));
//
//  modarray_rd[15] = (modarray_rd[15] | ( (((unsigned char*)&M040)[0]) << 0 ));
//    modarray_rd[15] = (modarray_rd[15] | ( (((unsigned char*)&M041)[0]) << 1 ));
//    modarray_rd[15] = (modarray_rd[15] | ( (((unsigned char*)&M042)[0]) << 2 ));
//  modarray_rd[15] = (modarray_rd[15] | ( (((unsigned char*)&M043)[0]) << 3 ));
//  modarray_rd[15] = (modarray_rd[15] | ( (((unsigned char*)&M044)[0]) << 4 ));
//  modarray_rd[15] = (modarray_rd[15] | ( (((unsigned char*)&M045)[0]) << 5 ));
//    modarray_rd[15] = (modarray_rd[15] | ( (((unsigned char*)&M046)[0]) << 6 ));
//    modarray_rd[15] = (modarray_rd[15] | ( (((unsigned char*)&M047)[0]) << 7 ));
//
//            /************************ Memory Bits 48 to 63 *************************/  //16
//  modarray_rd[16] =                     (((unsigned char*)&M048)[0]);
//    modarray_rd[16] = (modarray_rd[16] | ( (((unsigned char*)&M049)[0]) << 1 ));
//  modarray_rd[16] = (modarray_rd[16] | ( (((unsigned char*)&M050)[0]) << 2 ));
//  modarray_rd[16] = (modarray_rd[16] | ( (((unsigned char*)&M051)[0]) << 3 ));
//  modarray_rd[16] = (modarray_rd[16] | ( (((unsigned char*)&M052)[0]) << 4 ));
//    modarray_rd[16] = (modarray_rd[16] | ( (((unsigned char*)&M053)[0]) << 5 ));
//  modarray_rd[16] = (modarray_rd[16] | ( (((unsigned char*)&M054)[0]) << 6 ));
//  modarray_rd[16] = (modarray_rd[16] | ( (((unsigned char*)&M055)[0]) << 7 ));
//
//  modarray_rd[17] = (modarray_rd[17] | ( (((unsigned char*)&M056)[0]) << 0 ));
//    modarray_rd[17] = (modarray_rd[17] | ( (((unsigned char*)&M057)[0]) << 1 ));
//    modarray_rd[17] = (modarray_rd[17] | ( (((unsigned char*)&M058)[0]) << 2 ));
//  modarray_rd[17] = (modarray_rd[17] | ( (((unsigned char*)&M059)[0]) << 3 ));
//  modarray_rd[17] = (modarray_rd[17] | ( (((unsigned char*)&M060)[0]) << 4 ));
//  modarray_rd[17] = (modarray_rd[17] | ( (((unsigned char*)&M061)[0]) << 5 ));
//    modarray_rd[17] = (modarray_rd[17] | ( (((unsigned char*)&M062)[0]) << 6 ));
//    modarray_rd[17] = (modarray_rd[17] | ( (((unsigned char*)&M063)[0]) << 7 ));
//
//            /************************ Memory Bits 64 to 79 *************************/  //17
//  modarray_rd[18] =                     (((unsigned char*)&M064)[0]);                        //1
//    modarray_rd[18] = (modarray_rd[18] | ( (((unsigned char*)&M065)[0]) << 1 ));  //2
//  modarray_rd[18] = (modarray_rd[18] | ( (((unsigned char*)&M066)[0]) << 2 ));    //4
//  modarray_rd[18] = (modarray_rd[18] | ( (((unsigned char*)&M067)[0]) << 3 ));    //8
//  modarray_rd[18] = (modarray_rd[18] | ( (((unsigned char*)&M068)[0]) << 4 ));    //
//    modarray_rd[18] = (modarray_rd[18] | ( (((unsigned char*)&M069)[0]) << 5 ));
//  modarray_rd[18] = (modarray_rd[18] | ( (((unsigned char*)&M070)[0]) << 6 ));
//  modarray_rd[18] = (modarray_rd[18] | ( (((unsigned char*)&M071)[0]) << 7 ));
//
//  modarray_rd[19] = (modarray_rd[19] | ( (((unsigned char*)&M072)[0]) << 0 ));
//    modarray_rd[19] = (modarray_rd[19] | ( (((unsigned char*)&M073)[0]) << 1 ));
//    modarray_rd[19] = (modarray_rd[19] | ( (((unsigned char*)&M074)[0]) << 2 ));
//  modarray_rd[19] = (modarray_rd[19] | ( (((unsigned char*)&M075)[0]) << 3 ));
//  modarray_rd[19] = (modarray_rd[19] | ( (((unsigned char*)&M076)[0]) << 4 ));
//  modarray_rd[19] = (modarray_rd[19] | ( (((unsigned char*)&M077)[0]) << 5 ));
//    modarray_rd[19] = (modarray_rd[19] | ( (((unsigned char*)&M078)[0]) << 6 ));
//    modarray_rd[19] = (modarray_rd[19] | ( (((unsigned char*)&M079)[0]) << 7 ));
//
//    /************************ Memory Bits 80 to 95 *************************/      //18
//  modarray_rd[20] =                     (((unsigned char*)&M080)[0]);
//    modarray_rd[20] = (modarray_rd[20] | ( (((unsigned char*)&M081)[0]) << 1 ));
//  modarray_rd[20] = (modarray_rd[20] | ( (((unsigned char*)&M082)[0]) << 2 ));
//  modarray_rd[20] = (modarray_rd[20] | ( (((unsigned char*)&M083)[0]) << 3 ));
//  modarray_rd[20] = (modarray_rd[20] | ( (((unsigned char*)&M084)[0]) << 4 ));
//    modarray_rd[20] = (modarray_rd[20] | ( (((unsigned char*)&M085)[0]) << 5 ));
//  modarray_rd[20] = (modarray_rd[20] | ( (((unsigned char*)&M086)[0]) << 6 ));
//  modarray_rd[20] = (modarray_rd[20] | ( (((unsigned char*)&M087)[0]) << 7 ));
//
//  modarray_rd[21] = (modarray_rd[21] | ( (((unsigned char*)&M088)[0]) << 0 ));
//    modarray_rd[21] = (modarray_rd[21] | ( (((unsigned char*)&M089)[0]) << 1 ));
//    modarray_rd[21] = (modarray_rd[21] | ( (((unsigned char*)&M090)[0]) << 2 ));
//  modarray_rd[21] = (modarray_rd[21] | ( (((unsigned char*)&M091)[0]) << 3 ));
//  modarray_rd[21] = (modarray_rd[21] | ( (((unsigned char*)&M092)[0]) << 4 ));
//  modarray_rd[21] = (modarray_rd[21] | ( (((unsigned char*)&M093)[0]) << 5 ));
//    modarray_rd[21] = (modarray_rd[21] | ( (((unsigned char*)&M094)[0]) << 6 ));
//    modarray_rd[21] = (modarray_rd[21] | ( (((unsigned char*)&M095)[0]) << 7 ));
//
//    /****************************** DIO Modbus ********************************************/
//    //DIO_modarray_wr[0] = O006;
//    //DIO_modarray_wr[0] = (DIO_modarray_wr[0]| ( (((O007) << 1 ))));
//    //DIO_modarray_wr[0] = (DIO_modarray_wr[0]| ( (((O008) << 2 ))));
//    //DIO_modarray_wr[0] = (DIO_modarray_wr[0]| ( (((O009) << 3 ))));
//
///************************* Modbus Output Registers  ******************************************/
//modarray_rd[22] = ((unsigned char*)&(Actual_Feedrate))[1];   //Actual Feedrate 19
//modarray_rd[23] = ((unsigned char*)&(Actual_Feedrate))[0];     //Actual Feedrate 19
//
//modarray_rd[24] = ((unsigned char*)&(Totaliser1_Display))[3];  //Totaliser2    20
//modarray_rd[25] = ((unsigned char*)&(Totaliser1_Display))[2];     //Totaliser2    20
//modarray_rd[26] = ((unsigned char*)&(Totaliser1_Display))[1];  //Totaliser2    21
//modarray_rd[27] = ((unsigned char*)&(Totaliser1_Display))[0];     //Totaliser2    21
//
//modarray_rd[28] = ((unsigned char*)&(Totaliser2_Display))[3];  //Totaliser2    22
//modarray_rd[29] = ((unsigned char*)&(Totaliser2_Display))[2];     //Totaliser2    22
//modarray_rd[30] = ((unsigned char*)&(Totaliser2_Display))[1];  //Totaliser2    23
//modarray_rd[31] = ((unsigned char*)&(Totaliser2_Display))[0];     //Totaliser2    23
//
//if(Deviation_Sign == Positive)
//modarray_rd[32] = ((unsigned char*)&(Deviation))[1];                      //Deviation   24
//if(Deviation_Sign == Negative)
//modarray_rd[32] = ((((unsigned char*)&(Deviation))[1]) | 0x80); //Deviation   24
//modarray_rd[33] = ((unsigned char*)&(Deviation))[0];                        //Deviation   24
//

//modarray_rd[34] = ((unsigned char*)&(Current_Setpoint))[1]; //Current Setpoint 25
//modarray_rd[35] = ((unsigned char*)&(Current_Setpoint))[0]; //Current Setpoint 25
//if(Display_Sign == Positive)
//modarray_rd[36] = ((unsigned char*)&(Belt_Load))[1];                       //Belt Load 26
//if(Display_Sign == Negative)
//modarray_rd[36] = ((((unsigned char*)&(Belt_Load))[1]) | 0x80);  //Belt Load 26
//modarray_rd[37] = ((unsigned char*)&(Belt_Load))[0];                       //Belt Load 26

//modarray_rd[38] = ((unsigned char*)&(Belt_Speed))[1]; //Tacho 27
//modarray_rd[39] = ((unsigned char*)&(Belt_Speed))[0];    //Tacho 27
//
//modarray_rd[40] = ((unsigned char*)&(WF_Drive_Setpoint_per))[1];  //WF_Drive_Setpoint_per 28
//modarray_rd[41] = ((unsigned char*)&(WF_Drive_Setpoint_per))[0];    //WF_Drive_Setpoint_per 28
//
//modarray_rd[42] = ((unsigned char*)&(PF_Drive_Setpoint_Per))[1];  //PF_Drive_Setpoint_per 29  Higher byte
//modarray_rd[43] = ((unsigned char*)&(PF_Drive_Setpoint_Per))[0];    //PF_Drive_Setpoint_per 29 Lower byte

///*Hopper,Postn,HopToatl,FDR_Total,CM_Diff,CM%*/
//modarray_rd[44] = ((unsigned char*)&(Hopper_val))[2];  //Hopper Load 30  Higher byte
//modarray_rd[45] = ((unsigned char*)&(Hopper_val))[1];    //Hopper Load 30 Lower byte
//modarray_rd[46] = ((unsigned char*)&(Hopper_val))[0];    //Hopper Load 31 Lower byte

//modarray_rd[47] = ((unsigned char*)&(Gate_Postion))[1];  //Gate Postn 32  Higher byte
//modarray_rd[48] = ((unsigned char*)&(Gate_Postion))[0];    //Gate Postn 32 Lower byte

//modarray_rd[49] = ((unsigned char*)&(HOpper_total))[2];  //Hopper Total 33  Higher byte
//modarray_rd[50] = ((unsigned char*)&(HOpper_total))[1];    //Hopper Total 33 Lower byte
//modarray_rd[51] = ((unsigned char*)&(HOpper_total))[0];    //Hopper Total 34 Lower byte

//modarray_rd[52] = ((unsigned char*)&(FDR_Total))[2];  //Feeder Total 35  Higher byte
//modarray_rd[53] = ((unsigned char*)&(FDR_Total))[1];    //Feeder Total 35 Lower byte
//modarray_rd[54] = ((unsigned char*)&(FDR_Total))[0];    //Feeder Total 36 Lower byte

//modarray_rd[55] = ((unsigned char*)&(CM_Difference))[1];  //CM Difference 37  Higher byte
//modarray_rd[56] = ((unsigned char*)&(CM_Difference))[0];    //CM Difference 37 Lower byte

//modarray_rd[57] = ((unsigned char*)&(CM_Display))[1];  //CM% 38  Higher byte
//modarray_rd[58] = ((unsigned char*)&(CM_Display))[0];    //CM% 38 Lower byte

///************************************************************************************/

///********************** Write Address Starts From 0*****************************************/

// ((unsigned char*)&Common_Setpoint)[1] = modarray_wr[0];            //communication setpoint at Register 0
// ((unsigned char*)&Common_Setpoint)[0] = modarray_wr[1];
//
//(((unsigned char*)&M000)[0]) = (modarray_wr[3] & 0x01) ;
//(((unsigned char*)&M001)[0]) = ((modarray_wr[3] >> 1) & 0x01 ) ;
//(((unsigned char*)&M002)[0]) = ((modarray_wr[3] >> 2) & 0x01 ) ;
//(((unsigned char*)&M003)[0]) = ((modarray_wr[3] >> 3) & 0x01 ) ;
//
//(((unsigned char*)&M004)[0]) = ((modarray_wr[3] >> 4) & 0x01 ) ;
//(((unsigned char*)&M005)[0]) = ((modarray_wr[3] >> 5) & 0x01 ) ;
//(((unsigned char*)&M006)[0]) = ((modarray_wr[3] >> 6) & 0x01 ) ;
//(((unsigned char*)&M007)[0]) = ((modarray_wr[3] >> 7) & 0x01 ) ;

///**************************  After ************************************/
//(((unsigned char*)&M008)[0]) = (modarray_wr[4] & 0x01) ;
//(((unsigned char*)&M009)[0]) = ((modarray_wr[4] >> 1) & 0x01 ) ;
//(((unsigned char*)&M010)[0]) = ((modarray_wr[4] >> 2) & 0x01 ) ;
//(((unsigned char*)&M011)[0]) = ((modarray_wr[4] >> 3) & 0x01 ) ;
//
//(((unsigned char*)&M012)[0]) = ((modarray_wr[4] >> 4) & 0x01 ) ;
//(((unsigned char*)&M013)[0]) = ((modarray_wr[4] >> 5) & 0x01 ) ;
//(((unsigned char*)&M014)[0]) = ((modarray_wr[4] >> 6) & 0x01 ) ;
//(((unsigned char*)&M015)[0]) = ((modarray_wr[4] >> 7) & 0x01 ) ;

//(((unsigned char*)&M016)[0]) = (modarray_wr[5] & 0x01) ;
//(((unsigned char*)&M017)[0]) = ((modarray_wr[5] >> 1) & 0x01 ) ;
//(((unsigned char*)&M018)[0]) = ((modarray_wr[5] >> 2) & 0x01 ) ;
//(((unsigned char*)&M019)[0]) = ((modarray_wr[5] >> 3) & 0x01 ) ;

//(((unsigned char*)&M020)[0]) = ((modarray_wr[5] >> 4) & 0x01 ) ;
//(((unsigned char*)&M021)[0]) = ((modarray_wr[5] >> 5) & 0x01 ) ;
//(((unsigned char*)&M022)[0]) = ((modarray_wr[5] >> 6) & 0x01 ) ;
//(((unsigned char*)&M023)[0]) = ((modarray_wr[5] >> 7) & 0x01 ) ;

//(((unsigned char*)&M024)[0]) = (modarray_wr[6] & 0x01) ;
//(((unsigned char*)&M025)[0]) = ((modarray_wr[6] >> 1) & 0x01 ) ;
//(((unsigned char*)&M026)[0]) = ((modarray_wr[6] >> 2) & 0x01 ) ;
//(((unsigned char*)&M027)[0]) = ((modarray_wr[6] >> 3) & 0x01 ) ;
//
//(((unsigned char*)&M028)[0]) = ((modarray_wr[6] >> 4) & 0x01 ) ;
//(((unsigned char*)&M029)[0]) = ((modarray_wr[6] >> 5) & 0x01 ) ;
//(((unsigned char*)&M030)[0]) = ((modarray_wr[6] >> 6) & 0x01 ) ;
//(((unsigned char*)&M031)[0]) = ((modarray_wr[6] >> 7) & 0x01 ) ;

//(((unsigned char*)&M032)[0]) = (modarray_wr[7] & 0x01) ;
//(((unsigned char*)&M033)[0]) = ((modarray_wr[7] >> 1) & 0x01 ) ;
//(((unsigned char*)&M034)[0]) = ((modarray_wr[7] >> 2) & 0x01 ) ;
//(((unsigned char*)&M035)[0]) = ((modarray_wr[7] >> 3) & 0x01 ) ;

//(((unsigned char*)&M036)[0]) = ((modarray_wr[7] >> 4) & 0x01 ) ;
//(((unsigned char*)&M037)[0]) = ((modarray_wr[7] >> 5) & 0x01 ) ;
//(((unsigned char*)&M038)[0]) = ((modarray_wr[7] >> 6) & 0x01 ) ;
//(((unsigned char*)&M039)[0]) = ((modarray_wr[7] >> 7) & 0x01 ) ;
///************************************************************************************/

// }
//
// /*
// 07/02/2016
// Updated hoppee Related values displayed in Online Screen 3
// Added Memory input and output bits;30-38
// Added Inv Digital inputs;10
// Updated adress ALL
//
// */
