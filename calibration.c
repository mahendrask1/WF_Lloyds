/* Calibration.c     */

#include "LPC17xx.h"                    // Device header
#include <stdint.h>
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "main_routine.h"
#include "OLED_2_7_12864.h"
#include "parameters.h"  //#include "menu.h"
#include "keys.h"
#include "adc.h"
#include "eeprom.h"
#include "functions.h"
#include "spi.h"
#include "dac.h"
#include "stdbool.h"
void save_par_eeprom(void);
void    Read_adc_ltc1288(void) ;
void    AI_adc_avg_cal(void);
void save_AO_calib_val(void);
void read_AO_calib_val(void);
void read_AO_calib_val_ch1(void);
void read_AO_calib_val_ch2(void);
void read_AO_calib_val_ch3(void);
void read_calib_val(void);
void read_adc_conversion(void);
void sendAOquery(_Bool);

extern uint16_t AO_cal_Values[6];
extern uint16_t DAC_Scale_Values[6];
extern  uint16_t AI_avg,AI_reg[20],AI1_avg,AI1_reg[20],AI2_avg,AI2_reg[20];// ,testarray[16];
extern unsigned long BL_sum,BL_reg[1600],BL_avg;
extern uint16_t AO_calval[32];
//extern int bytes_written;
unsigned long calib_val[48];
uint16_t  AI_calb_val[16];
//uint16_t AO_val[32];
char AO_Calib_status=0,Esc_Status=0;

extern uint16_t dacop1,dacop2;
extern uint16_t scan_count;
extern   uint32_t    Key_Input;
extern unsigned long adc_final_val,bsf_adc_final_val;
extern char flag;
unsigned long Present_Count,Present_Count_ch2,Count_DL_ch2,Span_Count,Span_Count_ch2,zero_load,zero_count_cal,zero_load_ch2,full_load,full_load_ch2;
//Count_DL
extern unsigned long Calib_Load,Calib_Load_ch2;
unsigned char Calib_status;
unsigned long Scale_Weight;
char calib_string[20];
char *calib_string_ptr = calib_string;
float millivolt;

extern char *clear_string;// ="                             ";
extern  char *unit_string_ptr,unit_string[2][5],unit_string_t1[2][5];
extern uint16_t New_menu_parameters[105];
unsigned long Min_Count_Channel1,Min_Count_Channel2,Min_Count_Channel3,Max_Count_Channel1,Max_Count_Channel2,Max_Count_Channel3,temp_AO_val;
unsigned long Dac1_SpanCounts,Dac2_SpanCounts,Dac3_SpanCounts;
//uint16_t Dac1_ScaleFactor, Dac2_ScaleFactor, Dac3_ScaleFactor; //These are macros in parameters.h
char val_temp_1=0,val_temp_2=0,AI_cal_status=0,temp_status;
uint16_t  Min_adc1_val,Min_adc2_val,Min_adc3_val,Max_adc1_val,Max_adc2_val,Max_adc3_val;
extern unsigned int Dac1_Val,Dac2_Val,Dac3_Val,Capacity_1;
extern uint16_t AI_Value,AI1_Value,AI2_Value;
char cal_disp_status;

