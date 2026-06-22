/*keys.c*/

#include "functions.h"
#include "menu.h"
#include "parameters.h"  //#include "menu.h"
#include "main_routine.h"
#include "OLED_2_7_12864.h"
#include "stdio.h"
#include "LPC17xx.h"
#include "cmsis_os.h"
#include "Board_Joystick.h"
#include "lpc17xx_gpio.h"
#include "GPIO_LPC17xx.h"
#include "keys.h"
#include "plc.h"

#define latch_port 1
#define latch_pin 4

char Togl_Led_Srt_Stp,Togl_Led_Zero,Togl_Led_Auto,Togl_Led_Man;
extern char ram_values[RAM_SIZE];
extern char Error_No;
uint32_t    key_ip,VarB,Key_Input,Key_Input_Bak,Key_status,Key_Press;
char key_lookup_table[21] = {3,6,9,10,11,2,5,8,0,13,1,4,7,12,14,19,18,17,16,15,0xff};
extern char Start_Stop_status;
//char Auto_Man_status;
extern char DInt_Int_status;
extern unsigned char Mode_Status,ACAL_Status;
extern unsigned char ZERO_STATUS,Zero_Tacho_Feed_Buffer_Flag ;
extern uint16_t New_menu_parameters[105];

unsigned char Operating_Mode;

void latch_control(char on)
{
  GPIO_PinWrite (latch_port,latch_pin, on);
}

void keypad_init()
{
//  GPIO_SetDir (0,,1);    //Initialize Keypad Output Pins
    GPIO_SetDir(latch_port,latch_pin,1);        //Initiallize Latch

    GPIO_SetDir(1,8,0);                                            //Initialize Keypad Input Pins
    GPIO_SetDir(1,9,0);
    GPIO_SetDir(1,10,0);
    GPIO_SetDir(1,14,0);
    GPIO_SetDir(1,15,0);

    GPIO_PinWrite(latch_port,latch_pin,1);

    GPIO_PinWrite(2,0,1);        //LED        //START_STOP
    GPIO_PinWrite(2,1,1);        //LED        //Zeroing
    GPIO_PinWrite(2,2,1);        //LED        //De-Interlock / Interlock
    GPIO_PinWrite(2,3,1);        //LED        //AUTO_MAN

    GPIO_PinWrite(latch_port,latch_pin,0);

}

void Key_Decode(void )
{
    Key_Press = 0;
    GPIO_SetValue(2,0xFF);    //Make K_OP1 = 0;
    GPIO_ClearValue(2,0x10);    //Make K_OP1 = 0;
    latch_control(1);
    latch_control(0);
        VarB = GPIO_ReadValue(1);
        VarB = VarB & 0xC700;
    if(VarB !=0xC700)
        goto jump_key1;
    Key_Press = Key_Press + 5;

    GPIO_SetValue(2,0xFF);    //Make K_OP2 = 0;
    GPIO_ClearValue(2,0x20);    //Make K_OP2 = 0;
    latch_control(1);
    latch_control(0);
    VarB = GPIO_ReadValue(1);
        VarB = VarB & 0xC700;
    if(VarB !=0xC700)
        goto jump_key1;
    Key_Press = Key_Press + 5;

    GPIO_SetValue(2,0xFF);    //Make K_OP3 = 0;
    GPIO_ClearValue(2,0x40);    //Make K_OP3 = 0;
    latch_control(1);
    latch_control(0);
    VarB = GPIO_ReadValue(1);
        VarB = VarB & 0xC700;
    if(VarB !=0xC700)
        goto jump_key1;
    Key_Press = Key_Press + 5;

    GPIO_SetValue(2,0xFF);    //Make K_OP4 = 0;
    GPIO_ClearValue(2,0x80);    //Make K_OP4 = 0;
    latch_control(1);
    latch_control(0);
    VarB = GPIO_ReadValue(1);
        VarB = VarB & 0xC700;
    if(VarB !=0xC700)
        goto jump_key1;
    Key_Press = 19;
    return;

    jump_key1:
    if((GPIO_PinRead(1,8))!=0)        //Check if K_IN1 Low
        goto jump_key2;
    return;

    jump_key2:
    Key_Press++;
        if((GPIO_PinRead(1,9))!=0)    //Check if K_IN2 Low
        goto jump_key3;
    return;

        jump_key3:
    Key_Press++;
        if((GPIO_PinRead(1,10))!=0)    //Check if K_IN3 Low
        goto jump_key4;
    return;

        jump_key4:
            Key_Press++;
        if((GPIO_PinRead(1,14))!=0)    //Check if K_IN4 Low
        goto jump_key5;
    return;

        jump_key5:
            Key_Press++;
        if((GPIO_PinRead(1,15))!=0)    //Check if K_IN5 Low
        goto jump_key6;
    return;

        jump_key6:
        Key_Press = 20;                            // No Key Pressed
        return;

}

