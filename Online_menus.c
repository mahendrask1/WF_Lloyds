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
#include "functions.h"

extern char const Online_menu_options[5][14][10][20];
extern sub_menu_struct Online_menu[5][14];
extern main_menu_struct Online_Main_menu[Main_Menu_Max];

extern char Rows_clear,Oled_flag;
extern char Submenu_flag,display_cursor;
extern char menu_pointer;
extern int temp_count1,temp_count2;
extern char lcd_flag;

extern uint16_t scan_count;
extern char *setpoint_string;
extern uint16_t New_menu_parameters[105];
extern uint32_t Key_Input;

extern char ram_values[RAM_SIZE];
extern char submenu_Staus_flag;
extern int pass_check;
char IO_Online_flag = 0,row_no ;
char direction;

extern const char *io_str_name[4][13];
extern const char *io_str[30][3];
extern char *Ram_add_no[16][1];

unsigned char IO_database[11],IO_Ptr=11,IO_Ptr1,IO_Ptr2,IO_Stat;

unsigned char IO_Disp_L1 = 0,IO_Disp_L2 =10 ,IO_Disp_L3 = 20,IO_Disp_L4 = 30,IO_Disp_L5= 40;
unsigned char IO_Par_L1 = 11 ,IO_Par_L2 = 12 , IO_Par_L3 = 13 ,IO_Par_L4 = 14 ,IO_Par_L5 = 15;

void IO_adrs_data_Identify(char data_,char Row_no,char col_no)
{
    switch(((char)(data_)))
     {
        case 0:Print_String2(io_str[0],Row_no,col_no,font10,0);break;      //'.'
        case 1:Print_String2(Ram_add_no[0],Row_no,col_no,font10,0);break;      //'0'
        case 2:Print_String2(Ram_add_no[1],Row_no,col_no,font10,0);break;      //'1'
        case 3:Print_String2(Ram_add_no[2],Row_no,col_no,font10,0);break;      //'2'
        case 4:Print_String2(Ram_add_no[3],Row_no,col_no,font10,0);break;      //'3'
        case 5:Print_String2(Ram_add_no[4],Row_no,col_no,font10,0);break;      //'4'
        case 6:Print_String2(Ram_add_no[5],Row_no,col_no,font10,0);break;      //'5'
        case 7:Print_String2(Ram_add_no[6],Row_no,col_no,font10,0);break;      //'6'
        case 8:Print_String2(Ram_add_no[7],Row_no,col_no,font10,0);break;      //'7'
        case 9:Print_String2(Ram_add_no[8],Row_no,col_no,font10,0);break;      //'8'
    case 10:Print_String2(Ram_add_no[9],Row_no,col_no,font10,0);break;  //'9'

      case 11:Print_String2(io_str[1],Row_no,col_no,font10,0);break;        //'I00'
        case 12:Print_String2(io_str[2],Row_no,col_no,font10,0);break;        //'I01'
        case 13:Print_String2(io_str[3],Row_no,col_no,font10,0);break;        //'I02'
        case 14:Print_String2(io_str[4],Row_no,col_no,font10,0);break;        //'I03'

        case 15:Print_String2(io_str[5],Row_no,col_no,font10,0);break;        //'O00'
        case 16:Print_String2(io_str[6],Row_no,col_no,font10,0);break;        //'O01'
        case 17:Print_String2(io_str[7],Row_no,col_no,font10,0);break;        //'O02'
        case 18:Print_String2(io_str[8],Row_no,col_no,font10,0);break;        //'O03'

        case 19:Print_String2(io_str[9],Row_no,col_no,font10,0);break;        //'M00'
        case 20:Print_String2(io_str[10],Row_no,col_no,font10,0);break;        //'M01'
        case 21:Print_String2(io_str[11],Row_no,col_no,font10,0);break;        //'M02'
        case 22:Print_String2(io_str[12],Row_no,col_no,font10,0);break;        //'M03'
        case 23:Print_String2(io_str[13],Row_no,col_no,font10,0);break;        //'M04'
        case 24:Print_String2(io_str[14],Row_no,col_no,font10,0);break;        //'M05'
        case 25:Print_String2(io_str[15],Row_no,col_no,font10,0);break;        //'M06'
        case 26:Print_String2(io_str[16],Row_no,col_no,font10,0);break;        //'M07'
        case 27:Print_String2(io_str[17],Row_no,col_no,font10,0);break;        //'M08'
        case 28:Print_String2(io_str[18],Row_no,col_no,font10,0);break;        //'M09'

        case 29:Print_String2(io_str[19],Row_no,col_no,font10,0);break;        //'T00'
        case 30:Print_String2(io_str_name[4],0,col_no,font10,0);break;        //'T00'
        default:break;
    }
}

