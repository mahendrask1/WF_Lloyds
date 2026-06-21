#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmsis_os.h" // ARM::CMSIS:RTOS:Keil RTX
#include "LPC17xx.h"  // Device header
#include "lpc17xx_spi.h"
#include "lpc17xx_gpio.h"
#include "GPIO_LPC17xx.h"
#include "lpc17xx_rtc.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_uart.h"
#include "main_routine.h"
#include "OLED_2_7_12864.h"
#include "functions.h"
#include "parameters.h" // #include "menu.h"
#include "timer.h"
#include "eeprom.h"
#include "adc.h"
#include "dac.h"
#include "spi.h"
#include "plc.h"
#include "io_control.h"
#include "modbus.h"
#include "keys.h"

extern unsigned int External_Setpoint, Common_Setpoint, Current_Setpoint, Deviation,
    PF_Drive_Setpoint;
extern float Actual_Feedrate, CM_Percentage;
extern float WF_Drive_Setpoint;
extern uint8_t ram_values[RAM_SIZE]; // values in RAM
extern uint16_t New_menu_parameters[105];

extern unsigned char Mode_Status, ACAL_Status, BIN_PF_Start_Status,CM_Negative;

extern unsigned int New_Integral, Old_Integral;
extern unsigned char Zero_Tacho_Feed_Buffer_Flag;
extern char Start_Stop_status, Error_No;

extern unsigned int Belt_Speed;
extern unsigned int Totaliser1_Display;
extern double Totaliser1, Totaliser1_Update;
extern char DInt_Int_status, Auto_Man_status;
extern unsigned long Present_Count, Present_Count_ch2, Count_DL, Count_DL_ch2, Span_Count,
    Span_Count_ch2, zero_load, zero_load_ch2, full_load, full_load_ch2;