///unsigned int  full_load, zero_count_cal;
/**********************************************************************************
//    Calib_Display()
Routine Updates and Displays Scale_Weight during Calibration
**********************************************************************************/
//-------------------------------------------------
void calib_display(void )
{
    float mvz,mvf;
if(Calib_status==1)
{

    //display Scale_Weight in line2
      zero_count_cal = BL_avg;//bsf_adc_final_val;
            Clear_String(&clear_string,line2,10,font16);
        sprintf(calib_string,"Wt=%lu ",zero_count_cal);
        calib_string_ptr=calib_string;
            Print_String(&calib_string_ptr,line2,0,font16,0);   // 0 % load value
    //    zero_count_cal= Scale_Weight;
}
if(Calib_status==3)
{
            Clear_String(&clear_string,line3,10,font16);
            //sprintf(calib_string,"Wt=%lu ",Present_Count );
            sprintf(calib_string,"Wt=%lu ",BL_avg );
            calib_string_ptr=calib_string;
            Print_String(&calib_string_ptr,line3,0,font16,0);   //100 % load value
        //    full_load= Scale_Weight;
}
if(Calib_status==12)
{
    Clear_String(&clear_string,line1,10,font10);
    calib_string_ptr = "ADC counts ...";
    Clear_String(&clear_string,line1,0,font10);
    Print_String(&calib_string_ptr,line1,0,font10,0);

    if(calib_val[2] == 0)
    {
        read_calib_val();
    }

    Clear_String(&clear_string,line3,10,font10);
    sprintf(calib_string,"Full Scl=%lu",calib_val[0]);
    calib_string_ptr=calib_string;
    Print_String(&calib_string_ptr,line3,0,font10,0);   // Zero load value

            Clear_String(&clear_string,line4,10,font10);
            //sprintf(calib_string,"Present=%lu ",Present_Count);
            sprintf(calib_string,"Present=%lu ",BL_avg);    //present averaged counts
            calib_string_ptr=calib_string;
    Print_String(&calib_string_ptr,line4,0,font10,0);   // Zero load value

        millivolt=(float)((float)BL_avg * (float)(39.000f * 16.00f))/(float)(0xffffff);    // multiply by 16 because the counts stored afer div/16

            Clear_String(&clear_string,line5,10,font10);
    //        sprintf(calib_string,"mV = %4.2f ",millivolt);
    //        calib_string_ptr=calib_string;
    //        Print_String(&calib_string_ptr,line5,0,font10,0);   // Zero load value

        mvz=(float)((float)calib_val[2] * (float)(39.000f * 16.00f))/(float)(0xffffff);    // multiply by 16 because the counts stored afer div/16
        mvf=(float)((float)calib_val[0] * (float)(39.000f * 16.00f))/(float)(0xffffff);    // multiply by 16 because the counts stored afer div/16

            Clear_String(&clear_string,line6,10,font10);
//            sprintf(calib_string,"mVZ=%4.2f:F=%4.2f",mvz,mvf);
//            calib_string_ptr=calib_string;
//            Print_String(&calib_string_ptr,line6,0,font10,0);   // Zero load value

            Clear_String(&clear_string,line2,10,font10);
            sprintf(calib_string,"Zero   =%lu",calib_val[2]);
            calib_string_ptr=calib_string;
            Print_String(&calib_string_ptr,line2,0,font10,0);   // Zero load value
}
if(Calib_status==14)
{
    Clear_String(&clear_string,line1,10,font10);
    calib_string_ptr = "ADC counts ...";
    Clear_String(&clear_string,line1,0,font10);
    Print_String(&calib_string_ptr,line1,0,font10,0);

    if(calib_val[2] == 0)
    {
        read_calib_val();
    }

    Clear_String(&clear_string,line3,10,font10);
    sprintf(calib_string,"Full Scl=%lu",calib_val[0]);
    calib_string_ptr=calib_string;
    Print_String(&calib_string_ptr,line3,0,font10,0);   // Zero load value

            Clear_String(&clear_string,line4,10,font10);
            //sprintf(calib_string,"Present=%lu ",Present_Count);
            sprintf(calib_string,"Present=%lu ",BL_avg);    //present averaged counts
            calib_string_ptr=calib_string;
    Print_String(&calib_string_ptr,line4,0,font10,0);   // Zero load value

        millivolt=(float)((float)BL_avg * (float)(39.000f * 16.00f))/(float)(0xffffff);    // multiply by 16 because the counts stored afer div/16

            Clear_String(&clear_string,line5,10,font10);
            sprintf(calib_string,"mV = %4.2f ",millivolt);
            calib_string_ptr=calib_string;
            Print_String(&calib_string_ptr,line5,0,font10,0);   // Zero load value

            mvz=(float)((float)calib_val[2] * (float)(39.000f * 16.00f))/(float)(0xffffff);    // multiply by 16 because the counts stored afer div/16
            mvf=(float)((float)calib_val[0] * (float)(39.000f * 16.00f))/(float)(0xffffff);    // multiply by 16 because the counts stored afer div/16

            Clear_String(&clear_string,line6,10,font10);
            sprintf(calib_string,"mVZ=%4.2f:F=%4.2f",mvz,mvf);
            calib_string_ptr=calib_string;
            Print_String(&calib_string_ptr,line6,0,font10,0);   // Zero load value

            Clear_String(&clear_string,line2,10,font10);
            sprintf(calib_string,"Zero   =%lu",calib_val[2]);
            calib_string_ptr=calib_string;
            Print_String(&calib_string_ptr,line2,0,font10,0);   // Zero load value
}

}
/**********************************************************************************
//    Calibration()
This routine is called for calibration process
**********************************************************************************/
void     Calibration (void)
{
    uint32_t cnts;

    float Span_Count_R,Span_Count_Q,scale_cap1;
    Key_Input=NO_KEY;
    Clear_LCD();
    super_delay(40);
    calib_string_ptr = "Erase Old Data";
    Print_String(&calib_string_ptr,line1,0,font10,0);
    calib_string_ptr = "New Calibration....";
    Print_String(&calib_string_ptr,line2,0,font10,0);
    calib_string_ptr = "Press 1 to View";
    Print_String(&calib_string_ptr,line3,0,font10,0);
    calib_string_ptr = "Enter : Start Cal  ";
    Print_String(&calib_string_ptr,line4,0,font10,0);
    calib_string_ptr = "Up   : Manual entry ";
    Print_String(&calib_string_ptr,line5,0,font10,0);
    calib_string_ptr = "Return: EXIT   ";
    Print_String(&calib_string_ptr,line6,0,font10,0);
    super_delay(40);
    Calib_status=0;

    calib_jump1:             //  keep executing loop until 5 msec interrupt  rotine
//    loadcell_read();
if(flag==1)    // flag becomes 1 after every 5 ms (Timer0 tick)
    {
        flag=0;
    Key_Input=NO_KEY;
        if(scan_count%20 ==0)
        {
            key_check_routine();
        }
            read_adc_conversion();        //load cell read

if(    Key_Input==NO_KEY)
{
    goto calib_jump3;              //display calibration routine
}

if(Calib_status ==0)
{
        if(Key_Input==K_INC)        //msk
        {
            read_calib_val();
            full_load = calib_val[0];
            Span_Count = calib_val[1];
            zero_count_cal = (unsigned long)calib_val[2];
            BL_avg = calib_val[6];
                        Calib_Load = calib_val[7];
            Calib_status =10;
            Clear_LCD();
        }

        if(Key_Input==Setpoint)        //msk
        {
            read_calib_val();
            full_load = calib_val[0];
            Span_Count = calib_val[1];
            zero_count_cal = (unsigned long)calib_val[2];
            BL_avg = calib_val[6];
                        Calib_Load = calib_val[7];
            Calib_status =12;
            Clear_LCD();
        }
        if(Key_Input==k_Total1)        //msk
        {
            read_calib_val();
            full_load = calib_val[0];
            Span_Count = calib_val[1];
            zero_count_cal = (unsigned long)calib_val[2];
            BL_avg = calib_val[6];
                        Calib_Load = calib_val[7];
            Calib_status =14;
            Clear_LCD();
        }
        if(Key_Input==HOR)
        {
            Calib_status=1;
            read_calib_val();

            full_load = calib_val[0];
            Span_Count = calib_val[1];
            zero_count_cal = (unsigned long)calib_val[2];
            BL_avg = calib_val[6];
            Calib_Load = calib_val[7];
            //zero_load=zero_count_cal;
            Clear_LCD();

            calib_string_ptr = "Tare_Calibration";
            Print_String(&calib_string_ptr,line1,0,font10,0);

            calib_string_ptr = "Emty & Press->";
            Print_String(&calib_string_ptr,line3,0,font16,0);
            calib_string_ptr = "->Tare,Clear-exit";
            Print_String(&calib_string_ptr,line4,0,font16,0);
            super_delay(40);
        }
        else if(Key_Input==ESC)
        {
            Clear_LCD();
                Calib_status=0;
            return;        //exit routine if any other key pressed
        }
}

else if(Calib_status ==1)
{
        if(Key_Input==CL)
        {
            Calib_status=5;
            Clear_LCD();
            calib_string_ptr = "Calib Aborted";
            Print_String(&calib_string_ptr,line2,0,font16,0);
            Clear_String(&clear_string,line3,10,font16);
            calib_string_ptr = "Any Key- exit";
            Print_String(&calib_string_ptr,line4,0,font16,0);
        }
        if(Key_Input==HOR)
        {
            Calib_status=2;
            Clear_LCD();
            //zero_count_cal = Present_Count ; //bsf_adc_final_val;        ////       0% load value            zero_count_cal = BL_avg ; //bsf_adc_final_val;                 ////       0% load value

            calib_val[2] = (unsigned long)zero_count_cal;
            calib_string_ptr = "Tare Calib OK";
            Clear_String(&clear_string,line2,0,font16);
            Print_String(&calib_string_ptr,line2,0,font16,0);
            Clear_String(&clear_string,line3,0,font16);
            calib_string_ptr = "Down-Next";
            Clear_String(&clear_string,line4,0,font16);
            Print_String(&calib_string_ptr,line4,0,font16,0);
        }
        goto calib_jump3;
    }
else if(Calib_status ==2)
{
    if(Key_Input==CL)
        {
            Calib_status=5;
            Clear_LCD();
            calib_string_ptr = "Calib Aborted";
            Print_String(&calib_string_ptr,line2,0,font16,0);
            Clear_String(&clear_string,line3,10,font16);
            calib_string_ptr = "Any Key- exit";
            Print_String(&calib_string_ptr,line4,0,font16,0);
        }
    if(Key_Input==K_DEC)
    {
        Key_Input=NO_KEY;
        Calib_status=3;
            Clear_LCD();
            calib_string_ptr = "LoadCell Calibr";
            Clear_String(&clear_string,line1,0,font16);
            Print_String(&calib_string_ptr,line1,0,font16,0);
            calib_string_ptr = "Load=";

            Clear_String(&clear_string,line2,0,font16);
            Print_String(&calib_string_ptr,line2,0,font16,0);

            calib_string_ptr = "0-9 ,Ver- Span";
            Clear_String(&clear_string,line4,0,font16);
            Print_String(&calib_string_ptr,line4,0,font16,0);

        while(Key_Input!=K_DEC && Key_Input!=HOR && Key_Input !=ESC)
        {
            Calib_Load =    Enter_Variable2(Calib_Load,line3,20,font16,4,1);   //  known load value used for calibration
        }
        if(Key_Input==ESC)
        {
            Clear_LCD();
                Calib_status=0;
            return;
        }
            if((Key_Input==K_DEC)||(Key_Input==HOR))
            {
                super_delay(20);
            }
//        par_val =    Enter_Variable2(par_val,line3,10,font16,no_of_digits,no_of_dec);
    }
}
if(Calib_status ==3)
{
        while(Key_Input!=K_DEC && Key_Input!=HOR && Key_Input !=ESC)
        {
            Calib_Load =    Enter_Variable2(Calib_Load,line3,20,font16,4,1); //  known load value used for calibration
        }
        if(Key_Input==ESC)
        {
            Clear_LCD();
            Calib_status= 1;
            return;
        }
            if((Key_Input==K_DEC)||(Key_Input==HOR))
            {
                super_delay(20);
            }
        sprintf(calib_string,"Load=");
        calib_string_ptr=calib_string;
        Clear_String(&clear_string,line2,0,font16);
        Print_String(&calib_string_ptr,line2,0,font16,0);
        Print_Variablex(Calib_Load,line2,20,font16,0,1,30);
        scale_cap1 = (float)Capacity/(float)1000;

    if(Key_Input==HOR)
    {
        Calib_status =4;
        //full_load=Present_Count ;
        full_load=BL_avg ;
        if(full_load>=zero_count_cal)
        {
            //full_load=Present_Count ; ///100% load value
            //Span_Count=((Present_Count-Count_DL)*Scale_Capacity)/Calib_Load    ;
            //Span_Count=((Present_Count - zero_count_cal));
            Span_Count=((BL_avg - zero_count_cal));
            Span_Count_Q = Span_Count / Calib_Load;             // Calib_Load is the weight we are entering
            Span_Count_R = Span_Count%Calib_Load;
            full_load= ((Span_Count_Q*1000)+Span_Count_R*1000/Calib_Load)+zero_count_cal;        //as calib load entered is 4 digit long so Q & R are divided by 1000
            Span_Count=full_load-zero_count_cal;

            calib_string_ptr = "LoadCell Calib OK";
            Clear_String(&clear_string,line2,0,font16);
            Print_String(&calib_string_ptr,line2,0,font16,0);
            super_delay(10);
            calib_val[0] = full_load;
            calib_val[1] = Span_Count;
            calib_val[2] =  (unsigned long)zero_count_cal;
            calib_val[6] = BL_avg;
            calib_val[7] = Calib_Load;
            zero_load=zero_count_cal;
            save_calib_val();
        }
        else
        {
//                Span_Count=(( zero_count_cal-full_load ));
            calib_string_ptr = "Span Calib Error";
            Clear_String(&clear_string,line2,0,font16);
            Print_String(&calib_string_ptr,line2,0,font16,0);
            Clear_String(&clear_string,line3,0,font16);
            calib_string_ptr = "Press any Key to";
            Clear_String(&clear_string,line3,0,font16);
            Print_String(&calib_string_ptr,line3,0,font16,0);
            calib_string_ptr = "Exit";
            Clear_String(&clear_string,line4,0,font16);
            Print_String(&calib_string_ptr,line4,0,font16,0);
        }
    }
        goto calib_jump3;
    }
else if(Calib_status==4)
{
    if(Key_Input != NO_KEY)
    {
        Clear_LCD();
        return;
    }
}
else if(Calib_status==5)
{
    if(Key_Input != NO_KEY)
    {
        Clear_LCD();
        return;
    }
}
else if(Calib_status==10)
{
            calib_string_ptr = "Old Zero Counts ";
            Clear_String(&calib_string_ptr,line1,0,font10);
            sprintf(calib_string,"Old Counts=%lu ",calib_val[2]);
            calib_string_ptr=calib_string;
            Print_String(&calib_string_ptr,line2,0,font10,0);   // Zero load value

            calib_string_ptr = "Enter New Counts... ";
            Print_String(&calib_string_ptr,line3,0,font10,0);

            calib_string_ptr = " =";
            Print_String(&calib_string_ptr,line4,0,font16,0);   // Zero load value
            while(Key_Input!=K_DEC && Key_Input!=HOR && Key_Input !=ESC)
            {
                zero_count_cal =    Enter_Variable2(zero_count_cal,line4,2,font16,6,0);   //  zero count manual entry
            }
            if(Key_Input==ESC)
            {
                    Calib_status=0;
                Clear_LCD();
                return;
            }
            if((Key_Input==K_DEC)||(Key_Input==HOR))
            {
                Key_Input=NO_KEY;
                super_delay(10);
            }
            Key_Input=NO_KEY;
            Clear_LCD();
            Calib_status=11;
}
if(Calib_status==11)
{
            Key_Input=NO_KEY;
            key_check_routine();
            if(Key_Input==ESC){return;}
            calib_string_ptr = "Old Span Counts ";
            Clear_String(&calib_string_ptr,line1,0,font10);
            sprintf(calib_string,"Old Counts=%lu ",calib_val[6]);
            calib_string_ptr=calib_string;
            Print_String(&calib_string_ptr,line2,0,font10,0);   // Zero load value

            calib_string_ptr = "Full scale Counts";
            Print_String(&calib_string_ptr,line3,0,font10,0);

            calib_string_ptr = " =";
            Print_String(&calib_string_ptr,line4,0,font16,0);   // Zero load value
            cnts=calib_val[6];
            while(Key_Input!=K_DEC && Key_Input!=HOR && Key_Input !=ESC)
            {
                cnts =    Enter_Variable2(cnts,line4,2,font16,6,0);   //  full scale counts manual entry
            }
            Clear_LCD();
            if(Key_Input==ESC)
            {
                    Calib_status=0;
                return;
            }
            else
            {
                if(cnts >= zero_count_cal)
                {
                    //Calib_Load=1000;    //calib_display load is assumed to be 100.0% as full scale counts are entered
                    full_load=cnts; ///100% load value
                    //Span_Count=((Present_Count-Count_DL)*Scale_Capacity)/Calib_Load    ;
                    //Span_Count=((cnts - Count_DL));
                    //Span_Count_Q = Span_Count / Calib_Load;             // Calib_Load is the weight we are entering
                    //Span_Count_R = Span_Count%Calib_Load;
                    //full_load= ((Span_Count_Q*1000)+Span_Count_R*1000/Calib_Load)+zero_count_cal;        //as calib load entered is 4 digit long so Q & R are divided by 1000
                    //full_load= ((Span_Count_Q*1000)+Span_Count_R*1000/Calib_Load);    //zero load not added, manual entry     //as calib load entered is 4 digit long so Q & R are divided by 1000
                    Span_Count=full_load-zero_count_cal;

                    calib_string_ptr = "LoadCell Calib OK";
                    Clear_String(&clear_string,line2,0,font16);
                    Print_String(&calib_string_ptr,line2,0,font16,0);
                    super_delay(20);

                    calib_val[0] = full_load;//Count_DL;
                    calib_val[1] = Span_Count;
                    calib_val[2] =  (unsigned long)zero_count_cal;//Count_DL;
                    calib_val[6]=cnts;
                    zero_load=zero_count_cal;
                    save_calib_val();

                    Calib_status=13;
                }
                else
                {
                    calib_string_ptr = "Span Calib Error";
                    Clear_String(&clear_string,line2,0,font16);
                    Print_String(&calib_string_ptr,line2,0,font16,0);
                    Clear_String(&clear_string,line3,0,font16);
                    calib_string_ptr = "Press any Key to";
                    Clear_String(&clear_string,line3,0,font16);
                    Print_String(&calib_string_ptr,line3,0,font16,0);
                    calib_string_ptr = "Exit";
                    Clear_String(&clear_string,line4,0,font16);
                    Print_String(&calib_string_ptr,line4,0,font16,0);
                    Calib_status=13;
                }
        }
}
if(Calib_status==12)
{
    //counts are displayed in another routine

            if(Key_Input == ESC){        Calib_status=0;    Clear_LCD();return;}
            //goto calib_jump3;
}
if(Calib_status==13)
{
    Clear_String(&clear_string,line1,0,font16);
            sprintf(calib_string,"Press ESC");
        calib_string_ptr=calib_string;
            Print_String(&calib_string_ptr,line4,0,font16,0);
    if(Key_Input==ESC){        Calib_status=0;    Clear_LCD();return;}
}
if(Calib_status==14)
{
    if(Key_Input==ESC){        Calib_status=0;    Clear_LCD();return;}
}
calib_jump3:
if(scan_count==49 || scan_count==149) /*99   299*/
    calib_display();
}
if(scan_count>=200) /*400*/
    scan_count=0;

goto calib_jump1;

}
//--------------------------------------
void Loadcell_ch2_calib_display(void )
{

if(Calib_status==1)
    {
      zero_load_ch2 = Present_Count_ch2;//bsf_adc_final_val;
            Clear_String(&clear_string,line2,0,font16);
            sprintf(calib_string,"Wt=%lu ",zero_load_ch2);
        calib_string_ptr=calib_string;
            Print_String(&calib_string_ptr,line2,0,font16,0);   // Zero load value

    }
if(Calib_status==3)
    {
            Clear_String(&clear_string,line3,10,font16);
            sprintf(calib_string,"Wt=%lu ",Present_Count_ch2);
            calib_string_ptr=calib_string;
            Print_String(&calib_string_ptr,line3,0,font16,0);   // Calib load value

    }

return;
}
/**********************************************************************************
//    Calibration()
This routine is called for calibration process
**********************************************************************************/
void     Loadcell_ch2_Calibration (void)
{
    double full_load_ch2_R,full_load_ch2_Q,scale_cap2;

    Key_Input=NO_KEY;
    Clear_LCD();
    super_delay(40);
    calib_string_ptr = "Calibration";
    Print_String(&calib_string_ptr,line1,0,font16,0);
    calib_string_ptr = "Clear Old data &";
    Print_String(&calib_string_ptr,line2,0,font16,0);
    calib_string_ptr = "start new Calib?";
    Print_String(&calib_string_ptr,line3,0,font16,0);
    calib_string_ptr = "->-YES,Return-NO";
    Print_String(&calib_string_ptr,line4,0,font16,0);
    super_delay(40);
    Calib_status=0;
    calib_jump1:             // keep executing loop until 5 ms Timer0 interrupt routine
//    loadcell_read();
if(flag==1)    // flag becomes 1 after every 5 ms (Timer0 tick)
    {
        flag=0;
        read_adc_conversion();        //load cell read
//        Present_Count = adc_final_val;
    Key_Input=NO_KEY;

    if(scan_count%20 ==0)
        key_check_routine();

if(    Key_Input==NO_KEY)
    goto calib_jump3;              //display calibration routine
if(Calib_status ==0)
{
        if(Key_Input==HOR)
        {
            Calib_status=1;
            Clear_LCD();
            calib_string_ptr = "Tare_Calibration";
            Print_String(&calib_string_ptr,line1,0,font10,0);

            calib_string_ptr = "Emty & Press->";
            Print_String(&calib_string_ptr,line3,0,font16,0);
            calib_string_ptr = "->Tare,Clear-exit";
            Print_String(&calib_string_ptr,line4,0,font16,0);
            super_delay(40);
        }
        else if(Key_Input==ESC)
        {
            Clear_LCD();
            return;        //exit routine if any other key pressed
        }
}

else if(Calib_status ==1)
{

        if(Key_Input==CL)
        {
            Calib_status=5;
            Clear_LCD();
            calib_string_ptr = "Calib Aborted";
            Print_String(&calib_string_ptr,line2,0,font16,0);
            Clear_String(&clear_string,line3,10,font16);
            calib_string_ptr = "Any Key- exit";
            Print_String(&calib_string_ptr,line4,0,font16,0);
        }
        if(Key_Input==HOR)
        {
            Calib_status=2;
            Clear_LCD();
            zero_load_ch2 = Present_Count_ch2 ; //bsf_adc_final_val;                 ////       0% load value
            Count_DL_ch2 = zero_load_ch2;
            calib_string_ptr = "Tare Calib OK";
            Clear_String(&clear_string,line2,0,font16);
            Print_String(&calib_string_ptr,line2,0,font16,0);
            Clear_String(&clear_string,line3,0,font16);
            calib_string_ptr = "Down-Next";
            Clear_String(&clear_string,line4,0,font16);
            Print_String(&calib_string_ptr,line4,0,font16,0);
        }
        goto calib_jump3;
    }
else if(Calib_status ==2)
{
    if(Key_Input==CL)
        {
            Calib_status=5;
            Clear_LCD();
            calib_string_ptr = "Calib Aborted";
            Print_String(&calib_string_ptr,line2,0,font16,0);
            Clear_String(&clear_string,line3,10,font16);
            calib_string_ptr = "Any Key- exit";
            Print_String(&calib_string_ptr,line4,0,font16,0);
        }

    if(Key_Input==K_DEC)
        {
            Calib_status=3;
            Clear_LCD();
            calib_string_ptr = "LoadCell Calibr";
            Clear_String(&clear_string,line1,0,font16);
            Print_String(&calib_string_ptr,line1,0,font16,0);
            calib_string_ptr = "Load=";

            Clear_String(&clear_string,line2,0,font16);
            Print_String(&calib_string_ptr,line2,0,font16,0);

            calib_string_ptr = "0-9 ,Ver- Span";
            Clear_String(&clear_string,line4,0,font16);
            Print_String(&calib_string_ptr,line4,0,font16,0);
            if(Units_Totaliser1==0)
             {
                    calib_string_ptr = "Kg";
                    Print_String(&calib_string_ptr,line2,55,font16,0);
             }
            else
             {
                    calib_string_ptr = "t";
                    Print_String(&calib_string_ptr,line2,55,font16,0);
             }
                //Print_String((unit_string_t1[0]),line2,38,font10,0);
            Calib_Load_ch2 =    Enter_Variable2(Calib_Load_ch2,line3,20,font16,6,Dec_Pt_Totaliser1);   //  known load value used for calibration

            if(Key_Input==ESC)
                {
                    Clear_LCD();
                    return;
                }
        }
}

if(Calib_status ==3)
{
    while(Key_Input!=K_DEC && Key_Input!=HOR && Key_Input !=ESC)
        Calib_Load_ch2 =    Enter_Variable2(Calib_Load_ch2,line3,20,font16,6,Dec_Pt_Totaliser1); //  known load value used for calibration

    if(Key_Input==ESC)
        {
            Clear_LCD();
            return;
        }

        sprintf(calib_string,"Load=");
        calib_string_ptr=calib_string;
        Clear_String(&clear_string,line2,0,font16);
        Print_String(&calib_string_ptr,line2,0,font16,0);
        if(Units_Totaliser1==0)
             {
                    calib_string_ptr = "Kg";
                    Print_String(&calib_string_ptr,line2,55,font16,0);
             }
            else
             {
                    calib_string_ptr = "t";
                    Print_String(&calib_string_ptr,line2,55,font16,0);
             }
        Print_Variablex(Calib_Load_ch2,line2,20,font16,0,Dec_Pt_Totaliser1,30);

        scale_cap2 = (float)Capacity_1/1000;   //Hopper Capacity;

    if(Key_Input==HOR)
        {
            Calib_status =4;
        if(Present_Count_ch2 >= zero_load_ch2)
            {
/*
//                full_load_ch2=Present_Count_ch2 ; ///100% load value
//                Span_Count_ch2=((full_load_ch2 - zero_load_ch2));

//              Span_Count=( ( ( Present_Count-Count_DL ) * Scale_Capacity) / Calib_Load    )+ Count_DL;
//                Span_Count=((Present_Count-Count_DL)*scale_cap2);
//                Span_Count=((Present_Count - Count_DL)*scale_cap2);
//                Span_Count_Q = Span_Count / Calib_Load;             // Calib_Load is the weight we are entering
//                Span_Count_R = Span_Count % Calib_Load;
//                Span_Count= ((Span_Count_Q*1000)+(Span_Count_R*1000)/Calib_Load)+Count_DL;

              //Span_Count_Q = Span_Count / Calib_Load;             // Calib_Load is the weight we are entering
                //Span_Count_R = Span_Count % Calib_Load;
                //Span_Count= ((Span_Count_Q*1000)+(Span_Count_R*1000)/Calib_Load)+Count_DL;

//              Clear_String(&clear_string,line2,10,font16);
//                sprintf(calib_string,"Wt=%lu ",Span_Count    );
//                calib_string_ptr=calib_string;
//                Print_String(&calib_string_ptr,line2,0,font16,0);   // 0 % load value
//                zero_count_cal= Scale_Weight    ;


*/
                full_load_ch2=((Present_Count_ch2 -zero_load_ch2)*scale_cap2);
                full_load_ch2_Q = full_load_ch2 / Calib_Load_ch2;             // Calib_Load is the weight we are entering
                full_load_ch2_R = full_load_ch2 % Calib_Load_ch2;
                full_load_ch2= ((full_load_ch2_Q*1000)+(full_load_ch2_R*1000)/Calib_Load_ch2)+zero_load_ch2;
                Span_Count_ch2 = ((full_load_ch2 - zero_load_ch2));

                calib_string_ptr = "LoadCell Calib OK";
                Clear_String(&clear_string,line2,0,font16);
                Print_String(&calib_string_ptr,line2,0,font16,0);
                super_delay(40);

                calib_val[3] = Present_Count_ch2 ;
                calib_val[4] = Span_Count_ch2;
                calib_val[5] = zero_load_ch2;//Count_DL;

                save_calib_val();
            }
            else
                {
                    calib_string_ptr = "Span Calib Error";
                    Clear_String(&clear_string,line2,0,font16);
                    Print_String(&calib_string_ptr,line2,0,font16,0);
                    Clear_String(&clear_string,line3,0,font16);
                    calib_string_ptr = "Press any Key to";
                    Clear_String(&clear_string,line3,0,font16);
                    Print_String(&calib_string_ptr,line3,0,font16,0);
                    calib_string_ptr = "Exit";
                    Clear_String(&clear_string,line4,0,font16);
                    Print_String(&calib_string_ptr,line4,0,font16,0);
                }
        }
        goto calib_jump3;
    }
else if(Calib_status==4)
{
    if(Key_Input != NO_KEY)
    {
        Clear_LCD();
        return;
    }
}
else if(Calib_status==5)
{
    if(Key_Input != NO_KEY)
    {
        Clear_LCD();
        return;

    }
}

calib_jump3:
if(scan_count==49 || scan_count==149) /*99   299*/
    Loadcell_ch2_calib_display();

}
if(scan_count>=200) /*400*/
    scan_count=0;

goto calib_jump1;

 }

