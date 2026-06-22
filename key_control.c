/*    key_control.c */

#include "LPC17xx.h"                    // Device header
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "main_routine.h"
#include "OLED_2_7_12864.h"
#include "keys.h"
#include "parameters.h" //#include "menu.h"
#include "plc.h"
#include "Errors.h"
#include "functions.h"
#include "lpc_types.h"

char io_status;
extern char* P_String;
extern char A_String[200];
extern long calib_val[8];
extern uint32_t Key_Input;
extern char Error_No;
extern char Start_Stop_status;
extern char Auto_Man_status;
extern uint16_t New_menu_parameters[105],Online_menu_parameters[15];
extern unsigned long Present_Count,adc_final_val,bsf_adc_final_val,Count_DL,zero_load,full_load;
extern char ram_values[RAM_SIZE];
extern main_menu_struct Main_menu1[Main_Menu_Max];
extern main_menu_struct Online_Main_menu[Main_Menu_Max];
extern sub_menu_struct sub_menu1[12][26];
extern sub_menu_struct Online_menu[5][14];
extern char const sub_menu_options1[13][26][12][20];
extern char const Online_menu_options[5][14][10][20];
extern char Check_Start,increment,i5,Counter1,Counter2,Counter3, direction;

extern unsigned char Zero_Status,Zero_Tacho_Feed_Buffer_Flag,CM_Negative ;
extern unsigned int Totaliser1_Display;
extern double Totaliser1_Update,Totaliser1;
char Totaliser_Clear = 0;
extern unsigned char ZERO_STATUS;
extern unsigned char Mode_Status;
//char DInt_Int_status;
void eeprom_write_array(unsigned int , unsigned char *, unsigned int );extern unsigned int Tacho_Sum,Tacho_Register[6];
extern float Tacho_Avg;
extern unsigned char Tacho_Pointer,Tacho_Buffer;
extern char  display_line1,display_cursor;
extern     char menu_counter,Sub_Menu_no;
extern char Submenu_flag,submenu_Staus_flag;
extern char sub_display_cursor;
char key_vlaue ;
extern  char check_flag;
extern char menu_pointer;

extern long VARI_online;
extern char new_string_online[15];
extern char print_new_string_online[15];
extern char old_string_online[15];
extern char *new_string_print_online;
extern char str_len1_online,str_len2_online;
extern unsigned char j_online,k_online,j2_online;
extern unsigned char str_loop_online;
extern signed char diff_online;
extern char dec_flag_online;
extern unsigned char j_online,k_online,j2_online,CM_Negative;
extern char Sel_para_flag,Temp_par_val,Opt_para_flag ;

extern char Rows_clear,Oled_flag,lcd_flag;
extern int lcd_count_disp;
extern char Oled_flag_Temp ;
//extern int bytes_written;
extern char IO_Online_flag;
extern uint8_t  Modbus_Monitor_Time,Mod_Monitoring_Flag ;

extern uint16_t scan_count;
extern int temp_count1,temp_count2;
extern char total_status;

extern unsigned long scan_count_deviation_monitor,scan_count_deviation,CM_Difference;
//extern char Ram_display_status;
extern uint16_t Ram_display_Enable_counter;
extern int Ram_adres,Ram_adres_1,Ram_adres_;
extern unsigned char *Ram_pointer,*Ram_pointer_1,*Ram_pointer_;
extern unsigned char cc,dd;
extern char Ram_status,Adress_Counter;

extern unsigned char IO_Disp_L1,IO_Disp_L2,IO_Disp_L3,IO_Disp_L4,IO_Disp_L5;
extern unsigned char IO_Par_L1,IO_Par_L2,IO_Par_L3,IO_Par_L4,IO_Par_L5;