void Digital_Input_Action()
{
    if ((Start_Stop_status == START) && ((T000 == 1) || (T001 == 1)) && (Mode_Status != Local)) {
        Error_No = 13; // Zero Speed Switch faulr Alarm
        M066 = 0;
        M046 = 1; // ZSS faulr Marker
        M044 = 1; // General Fault Markler
        Start_Stop_status = STOP
        Zero_Tacho_Feed_Buffer_Flag = 2;
    }

    // if(( Start_Stop_status == START )  && (M017 == 1) && (Mode_Status != Local) )
    if ((Start_Stop_status == START) && (M017 == 0) && (Mode_Status != Local)) // msk marker reversal 31-aug-25
    {
        M066 = 0;
        Start_Stop_status = STOP;
        M044 = 1; // General Fault Marker
        Error_No = 12; // Down Stream Interlock
        Zero_Tacho_Feed_Buffer_Flag = 2;
    }

    if ((Start_Stop_status == START) && (M010 == 0)) //m10= emergency fault
    {
        M066 = 0;
        Start_Stop_status = STOP;
        M044 = 1; // General Fault
        Error_No = 9; // Emergency faulr Alarm
        Zero_Tacho_Feed_Buffer_Flag = 2;
    }

    if ((Start_Stop_status == START) && (M019 == 0)) { //m19= drive healthy-wf, 0=fault
        M066 = 0; // STOP
        Start_Stop_status = STOP;
        M044 = 1; // Gneral Fault Marker
        M042 = 1; // Set WF Drive Fault Marker
        Zero_Tacho_Feed_Buffer_Flag = 2;

        Error_No = 8; // Drive faulr Alarm
    }

    if ((Start_Stop_status == START) && (M020 == 0)) { //m20= drive healthy-pf, 0=fault
        M067 = 0; // Start/Stop  Marker PF
        M066 = 0; // Start/Stop  Marker WF
        Start_Stop_status = STOP;
        M044 = 1; // General Fault
        M045 = 1; // PF Drive Fault Marker
        Error_No = 8; // Drive faulr Alarm
        Zero_Tacho_Feed_Buffer_Flag = 2;
    }

    if (M018 == 1) // Fault ack_i/p
    {
        ApplyFaultResetMarkers();
        M018 = 0;
    }

    // if(Error_No ==0) //general fault reset if no error
    //     M044 = 0;
    // else
    //     M044 = 1;

    /****************************************** Remote Mode***********************************************************/
    if (M008 == 1) // If Remote mode ;
    {
        M051 = 0; // Output Marker M051;0-Remote ; 1-Local

        /****************************************** Modbus ***********************************************************/

        if ((Mode_Status == Interlock) && (M002 == 1)) // If M002 =1 ; Then S.P,Start/Stop via Modbus
        {
            Mode_Status = Remote;
            Current_Setpoint = Common_Setpoint; // Setpoint_Commnication
        }

        if ((Mode_Status == ) && (M002 == 0)) // If M002 =0;Then S.P=analog i.e either 0-10vdc or 4-20mA,Start/Stop via PLC/DCS i.e via markers
        {
            Current_Setpoint = External_Setpoint; // Setpoint_External analog i.e either 0-10vdc or 4-20mA
        }

        if ((Mode_Status == Remote) && (M000 == 1) && (M001 == 0)) {
            Start_Stop_status = START;
            M066 = 1;//m066= wf-start/stop/fault, 1=start
        }

        if ((Mode_Status == Remote) && (M000 == 0) && (M001 == 1) && (Start_Stop_status == START)) {
            M066 = 0;//m066= wf-start/stop/fault, 0=stop
            Start_Stop_status = STOP;
            Zero_Tacho_Feed_Buffer_Flag = 2;
        }

        if ((Mode_Status == Remote) && (M002 == 0)) {
            Mode_Status = Interlock;
        }

        if ((Mode_Status == Remote) && (M003 == 1)) {
            Totaliser1_Display = 0;
            Totaliser1 = 0;
            Totaliser1_Update = 0;
        }

        /*************************************************Modbus End*************************************************************/
        /*************************************************External *************************************************************/
        if (Mode_Select == Ext) // EXT Remote .i.e Mode selection from Digital Inputs or PLC/DCS
        {
            // M007;0-Manual or Volumetric ; 1-Gravemetric
            if (M007 == 1) // Gravemetric
            {
                Mode_Status = Auto; // Make it Gravemetric mode
                Auto_Man_status = 0; // Make Manual LED on
                M052 = 1; // M052 set for gravemetric  mode
            } else // Manual or Volumetric
            {
                Mode_Status = Manual; // Default Condition
                DInt_Int_status = 1; // DeInterlock Manual Mode
                Auto_Man_status = 1; // MakeManual Led Off
                M052 = 0; // M052 cleared for volumetric  mode
            }

            // first M52 will be set, and followed by
            if (M007 == 1) // if Grvemetric mode
            {
                if (M015 == 0) // Remote:M015 = 0 :Interlock
                {
                    Mode_Status = Interlock;
                    DInt_Int_status = 0; // Interlock Mode
                    M053 = 1; // M053 = 1 for Interlock
                }

                if (M015 == 1) // Remote:M015 = 1 :De-Interlock
                {
                    Mode_Status = De_Interlock;
                    DInt_Int_status = 1; // De-Interlock Mode
                    M053 = 0; // M053 = 0 for De-Interlock
                }

                // if((Mode_Status == Interlock ) && (M016 == 1))/*Previous*/
                // M016 = 1 means Remote Start
                // M044 is set when general fault
                if ((Mode_Status == Interlock) && (M016 == 1) && 
                (M044 == 0)) /*changed 06042022 by prince*/
                {
                    M066 = 1; // M066 = 1 means feeder Start
                    Start_Stop_status = START;
                }

                // if((Mode_Status == Interlock ) && (M016 == 0))/*Previous*/
                if ((Mode_Status == Interlock) && ((M016 == 0) || 
                (M044 == 1))) /*changed 06042022 by prince*/
                {
                    M066 = 0; // Stop
                    Start_Stop_status = STOP;
                    Zero_Tacho_Feed_Buffer_Flag = 2;
                }
            }//if (M007 == 1) ends
        }//if (Mode_Select == Ext) ends
        /**************************************************External End************************************************************/
        /*************************************************Internal *************************************************************/
        if (Mode_Select == Int) // Int Remote.i.e Mode selection from keypad
        {
            // if((Mode_Status == Interlock ) && (M016 == 1))/*Previous*/
            if ((Mode_Status == Interlock) && (M016 == 1) && (M044 == 0)) /*changed 06042022 by prince*/
            {//m016= remote start/stop, 1=start, 0=stop, 
            // //M044= general fault, 0=no fault
                M066 = 1;//m066= wf-start/stop/fault, 1=start
                Start_Stop_status = START;
            }

            // if((Mode_Status == Interlock ) && (M016 == 0))/*Previous*/
            if ((Mode_Status == Interlock) && ((M016 == 0) || (M044 == 1))) /*changed 06042022 by prince*/
            {//m016= remote start/stop, 1=start, 0=stop, 
                M066 = 0;
                Start_Stop_status = STOP;
                Zero_Tacho_Feed_Buffer_Flag = 2;
            }
        }
        /*************************************************Internal End*************************************************************/
        /*************************************************Vibrator Motor Logic*************************************************************/
        if ((M066 == 1) && (Start_Stop_status == START) && (M029 == 1)) // If WF==ON & Material detect switch(M029)==ON
        {
            M069 = 1; // Vibrator Motor On-Off(1-On/0-Off)
        } else {
            M069 = 0; // Vibrator Motor On-Off(1-On/0-Off)
        }
        /*************************************************Vibrator Motor Logic ends*************************************************************/
    }
    /*************************************************Remote End*************************************************************/

    /*************************************************Local ***************************************************/
    if (M008 == 0) {
        Mode_Status = Local;
        M051 = 1; // Output Marker M051;0-Remote ; 1-Local
    } else {
        M051 = 0; // Output Marker M051;0-Remote ; 1-Local
    }

    if (Mode_Status == Local) {
        M052 = 0; // Volumetric Mode
        if ((M011 == 1) && (M010 == 1) && 
        (M019 == 1) && (M020 == 1)) // Emegency,WFD,PFD must be ok else Dont start
        {
            //m011= local start wf, 1=start, 
            //M010= emergency fault, 1=healthy, 0=error
            //M019= drive healthy-wf, 1=healthy, 0=error
            //M020= drive healthy-pf, 1=healthy, 0=error        
            M066 = 1;//m066= wf-start/stop/fault, 1=start
            Start_Stop_status = START;
            M012 = 0;//m12= local  wf, 1=stop
        }

        if (M012 == 1) {//M12= local stop wf, 1=stop
            M066 = 0;   //m66= wf-start/stop/fault, 0=stop
            Start_Stop_status = STOP;
            M011 = 0; //m11= local start wf, 1=start, 0=stop
            Zero_Tacho_Feed_Buffer_Flag = 2;
        }

        if (M028 == 1) // Local_Vibrator On_Off(1-On/0-Off)
        {
            M069 = 1; // Vibrator Motor On-Off(1-On/0-Off)
        } else {
            M069 = 0; // Vibrator Motor On-Off(1-On/0-Off)
        }

        if (Prefeeder == 1) {
            if (M013 == 1) // Push button Switch Local Start
            {
                M067 = 1;
                // Start_Stop_status = START;
                M014 = 0;
            }

            if (M014 == 1) // Push button Switch Local Stop
            {
                M067 = 0;
                // Start_Stop_status = STOP;
                M013 = 0;
            }
        }
    }
    /******************************************************************************************************/
    if ((Mode_Status == Local) && (M008 == 1) && 
    (Mode_Select == Int)) // INT Remote
    {
        Mode_Status = Manual; // Default Condition
        DInt_Int_status = De_Interlock;
        Auto_Man_status = STOP; // make it Manual mode
        Start_Stop_status = STOP; // Status STOP on start up
        M052 = 0; // Volumetric Mode
    }

    /***************************************Auto Calibration ***************************************************************/
    /*
    IP Markers
    M024: DS 01H   ;BIN PRE-FEEDER START/STOP FEEDBACK
    M025: DS 01H   ;START CONTROL MEASURING
    M026: DS 01H   ;STOP CONTROL MEASURING
    M027: DS 01H   ;ACCEPT CORRECTION

    OP Markers
    M075: DS 01H   ;BIN PREFEEDER ON
    M076: DS 01H   ;CONTROL MEASUREMENT ON
    M077: DS 01H   ;CONTROLMEASUREMENT ABORTED.
    M078: DS 01H   ;CORRECTION MAX EXEEDED

    */
    if (Pre_Hopper == 1) // If Prehopper is selected
    {
        if ((Start_Stop_status == START) && (M025 == 1) && ((Mode_Status == Interlock) || (Mode_Status == De_Interlock))) {
            ACAL_Status = 1; // Auto cal Started
        }

        if ((M026 == 1) && (ACAL_Status == 2)) {
            ACAL_Status = 0;
            M077 = 1; // Auto cal aborted
        }

        if (M027 == 1) // Accept Auto cal Val marker
        {
            if (ACAL_Status == 3) // Accept auto cal
            {
                ACAL_Status = 0;
                After_AutoCAL();
            }

            if (ACAL_Status == 4) // Greater than max limit
            {
                ACAL_Status = 0;
                M078 = 1; // Limit Exeeded
            }
        }//if (M027 == 1) ends

        if (M024 == 1) // BIN_PF Start/stop Feedback
            BIN_PF_Start_Status = 1;
        else
            BIN_PF_Start_Status = 0;
    }//if (Pre_Hopper == 1) ends
    /***********************************************************************************************************************/
}

/*
30/03/2018
if(Mode Select =EXT)
M007 = 1-Gravimetric/0-Volumetric

M052 = 1-Gravimetric/0-Volumetric

06/04/2022
added M044 interlock for START/STOP in interlock mode
line no 232, 251 : if((Mode_Status == Interlock ) && (M016 == 1) && (M044 == 0))//changed 06042022 by prince
line no 238, 257 : if((Mode_Status == Interlock ) && ((M016 == 0) || (M044 == 1)))//changed 06042022 by prince
*/