void calib_init_display()
{

    Clear_LCD();
//    super_delay(40);
//    calib_string_ptr = "Calibration";
//    Print_String(&calib_string_ptr,line1,0,font16,0);
//    calib_string_ptr = "Clear Old data &";
//    Print_String(&calib_string_ptr,line2,0,font16,0);
//    calib_string_ptr = "start new Calib?";
//    Print_String(&calib_string_ptr,line3,0,font16,0);
//    calib_string_ptr = "->-YES, Return-NO";
//    Print_String(&calib_string_ptr,line4,0,font16,0);
//    super_delay(40);
    calib_string_ptr = "Select Channel:";
    Print_String(&calib_string_ptr,line1,0,font16,0);
    calib_string_ptr = "Channel1?<>";
    Print_String(&calib_string_ptr,line2,0,font16,0);
    calib_string_ptr = "Channel2?";
    Print_String(&calib_string_ptr,line3,0,font16,0);
    Key_Input=NO_KEY;
    cal_disp_status =1;
calib_jmp1:             //  keep executing loop until 5 msec interrupt  rotine
    if(flag==1)    // flag becomes 1 after every 5 ms (Timer0 tick)
    {
        flag=0;
        Key_Input=NO_KEY;

    if(scan_count%20 ==0)
                key_check_routine();
    if(    Key_Input==NO_KEY)
    goto calib_jmp1;              //display calibration routine

    if((Key_Input == HOR)&&(cal_disp_status == 0))
            {
                            cal_disp_status=1;
                            Key_Input = NO_KEY;
                            Clear_LCD();
            }

    if((Key_Input == K_DEC)&&(cal_disp_status == 1)&&(Pre_Hopper==1))
            {
                            cal_disp_status=2;
                            Key_Input = NO_KEY;
                            Clear_LCD();

            }
    if((Key_Input == K_INC)&&(cal_disp_status== 2))
            {
                            cal_disp_status=1;
                            Key_Input = NO_KEY;
                            Clear_LCD();

            }
    if((Key_Input == HOR)&&(cal_disp_status == 1))
            {
                            Calibration ();
                    Calib_status=0;
                            Key_Input = NO_KEY;

            }
    if((Key_Input == HOR)&&(cal_disp_status == 2))
            {
                            Loadcell_ch2_Calibration();
                            Key_Input = NO_KEY;

            }

            if(cal_disp_status ==1)
                {
                    //Clear_LCD();
                        calib_string_ptr = "Select Channel:";
                        Print_String(&calib_string_ptr,line1,0,font16,0);
                        calib_string_ptr = "Channel1?<>";
                        Print_String(&calib_string_ptr,line2,0,font16,0);
                        calib_string_ptr = "Channel2?";
                        Print_String(&calib_string_ptr,line3,0,font16,0);
                    //super_delay(10);
                }
            if(cal_disp_status ==2)
            {

                calib_string_ptr = "Select Channel:";
                Print_String(&calib_string_ptr,line1,0,font16,0);
                calib_string_ptr = "Channel1?";
                Print_String(&calib_string_ptr,line2,0,font16,0);
                calib_string_ptr = "Channel2?<>";
                Print_String(&calib_string_ptr,line3,0,font16,0);
                //super_delay(10);
            }
    if((Key_Input == ESC))
            {
                    Clear_LCD();
                    cal_disp_status =0;
                            Key_Input = NO_KEY;
                            return;

            }

    }
    goto calib_jmp1;

}