void Key_Convert(void)
{
    Key_Press=key_lookup_table[Key_Press];
    return;
}
//----------------------------------------
void key_check_routine(void)
            {

    latch_control(0);
    GPIO_SetValue(2,0x0F);
    GPIO_ClearValue(2,0xF0);

    latch_control(1);
    latch_control(0);
    VarB = GPIO_ReadValue(1);
    VarB = VarB & 0xC700;
            if(VarB != 0xC700)
                goto key_jump1;
            Key_Input = 0xFF;
            Key_Input_Bak = 0xFF;
            Key_status = 0;
//            Key_Press=0xFF;
            return;

key_jump1:
                Key_Decode();
                Key_Convert();
            if(Key_status !=1)
                goto key_jump2;
            if(Key_Press !=0xFF)
                goto key_jump3;
            Key_status=0;
            Key_Input=0xFF;
            return;

key_jump3:
            Key_Input = Key_Press;
            Key_status=2;
            return;

key_jump2:
            if(Key_status !=2)
                goto key_jump4;
            if(Key_Press !=0xFF)
                goto key_jump5;
            Key_Input=0xFF;
            Key_status=0;
            return;

key_jump5:
            if(Key_Press != Key_Input)
                goto key_jump6;
            Key_Input_Bak = Key_Input;
            Key_Input = 0xFF;
            Key_status=3;
            return;

key_jump6:
            Key_Input = 0xFF;
            Key_Input_Bak = 0xFF;
            Key_status=1;
            return;

key_jump4:
            if(Key_status !=3)
                goto key_jump6;
            if(Key_Press == 0xFF)
                goto key_jump6;
            if(Key_Press != Key_Input_Bak)
                goto key_jump6;
            return;

}

void toggle_start_stop(void)//Error
{
    Togl_Led_Srt_Stp++;
    if(Togl_Led_Srt_Stp== 10)
    {
            GPIO_PinWrite(2,0,0);            //LED    //START_STOP //OFF
            M064=0;
            if(Prefeeder==1)
                M065 = 0 ;
    }
    if(Togl_Led_Srt_Stp >= 20)
    {
          Togl_Led_Srt_Stp=0;
            GPIO_PinWrite(2,0,1);            //LED    //START_STOP //On
            M064 =1;
            if(Prefeeder==1)
                M065 = 1 ;
    }

}

void toggle_start_stop_Zeroing(void)//Zeroing
{
        Togl_Led_Zero++;
    if(Togl_Led_Zero == 3)
    {
            GPIO_PinWrite(2,1,0);      //ZEROING //OFF
    }
    if(Togl_Led_Zero >= 6)
    {
        Togl_Led_Zero=0;
            GPIO_PinWrite(2,1,1);       //LED    //ZEROING //On
    }
}
void toggle_Int_DInt_Led() //Remote
{
        Togl_Led_Auto++;
    if(Togl_Led_Auto == 3)
    {
            GPIO_PinWrite(2,2,0);      //LED    //Auto_Led//OFF
    }
    if(Togl_Led_Auto >= 6)
    {
        Togl_Led_Auto=0;
            GPIO_PinWrite(2,2,1);     //LED    //Auto_Led//On
    }

}
void toggle_Auto_Man_Local() //Local
{
        Togl_Led_Man++;
    if(Togl_Led_Man == 3)
    {
            GPIO_PinWrite(2,3,0);      //Man_Led//OFF
    }
    if(Togl_Led_Man >= 6)
    {
        Togl_Led_Man=0;
        GPIO_PinWrite(2,3,1);    //Man_Led//On
    }
}
//-------------------------