void IO_Monitor_Online()   //IO_Monitor
{
    char IO;
        switch(IO_Stat)
            {
                case 0:
                                switch(row_no)
                                 {
                                        case 0:IO_Ptr2 = IO_Disp_L1;
                                                     IO_Ptr = IO_Par_L1;break;
                                        case 1:IO_Ptr2 = IO_Disp_L2;
                                                     IO_Ptr = IO_Par_L2;break;
                                        case 2:IO_Ptr2 = IO_Disp_L3;
                                                     IO_Ptr = IO_Par_L3;break;
                                        case 3:IO_Ptr2 = IO_Disp_L4;
                                                     IO_Ptr = IO_Par_L4;break;
                                        case 4:IO_Ptr2 = IO_Disp_L5;
                                                     IO_Ptr = IO_Par_L5;break;
                                        default:break;
                                 }

                                    for(IO=0;IO<10;IO++)
                                     {
                                            if(ram_values[ IO + IO_Ptr2 ] == 1) // Udating 10 values of IOTM  for 0 or 1 and store it into database array
                                            IO_database[ IO ] = IO + 1;                  // if 1
                                            else                                                                // else 0
                                            IO_database[ IO ] = 0;
                                     }
                                         IO_Stat = 1;                                               // Change status  to 1 once database gets 10values of IOTM
                    break;
                 case 1:                                                              // To print corrosponding IOTM values  only 10 values in on row i.e line 2,3,4,5,6
                                switch(IO_Ptr1)
                                    {
                                        case 0: IO_adrs_data_Identify(IO_database[IO_Ptr1],row_no+1,15);    break;
                                        case 1:    IO_adrs_data_Identify(IO_database[IO_Ptr1],row_no+1,20);    break;
                                        case 2: IO_adrs_data_Identify(IO_database[IO_Ptr1],row_no+1,25);    break;
                                        case 3:    IO_adrs_data_Identify(IO_database[IO_Ptr1],row_no+1,30);    break;
                                        case 4: IO_adrs_data_Identify(IO_database[IO_Ptr1],row_no+1,35);    break;
                                        case 5: IO_adrs_data_Identify(IO_database[IO_Ptr1],row_no+1,40);    break;
                                        case 6: IO_adrs_data_Identify(IO_database[IO_Ptr1],row_no+1,45);    break;
                                        case 7:    IO_adrs_data_Identify(IO_database[IO_Ptr1],row_no+1,50);    break;
                                        case 8: IO_adrs_data_Identify(IO_database[IO_Ptr1],row_no+1,55);    break;
                                        case 9:    IO_adrs_data_Identify(IO_database[IO_Ptr1],row_no+1,60);    break;
                                        case 10:IO_adrs_data_Identify((IO_Ptr),row_no+1,0);break;
                                        case 11:IO_adrs_data_Identify(30,0,0);break;//To Print Heading at line 1
                                        default:break;
                                    }

                                        IO_Ptr1++;                                        // Identify parameter value from database
                                            if(IO_Ptr1 > 11)                         // if single line displaying is done
                                                {
                                                    IO_Stat = 0;                      // Make status 0 to update next line next parameter
                                                    IO_Ptr1 = 0;                            // make ptr zero
                                                        row_no++;                         // Update row no
                                                        if(row_no > 4)                // after 5 lines make it 0 again
                                                            row_no = 0;
                                                }
                    break;
                 default:
                    break;
            }
}

void Online_Submenu_Routine(char Sub_menu_no)
{
                //if(pass_check!=5)
                //    Check_password_online();
                //    if(pass_check==5)        //pass check ok
            {
                switch(display_cursor)
                    {
                            case 1: Online_Sub_Menu(Online_Main_menu,1,Online_menu,Online_menu_options,Sub_menu_no);
                                            break;
                            case 2: Online_Sub_Menu(Online_Main_menu,2,Online_menu,Online_menu_options,Sub_menu_no);//Enter_setpoint();
                                            break;
                            case 3:    submenu_Staus_flag = 1;
                                            IO_Online_flag = 1;
                                            break;
                            case 4:    Online_Sub_Menu(Online_Main_menu,4,Online_menu,Online_menu_options,Sub_menu_no);    // Condition = (WF_Type == Feed)
                                            break;
                            default:
                                        break;
                    }
            }
}

void Cursor_Location_Temp(char line_no)
{
    char *cor[1] = {">"};
    char *blank[1] = {" "};
    char temp_line;
    temp_line = line_no;

        if((direction == 0) && (temp_line > 1) )
                        Print_String2(blank,(temp_line-1), 0,font10,0);
        if ((direction == 1)&& (temp_line < 4) )
                         Print_String2(blank,(temp_line+1), 0,font10,0);

        Print_String2(cor, line_no, 0,font10,0);

}

void Print_Online_Main_Menu(char menu_no)
{
    if(Submenu_flag == 0)  //For Main menu Display ,Submenu_flag = 0;
        {
            switch(menu_no)
                    {
                        case 0:Print_String2(&(*(Online_Main_menu[0].mm_name)), line1,0,font10,0);    //.450ms 1 line
                                    break;
                        case 1: Print_String2(&(*(Online_Main_menu[1].mm_name)), line2,7,font10,0);
                                    break;
                        case 2: Print_String2(&(*(Online_Main_menu[2].mm_name)), line3,7,font10,0);
                                    break;
                        case 3:Print_String2(&(*(Online_Main_menu[3].mm_name)), line4,7,font10,0);
                                    break;
                        case 4:/*if(WF_Type == 1)
                                        Print_String2(&(*(Online_Main_menu[4].mm_name)), line5,7,font10,0);*/
                                    break;
                        default:
                                    break;

                    }
                Cursor_Location_Temp(display_cursor);        //Cursor Position
        }

        if(Submenu_flag == 1)  //For Sub menu Display ,Submenu_flag = 1;
            {
             Online_Submenu_Routine(menu_no);
            }
}

void Online_Menu_Routine()
{
    if(Oled_flag ==3) //wait for clearing lcd
        {
             if( menu_pointer < 12 )
                {
                     if(lcd_flag==3)
                         lcd_flag = 1;

                     if(IO_Online_flag == 0)
                      Print_Online_Main_Menu(menu_pointer);

                      menu_pointer++;

                     if(menu_pointer == 12)
                        {
                            if(IO_Online_flag == 0)
                                menu_pointer = 15;  //To exit from loop

                            if(IO_Online_flag == 1)
                                menu_pointer = 0;
                        }
                }
        }
}