void AI_calib_display()
{
    Read_adc_ltc1288() ;
    AI_adc_avg_cal();
if((AI_cal_status==4))
    {
          //Min_adc1_val = AI_Value;
          Min_adc1_val = AI_avg;
        Clear_String(&clear_string,line3,0,font16);
            sprintf(calib_string,"%u ",Min_adc1_val );
            calib_string_ptr=calib_string;
            Print_String(&calib_string_ptr,line3,0,font16,0);

    }
if(AI_cal_status==5)
  {
        //Max_adc1_val=AI_Value;
      Max_adc1_val=AI_avg;
        Clear_String(&clear_string,line3,0,font16);
            sprintf(calib_string,"%u ",Max_adc1_val);
            calib_string_ptr=calib_string;
            Print_String(&calib_string_ptr,line3,0,font16,0);
    }
if((AI_cal_status==6))
    {
          Min_adc2_val = AI1_Value;
        Clear_String(&clear_string,line3,0,font16);
            sprintf(calib_string,"%u ",Min_adc2_val );
            calib_string_ptr=calib_string;
            Print_String(&calib_string_ptr,line3,0,font16,0);

    }
if(AI_cal_status==7)
  {    Max_adc2_val=AI1_Value;
        Clear_String(&clear_string,line3,0,font16);
            sprintf(calib_string,"%u ",Max_adc2_val);
            calib_string_ptr=calib_string;
            Print_String(&calib_string_ptr,line3,0,font16,0);
    }
if((AI_cal_status==8))
    {
          Min_adc3_val = AI2_Value;
        Clear_String(&clear_string,line3,0,font16);
            sprintf(calib_string,"%u ",Min_adc3_val );
            calib_string_ptr=calib_string;
            Print_String(&calib_string_ptr,line3,0,font16,0);

    }
if(AI_cal_status==9)
  {    Max_adc3_val=AI2_Value;
        Clear_String(&clear_string,line3,0,font16);
            sprintf(calib_string,"%u ",Max_adc3_val);
            calib_string_ptr=calib_string;
            Print_String(&calib_string_ptr,line3,0,font16,0);
    }

}