//------------------------
void LED_Routine(void)
{
    latch_control(1);
        /*
        GPIO_PinWrite(2,0,1);//start/stop led on
        GPIO_PinWrite(2,1,1);//zeroing led on
        GPIO_PinWrite(2,2,1);//Int/Deint Led on
        GPIO_PinWrite(2,3,1);// auto/man led on
        */
        if (((Error_No != 0) && (Error_No != 5) && (Error_No != 6)) || (M044 == 1))
         /* blink: fault display or M044 trip until M018 ack */
        {
            toggle_start_stop();  //for Errors
            goto jump_a;
        }
        if(( ZERO_STATUS > 1)&&( ZERO_STATUS < 6))/*Zerro Status*/
        {
            toggle_start_stop_Zeroing();  //for Zeroing
            goto jump_a;
        }
        if( ACAL_Status == 2)/*Zerro Status*/
        {
            toggle_start_stop_Zeroing();  //for Auto Calibration
            goto jump_a;
        }
    jump_a:
        //msk removed error condition check 10-05-2026
        //if( Error_No == 0 )    //if no error
        if (((Error_No == 0) || (Error_No == 5) || (Error_No == 6)) && (M044 == 0))
         /* steady run LED when no trip and no fault display */
        {
            GPIO_PinWrite(2,0,Start_Stop_status);//START_STOP //OFF
            M064 = M066;
            if(Prefeeder==1)
            M065 = M067;
        }
//--old cod--------
//if( (Mode_Status == Manual) || (Mode_Status == Interlock) || (Mode_Status == De_Interlock))
//{
//   GPIO_PinWrite(2,3,Auto_Man_status );	//msk: AUTO_MAN (Auto_man_status = 0 when START)
//    GPIO_PinWrite(2,2, DInt_Int_status );	//Interlock_DE-Interlock 
// (macro values are Deinterlock =1, and Interlock =1)
//  example of other snippet inother part of code : 
// In case of Deinterlock mode,
//DInt_Int_status = STOP; Macro value for STOP means '0', therefore DInt_Int_status becomes zero in de-int mode
//Mode_Status = De_Interlock; Macro value for De_Interlock means '1', therefore Mode_Status becomes 1 in de-int mode
//}
//-- old code end -------
                //if( (Mode_Status == Manual) || (Mode_Status == Interlock) || (Mode_Status == De_Interlock))
                //if( GetDeIntVolMode() || (Mode_Select == Int))
                //{
                //    GPIO_PinWrite(2,3,GetVolMode() );  //AUTO_MAN led
                //    GPIO_PinWrite(2,2, GetDeIntMode() );  //Interlock_DE-Interlock led
                //}
                if(GetIntMode())
                {
                    GPIO_PinWrite(2,2,0);   //AUTO_MAN led on
                    //GPIO_PinWrite(2,2, GetDeIntMode() );  //Interlock_DE-Interlock
                }
                else
                {
                    GPIO_PinWrite(2,2,1);   //AUTO_MAN led off
                }
                if(GetVolMode())
                {
                    GPIO_PinWrite(2,3,0);   //Interlock_DE-Interlock led on
                }
                else
                {
                    GPIO_PinWrite(2,3,1);   //Interlock_DE-Interlock led off
                }

                /*Shifted here for kepping all led status as per there mode on 24/05/2018 */
                //  if(Mode_Status == Remote )                /*IF Mode Status == Remote*/
                //if(GetFieldMode())                /*IF Mode Status == Remote*/
                //   { toggle_Int_DInt_Led(); }        //for Remote
                /*Shifted here for kepping all led status as per there mode on 24/05/2018 */
                //if( Mode_Status == Local)
                if(GetFieldMode())
                {
                    toggle_Auto_Man_Local();  //for local
                    GPIO_PinWrite(2,2,1);//int/deint led off
                    //GPIO_PinWrite(2,1,1); //Interlock_DE-Interlock
                }
    latch_control(0);
}
