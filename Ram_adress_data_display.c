//#include <stdio.h>
//#include "parameters.h"    //#include "menu.h"
//#include "main_routine.h"
//#include "OLED_2_7_12864.h"
//#include "stdio.h"
//#include "LPC17xx.h"
//#include "cmsis_os.h"
//#include "Board_Joystick.h"
//#include "keys.h"
//#include "lpc_types.h"
//#include "lpc17xx_rtc.h"
//#include "functions.h"

//void Ram_adrs_data_display();
//char Ram_display_status;
char *Ram_add_no[16][1]={ {"0"},{"1"},{"2"},{"3"},{"4"},{"5"},{"6"},{"7"},{"8"},{"9"},
                                                    {"a"},{"b"},{"c"},{"d"},{"e"},{"f"}};
//
//char Ascii_database[20],inc_pointer,row_no;
//char Ram_status,Adress_Counter;
//char database_[11];
//unsigned char *Ram_pointer,*Ram_pointer_1,*Ram_pointer_;
//uint16_t Ram_display_Enable_counter;
//int Ram_adres,Ram_adres_1,Ram_adres_;
//extern char Oled_flag;
//void Ram_adrs_data_Identify(char data_, char Shift_val,char Row_no,char col_no)
//{
//    switch(((char)(data_)))
//     {
//        case 0:Print_String(Ram_add_no[0],Row_no,col_no,font10,0);break;      //'0'
//        case 1:Print_String(Ram_add_no[1],Row_no,col_no,font10,0);break;      //'1'
//        case 2:Print_String(Ram_add_no[2],Row_no,col_no,font10,0);break;      //'2'
//        case 3:Print_String(Ram_add_no[3],Row_no,col_no,font10,0);break;      //'3'
//        case 4:Print_String(Ram_add_no[4],Row_no,col_no,font10,0);break;      //'4'
//        case 5:Print_String(Ram_add_no[5],Row_no,col_no,font10,0);break;      //'5'
//        case 6:Print_String(Ram_add_no[6],Row_no,col_no,font10,0);break;      //'6'
//        case 7:Print_String(Ram_add_no[7],Row_no,col_no,font10,0);break;      //'7'
//        case 8:Print_String(Ram_add_no[8],Row_no,col_no,font10,0);break;      //'8'
//        case 9:Print_String(Ram_add_no[9],Row_no,col_no,font10,0);break;      //'9'
//        case 10:Print_String(Ram_add_no[10],Row_no,col_no,font10,0);break;  //'a'
//        case 11:Print_String(Ram_add_no[11],Row_no,col_no,font10,0);break;  //'b'
//        case 12:Print_String(Ram_add_no[12],Row_no,col_no,font10,0);break;  //'c'
//        case 13:Print_String(Ram_add_no[13],Row_no,col_no,font10,0);break;  //'d'
//        case 14:Print_String(Ram_add_no[14],Row_no,col_no,font10,0);break;  //'e'
//        case 15:Print_String(Ram_add_no[15],Row_no,col_no,font10,0);break;  //'f'
//        default:break;
//
//    }

//}

//void Ram_adrs_data_display()
//{
//    char i,j,kpoint=0;
//
//
//    if(( Ram_display_status == 3 )&&(Oled_flag == 3))//if lcd is clear
//         {
//            switch(Ram_status)
//            {
//                case 0:
//                                Ram_adres_ = Ram_adres;
//                                Ram_pointer_ = Ram_pointer;
//                                Ram_status = 1;
//                break;
//                case 1:
//                                Ram_adres_1 = Ram_adres_;
//                                Ram_pointer_1 = Ram_pointer_;
//                                Ram_status = 2;
//                    break;
//              case 2:
//                                database_[0]= (char)(Ram_adres_1>>8);
//                                database_[1]= (char)(Ram_adres_1);
//                                for(i=2;i<10;i++)
//                                    {
//
//                                        database_[i] = *(Ram_pointer_1++);
//                                    }
//                                for(j=0;j<11;(j++))
//                                    {
//                                        Ascii_database[kpoint] = ( database_[j] >> 4);
//                                        Ascii_database[kpoint+1] = ( database_[j] & 0x0f);
//                                        kpoint=kpoint+2;
//                                    }
//                                    Ram_status = 3;
//
//                    break;
//                case 3:
//                                switch(inc_pointer)
//                                    {
//                                        case 0 :Ram_adrs_data_Identify(Ascii_database[inc_pointer],0,row_no,0);
//                                                        Ram_adrs_data_Identify(Ascii_database[inc_pointer+1],0,row_no,3);break;
//
//                                        case 2 :Ram_adrs_data_Identify(Ascii_database[inc_pointer],0,row_no,6);
//                                                        Ram_adrs_data_Identify(Ascii_database[inc_pointer+1],0,row_no,9);break;
//
//                                        case 4 :Ram_adrs_data_Identify(Ascii_database[inc_pointer],0,row_no,15);
//                                                        Ram_adrs_data_Identify(Ascii_database[inc_pointer+1],0,row_no,18);break;
//
//                                        case 6 :Ram_adrs_data_Identify(Ascii_database[inc_pointer],0,row_no,21);
//                                                        Ram_adrs_data_Identify(Ascii_database[inc_pointer+1],0,row_no,24);break;
//
//                                        case 8 :Ram_adrs_data_Identify(Ascii_database[inc_pointer],0,row_no,27);
//                                                        Ram_adrs_data_Identify(Ascii_database[inc_pointer+1],0,row_no,30);break;
//
//                                        case 10 :Ram_adrs_data_Identify(Ascii_database[inc_pointer],0,row_no,33);
//                                                        Ram_adrs_data_Identify(Ascii_database[inc_pointer+1],0,row_no,36);break;
//
//                                        case 12 :Ram_adrs_data_Identify(Ascii_database[inc_pointer],0,row_no,39);
//                                                        Ram_adrs_data_Identify(Ascii_database[inc_pointer+1],0,row_no,42);break;
//
//                                        case 14 :Ram_adrs_data_Identify(Ascii_database[inc_pointer],0,row_no,45);
//                                                         Ram_adrs_data_Identify(Ascii_database[inc_pointer+1],0,row_no,48);break;
//
//                                        case 16 :Ram_adrs_data_Identify(Ascii_database[inc_pointer],0,row_no,51);
//                                                         Ram_adrs_data_Identify(Ascii_database[inc_pointer+1],0,row_no,54);break;
//
//                                        case 18 :Ram_adrs_data_Identify(Ascii_database[inc_pointer],0,row_no,57);
//                                                         Ram_adrs_data_Identify(Ascii_database[inc_pointer+1],0,row_no,60);break;
//
//                                        default: break;
//
//                                        }
//
//                                        inc_pointer++;
//
//                                        if(inc_pointer>18)
//                                            {
//                                                Ram_status = 2;
//                                                inc_pointer= 0;
//                                                row_no++;
//                                                    Ram_adres_1 = Ram_adres_1 + 8;
//                                                    if(row_no > 5)
//                                                        {
//                                                            Ram_status = 1;
//                                                            row_no = 0;
//                                                        }
//                                            }
//                                    break;
//                                default: break;
//
//            }
//
//
//
//
//
//
//            }
//}