void AI_Calibration()
{
    /*adc configuration*/
    AI_cal_status=0;  // At start make status zero to avoid false Routine
    temp_status=0;    // At start make status zero to avoid false Routine

        Clear_LCD();
        super_delay(35);
        calib_string_ptr = "AI-Calibration";
        Print_String(&calib_string_ptr,line1,0,font16,0);
        calib_string_ptr = "Re-Calibrate ?";
        Print_String(&calib_string_ptr,line2,0,font16,0);
        calib_string_ptr = "->-YES,Return-NO";
        Print_String(&calib_string_ptr,line3,0,font16,0);
        super_delay(35);

    AI_calib_jump1:
            if(flag==1)//flag becomes 1 after every 5 ms (Timer0 tick)
                    {
                                Key_Input=NO_KEY;
                                            if(scan_count%40 ==0)  /* every 40th scan: 40 x 5 ms = 200 ms */
                                                    {
                                                        key_check_routine(); //check for key press

                                                    }
                                            if(scan_count%25 ==0) //every 35th/20th scan
                                               {
                                                    if((AI_cal_status == 4)||( AI_cal_status==5 ))
                                                        {
                                                            Read_adc_ltc1288(); // Read Val from 12bit adc  for AI calibration
                                                            AI_adc_avg_cal();
                                                        }
                                                    if((AI_cal_status==6 )||( AI_cal_status==7 ))
                                                        {
                                                         Read_adc1_ltc1288(); // Read Val from 12bit adc  for AI calibration
                                                        }
                                                    if((AI_cal_status==8 )||( AI_cal_status==9 ))
                                                        {
                                                         Read_adc2_ltc1288(); // Read Val from 12bit adc  for AI calibration
                                                        }

                                                 }
                                                    if(    Key_Input==NO_KEY)
                                                            goto AI_calib_jump2;              //display calibration routine

                                             if((Key_Input == HOR)&&(AI_cal_status == 0))
                                                    {
                                                                    AI_cal_status=1;
                                                                    Key_Input = NO_KEY;

                                                    }

                                            if((Key_Input == K_DEC)&&(AI_cal_status == 1)&&(AI_Nos>0))
                                                    {
                                                                    AI_cal_status=2;
                                                                    Key_Input = NO_KEY;

                                                    }
                                            if((Key_Input == K_INC)&&(AI_cal_status == 2))
                                                    {
                                                                    AI_cal_status=1;
                                                                    Key_Input = NO_KEY;

                                                    }

                                            if((Key_Input == K_DEC)&&(AI_cal_status == 2)&&(AI_Nos>1))
                                                    {
                                                                    AI_cal_status=3;
                                                                    Key_Input = NO_KEY;

                                                    }
                                            if((Key_Input == K_INC)&&(AI_cal_status == 3))
                                                    {
                                                                    AI_cal_status= 2;
                                                                    Key_Input = NO_KEY;

                                                    }
                                            if((Key_Input == HOR) && (AI_cal_status == 1))
                                                    {

                                                                    Key_Input=NO_KEY;
                                                                    AI_cal_status=4;

                                                    }
                                            if((Key_Input==HOR)&&(AI_cal_status ==2))    //
                                                    {

                                                                    Key_Input=NO_KEY;
                                                                    AI_cal_status=6;
                                                    }
                                            if((Key_Input==HOR)&&(AI_cal_status==3))    //
                                                    {

                                                                    Key_Input=NO_KEY;
                                                                    AI_cal_status=8;
                                                    }

                                            if((Key_Input==K_DEC)&&(AI_cal_status==4))    //
                                                    {

                                                                    AI_cal_status=5;
                                                                    Key_Input = NO_KEY;
                                                    }
                                            if((Key_Input==K_DEC)&&(AI_cal_status==5))    //
                                                    {
                                                            if(Max_adc1_val<Min_adc1_val)
                                                            {
                                                                Clear_LCD();
                                                                calib_string_ptr = "AI Calib Error";
                                                                Clear_String(&clear_string,line2,0,font16);
                                                                Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                calib_string_ptr = "Press Enter";
                                                                Clear_String(&clear_string,line3,0,font16);
                                                                Print_String(&calib_string_ptr,line3,0,font16,0);

                                                                AI_cal_status=10;
                                                          }
                                                        else
                                                            {
                                                                Clear_LCD();
                                                                calib_string_ptr = "AI Calib ok";
                                                                Clear_String(&clear_string,line2,0,font16);
                                                                Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                calib_string_ptr = "Press Enter";
                                                                Clear_String(&clear_string,line3,0,font16);
                                                                Print_String(&calib_string_ptr,line3,0,font16,0);
                                                                AI_cal_status=1;
                                                                AI_calb_val[0] = Min_adc1_val;       ////save  min value in RAM adrees of RAM location = 0x10001184
                                                                AI_calb_val[1] = Max_adc1_val;  //save  max value in RAM adrees of RAM location = 0x10001188

                                                            }
                                                                AI_cal_status=10;
                                                                Key_Input = NO_KEY;
                                                            }

                                            if((Key_Input==K_DEC)&&(AI_cal_status==6))    //
                                                    {

                                                                    AI_cal_status=7;
                                                                    Key_Input = NO_KEY;
                                                    }
                                            if((Key_Input==K_DEC)&&(AI_cal_status==7))    //
                                                    {
                                                            if(Max_adc2_val<Min_adc2_val)
                                                            {
                                                                Clear_LCD();
                                                                calib_string_ptr = "AI Calib Error";
                                                                Clear_String(&clear_string,line2,0,font16);
                                                                Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                calib_string_ptr = "Press Enter";
                                                                Clear_String(&clear_string,line3,0,font16);
                                                                Print_String(&calib_string_ptr,line3,0,font16,0);
                                                                AI_cal_status=10;
                                                          }
                                                        else
                                                            {
                                                                Clear_LCD();
                                                                calib_string_ptr = "AI Calib ok";
                                                                Clear_String(&clear_string,line2,0,font16);
                                                                Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                calib_string_ptr = "Press Enter";
                                                                Clear_String(&clear_string,line3,0,font16);
                                                                Print_String(&calib_string_ptr,line3,0,font16,0);
                                                                AI_cal_status=1;
                                                                AI_calb_val[2] = Min_adc2_val;       ////save  min value in RAM adrees of RAM location = 0x10001184
                                                                AI_calb_val[3] = Max_adc2_val;  //save  max value in RAM adrees of RAM location = 0x10001188

                                                            }
                                                                AI_cal_status=10;
                                                                Key_Input = NO_KEY;

                                                    }

                                            if((Key_Input==K_DEC)&&(AI_cal_status==8))    //
                                                    {

                                                                    AI_cal_status=9;
                                                                    Key_Input = NO_KEY;
                                                    }
                                            if((Key_Input==K_DEC)&&(AI_cal_status==9))    //
                                                    {

                                                        if(Max_adc3_val<Min_adc3_val)
                                                            {
                                                                Clear_LCD();
                                                                calib_string_ptr = "AI Calib Error";
                                                                Clear_String(&clear_string,line2,0,font16);
                                                                Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                calib_string_ptr = "Press Enter";
                                                                Clear_String(&clear_string,line3,0,font16);
                                                                Print_String(&calib_string_ptr,line3,0,font16,0);
                                                                AI_cal_status=10;
                                                          }
                                                        else
                                                            {
                                                                Clear_LCD();
                                                                calib_string_ptr = "AI Calib ok";
                                                                Clear_String(&clear_string,line2,0,font16);
                                                                Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                calib_string_ptr = "Press Enter";
                                                                Clear_String(&clear_string,line3,0,font16);
                                                                Print_String(&calib_string_ptr,line3,0,font16,0);
                                                                AI_cal_status=1;
                                                                AI_calb_val[4] = Min_adc3_val;       ////save  min value in RAM adrees of RAM location = 0x10001184
                                                                AI_calb_val[5] = Max_adc3_val;  //save  max value in RAM adrees of RAM location = 0x10001188

                                                            }
                                                                AI_cal_status=10;
                                                                Key_Input = NO_KEY;

                                                    }

                                            if((Key_Input==ESC)&&(AI_cal_status<4))    //
                                                    {
                                                        if((AI_cal_status==1)||(AI_cal_status==2)||(AI_cal_status==3))
                                                            {
                                                                        Key_Input = NO_KEY;
                                                                        PINSEL_Configure();           //Reintialise SPI pin for SPI Communication
                                                                        save_AI_calib_val();    //Save Analog calibration Values in EEPROM
                                                                        return;
                                                            }
                                                        if(AI_cal_status==0)
                                                            {
                                                                        Key_Input = NO_KEY;
                                                                        PINSEL_Configure();           //Reintialise SPI pin for SPI Communication
                                                                        return;
                                                            }
                                                    }

                                            if(AI_cal_status==1)
                                                {
                                                                    Clear_LCD();
                                                                    calib_string_ptr = "Select Channel:";
                                                                    Print_String(&calib_string_ptr,line1,0,font16,0);
                                                                    calib_string_ptr = "Channel1?<>";
                                                                    Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                    calib_string_ptr = "Channel2?";
                                                                    Print_String(&calib_string_ptr,line3,0,font16,0);
                                                                    calib_string_ptr = "Channel3?";
                                                                    Print_String(&calib_string_ptr,line4,0,font16,0);
                                                                    //super_delay(10);
                                                }
                                            if(AI_cal_status==2)
                                                {
                                                                    Clear_LCD();
                                                                    calib_string_ptr = "Select Channel:";
                                                                    Print_String(&calib_string_ptr,line1,0,font16,0);
                                                                    calib_string_ptr = "Channel1?";
                                                                    Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                    calib_string_ptr = "Channel2?<>";
                                                                    Print_String(&calib_string_ptr,line3,0,font16,0);
                                                                    calib_string_ptr = "Channel3?";
                                                                    Print_String(&calib_string_ptr,line4,0,font16,0);
                                                                    //super_delay(10);
                                                }
                                            if(AI_cal_status==3)
                                                {
                                                                    Clear_LCD();
                                                                    calib_string_ptr = "Select Channel:";
                                                                    Print_String(&calib_string_ptr,line1,0,font16,0);
                                                                    calib_string_ptr = "Channel1?";
                                                                    Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                    calib_string_ptr = "Channel2?";
                                                                    Print_String(&calib_string_ptr,line3,0,font16,0);
                                                                    calib_string_ptr = "Channel3?<>";
                                                                    Print_String(&calib_string_ptr,line4,0,font16,0);
                                                                    //super_delay(10);
                                                }

                                            if(AI_cal_status==4)
                                                {

                                                                    Clear_LCD();

                                                                    calib_string_ptr = "Channel1 Calib:";
                                                                    Print_String(&calib_string_ptr,line1,0,font16,0);
                                                                    calib_string_ptr = "Min_Count:";
                                                                    Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                    Clear_String(&clear_string,line3,0,font16);
                                                                    sprintf(calib_string,"%u ",Min_adc1_val);
                                                                    calib_string_ptr=calib_string;
                                                                    Print_String(&calib_string_ptr,line3,0,font16,0);   //100 % load value
                                                                    //super_delay(10);
                                                                    AI_calb_val[0] = Min_adc1_val;

                                                }
                                            if(AI_cal_status==5)    // //Chanel 1 Max count display
                                                {
                                                                        Clear_LCD();

                                                                        calib_string_ptr = "Channel1 Calib:";
                                                                        Print_String(&calib_string_ptr,line1,0,font16,0);
                                                                        calib_string_ptr = "Max_Count:";
                                                                        Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                        Clear_String(&clear_string,line3,0,font16);
                                                                        sprintf(calib_string,"%lu ",Max_adc1_val);
                                                                        calib_string_ptr=calib_string;
                                                                        Print_String(&calib_string_ptr,line3,0,font16,0);   //100 % load value
                                                                    //    super_delay(10);
                                                                    AI_calb_val[1] = Max_adc1_val;
                                                }
                                            if(AI_cal_status==6)                        // //Chanel 2 min count display
                                                {
                                                                        Clear_LCD();

                                                                        calib_string_ptr = "Channel2 Calib:";
                                                                        Print_String(&calib_string_ptr,line1,0,font16,0);
                                                                        calib_string_ptr = "Min_Count:";
                                                                        Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                        Clear_String(&clear_string,line3,0,font16);
                                                                        sprintf(calib_string,"%u ",Min_adc2_val);
                                                                        calib_string_ptr=calib_string;
                                                                        Print_String(&calib_string_ptr,line3,0,font16,0);   //100 % load value
                                                                        //super_delay(10);
                                                        AI_calb_val[2] = Min_adc2_val;
                                                    }
                                            if(AI_cal_status==7)
                                                {
                                                                        Clear_LCD();

                                                                        calib_string_ptr = "Channel2 Calib:";
                                                                        Print_String(&calib_string_ptr,line1,0,font16,0);
                                                                        calib_string_ptr = "Max_Count:";
                                                                        Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                        Clear_String(&clear_string,line3,0,font16);
                                                                        sprintf(calib_string,"%u ",Max_adc2_val);
                                                                        calib_string_ptr=calib_string;
                                                                        Print_String(&calib_string_ptr,line3,0,font16,0);   //100 % load value
                                                                        //super_delay(10);
                                                                        AI_calb_val[3] = Max_adc2_val;
                                                        }
                                            if(AI_cal_status==8)                        // //Chanel 2 min count display
                                                {
                                                                        Clear_LCD();

                                                                        calib_string_ptr = "Channel3 Calib:";
                                                                        Print_String(&calib_string_ptr,line1,0,font16,0);
                                                                        calib_string_ptr = "Min_Count:";
                                                                        Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                        Clear_String(&clear_string,line3,0,font16);
                                                                        sprintf(calib_string,"%u ",Min_adc3_val);
                                                                        calib_string_ptr=calib_string;
                                                                        Print_String(&calib_string_ptr,line3,0,font16,0);   //100 % load value
                                                                        //super_delay(10);
                                                                        AI_calb_val[4] = Min_adc2_val;

                                                    }
                                            if(AI_cal_status==9)
                                                {
                                                                        Clear_LCD();

                                                                        calib_string_ptr = "Channel3 Calib:";
                                                                        Print_String(&calib_string_ptr,line1,0,font16,0);
                                                                        calib_string_ptr = "Max_Count:";
                                                                        Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                        Clear_String(&clear_string,line3,0,font16);
                                                                        sprintf(calib_string,"%u ",Max_adc3_val);
                                                                        calib_string_ptr=calib_string;
                                                                        Print_String(&calib_string_ptr,line3,0,font16,0);   //100 % load value
                                                                        //super_delay(10);
                                                                AI_calb_val[5] = Max_adc3_val;
                                                        }

                                            if((Key_Input==HOR)&&(AI_cal_status==10))    //
                                                {

                                                    AI_cal_status = 0;
                                                    //Key_Input = NO_KEY;
                                                }

                    }

                        AI_calib_jump2:
                                        if((scan_count >= 100 )&&(scan_count <= 110 ))  //Every 500msec
                                            AI_calib_display(); //Display ADC_Count i.e Min val & Max val

    if(scan_count>=200)
     scan_count=0;
    goto AI_calib_jump1;
}