extern unsigned char IO_Ptr1,IO_Stat;
extern char New_Start,PF_Status;
extern unsigned char ACAL_Status,BIN_PF_Start_Status;
extern float CM_Percentage;
/**********************************************************************************/
void key_action (void)
{
    switch(Key_Input)
    {
        case CL:
            if((Totaliser_Clear==1)&& (Check_Start!=20) ) //if total1 on 4th line and online menu is not displayed
            {
                Oled_flag = 1;  //to clear lcd
                lcd_flag  = 2;  //to clear lcd
                if( total_status==0 )
                    total_status = 1;
                if(total_status ==2)
                    total_status = 4;
                return;
            }
            if(Error_No==2)
            {
                Modbus_Monitor_Time = SIO_Monitor;
                ErrorLatch_ClearAll();
                Error_No=0;
                return;
            }

            Error_No = 0 ;
            M056 = 0 ;//Limit1
            M057 = 0 ;//Limit2
            M058 = 0 ;//Overload Alarm
            M059 = 0 ;//Underload Alarm
            M060 = 0 ;//Loadcell Fault
            M040 = 0 ;//Tacho Fault
            M041 = 0 ; //Belt Tracking
            M042 = 0 ; //Reset WF Drive Fault
            M043 = 0 ; //Deviation Fault
            M044 = 0 ; //General Fault
            M045 = 0;  //PF Drive Fault Marker
            M010 = 1 ; //Emergency Fault if M010 = 0
            M019 = 1 ; //WF Drive Fault  if M019 = 0
            M020 = 1 ; //PF Drive Fault  if M020 = 0
            M077 = 0 ; //ACAL Abort clear
            M046 = 0 ; //ZSS faulr Marker
            ErrorLatch_ClearAll();
            break;

        
        case k_SET:
            if(Start_Stop_status ==STOP)        //if stop show this Main Menu on screen
            {
                Lcd_ReinitForMenu(); /* msk29-may-26: recover hung OLED before offline menu */
                InvComm_NotifyOfflineMenuEnter(); /* msk29-may-26 */
                Check_Start=10;
                Print_main_menu(Main_menu1,sub_menu1,sub_menu_options1,1);        //Offline
                return;
            }
            if(Start_Stop_status == START)        //if Start show this Main Menu on screen
            {
                Lcd_ReinitForMenu(); /* msk29-may-26 */
                Check_Start=20;
                display_cursor = 1;
                if(menu_pointer == 0)
                {
                    Oled_flag =1;
                    lcd_flag=2;
                }
                return;
            }
            break;
        case Start_Stop:
            //if(Mode_Select == Int)//if internal (panel) mode
            if(true)//msk removed panel mode (Internal setting) condition check 10-05-2026
            {
                if((Operating_Mode == DeInt_Gra_Dio) || (Operating_Mode == DeInt_Vol_Dio) && 
                (Check_Start!=20))// must be de-interlock mode
                {
                    //MSK 03-mar-26
                    if(Start_Stop_status == START) 
                    {
                        Start_Stop_status = STOP;
                        Zero_Tacho_Feed_Buffer_Flag =2;
                        M066=0;
                    }
                    else if ((M044 == 0) && (M010 == 1))
                    {
                        Start_Stop_status = START;
                        M066=1;
                        New_Start = 1;
                    }
                }//end of internal (panel) mode
            }//end of internal (panel) mode                    
        break;//end of case Start_Stop
        case INT_DE_INT://interlock/de-interlock mode selection key for 'Internal' mode
            if(Mode_Select == Int)//Operate only in Internal (panel) mode
            {
                if((Start_Stop_status == STOP) && (Check_Start != 20))
                {
                    /* Invert interlock/de-interlock only; keep gravimetric/volumetric unchanged */
                    if(Operating_Mode == Int_Vol_Dio)   /* Volumetric Interlock → Volumetric De-Interlock */
                    {
                        Operating_Mode = DeInt_Vol_Dio;
                        M052 = 0; // Volumetric Mode
                        M053 = 0;//Interlock Status (1=Interlocked,0=De-Interlocked)
                        //M015 = 1;//Int/De-Interlock Mode Select (1-De-Interlock, 0-Int)
                    }
                    else if(Operating_Mode == DeInt_Vol_Dio)  /* Volumetric De-Interlock → Volumetric Interlock */
                    {
                        Operating_Mode = Int_Vol_Dio;
                        M052 = 0; // Volumetric Mode
                        M053 = 1;//Interlock Status (1=Interlocked,0=De-Interlocked)
                        //M015 = 0;//Int/De-Interlock Mode Select (1-De-Interlock, 0-Int)
                    }
                    else if(Operating_Mode == Int_Gra_Dio)    /* Gravimetric Interlock → Gravimetric De-Interlock */
                    {
                        Operating_Mode = DeInt_Gra_Dio;
                        M052 = 1; // Gravimetric Mode
                        M053 = 0;//Interlock Status (1=Interlocked,0=De-Interlocked)
                        //M015 = 1;//Int/De-Interlock Mode Select (1-De-Interlock, 0-Int)
                    }
                    else if(Operating_Mode == DeInt_Gra_Dio)  /* Gravimetric De-Interlock → Gravimetric Interlock */
                    {
                        Operating_Mode = Int_Gra_Dio;
                        M052 = 1; // Gravimetric Mode
                        M053 = 1;//Interlock Status (1=Interlocked,0=De-Interlocked)
                        //M015 = 0;//Int/De-Interlock Mode Select (1-De-Interlock, 0-Int)
                    }
                }//end of if((Operating_Mode == Int_Gra_Dio) || (Operating_Mode == DeInt_Gra_Dio) || 
            }//end of internal (panel) mode
            break;

            case Auto_Man:  //WORK AS volumetric/gravimetric mode selection key 
                            // in case of de-interlock mode
            if((Start_Stop_status == STOP) && (Mode_Select == Int)&& 
            (Check_Start!=20) )//react only if STOP condition and 'Internal' mode is selected
            {
                //As per new control logic, if Auto_Man is pressed in 
                // Internal mode, then the mode should be toggled to Field Mode and vice versa
                if(Operating_Mode == DeInt_Vol_Dio)
                {
                    Operating_Mode =    DeInt_Gra_Dio;
                    M052 = 1; // Gravimetric Mode
                    M053 = 0; // Interlock Status (1=Interlocked,0=De-Interlocked)
                    //M015 = 0; // Int/De-Interlock Mode Select (1-De-Interlock, 0-Int)
                    return;
                }
                else if(Operating_Mode == DeInt_Gra_Dio)
                {
                    Operating_Mode = DeInt_Vol_Dio;
                    M052 = 0; // Volumetric Mode
                    M053 = 0; // Interlock Status (1=Interlocked,0=De-Interlocked)
                    //M015 = 0; // Int/De-Interlock Mode Select (1-De-Interlock, 0-Int)
                    return;
                }
                else if(Operating_Mode == Int_Gra_Dio)
                {
                    Operating_Mode = Int_Vol_Dio; // Interlock Gravimetric Mode  
                    M052 = 0; // Volumetric Mode
                    M053 = 1; // Interlock Status (1=Interlocked,0=De-Interlocked)
                    //M015 = 0; // Int/De-Interlock Mode Select (1-De-Interlock, 0-Int)
                    return;
                }
                else if(Operating_Mode == Int_Vol_Dio)
                {
                    Operating_Mode = Int_Gra_Dio; // Interlock Gravimetric Mode  
                    M052 = 1; // Gravimetric Mode
                    M053 = 1; // Interlock Status (1=Interlocked,0=De-Interlocked)
                    //M015 = 0; // Int/De-Interlock Mode Select (1-De-Interlock, 0-Int)
                    return;
                }
                /* OLD CODE
                if(Mode_Status == Manual )
                {
                    Mode_Status = De_Interlock;
                    Auto_Man_status = START;  // make it Auto mode
                    DInt_Int_status = STOP;
                    M052 = 1;//Gravimetric Mode
                    return;
                }
                if((Mode_Status == Interlock)|| 
                (Mode_Status == De_Interlock))
                {
                    Auto_Man_status = STOP; // make it Manual Mode
                    Mode_Status = Manual;
                    DInt_Int_status = De_Interlock;
                    M052 = 0;//Volumetric Mode
                    return;
                }
                    */
            }//end of if((Start_Stop_status == STOP) && (Mode_Select == Int)&& (Check_Start!=20) ) ends
            break;
        case Zero_Key:
            //if((Start_Stop_status==START)&&(Mode_Status!=Manual)&&(Mode_Status!=Local))//;2.1 >0< PRESSED & START & GRAVIMETRIC MODE, MAKE ACAL_STATUS=01
            if((Start_Stop_status==START)&&(!GetDeIntMode())&&(!GetFieldMode()))//;2.1 >0< PRESSED & START & GRAVIMETRIC MODE, MAKE ACAL_STATUS=01
            {
                increment=16;
                Counter3=15;
                Counter2=14;
                Counter1=13;

                if((ACAL_Status == 1)||(ACAL_Status == 2))
                {
                    ACAL_Status =0;
                    M077= 1;
                    return;
                }
                if(ACAL_Status ==0)
                {
                    ACAL_Status = 1;
                    return;
                }
                Key_Input = 0xFF;
            }

            /* Abort belt zero / dynamic zero: same key as start. Do not require Check_Start —
               online menu leaves Check_Start==20 and would otherwise block abort entirely. */
            if ((GetDeIntVolMode() || GetFieldMode())
                && (ZERO_STATUS == 1 || ZERO_STATUS == 2))
            {
                ZERO_STATUS = 5;
                return;
            }
            /* Start zero only from idle (must not follow failed abort guard with accidental restart mid-sequence). */
            if ((GetDeIntVolMode() || GetFieldMode())
                && (Start_Stop_status == START)
                && (Check_Start != 20)
                && (ZERO_STATUS == 0))
            {
                ZERO_STATUS = 1;
                return;
            }
            break;
/*********************************************************************************************************************/
        case 0:
            if((Check_Start == 20)&&(submenu_Staus_flag == 2 ))
            {
                menu_pointer = 0;
                key_vlaue = 0;
                j2_online++;
                return;
            }

            break;
        case 1:
            if((i5==10)&& (Check_Start!=20) ) /*Current_Setpoint*/
            {
                increment=9; /*Current_Setpoint*/ //To display Current setpoint on 5th line on pressing key 1
                Totaliser_Clear = 0;
                return;
            }
            if((Check_Start == 20)&&(submenu_Staus_flag == 2 ))
            {
                menu_pointer = 0;
                key_vlaue = 1;
                j2_online++;
                return;
            }
//                                                        if((Ram_display_status == 1)&& (Ram_display_Enable_counter > 0))
//                                                        {
//                                                            Ram_display_status =2;
//                                                            return;
//                                                        }
            break;

        case 2:
            if((i5==10)&& (Check_Start!=20) )/*Belt_Load*/
            {
                increment=3; /*Belt_Load*/        //To display Belt Load on 5th line on pressing key 2
                Totaliser_Clear = 0;
                return;
            }
            if((Check_Start == 20)&&(submenu_Staus_flag == 2 ))
            {
                menu_pointer = 0;
                key_vlaue = 2;
                j2_online++;
                return;
            }
            break;
        case 3:
            if((i5==10)&& (Check_Start!=20) )/*Total1*/
            {
                increment=1; /*Total1*/           //To display Totaliser 1 value on 5th line on pressing key 3
                Totaliser_Clear = 1;
                return;
            }
            if((Check_Start == 20)&&(submenu_Staus_flag == 2 ))
            {
                menu_pointer = 0;
                key_vlaue = 3;
                j2_online++;
                return;
            }
            break;

        case 4:
            if((i5==10)&& (Check_Start!=20) )/*Feedrate*/
            {
                increment=0; /*Feedrate*/         //To display Feedrate value on 5th line on pressing key 4
                Totaliser_Clear = 0;
                return;
            }
            if((Check_Start == 20)&&(submenu_Staus_flag == 2 ))
            {
                menu_pointer = 0;
                key_vlaue = 4;
                j2_online++;
                return;
            }
            break;
        case 5:
            if((i5==10)&& (Check_Start!=20) )/*Speed*/
            {
                increment=4; /*Speed*/            //To display Speed value on 5th line on pressing key 5
                Totaliser_Clear = 0;
                return;
            }
            if((Check_Start == 20)&&(submenu_Staus_flag == 2 ))
            {
                menu_pointer = 0;
                key_vlaue = 5;
                j2_online++;
                return;
            }
            break;
        case 6:
            if((i5==10)&& (Check_Start!=20) )/*Belt_Load*/
            {
                increment=2; /*Belt_Load*/        //To display Belt Load on 5th line on pressing key 2
                Totaliser_Clear = 0;
                return;
            }
            if((Check_Start == 20)&&(submenu_Staus_flag == 2 ))
            {
                menu_pointer = 0;
                key_vlaue = 6;
                j2_online++;
                return;
            }
//                                                        if((Ram_display_status == 2)&& (Ram_display_Enable_counter > 0))
//                                                            {
//                                                                Ram_display_status =3;
//                                                                Oled_flag =1;
//                                                                lcd_flag=2;
//                                                                return;
//                                                            }
            break;
        case 7:
            if((Check_Start == 20)&&(submenu_Staus_flag == 2 ))
            {
                menu_pointer = 0;
                key_vlaue = 7;
                j2_online++;
                return;
            }
            break;
        case 8:
            if((Check_Start == 20)&&(submenu_Staus_flag == 2 ))
            {
                menu_pointer = 0;// if (menu_pointer == 0) ; enter into submenu routine
                key_vlaue = 8;
                j2_online++;   // increment cursor location
                return;
            }
            break;
        case 9:
            if(ACAL_Status==3)
            {
                ACAL_Status= 0;
                After_AutoCAL();
            }
            if(ACAL_Status==4)//
            {
                ACAL_Status=0;
                M078=1;//Cor>Zero Limit
            }
            if((Check_Start == 20)&&(submenu_Staus_flag == 2 ))
            {
                menu_pointer = 0;
                key_vlaue = 9;
                j2_online++;
                return;
            }
            break;

        case K_INC:
            if((Check_Start ==20)&&(Submenu_flag==0))  //if Online Menu is diplaying
            {
                direction = 1;
                menu_pointer =0;
                if(display_cursor > 1)
                    display_cursor = display_cursor - 1;

                if((display_cursor == 1))//if PF parameter is disabled
                    display_cursor = 1;
                return;
            }
            if((Check_Start == 20)&&(submenu_Staus_flag == 2 ))
            {
                submenu_Staus_flag = 1; //For directly switching to submenu parameter name
                if(sub_display_cursor>0)
                    sub_display_cursor--;   //To display next sub menu parameter
                menu_pointer = 0;           //To enter back into online menu or submenu routine
                Opt_para_flag = 0;             //required for Entery pameter (if Sub menu type is "E ")  routine
                check_flag = 0;                //required for Enter Online variable routine
                return;
            }
            /*************** IO Online monitor Routine ******************/
            if(IO_Online_flag ==1 )
            {
                Oled_flag = 1;  //to clear lcd
                lcd_flag  = 2;  //to clear lcd

                IO_Disp_L5 = IO_Disp_L4;
                IO_Disp_L4 = IO_Disp_L3;
                IO_Disp_L3 = IO_Disp_L2;
                IO_Disp_L2 = IO_Disp_L1;
                IO_Disp_L1 = IO_Disp_L1-10;

                IO_Par_L5 = IO_Par_L4;
                IO_Par_L4 = IO_Par_L3;
                IO_Par_L3 = IO_Par_L2;
                IO_Par_L2 = IO_Par_L1;
                IO_Par_L1 = IO_Par_L1-1;

                if((IO_Disp_L1 > 184))
                {
                    IO_Disp_L1 = 184;
                    IO_Par_L1 = 29;
                }
                return;
            }
            /*********************************/

            if( (i5==10) && (Check_Start!=20))  //Online Routine
            {
                Totaliser_Clear = 0;
                if(i5==10)
                {
                    increment=Counter3;
                    Counter3=Counter2;
                    Counter2=Counter1;
                    Counter1--;
                    if(Counter1==255)
                        Counter1=22;
                }
                return;
            }

            break;

        case K_DEC:
//                                                            if( (ZERO_STATUS  == 0) && (Ram_display_status==3) ) //if zeroing is not in progress and F is pressed in Start mode
//                                                                {
//
//                                                                    Ram_adres = Ram_adres+48;
//                                                                    Ram_pointer = Ram_pointer+48;
//                                                                    Ram_status = 0;
//                                                                        Ram_display_status =3;
//                                                                        Oled_flag =1;
//                                                                        lcd_flag=2;
//                                                                        return;
//                                                                }

            if( (i5==10) && (Check_Start!=20))  //Online Routine
            {
                Totaliser_Clear = 0;
                if(i5==10)
                {
                    Counter1=Counter2;
                    Counter2=Counter3;
                    Counter3=increment;
                    increment++;
                    if(increment==23)
                        increment=0;
                }
                return;
            }

            if((Check_Start ==20)&&(Submenu_flag==0))  //if Online Menu is diplaying
            {
                menu_pointer =0;
                direction = 0;
                display_cursor = display_cursor+ 1;
                if((display_cursor > 3) )//if PF parameter is disabled
                    display_cursor = 3;

                return;
            }
            if((Check_Start == 20)&&(submenu_Staus_flag == 2 ))//online submenu
            {
                submenu_Staus_flag = 1; //For directly switching to submenu parameter name
                sub_display_cursor++;   //To display next sub menu parameter
                menu_pointer = 0;           //To enter back into online menu or submenu routine
                Opt_para_flag = 0;             //required for Entery pameter (if Sub menu type is "E ")  routine
                check_flag = 0;                //required for Enter Online variable routine
                return;
            }
            /*************** IO Online monitor Routine ******************/
            if(IO_Online_flag ==1 )
            {
                Oled_flag = 1;  //to clear lcd
                lcd_flag  = 2;  //to clear lcd

                IO_Disp_L1 = IO_Disp_L2;
                IO_Disp_L2 = IO_Disp_L3;
                IO_Disp_L3 = IO_Disp_L4;
                IO_Disp_L4 = IO_Disp_L5;
                IO_Disp_L5 = IO_Disp_L5+10;

                IO_Par_L1 = IO_Par_L2;
                IO_Par_L2 = IO_Par_L3;
                IO_Par_L3 = IO_Par_L4;
                IO_Par_L4 = IO_Par_L5;
                IO_Par_L5 = IO_Par_L5+1;

                if((IO_Disp_L5 > 184))
                {
                    IO_Disp_L5 = 0;
                    IO_Par_L5 = 11;
                }
                return;
            }
            /*********************************/
            break;

        case HOR:
            if(total_status ==2)
            {
                total_status = 3;
                return;
            }
            if((Check_Start==20)&&(Oled_flag == 3)) //If in online menu display routine
            {

                if(IO_Online_flag ==1 )
                {
                    Clear_single_Line(50,60);       // 0.078 msec
                    IO_Stat = 0;                      // Make status 0 to update next line next parameter
                    IO_Ptr1 = 0;                            // make ptr zero

                    if( ( IO_Disp_L5 >= 0 ) && ( IO_Disp_L5 < 40 ) )
                    {
                        IO_Disp_L5 = 40;
                        IO_Par_L5 = 15;
                        return;
                    }
                    if( ( IO_Disp_L5 >= 40 ) && (IO_Disp_L5 < 80) )
                    {
                        IO_Disp_L5 = 80;
                        IO_Par_L5 = 19;
                        return;
                    }
                    if( ( IO_Disp_L5 >= 80 ) && (IO_Disp_L5 < 184 ) )
                    {
                        IO_Disp_L5 = 184 ;
                        IO_Par_L5 = 29;
                        return;
                    }
                    if( (IO_Disp_L5 >= 184) )
                    {
                        IO_Disp_L5 = 0;
                        IO_Par_L5 = 11;
                        return;
                    }
                    return;
                }

                if( display_cursor != 4 )
                {
                    menu_pointer = 0;
                    Submenu_flag = 1;
                    if((menu_pointer ==0)&&(submenu_Staus_flag == 0 ))
                    {

                        Oled_flag = 1;  //to clear lcd
                        lcd_flag  = 2;  //to clear lcd
                    }
                    if((submenu_Staus_flag == 2 )&&(Sel_para_flag  == 3))
                    {
                        Sel_para_flag = 1;
                        Temp_par_val ++;
                    }
                    return;

                }
//                                                        if(( display_cursor == 4 ) && (WF_Type == 1 ))
//                                                            {
//                                                                 menu_pointer = 0;
//                                                             Submenu_flag = 1;
//                                                            if((menu_pointer ==0)&&(submenu_Staus_flag == 0 ))
//                                                                {
//
//                                                                     Oled_flag = 1;  //to clear lcd
//                                                                   lcd_flag  = 2;  //to clear lcd
//                                                                }
//                                                           if((submenu_Staus_flag == 2 )&&(Sel_para_flag  == 3))
//                                                                {
//                                                                    Sel_para_flag = 1;
//                                                                    Temp_par_val ++;
//                                                                }
//                                                                return;
//                                                            }

            }

            break;

        case ESC:
            if(Check_Start==20)
            {
                if(Submenu_flag !=1) //if displaying online Menu
                {
                    Check_Start = 0;   //online menu flag becomes zero
                    menu_pointer = 0;   //
                    //bytes_written = 0; //
                    eeprom_write_array(0, (unsigned char *)New_menu_parameters , 324 );
                    New_menu_parameters[34]=Online_menu_parameters[0]  ;
                    New_menu_parameters[35]=Online_menu_parameters[1]  ;
                    New_menu_parameters[36]=Online_menu_parameters[2]  ;
                    New_menu_parameters[37]=Online_menu_parameters[3]  ;
                    New_menu_parameters[38]=Online_menu_parameters[4]  ;
                    New_menu_parameters[82]=Online_menu_parameters[5]  ;
                    New_menu_parameters[55]=Online_menu_parameters[6]  ;
                    New_menu_parameters[56]=Online_menu_parameters[7]  ;
                    New_menu_parameters[49]=Online_menu_parameters[8]  ;
                    New_menu_parameters[50]=Online_menu_parameters[9]  ;
                    New_menu_parameters[51]=Online_menu_parameters[10] ;
                    New_menu_parameters[54]=Online_menu_parameters[11] ;
                    New_menu_parameters[83]=Online_menu_parameters[12] ;
                    New_menu_parameters[39]=Online_menu_parameters[13] ;
                    New_menu_parameters[40]=Online_menu_parameters[14] ;
                    scan_count_deviation_monitor = (( Dev_Time * 1000 ) / 2000 );
                    scan_count_deviation = scan_count_deviation_monitor ;
                }
                if(Submenu_flag ==1) //if displaying online SubMenu
                {
                    submenu_Staus_flag = 0;
                    Submenu_flag = 0;    //To print online menu again
                    menu_pointer = 0;   //To print online menu again
                    Oled_flag = 1;     //To clear Lcd Screen
                    lcd_flag  = 2;            //To call Print Online function
                    sub_display_cursor = 0;  //make sub menu parameter back to first parameter
                    Opt_para_flag = 0;             //required for Entery pameter (if Sub menu type is "E ")  routine
                    check_flag = 0;                //required for Enter Online variable routine
                    IO_Online_flag =0;
                    Sel_para_flag = 0;

                }
                /******************* Reset IOTM monitor display variables  ************************/
                IO_Disp_L1 = 0,IO_Disp_L2 =10 ,IO_Disp_L3 = 20,IO_Disp_L4 = 30,IO_Disp_L5= 40;
                IO_Par_L1 = 11 ,IO_Par_L2 = 12 , IO_Par_L3 = 13 ,IO_Par_L4 = 14 ,IO_Par_L5 = 15;
                /*******************************************/
                return;
            }
//                                                     if(Ram_display_status == 3)
//                                                            {
//                                                                Ram_adres = (int)(&cc);
//                                                                Ram_pointer = (&cc);
//                                                                Ram_status = 0;
//                                                                Ram_display_status = 0;
//                                                                return;
//                                                            }

            break;

        default:
            break;

    }

}