//---------------------------------------------------------------------------------
void AO_calib_display(void )
{

 if(AO_Calib_status==4)// min count channel1
    {
    temp_AO_val=Min_Count_Channel1;
    }
else if(AO_Calib_status==5) // max count channel1
    {
        temp_AO_val=Max_Count_Channel1;

    }
 if(AO_Calib_status==6)// min count channel2
    {
        temp_AO_val=Min_Count_Channel2;
    }
 if(AO_Calib_status==7)// max count channel2
    {
        temp_AO_val=Max_Count_Channel2;

    }
 if(AO_Calib_status==8)// min count channel2
    {
        temp_AO_val=Min_Count_Channel3;
    }
 if(AO_Calib_status==9)// max count channel2
    {
        temp_AO_val=Max_Count_Channel3;

    }

if((AO_Calib_status==4)||(AO_Calib_status==5))
{
            Clear_String(&clear_string,line3,0,font16);
            //Print_String(&clear_string,line3,0,font16,0);
            sprintf(calib_string,"%lu",temp_AO_val);
        calib_string_ptr=calib_string;
            Print_String(&calib_string_ptr,line3,0,font16,0);

}

if((AO_Calib_status==6)||(AO_Calib_status==7))
  {
            Clear_String(&clear_string,line3,0,font16);
            sprintf(calib_string,"%lu ",temp_AO_val);
            calib_string_ptr=calib_string;
            Print_String(&calib_string_ptr,line3,0,font16,0);

  }
    if((AO_Calib_status==8)||(AO_Calib_status==9))
  {
            Clear_String(&clear_string,line3,0,font16);
            sprintf(calib_string,"%lu ",temp_AO_val);
            calib_string_ptr=calib_string;
            Print_String(&calib_string_ptr,line3,0,font16,0);

  }

return;
}
//-------------------------------------------------------------------
bool EditAOScaleFactors(bool flag1)
{
    bool inputflag;
    long long1=10000;
        static unsigned char smode=0;
    inputflag=flag1;

            Key_Input=NO_KEY;
            super_delay(30);
            //Clear_LCD();
            key_check_routine(); //check for key press
    switch(smode)
    {
        case 0:

            //super_delay(10);
            calib_string_ptr = "AO-Scale Factor Edit..";
            Print_String(&calib_string_ptr,line1,0,font10,0);
        calib_string_ptr = "ENTER : Continue";
            Print_String(&calib_string_ptr,line2,0,font10,0);
        calib_string_ptr = "CLEAR : To exit";
            Print_String(&calib_string_ptr,line3,0,font10,0);
//            calib_string_ptr = " CLEAR -> SCALE";
//            Print_String(&calib_string_ptr,line4,0,font16,0);
            //super_delay(10);

            if(Key_Input == HOR)
            {
                Clear_LCD();
                smode=1;
                inputflag=true;    //scale factor edit mode have ended
            }
            else if(Key_Input == CL)
            {
                Clear_LCD();
                inputflag=false;    //scale factor edit mode have ended
                smode=0;    //since smode is static, must be initialised for next use.
            }
        break;
        case 1:
            calib_string_ptr = "AO-1 Scale Factor ..";
            Print_String(&calib_string_ptr,line1,0,font10,0);
            calib_string_ptr = "50.00% to 200.00%";
            Print_String(&calib_string_ptr,line2,0,font10,0);
            calib_string_ptr = "CLEAR : To exit";
            Print_String(&calib_string_ptr,line3,0,font10,0);
        calib_string_ptr = "Scale Factor % = ";
            Print_String(&calib_string_ptr,line4,0,font10,0);
            long1 =    Enter_Variable4(Dac1_ScaleFactor,line5,1,font10,5,2);   //  zero count manual entry
            if((long1 < 5000)||(long1 > 20000))        //check out of range
            {
                    calib_string_ptr = "                 ";
                    Print_String(&calib_string_ptr,line5,0,font10,0);
                    calib_string_ptr = "Out of Range       ";
                    Print_String(&calib_string_ptr,line5,0,font10,0);
                super_delay(80);
                Clear_LCD();
                Key_Input=NO_KEY;
                long1=Dac1_ScaleFactor;
                inputflag=true;
                smode=1;
            }
            else
            {
                if(Key_Input == HOR)
                {
                Dac1_ScaleFactor=long1;
                    Clear_LCD();
                    smode=2;    //since smode is static, must be initialised for next use.
                }
                if(Key_Input == CL)
                {
                    Clear_LCD();
                    smode=2;    //since smode is static, must be initialised for next use.
                }
            }
        break;
        case 2:
            calib_string_ptr = "AO-2 Scale Factor ..";
            Print_String(&calib_string_ptr,line1,0,font10,0);
            calib_string_ptr = "50.00% to 200.00%";
            Print_String(&calib_string_ptr,line2,0,font10,0);
            calib_string_ptr = "CLEAR : To exit";
            Print_String(&calib_string_ptr,line3,0,font10,0);
            calib_string_ptr = "Scale Factor % = ";
            Print_String(&calib_string_ptr,line4,0,font10,0);
            long1 =    Enter_Variable4(Dac2_ScaleFactor,line5,1,font10,5,2);   //  zero count manual entry
            if((long1 < 5000)||(long1 > 20000))        //check out of range
            {
                    calib_string_ptr = "                 ";
                    Print_String(&calib_string_ptr,line5,0,font10,0);
                    calib_string_ptr = "Out of Range       ";
                    Print_String(&calib_string_ptr,line5,0,font10,0);
                super_delay(80);
                Clear_LCD();
                Key_Input=NO_KEY;
                long1=Dac2_ScaleFactor;
                inputflag=true;
                smode=1;
            }
            else
            {
                if(Key_Input == HOR)
                {
                    Dac2_ScaleFactor=long1;
                    Clear_LCD();
                    smode=3;    //since smode is static, must be initialised for next use.
                }
                if(Key_Input == CL)
                {
                    Clear_LCD();
                    smode=3;    //since smode is static, must be initialised for next use.
                }
            }
        break;
        case 3:
            calib_string_ptr = "AO-3 Scale Factor ..";
            Print_String(&calib_string_ptr,line1,0,font10,0);
            calib_string_ptr = "50.00% to 200.00%";
            Print_String(&calib_string_ptr,line2,0,font10,0);
            calib_string_ptr = "CLEAR : To exit";
            Print_String(&calib_string_ptr,line3,0,font10,0);
        calib_string_ptr = "Scale Factor % = ";
            Print_String(&calib_string_ptr,line4,0,font10,0);
            long1 =    Enter_Variable4(Dac3_ScaleFactor,line5,1,font10,5,2);   //  zero count manual entry
            if((long1 < 5000)||(long1 > 20000))        //check out of range
            {
                    calib_string_ptr = "                 ";
                    Print_String(&calib_string_ptr,line5,0,font10,0);
                    calib_string_ptr = "Out of Range       ";
                    Print_String(&calib_string_ptr,line5,0,font10,0);
                super_delay(80);
                Clear_LCD();
                Key_Input=NO_KEY;
                long1=Dac3_ScaleFactor;
                inputflag=true;
                smode=1;
            }
            else
            {
                if(Key_Input == HOR)
                {
                Dac3_ScaleFactor=long1;
                    Clear_LCD();
                    inputflag=false;    //scale factor edit mode have ended
                    smode=0;    //since smode is static, must be initialised for next use.
                    save_par_eeprom();
                }
                if(Key_Input == CL)
                {
                    Clear_LCD();
                    inputflag=false;    //scale factor edit mode have ended
                    smode=0;    //since smode is static, must be initialised for next use.
                    save_par_eeprom();
                }
            }
        break;
        case 4:

        break;
        default:
            smode=0;
            inputflag=false;
        break;
    }
    Key_Input=NO_KEY;
    return(inputflag);
}
//------------------------------------------------------------------
void AO_Calibration()
{
/*Configure Dac */
    static bool scaleflag=false;        // if set then menu for scale factor setting
    init_dac();
    AO_Calib_status = 0;
         Key_Input=NO_KEY;
       Clear_LCD();

    mainaocalentry:
    if(scaleflag == true)
    {
        scaleflag = EditAOScaleFactors(scaleflag);
        goto mainaocalentry;
    }
                    Key_Input=NO_KEY;
                    Clear_LCD();
                    super_delay(10);
                    calib_string_ptr = "AO-Calibration";
                    Print_String(&calib_string_ptr,line1,0,font16,0);
                    calib_string_ptr = "Re-Calibrate ?";
                    Print_String(&calib_string_ptr,line2,0,font16,0);
                    calib_string_ptr = "->-YES,Return-NO";
                    Print_String(&calib_string_ptr,line3,0,font16,0);
                    calib_string_ptr = " CLEAR -> SCALE";
                    Print_String(&calib_string_ptr,line4,0,font16,0);
                    super_delay(50);

 AO_calib_jump: // keep executing loop until 5 ms Timer0 interrupt routine

    if(flag==1)//flag becomes 1 after every 5 ms (Timer0 tick)
                    {
                                    flag=0;
                                        Key_Input=NO_KEY;

                                        if(scan_count%18 ==0)  //every 35th/20th scan
                                               {
                                                      //init_dac();
                                                        key_check_routine(); //check for key press
                                                        //init_dac();
                                                        //dac2_write(Dac2_Val);//Dac2_Val  instead of constant val /*Temp Solution*/
                                               }

                                        if(scan_count%27 ==0) //every 35th/20th scan
                                               {
                                                     if((AO_Calib_status ==4 )||( AO_Calib_status ==5 ))
                                                     {
                                                            init_dac();
                                                            dac1_write(Dac1_Val);//Dac1_Val  instead of constant val /*Temp Solution*/
                                                     }
                                                     if((AO_Calib_status ==6 )||( AO_Calib_status ==7 ))
                                                     {
                                                            init_dac();
                                                            dac2_write(Dac2_Val);//Dac2_Val  instead of constant val /*Temp Solution*/
                                                     }
                                                     if((AO_Calib_status ==8 )||( AO_Calib_status ==9 ))
                                                     {
                                                            init_dac();
                                                            dac3_write(Dac3_Val);//Dac2_Val  instead of constant val /*Temp Solution*/
                                                     }
                                                         //dac2_write(Dac1_Val);//Dac1_Val  instead of constant val /*Temp Solution*/
                                                 }
                                            if(    Key_Input==NO_KEY)
                                            {
                                                goto AO_calib_jump3;              //display calibration routine
                                            }
                                            if((Key_Input == CL)&&(AO_Calib_status == 0))
                                            {
                                                scaleflag=true;
                                                Clear_LCD();
                                                goto mainaocalentry;
                                            }
//                                            else
//                                            {
//                                                    goto AO_calib_jump3;
//                                            }
                                          if((Key_Input == HOR)&&(AO_Calib_status == 0))
                                                    {
                                                                    AO_Calib_status=1;
                                                                    Key_Input = NO_KEY;

                                                    }

                                            if((Key_Input == K_DEC)&&(AO_Calib_status == 1)&&(AO_Nos>0))
                                                    {
                                                                    AO_Calib_status=2;
                                                                    Key_Input = NO_KEY;

                                                    }
                                            if((Key_Input == K_INC)&&(AO_Calib_status == 2))
                                                    {
                                                                    AO_Calib_status=1;
                                                                    Key_Input = NO_KEY;

                                                    }

                                            if((Key_Input == K_DEC)&&(AO_Calib_status == 2)&&(AO_Nos>1))
                                                    {
                                                                    AO_Calib_status=3;
                                                                    Key_Input = NO_KEY;
                                                    }
                                            if((Key_Input == K_INC)&&(AO_Calib_status == 3))
                                                    {
                                                                    AO_Calib_status = 2;
                                                                    Key_Input = NO_KEY;
                                                    }

                                            if((Key_Input == HOR) && (AO_Calib_status == 1))
                                                    {
                                                                    Min_Count_Channel1=AO_calz1;
                                                                    Max_Count_Channel1=AO_cals1;
                                                        //read_AO_calib_val();
                                                        //read_AO_calib_val_ch1();        //msk 17-aug-25
                                                                    Esc_Status=0;
                                                                    Key_Input=NO_KEY;
                                                                    AO_Calib_status=4;
                                                    }
                                            if((Key_Input==HOR)&&(AO_Calib_status==2))    //
                                                    {
                                                                    Min_Count_Channel2=AO_calz2;
                                                                    Max_Count_Channel2=AO_cals2;
                                                        //ead_AO_calib_val();    //msk 17-aug-25
                                                                    Esc_Status=0;
                                                                    Key_Input=NO_KEY;
                                                                    AO_Calib_status=6;
                                                    }
                                            if((Key_Input==HOR)&&(AO_Calib_status==3))    //
                                                    {
                                                                    Min_Count_Channel3=AO_calz3;
                                                                    Max_Count_Channel3=AO_cals3;
                                                        //ead_AO_calib_val();    //msk 17-aug-25
                                                                    Esc_Status=0;
                                                                    Key_Input=NO_KEY;
                                                                    AO_Calib_status=8;
                                                    }

                                            if((Key_Input==HOR)&&(AO_Calib_status==4))    //
                                                    {
                                                                    AO_Calib_status=5;
                                                        //bytes_written = 0;
                                                        //super_delay(1);
                                                        //super_delay(30);
                                                        save_AO_calib_val();       //Save Analog calibration Values in EEPROM
                                                        Key_Input = NO_KEY;
                                                    }
                                            if((Key_Input==HOR)&&(AO_Calib_status==5))    //
                                                    {
                                                                    AO_Calib_status=1;
                                                        //bytes_written = 0;
                                                        //save_par_eeprom();
                                                        //super_delay(30);
                                                        save_AO_calib_val();       //Save Analog calibration Values in EEPROM
                                                        Key_Input = NO_KEY;
                                                    }

                                            if((Key_Input==HOR)&&(AO_Calib_status==6))    //
                                                    {
                                                        AO_Calib_status=7;
                                                        //bytes_written = 0;
                                                        //save_par_eeprom();
                                                        //super_delay(30);
                                                        save_AO_calib_val();       //Save Analog calibration Values in EEPROM
                                                        Key_Input = NO_KEY;
                                                    }
                                            if((Key_Input==HOR)&&(AO_Calib_status==7))    //
                                                    {
                                                        AO_Calib_status=1;
                                                        //bytes_written = 0;
                                                        //save_par_eeprom();
                                                        //super_delay(30);
                                                        save_AO_calib_val();       //Save Analog calibration Values in EEPROM
                                                        Key_Input = NO_KEY;
                                                    }

                                            if((Key_Input==HOR)&&(AO_Calib_status==8))    //
                                                    {
                                                        AO_Calib_status=9;
                                                        //bytes_written = 0;
                                                        //save_par_eeprom();
                                                        //super_delay(30);
                                                        save_AO_calib_val();       //Save Analog calibration Values in EEPROM
                                                        Key_Input = NO_KEY;
                                                    }
                                            if((Key_Input==HOR)&&(AO_Calib_status==9))    //
                                                    {

                                                        AO_Calib_status=1;
                                                        //bytes_written = 0;
                                                        //save_par_eeprom();
                                                        //super_delay(30);
                                                        save_AO_calib_val();       //Save Analog calibration Values in EEPROM
                                                        Key_Input = NO_KEY;
                                                    }

                                            if((Key_Input==ESC)&&(AO_Calib_status<4))    //
                                                    {
                                                                if(((AO_Calib_status==1)||(AO_Calib_status==2)||(AO_Calib_status==3)))
                                                                {
                                                                    Key_Input = NO_KEY;
                                                                    PINSEL_Configure();           //Reintialise SPI pin for SPI Communication
                                                                    save_AO_calib_val();       //Save Analog calibration Values in EEPROM
                                                                    //bytes_written = 0;
                                                                    //save_par_eeprom();
                                                                    //super_delay(30);
                                                                    return;
                                                                }
                                                                if(AO_Calib_status==0)
                                                                {
                                                                    Key_Input = NO_KEY;
                                                                    PINSEL_Configure();           //Reintialise SPI pin for SPI Communication
                                                                    return;
                                                                }
                                                    }

                                            if(AO_Calib_status==1)
                                                {
                                                                    Clear_LCD();
                                                                    calib_string_ptr = "Select Channel:";
                                                                    Print_String(&calib_string_ptr,line1,0,font16,0);
                                                                    calib_string_ptr = "Channel1?<>";
                                                                    Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                    calib_string_ptr = "Channel2?";
                                                                    Print_String(&calib_string_ptr,line3,0,font16,0);
                                                                    calib_string_ptr = "Channel3?";
                                                                    Print_String(&calib_string_ptr,line4,0,font16,0);
                                                                    //super_delay(10);
                                                }
                                            if(AO_Calib_status==2)
                                                {
                                                                    Clear_LCD();
                                                                    calib_string_ptr = "Select Channel:";
                                                                    Print_String(&calib_string_ptr,line1,0,font16,0);
                                                                    calib_string_ptr = "Channel1?";
                                                                    Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                    calib_string_ptr = "Channel2?<>";
                                                                    Print_String(&calib_string_ptr,line3,0,font16,0);
                                                                    calib_string_ptr = "Channel3?";
                                                                    Print_String(&calib_string_ptr,line4,0,font16,0);
                                                                    //super_delay(10);
                                                }
                                            if(AO_Calib_status==3)
                                                {
                                                                    Clear_LCD();
                                                                    calib_string_ptr = "Select Channel:";
                                                                    Print_String(&calib_string_ptr,line1,0,font16,0);
                                                                    calib_string_ptr = "Channel1?";
                                                                    Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                    calib_string_ptr = "Channel2?";
                                                                    Print_String(&calib_string_ptr,line3,0,font16,0);
                                                                    calib_string_ptr = "Channel3?<>";
                                                                    Print_String(&calib_string_ptr,line4,0,font16,0);
                                                                    //super_delay(10);
                                                }

                                            if(AO_Calib_status==4)
                                                {

                                                                    Clear_LCD();

                                                                    calib_string_ptr = "Channel1 Calib:";
                                                                    Print_String(&calib_string_ptr,line1,0,font16,0);
                                                                    calib_string_ptr = "Min_Count:";
                                                                    Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                    Clear_String(&clear_string,line3,0,font16);
                                                                    sprintf(calib_string,"%lu ",Min_Count_Channel1);
                                                                    calib_string_ptr=calib_string;
                                                                    Print_String(&calib_string_ptr,line3,0,font16,0);   //100 % load value
                                                    Dac1_Val = Min_Count_Channel1;
                                                                    //super_delay(10);
                                                }

                                          if((Key_Input==K_INC)&&(AO_Calib_status==4))    // //Chanel 1 Min count ++
                                                {

                                                                    Key_Input=NO_KEY;
                                                                    Min_Count_Channel1++;
                                                                    if(Min_Count_Channel1>3900)/*Temp*/
                                                                        {
                                                                            Min_Count_Channel1=3900;
                                                                        }
                                                                    AO_calz1=Min_Count_Channel1;

                                                                    Dac1_Val = Min_Count_Channel1;
                                                                    dacop1=4000;
                                                }
                                                if((Key_Input==Setpoint)&&(AO_Calib_status==4))    // //Chanel 1 Min count ++ by 50 msk 17-aug-25
                                                {

                                                                    Key_Input=NO_KEY;
                                                                    Min_Count_Channel1+=50;
                                                                    if(Min_Count_Channel1>3900)/*Temp*/
                                                                        {
                                                                            Min_Count_Channel1=3900;
                                                                        }
                                                                    AO_calz1=Min_Count_Channel1;

                                                                    Dac1_Val = Min_Count_Channel1;
                                                                    dacop1=4000;    //msk 07-dec-2025
                                                }
                                            if((Key_Input==K_DEC)&&(AO_Calib_status==4))    // //Chanel 1 Min count --
                                                {
                                                                    Key_Input=NO_KEY;
                                                                    --Min_Count_Channel1;
                                                                    if((Min_Count_Channel1 < 1)||(Min_Count_Channel1> 3900))/*Temp*/
                                                                        {
                                                                            Min_Count_Channel1=0;
                                                                        }
                                                                        AO_calz1=Min_Count_Channel1;
                                                                        Dac1_Val = Min_Count_Channel1;
                                                                        dacop1=4000;    //msk 07-dec-2025
                                                }
                                                if((Key_Input==k_Load)&&(AO_Calib_status==4))    // //Chanel 1 Min count -- down by 50 msk-17-aug-25
                                                {
                                                                    Key_Input=NO_KEY;
                                                                    Min_Count_Channel1-=50;
                                                                    if((Min_Count_Channel1 < 1)||(Min_Count_Channel1 > 3900))/*Temp*/
                                                                        {
                                                                            Min_Count_Channel1=0;
                                                                        }
                                                                        AO_calz1=Min_Count_Channel1;
                                                                        Dac1_Val = Min_Count_Channel1;
                                                                        dacop1=4000;    //msk 07-dec-2025
                                                }
                                            if(  AO_Calib_status == 5 )    // //Chanel 1 Max count display
                                                {
                                                                        Clear_LCD();

                                                                        calib_string_ptr = "Channel1 Calib:";
                                                                        Print_String(&calib_string_ptr,line1,0,font16,0);
                                                                        calib_string_ptr = "Max_Count:";
                                                                        Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                        Clear_String(&clear_string,line3,0,font16);
                                                                        sprintf(calib_string,"%lu ",Max_Count_Channel1);
                                                                        calib_string_ptr=calib_string;
                                                                        Print_String(&calib_string_ptr,line3,0,font16,0);   //100 % load value
                                                    Dac1_Val = Max_Count_Channel1;
                                                                    //    super_delay(10);
                                                }
                                            if((Key_Input==K_INC)&&(AO_Calib_status==5))    // //Chanel 1 Max count ++
                                                {
                                                                        Key_Input=NO_KEY;
                                                                        Max_Count_Channel1++;
                                                                        if(Max_Count_Channel1>3900)/*Temp*/
                                                                            {
                                                                                Max_Count_Channel1=3900;
                                                                            }
                                                                        AO_cals1=Max_Count_Channel1;
                                                                        Dac1_Val = Max_Count_Channel1;
                                                                            dacop1=4000;    //msk 07-dec-2025
                                                }
                                            if((Key_Input==Setpoint)&&(AO_Calib_status==5))    // //Chanel 1 Max count ++
                                                {
                                                                        Key_Input=NO_KEY;
                                                                        Max_Count_Channel1+=50;
                                                                        if(Max_Count_Channel1>3900)/*Temp*/
                                                                            {
                                                                                Max_Count_Channel1=3900;
                                                                            }
                                                                        AO_cals1=Max_Count_Channel1;
                                                                        Dac1_Val = Max_Count_Channel1;
                                                                            dacop1=4000;    //msk 07-dec-2025
                                                }
                                                if((Key_Input == K_DEC )&&(AO_Calib_status==5))    // //Chanel 1 Max count --
                                                {
                                                                        Key_Input=NO_KEY;
                                                                    --Max_Count_Channel1;
                                                                        if((Max_Count_Channel1 < 1)||(Max_Count_Channel1 > 3900))/*Temp*/
                                                                         {
                                                                             Max_Count_Channel1=0;
                                                                         }
                                                                         AO_cals1=Max_Count_Channel1;
                                                                         Dac1_Val = Max_Count_Channel1;
                                                                         dacop1=4000;    //msk 07-dec-2025
                                                }

                                                if((Key_Input == k_Load )&&(AO_Calib_status==5))    // //Chanel 1 Max count --
                                                {
                                                                        Key_Input=NO_KEY;
                                                                    Max_Count_Channel1-=50;;
                                                                        if((Max_Count_Channel1 < 1)||(Max_Count_Channel1 > 3900))/*Temp*/
                                                                         {
                                                                             Max_Count_Channel1=0;
                                                                         }
                                                                        AO_cals1=Max_Count_Channel1;
                                                                         Dac1_Val = Max_Count_Channel1;
                                                                         dacop1=4000;    //msk 07-dec-2025
                                                }
                                                ///sendAOquery();    //msk 07-dec-2025    send AO values to ext AO module
                                                if(AO_Calib_status==6)                        // //Chanel 2 min count display
                                                    {
                                                                        Clear_LCD();

                                                                        calib_string_ptr = "Channel2 Calib:";
                                                                        Print_String(&calib_string_ptr,line1,0,font16,0);
                                                                        calib_string_ptr = "Min_Count:";
                                                                        Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                        Clear_String(&clear_string,line3,0,font16);
                                                                        sprintf(calib_string,"%lu ",Min_Count_Channel2);
                                                                        calib_string_ptr=calib_string;
                                                                        Print_String(&calib_string_ptr,line3,0,font16,0);   //100 % load value
                                                                        //super_delay(10);
                                                        Dac2_Val  = Min_Count_Channel2;
                                                    }
                                                if((Key_Input==K_INC)&&(AO_Calib_status==6))    // //Chanel 2 Min count ++
                                                    {
                                                                        Key_Input=NO_KEY;
                                                                        Min_Count_Channel2++;
                                                                        if(Min_Count_Channel2>3900)
                                                                            {
                                                                                Min_Count_Channel2=3900;
                                                                            }
                                                                        AO_calz2=Min_Count_Channel2;
                                                                        Dac2_Val  = Min_Count_Channel2;
                                                     }
                                                if((Key_Input==Setpoint)&&(AO_Calib_status==6))    // //Chanel 2 Min count ++
                                                    {
                                                                        Key_Input=NO_KEY;
                                                                        Min_Count_Channel2+=50;
                                                                        if(Min_Count_Channel2>3900)
                                                                            {
                                                                                Min_Count_Channel2=3900;
                                                                            }
                                                                        AO_calz2=Min_Count_Channel2;
                                                                        Dac2_Val  = Min_Count_Channel2;
                                                     }

                                                     if((Key_Input==K_DEC)&&(AO_Calib_status==6))    // //Chanel 2 Min count --
                                                      {
                                                                        Key_Input=NO_KEY;
                                                                    --Min_Count_Channel2;
                                                                         if((Min_Count_Channel2 < 0)||(Min_Count_Channel2 >3900))
                                                                            {
                                                                                Min_Count_Channel2=0;
                                                                            }
                                                                        AO_calz2=Min_Count_Channel2;
                                                                        Dac2_Val  = Min_Count_Channel2;
                                                        }
                                                     if((Key_Input==k_Load)&&(AO_Calib_status==6))    // //Chanel 2 Min count -- by 50 msk 17-aug-25
                                                      {
                                                                        Key_Input=NO_KEY;
                                                                        Min_Count_Channel2-=50;
                                                                         if((Min_Count_Channel2 < 1) || (Min_Count_Channel2 > 3900))
                                                                            {
                                                                                Min_Count_Channel2=0;
                                                                            }
                                                                        AO_calz2=Min_Count_Channel2;
                                                                        Dac2_Val  = Min_Count_Channel2;
                                                        }

                                                        if(AO_Calib_status==7)
                                                        {
                                                                        Clear_LCD();

                                                                        calib_string_ptr = "Channel2 Calib:";
                                                                        Print_String(&calib_string_ptr,line1,0,font16,0);
                                                                        calib_string_ptr = "Max_Count:";
                                                                        Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                        Clear_String(&clear_string,line3,0,font16);
                                                                        sprintf(calib_string,"%lu ",Max_Count_Channel2);
                                                                        calib_string_ptr=calib_string;
                                                                        Print_String(&calib_string_ptr,line3,0,font16,0);   //100 % load value
                                                                        //super_delay(10);
                                                            Dac2_Val  = Max_Count_Channel2;
                                                        }
                                                 if((Key_Input==K_INC)&&(AO_Calib_status==7))    // //Chanel 2 Max count ++
                                                        {
                                                                        Key_Input=NO_KEY;
                                                                        Max_Count_Channel2++;
                                                                        if(Max_Count_Channel2>3900)/*Temp*/
                                                                            {
                                                                                Max_Count_Channel2=3900;
                                                                            }
                                                                        AO_cals2=Max_Count_Channel2;
                                                                        Dac2_Val  = Max_Count_Channel2;
                                                        }

                                                 if((Key_Input==Setpoint)&&(AO_Calib_status==7))    // //Chanel 2 Max count ++
                                                        {
                                                                        Key_Input=NO_KEY;
                                                                        Max_Count_Channel2+=50;
                                                                        if(Max_Count_Channel2>3900)/*Temp*/
                                                                            {
                                                                                Max_Count_Channel2=3900;
                                                                            }
                                                                        AO_cals2=Max_Count_Channel2;
                                                                        Dac2_Val  = Max_Count_Channel2;
                                                        }

                                                        if((Key_Input==K_DEC)&&(AO_Calib_status==7))    // //Chanel 2 Max count --
                                                        {
                                                                        Key_Input=NO_KEY;
                                                                        --Max_Count_Channel2;
                                                                        if((Max_Count_Channel2 < 1) || (Max_Count_Channel2 >3900))
                                                                            {
                                                                                Max_Count_Channel2=0;
                                                                            }
                                                                        AO_cals2=Max_Count_Channel2;
                                                                        Dac2_Val  = Max_Count_Channel2;
                                                        }

                                                            if((Key_Input==k_Load)&&(AO_Calib_status==7))    // //Chanel 2 Max count --
                                                        {
                                                                        Key_Input=NO_KEY;
                                                                        Max_Count_Channel2-=50;
                                                                        if((Max_Count_Channel2 < 1) || (Max_Count_Channel2 >3900))
                                                                            {
                                                                                Max_Count_Channel2=0;
                                                                            }
                                                                        AO_cals2=Max_Count_Channel2;
                                                                        Dac2_Val  = Max_Count_Channel2;
                                                        }

                                                        if(AO_Calib_status==8)                        // //Chanel 2 min count display
                                                    {
                                                                        Clear_LCD();

                                                                        calib_string_ptr = "Channel3 Calib:";
                                                                        Print_String(&calib_string_ptr,line1,0,font16,0);
                                                                        calib_string_ptr = "Min_Count:";
                                                                        Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                        Clear_String(&clear_string,line3,0,font16);
                                                                        sprintf(calib_string,"%lu ",Min_Count_Channel3);
                                                                        calib_string_ptr=calib_string;
                                                                        Print_String(&calib_string_ptr,line3,0,font16,0);   //100 % load value
                                                                        //super_delay(10);
                                                        Dac3_Val  = Min_Count_Channel3;
                                                    }
                                                if((Key_Input==K_INC)&&(AO_Calib_status==8))    // //Chanel 2 Min count ++
                                                    {
                                                                        Key_Input=NO_KEY;
                                                                        Min_Count_Channel3++;
                                                                        if(Min_Count_Channel3>3900)
                                                                            {
                                                                                Min_Count_Channel3=3900;
                                                                            }
                                                                        AO_calz3=Min_Count_Channel3;
                                                                        Dac3_Val  = Min_Count_Channel3;
                                                     }
                                                if((Key_Input==Setpoint)&&(AO_Calib_status==8))    // //Chanel 2 Min count ++
                                                    {
                                                                        Key_Input=NO_KEY;
                                                                        Min_Count_Channel3+=50;
                                                                        if(Min_Count_Channel3>3900)
                                                                            {
                                                                                Min_Count_Channel3=3900;
                                                                            }
                                                                        AO_calz3=Min_Count_Channel3;
                                                                        Dac3_Val  = Min_Count_Channel3;
                                                     }

                                                     if((Key_Input==K_DEC)&&(AO_Calib_status==8))    // //Chanel 2 Min count --
                                                      {
                                                                        Key_Input=NO_KEY;
                                                                    --Min_Count_Channel3;
                                                                         if((Min_Count_Channel3 >3900) || (Min_Count_Channel3 <1))
                                                                            {
                                                                                Min_Count_Channel3=0;
                                                                            }
                                                                        AO_calz3=Min_Count_Channel3;
                                                                        Dac3_Val  = Min_Count_Channel3;
                                                        }
                                                     if((Key_Input==k_Load)&&(AO_Calib_status==8))    // //Chanel 2 Min count --
                                                      {
                                                                        Key_Input=NO_KEY;
                                                                    Min_Count_Channel3-=50;
                                                                         if((Min_Count_Channel3 >3900) || (Min_Count_Channel3 <1))
                                                                            {
                                                                                Min_Count_Channel3=0;
                                                                            }
                                                                        AO_calz3=Min_Count_Channel3;
                                                                        Dac3_Val  = Min_Count_Channel3;
                                                        }

                                                        if(AO_Calib_status==9)
                                                        {
                                                                        Clear_LCD();

                                                                        calib_string_ptr = "Channel3 Calib:";
                                                                        Print_String(&calib_string_ptr,line1,0,font16,0);
                                                                        calib_string_ptr = "Max_Count:";
                                                                        Print_String(&calib_string_ptr,line2,0,font16,0);
                                                                        Clear_String(&clear_string,line3,0,font16);
                                                                        sprintf(calib_string,"%lu ",Max_Count_Channel3);
                                                                        calib_string_ptr=calib_string;
                                                                        Print_String(&calib_string_ptr,line3,0,font16,0);   //100 % load value
                                                                        //super_delay(10);
                                                            Dac3_Val  = Max_Count_Channel3;
                                                        }
                                                 if((Key_Input==K_INC)&&(AO_Calib_status==9))    // //Chanel 2 Max count ++
                                                        {
                                                                        Key_Input=NO_KEY;
                                                                        Max_Count_Channel3++;
                                                                        if(Max_Count_Channel3>3900)/*Temp*/
                                                                            {
                                                                                Max_Count_Channel3=3900;
                                                                            }
                                                                        AO_cals3=Max_Count_Channel3;
                                                                        Dac3_Val  = Max_Count_Channel3;
                                                        }

                                                 if((Key_Input==Setpoint)&&(AO_Calib_status==9))    // //Chanel 2 Max count ++
                                                        {
                                                                        Key_Input=NO_KEY;
                                                                        Max_Count_Channel3+=50;
                                                                        if(Max_Count_Channel3>3900)/*Temp*/
                                                                            {
                                                                                Max_Count_Channel3=3900;
                                                                            }
                                                                        AO_cals3=Max_Count_Channel3;
                                                                        Dac3_Val  = Max_Count_Channel3;
                                                        }
                                                        if((Key_Input==K_DEC)&&(AO_Calib_status==9))    // //Chanel 2 Max count --
                                                        {
                                                                        Key_Input=NO_KEY;
                                                                        --Max_Count_Channel3;
                                                                        if((Max_Count_Channel3 >3900) || (Max_Count_Channel3 < 1))
                                                                            {
                                                                                Max_Count_Channel3=0;
                                                                            }
                                                                        AO_cals3=Max_Count_Channel3;
                                                                        Dac3_Val  = Max_Count_Channel3;
                                                        }

                                                        if((Key_Input==k_Load)&&(AO_Calib_status==9))    // //Chanel 2 Max count --
                                                        {
                                                                        Key_Input=NO_KEY;
                                                                        Max_Count_Channel3-=50;
                                                                        if((Max_Count_Channel3 >3900) || (Max_Count_Channel3 < 1))
                                                                            {
                                                                                Max_Count_Channel3=0;
                                                                            }
                                                                        AO_cals3=Max_Count_Channel3;
                                                                        Dac3_Val  = Max_Count_Channel3;
                                                        }
                                            sendAOquery(1);    //msk 07-dec-2025    send AO values to ext AO module

    }

 if(scan_count>=200)
    scan_count=0;

AO_calib_jump3:
if(scan_count==100 )
    AO_calib_display();

goto AO_calib_jump;

}

/*
Changes:
AI/AO Calibration:
Added: if Status==0&&Return(ESC is pressed)
                exit from routine

date:2/2/2018
Calibration:
    Span_Count is changed to Span_Count= ((((Present_count-Zero_Load_Count)*Capacity)/Calib_Load)+zero_count_cal)-zero_count_cal
    value is calculated wrt capacity
date:19/2/2018
Calibration:
    full_load is changed to full_load = ((((Present_count-Zero_Load_Count)*1000)/Calib_Load)+zero_count_cal)
    Span_Count is changed to Span_Count= full_load -zero_count_cal
both values are calculated wrt %

*/
