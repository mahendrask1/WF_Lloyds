/* PLC.c */

#include "plc.h"
#include "keys.h"
#include "LPC17xx.h"                    // Device header
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "OLED_2_7_12864.h"
#include "lpc_types.h"
#include "functions.h"
#include "timer.h"

uint8_t    plc_acc;
 char *msg_ptr1;
//extern char* P_String;
//extern char A_String[35];
int ass_counter=0, prog_counter=0;
uint8_t ram_values[RAM_SIZE];        //values in RAM
uint16_t TIMER_COUNT[10];
uint8_t timer_in_progress[10];

char TXXX_COUNT, TIMER_ENABLED, PREV_RESULT;

//Instructions
//plc_instruction_struct plc_instructions[50] =;                //decoded instructions
plc_instruction_struct *plc_ptr;// = plc_instructions;
plc_instruction_struct plc_ass_inst[200];
plc_instruction_struct *plc_ass = plc_ass_inst;
char plc_inst[200][7];

char plc_std[3][200][7]={
    {//DI/DO std prog
        {"AI 000"},{"=O 000"},
        {"AI 001"},{"=O 001"},
        {"AI 002"},{"=O 002"},
        {"AI 003"},{"=O 003"},
        {"AI 004"},{"=O 004"},
        {"AI 005"},{"=O 005"},

        {"AI 008"},{"=O 006"},{"=O 007"},{"=O 008"},
        {"AI 009"},{"=O 009"},
        {"AI 0010"},{"=O 010"},
        {"AI 0011"},{"=O 011"},

        {"AI 012"},{"=O 012"},
        {"AI 013"},{"=O 013"},
        {"AI 014"},{"=O 014"},
        {"AI 015"},{"=O 015"},

        {"***"}
  },
    {
        {"AI 001"},//Input 1 == 1
        {"=M 009"},//Belt track status fault/healthy
        {"AI 003"},//Input 4 == 1
        {"=M 019"},//WF Drive Interlock-Healthy
        {"AI 004"},//Input 3 == 1
        {"AM 053"},//Interlock == 1
        {"SM 016"},//Set marker = Remote Start
        {"ANI004"},//Input 3 != 0
        {"AM 053"},//Interlock == 1
        {"RM 016"},//Reset marker = Remote Stop
        {"AI 005"},//Input 2 == 1
        {"=M 017"},//Downstrem Interlock
        {"AI 024"},//Inverter 1 Input marker == 1
        {"=M 008"},//Remote Mode
        {"AI 025"},//Inverter 1 Input marker == 1
        {"=M 010"},//Emergency Status
        {"ANM008"},//Remote mode == 1
        {"AI 027"},//Inverter 1 Input marker == 1
        {"SM 011"},//Set marker Start WF
        {"RM 012"},//Reset marker Stop WF
      {"ANM008"},//Remote mode == 1
        {"ANI027"},//Inverter 1 Input marker == 0
    {"RM 011"},//Reset marker Start WF
    {"SM 012"},//Set marker Stop WF
        {"AM 064"},//WF Start == 1
    {"=O 000"},//Output 0 == 1
        {"=O 001"},//Output 1 == 1
        {"AM 044"},//General Fault == 1
        {"=O 002"},//Output 2 == 1
        {"AM 068"},//PANEL HEALTHY == 1
        {"=O 003"},//Output 3 == 1
        {"AM 066"},//WF Start == 1
        {"=O 005"},//Output 5 == 1
    {"***"}

    },
    {
        /////Prakesh Industries ///////
        {"ANI000"},
        {"=M 009"},
        {"ANI002"},
        {"=M 017"},
        {"AI 003"},
        {"=M 016"},
        {"AI 004"},
        {"=M 019"},
        {"AI 026"},
        {"=M 008"},
        {"AI 027"},
        {"=M 010"},
        {"ANM008"},
        {"AI 024"},
        {"SM 011"},
        {"RM 012"},
        {"ANM008"},
        {"ANI025"},
        {"RM 011"},
        {"SM 012"},
        {"AM 064"},
        {"=O 000"},
        {"=O 001"},
        {"ANM051"},
        {"=O 002"},
        {"ANM044"},
        {"=O 003"},
        {"AM 066"},
        {"=O 004"},
        {"SM 020"},
        {"***"}
        ///// Default Program /////////
        /*{"AI 026"},
        {"=M 008"},
        {"AI 027"},
        {"=M 010"},
        {"ANM 008"},
        {"AI 024"},
        {"SM 011"},
        {"RM 012"},
      {"ANM008"},
        {"ANI025"},
    {"RM 011"},
    {"SM 012"},
        {"AM 064"},
    {"=O 000"},
        {"=O 001"},
        {"ANM051"},
        {"=O 002"},
        {"AM 044"},
        {"=O 003"},
        {"AM 066"},
        {"=O 004"},
    {"***"}*/
    }
};
uint8_t plc_temp[400];
char *inst_ptr[200][7];
plc_edit_inst *plc_edit_ptr;
unsigned char cursor_row,cursor_col,start_line,esc_status;    //PLC Edit Parameters
extern uint32_t Key_Input;
extern char Status_No,Error_No;

//unsigned char PLC_Array[300][8];
//plc_instruction_struct *plc_inst2;

/**********************************************************************************
void    plc_execute(plc_instruction_struct *plc_inst[] )
//Executes the Decoded PLC program/instructions in structure of type plc_instruction_struct

;ADDRESS OFFSET FOR EACH O,I,M OR T ADDRESSES
;FOR I, OFFSET = 00  + "I" ADDRESS IN PLC PROGRAM
;FOR O, OFFSET = 40  + "O" ADDRESS IN PLC PROGRAM
;FOR M, OFFSET = 80  + "M" ADDRESS IN PLC PROGRAM
;FOR T, OFFSET = 184 + "T" ADDRESS IN PLC PROGRAM

;ALGORITHM
;1.0 "ACC." = "1"
;1.1 IF "01", GET STATUS OF ADDRESSED BYTE & "AND" WITH "ACC."
;1.2 IF "02", GET STATUS OF ADDRESSED BYTE,COMPLEMENT & "AND" WITH "ACC."
;1.3 IF "03", GET STATUS OF ADDRESSED BYTE & "OR" WITH "ACC."
;1.4 IF "04", GET STATUS OF ADDRESSED BYTE,COMPLEMENT & "OR" WITH "ACC."
;
;2.0 IF NEXT LINE NOT "01","02","03" OR "04", JUMP 3.0
;2.1 ELSE, JUMP 1.1
;
;3.0 IF NEXT LINE NOT "07", JUMP 4.0
;3.1 TRANSFER ACC. TO ADDRESSED BYTE
;3.2 JUMP 8.0
;
;4.0 IF NEXT LINE NOT "05", JUMP 5.0
;4.1 MAKE ADDRESSED BYTE "1", IF "ACC." = "01"
;4.2 JUMP 8.0
;
;5.0 IF NEXT LINE NOT "06", JUMP 6.0
;5.1 MAKE ADDRESSED BYTE "0", IF "ACC." = "01"
;5.2 IF TIMER RESET,MAKE "TIMER IN PROGRESS" BYTE="00"
;5.3 JUMP 8.0
;
;6.0 IF NEXT LINE NOT "08", JUMP 7.0
;6.1 SAVE TIMER NUMBER IN "TIMER_ENABLED" VARIABLE
;6.2 JUMP 8.0
;
;7.0 IF NEXT LINE NOT "09", JUMP 8.0
;7.1 IF "ACC"="00" & "TIMER IN PROGRESS"="01", MAKE "TIMER IN PROGRESS"=00 (STOP TIMER)
;7.2 IF "ACC"="01" & "TIMER IN PROGRESS"="00", MAKE "TIMER IN PROGRESS"=01 FOR TIMER IN "TIMER ENABLED"  (START TIMER)
;             SAVE TIME VALUE IN NEXT BYTE IN "TXXX COUNT"  AFTER MULTIPLYING BY 200
;7.3 IF "ACC"="01" & "TIMER IN PROGRESS"="01",JUMP TO 8.0
;7.4 JUMP 8.0
;
;8.0 IF NOT "FF", JUMP 1.0

;9.0     DECREMENT "TXXX COUNT" VALUE WITH "TIMER IN PROGRESS" = "01"
     IF "TXXX COUNT" = 0 & "TIMER IN PROGRESS" =01, MAKE "TXXX" = 01, "TIMER IN PROGRESS" =0

**********************************************************************************/
void    plc_execute(void)
{
int i=0,t;
char temp;
/*    plc_inst[0]->inst = 1;
    plc_inst[1]->inst = 2;
    plc_inst[2]->inst = 3;
    plc_inst[3]->inst = 4;
    plc_inst[4]->inst = 5;
    plc_inst[5]->inst = 6;
    plc_inst[6]->inst = 0xff;
*/

plc_acc = 1;
/*    if(plc_inst[i]->inst    == 0x01)
            plc_acc =plc_acc & ram_values[(plc_inst[i]->addr)];

    temp = ram_values[(plc_inst[i]->addr)];        //temporary storage of value
*/

    //while(1)
//    for(i=0;(plc_inst[i]->inst)!=0xFF    ;i++)
//    temp = *(    ram_values+plc_ptr[i].addr);//ram_values[(plc_ptr[i].addr)];        //temporary storage of value
    for(i=0;(plc_ptr[i].inst)!=0xFF    ;i++)
    {
//        Clear_LCD();
//    Print_Variable(plc_ptr[i].inst,line1,25,font32,0);
//  super_delay(70);
temp = *(ram_values+plc_ptr[i].addr);//ram_values[(plc_ptr[i].addr)];        //temporary storage of value

            switch(plc_ptr[i].inst)
    {
        case 1:    plc_acc =(plc_acc) & (temp) ;                //AND operation
                        //Print_Variable(plc_ptr[i].addr,line5,25,font10,0);
                        //super_delay(70);
                      break;
        case 2: plc_acc = (plc_acc) & (~(temp)) ;//(temp) ;            //Not & AND operation
                        //Print_Variable(plc_ptr[i].addr,line5,25,font10,0);
                        //plc_acc = (~plc_acc);
                        break;
        case 3: plc_acc = (((plc_acc)&(0x01)) | ((temp)&(0x01))) ;                //OR operation //chnaged to ((temp)&0x01) from (temp) ,to get only last bit as accumulator is performing single bit operation
                        break;
        case 4: plc_acc =(((plc_acc)&(0x01))  | ((~temp)&(0x01))) ;                //Not & OR operation
                        break;
        case 7: //ram_values[(plc_ptr[i].addr)]    = plc_acc;        //Equate Operation
                        *(    ram_values+plc_ptr[i].addr) =plc_acc;
                        if((plc_ptr[i+1].inst)<=4)
                            plc_acc=1;
                        break;
        case 5:    if(plc_acc==1)                                                                //SET
                            ram_values[(plc_ptr[i].addr)] = 1;
                        if((plc_ptr[i+1].inst)<=4)
                            plc_acc=1;
                        break;
        case 6: if(plc_acc ==1)                                                                //RESET
                {    //changed here on added "{ }"  7.9.2016
                        ram_values[(plc_ptr[i].addr)] = 0;

                        if((plc_ptr[i].addr)>=184)                //if inst is TIMER RESET
                        {
                            if((plc_ptr[i].addr)<=193)
                                timer_in_progress[((plc_ptr[i].addr)-184)] = 0;        //time_in_progress=0 if TIMER RESET    (selected TIMER+10)

                        }
                        }
                        if((plc_ptr[i+1].inst)<=4)
                                plc_acc=1;

                        break;
        case 8: TIMER_ENABLED = (plc_ptr[i].addr);        //LOAD TIMER
                            //Loads the TIMER with ADDRESS in instruction
                        if((plc_ptr[i+1].inst)<=4)
                            plc_acc=1;

                        break;

        case 9:    if(plc_acc ==0)
                            if(timer_in_progress[TIMER_ENABLED-184]==1)        //if timer_in_progress==1, and plc_acc==0; STOP TIMER
                                timer_in_progress[TIMER_ENABLED-184] = 0;      //STOP Timer
                        if(plc_acc ==1)                                                //if plc_acc==1, and timer_in_progress==0; START TIMER
                            if(timer_in_progress[TIMER_ENABLED-184] ==0 )
                            {
                                timer_in_progress[TIMER_ENABLED-184] = 1;            //START Timer enabled in TIMER ENABLED
//                                    Print_Variable(ram_values[(plc_ptr[i].addr)],3,3,font16,0);
//                super_delay(80);
                                TIMER_COUNT[TIMER_ENABLED-184] = ( ((plc_ptr[i].addr)-184) * 100);            //+20 is the address of SELECTED TIMERS COUNT VALUE =
                //Print_Variable(((plc_ptr[i].addr)-184),2,3,1,0);
                //Print_Variable(( ((plc_ptr[i].addr)-184) * 100),3,3,1,0);
    //            super_delay(80);

                                //                    Print_Variable(T000,3,3,font16,0);
                                //                                TXXX_COUNT =( plc_inst[i]->addr * 200);        //the Value in the ADDRESS * 200 i.e. 5msec * 200 =1sec
                            }
//                ;             SAVE TIME VALUE IN NEXT BYTE IN "TXXX COUNT"  AFTER MULTIPLYING BY 200
                            plc_acc=1;   //changed here on 7.9.2016

                        break;

        default: break;
    }

    }

    //if plc_inst[i]->inst ==0xFF    i.e. the instruction is E.O.P THEN DECREMENT THE TIMER COUNTS
for(t=0;t<10;t++)    //Check the timer
    if(timer_in_progress[t]==1)
    {
        TIMER_COUNT[t]--;

        if(TIMER_COUNT[t]==0)        //if TIMER_COUNT = 0
        {
        //    Print_Variable(TIMER_COUNT[t],2,3,1,0);
          ////Print_Variable(( ((plc_ptr[i].addr)-184) * 100),3,3,1,0);
            //Error_No = 13;
            ram_values[t+184] = 1;    //make TXXX=1,    if TIMER_COUNT=0
            timer_in_progress[t]=0; //timer_in_progress=0,if TIMER_COUNT=0
        }
    }

}

//PLC Assemble Function
void plc_assemble (void)
{ int i;
    int inc;
    char temp1;
    int add_offset;

    for(i=0;i<200;i++)
    {

            temp1=plc_inst[i][0];
            if(temp1 == 'A')                        //AND
            {
                if(plc_inst[i][1] =='N')    //NOT
                {
                    plc_ass[i].inst = 2;
                    inc=1;
                }
                else
                {
                    plc_ass[i].inst    =1;
                    inc=0;
                }

            }
                else if(temp1 == 'O')                    //OR
                {
                    if(plc_inst[i][1] =='N')    //NOT
                        {
                            plc_ass[i].inst = 4;
                            inc=1;
                        }
                        else
                        {
                            plc_ass[i].inst    = 3;
                            inc=0;
                        }
                    }
                else if(temp1 == 'S')                    //SET
                {
                    if(plc_inst[i][1] =='T')    //Timer
                        {
                            plc_ass[i].inst = 9;
                            inc=0;
                        }
                        else
                        {
                            plc_ass[i].inst    = 5;
                            inc=0;
                        }
                    }
                else if(temp1 == 'R')                //RESET
                {
                    plc_ass[i].inst = 6;
                    inc=0;
                }
                else if(temp1 == '=')                //ASSIGN
                {
                    plc_ass[i].inst = 7;
                    inc=0;
                }
                else if(temp1 == 'L')                //LOAD
                {
                    plc_ass[i].inst = 8;
                    inc=0;
                }
                else if(temp1 == '*')                //END
                {
                    if(plc_inst[i][1] == '*')
                        if(plc_inst[i][2] == '*')
                        {
                            plc_ass[i].inst = 255;
                            plc_ass[i].addr = 255;

plc_temp[i*2]=(unsigned char)plc_ass[i].inst;
plc_temp[(i*2)+1]=(unsigned char)plc_ass[i].addr;
                            inc=0;
                            break;
                        }
                }

if (plc_inst[i][1+inc] =='I') add_offset=0;                    //Input
else if (plc_inst[i][1+inc]=='O') add_offset=40;        //Output
else if (plc_inst[i][1+inc]=='M') add_offset=80;        //Memory
else if (plc_inst[i][1+inc]=='T') add_offset=184;        //Timer
else add_offset=255;

if((inc == 1)&&((plc_ass[i].inst == 4)||(plc_ass[i].inst == 2)))   ////changed here on 7.9.2016
{
    inc = 0;
}

plc_ass[i].addr = add_offset + (plc_inst[i][3+inc]-48)*100 + (plc_inst[i][4+inc]-48)*10  + (plc_inst[i][5+inc]-48);
////////////////////////

plc_temp[i*2]=(unsigned char)plc_ass[i].inst;
plc_temp[(i*2)+1]=(unsigned char)plc_ass[i].addr;

/*                Clear_LCD();
                Print_Variable(plc_ass[i].inst,line2,45,font10,0);
                Print_Variable(plc_ass[i].addr,line3,45,font10,0);
                super_delay(60);
*/
////////////////////////
    }//for loop

    save_ass_eeprom();

}    //function end

//PLC Delete Function
void plc_delete (void)
{
    int i,j;
    for(i=0;i<200;i++)
        for(j=0;j<6;j++)
            plc_inst[i][j]=' ';
    plc_inst[0][0]='*';
    plc_inst[0][1]='*';
    plc_inst[0][2]='*';

                save_plc_edit_eeprom();

}

void Print_PLC_Page(void)
{
    char under_line;
Clear_LCD_RAM();
Print_Variable1(start_line    ,0,1,font10,0);
Print_Variable1(start_line+1,1,1,font10,0);
Print_Variable1(start_line+2,2,1,font10,0);
Print_Variable1(start_line+3,3,1,font10,0);
Print_Variable1(start_line+4,4,1,font10,0);

    //    msg_ptr1="ABCDEF";// *PLC_Array[0];
msg_ptr1=plc_inst[start_line];//LINE1
    //    sprintf(msg_ptr1,"\0");
//strcat(msg_ptr1,"\0");
*(msg_ptr1+6)=0;
//    *(msg_ptr1+7)=0;
 //*(msg_ptr1+7)='0';
    if(cursor_row-start_line ==0)
        under_line=cursor_col+1;
    else
        under_line=0;
    Print_String2(&msg_ptr1,line1,15,font10,under_line);

msg_ptr1=plc_inst[start_line+1];//LINE2
*(msg_ptr1+6)=0;
    if(cursor_row-start_line ==1)
        under_line=cursor_col+1;
    else
        under_line=0;
    Print_String2(&msg_ptr1,line2,15,font10,under_line);

msg_ptr1=plc_inst[start_line+2];//LINE3
*(msg_ptr1+6)=0;
    if(cursor_row-start_line ==2)
        under_line=cursor_col+1;
    else
        under_line=0;
    Print_String2(&msg_ptr1,line3,15,font10,under_line);

msg_ptr1=plc_inst[start_line+3];//LINE4
*(msg_ptr1+6)=0;
    if(cursor_row-start_line ==3)
        under_line=cursor_col+1;
    else
        under_line=0;
    Print_String2(&msg_ptr1,line4,15,font10,under_line);

msg_ptr1=plc_inst[start_line+4];//LINE5
*(msg_ptr1+6)=0;
    if(cursor_row-start_line ==4)
        under_line=cursor_col+1;
    else
        under_line=0;
    Print_String2(&msg_ptr1,line5,15,font10,under_line);
    PRINT_LCD();
}

void    plc_edit(void)
{
    int i,j;
    cursor_row=0;
    cursor_col=0;
    start_line=0;
    esc_status=0;
    Key_Input=NO_KEY;

    back_key_check:
        Print_PLC_Page();
    Key_Input=NO_KEY;
while(Key_Input == NO_KEY)
                    key_check_routine();
super_delay(50);
/*
    sprintf(A_String,"Key:%lu %lu\n\r",Key_Input,cursor_col);
        P_String = A_String;
        UART_Send((LPC_UART_TypeDef *)LPC_UART2, P_String, strlen(P_String), BLOCKING);
*/
        if(Key_Input == ESC && esc_status==0)
            {
                    Key_Input=NO_KEY;
                    esc_status=1;
            }

        else if (esc_status==1)  //Esc + Operations
            {
                esc_status=0;
                if(Key_Input==CL)
                {
                    save_plc_edit_eeprom();
                        Key_Input=NO_KEY;
                    return;
                }//save and exit

                else if(Key_Input==4)        //Insert line
                    {
                        Key_Input=NO_KEY;
                        for(i=199;i>cursor_row;i--)
                                for(j=0;j<6;j++)
                                    plc_inst[i][j]=plc_inst[i-1][j];
                            plc_inst[cursor_row][0]=' ';
                            plc_inst[cursor_row][1]=' ';
                            plc_inst[cursor_row][2]=' ';
                            plc_inst[cursor_row][3]=' ';
                            plc_inst[cursor_row][4]=' ';
                            plc_inst[cursor_row][5]=' ';
                        cursor_col=0;
                    }

                else if(Key_Input==2)    // " Esc + D "Delete Line
                    {
                        if(   (plc_inst[cursor_row][0]=='*')&&
                                    (plc_inst[cursor_row][1]=='*')&&
                                     (plc_inst[cursor_row][2]=='*'))
                        {
                            Key_Input=NO_KEY;
                            goto back_key_check;

                        }

                        Key_Input=NO_KEY;
                        for(i=cursor_row;i<199;i++)
                            {
                                for(j=0;j<6;j++)
                                    {
                                        plc_inst[i][j]=plc_inst[i+1][j];
                                    }
                            }
                            cursor_col=0;
                    }
                else if(Key_Input==HOR)    //Move Reverse Horizontal
                    {
                    Key_Input=NO_KEY;
                    {    if(cursor_col==0)
                        {
                                cursor_col=6;
                                if((cursor_row-start_line==0) && (cursor_row!=0) )
                                    {
                                        start_line--;
                                            cursor_row--;
                                    }
                        }
                        cursor_col--;
                    }
                    }
                    else
                        Key_Input=NO_KEY;
            }
        else if(Key_Input == HOR)    //Move cursor Horizontal forword
            {Key_Input=NO_KEY;
                if(plc_inst[cursor_row][0]!='*' && plc_inst[cursor_row][1]!='*' && plc_inst[cursor_row][2]!='*')
                    {
                        cursor_col++;
                        if(cursor_col==6)
                            {
                                cursor_col=0;
                                cursor_row++;
                                if(cursor_row-start_line>4)
                                    start_line++;
                            }
                    }
            }
        else if(Key_Input==K_INC)    //Move cursor up
            {Key_Input=NO_KEY;
                if(cursor_row-start_line==0 && (cursor_row!=0))
                    {
                            start_line--;
                            cursor_row--;
                    }
                else if (cursor_row-start_line !=0)
                    cursor_row--;
            }

        else if(Key_Input ==K_DEC)        // Move cursor Down
            { Key_Input=NO_KEY;
                if(plc_inst[cursor_row][0]!='*' && plc_inst[cursor_row][1]!='*' && plc_inst[cursor_row][2]!='*')
                    {
                        cursor_row++;
                        if(cursor_row-start_line>4)
                            start_line++;
                    }
            }

        else if(Key_Input<10 && cursor_col>2)
            {
                if(plc_inst[cursor_row][0]!='*' && plc_inst[cursor_row][1]!='*' && plc_inst[cursor_row][2]!='*')
                {
                plc_inst[cursor_row][cursor_col] = Key_Input+48;
                cursor_col++;
                if(cursor_col==6)
                    {
                        cursor_col=0;
                        cursor_row++;
                        if(cursor_row-start_line>4)
                            start_line++;
                    }
                }
                Key_Input=NO_KEY;
            }
        else if(cursor_col<3)
                {if(plc_inst[cursor_row][0]!='*' && plc_inst[cursor_row][1]!='*' && plc_inst[cursor_row][2]!='*')
                    {
                    if(Key_Input == 1){Key_Input=NO_KEY;
                    plc_inst[cursor_row][cursor_col] = 'A';}
                    else if(Key_Input == 2    ){Key_Input=NO_KEY;
                        plc_inst[cursor_row][cursor_col] = 'D';}
                    else if(Key_Input == 3    ){Key_Input=NO_KEY;
                        plc_inst[cursor_row][cursor_col] = 'E';}
                    else if(Key_Input == 4    ){Key_Input=NO_KEY;
                        plc_inst[cursor_row][cursor_col] = 'I';}
                    else if(Key_Input == 5    ){Key_Input=NO_KEY;
                        plc_inst[cursor_row][cursor_col] = 'L';}
                    else if(Key_Input == 6    ){Key_Input=NO_KEY;
                        plc_inst[cursor_row][cursor_col] = 'M';}
                    else if(Key_Input == 7    ){Key_Input=NO_KEY;
                        plc_inst[cursor_row][cursor_col] = 'N';}
                    else if(Key_Input == 8    ){Key_Input=NO_KEY;
                        plc_inst[cursor_row][cursor_col] = 'O';}
                    else if(Key_Input == 9    ){Key_Input=NO_KEY;
                        plc_inst[cursor_row][cursor_col] = 'P';}
                    else if(Key_Input == 0    ){Key_Input=NO_KEY;
                        plc_inst[cursor_row][cursor_col] = '*';}
                    else if(Key_Input == INT_DE_INT        ){Key_Input=NO_KEY;
                        plc_inst[cursor_row][cursor_col] = 'S';}
                    else if(Key_Input == Auto_Man    ){Key_Input=NO_KEY;
                        plc_inst[cursor_row][cursor_col] = 'R';}
                    else if(Key_Input == Start_Stop)    {Key_Input=NO_KEY;
                        plc_inst[cursor_row][cursor_col] = 'U';}
                    else if(Key_Input == k_SET)    {Key_Input=NO_KEY;
                        plc_inst[cursor_row][cursor_col] = '=';}
                    else if(Key_Input == Zero_Key){Key_Input=NO_KEY;
                        plc_inst[cursor_row][cursor_col] = 'T';}
                    else{
                                goto back_key_check;}
                    cursor_col++;
                            }
                }
        goto back_key_check;
}

void plc_insert(char std)
{
    int i,j;
    for(i=0;i<300;i++)
        for(j=0;j<6;j++)
            plc_inst[i][j]=plc_std[std][i][j];
            save_plc_edit_eeprom();
        super_delay(50);
            Key_Input  = NO_KEY;
            plc_edit();
}

void plc_menu(void)
{
    int i,j;
    unsigned char plc_menu_str[10][20]=
        {
                                                        {"PLC EDITOR"},
                                                        {"PLCS :"},
                                                        {"PLCA :"},
                                                        {"A-Assmbl"},
                                                        {"D-Delete"},
                                                        {"E-Edit"},
                                                        {"I-Insert"},
                                                        {"Entered Sel:"}

      };

                read_plc_edit_eeprom();

back_menu:
    Clear_LCD_RAM();

                    msg_ptr1=plc_menu_str[0];
        Print_String2(&msg_ptr1,line1,1,font10,0);    //Print Header at line1

                    msg_ptr1=plc_menu_str[1];
        Print_String2(&msg_ptr1,line2,5,font10,0);    //Print PLCS at line2

                    msg_ptr1=plc_menu_str[2];
        Print_String2(&msg_ptr1,line3,5,font10,0);    //Print PLCA at line3

                    msg_ptr1=plc_menu_str[3];
        Print_String2(&msg_ptr1,line4,1,font10,0);    //Print Assemble at line4

                    msg_ptr1=plc_menu_str[4];
        Print_String2(&msg_ptr1,line4,35,font10,0);    //Print Delete at line4

                    msg_ptr1=plc_menu_str[5];
        Print_String2(&msg_ptr1,line5,1,font10,0);    //Print Edit at line5

                    msg_ptr1=plc_menu_str[6];
        Print_String2(&msg_ptr1,line5,35,font10,0);    //Print Insert at line5

                    msg_ptr1=plc_menu_str[7];
        Print_String2(&msg_ptr1,line6,1,font10,0);    //Print EnterPar at line6

        prog_counter=1;
        //1st time program
    for(i=0;((plc_inst[i][2]!='*')&&(i<=200));i++)
        prog_counter++;
Print_Variable1((prog_counter-1)    ,line2,30,font10,0);

        ass_counter=0;
    for(j=0;plc_temp[j] != 255;j++)
        ass_counter++;
Print_Variable1(ass_counter/2    ,line3,30,font10,0);

PRINT_LCD();
back_to_keycheck:
    Key_Input=NO_KEY;
    while(Key_Input == NO_KEY)
                    key_check_routine();
    super_delay(50);

    if (Key_Input == 1)
    {
        //if(plc_inst[0][0]!='*' && plc_inst[0][1]!='*' && plc_inst[0][2]!='*')
        plc_assemble();
        goto back_menu;
    }
    else if(Key_Input == 3)
    {
Print_Variable1(Key_Input    ,line2,55,font10,0);
PRINT_LCD();
        plc_edit();
        goto back_menu;
    }
    else if(Key_Input == 2)
    {
        plc_delete();
        goto back_menu;
    }
    else if(Key_Input == 4)
    {
        Key_Input=NO_KEY;
        super_delay(50);
            while(Key_Input >9 && Key_Input != ESC)
                    key_check_routine();
            if (Key_Input == ESC)
                return;
Print_Variable1(Key_Input    ,line6,45,font10,0);
PRINT_LCD();
//super_delay(100);
            plc_insert(Key_Input);

        super_delay(50);
        goto back_menu;
    }
    else if(Key_Input == ESC)
        return;

    goto back_to_keycheck;

}
/*
16/07/2018
PLC 'NOT+OR' and 'OR' operation

Chnaged to ((temp)&0x01) from (temp) ,to get only last bit as accumulator is performing single bit operation
Chnaged to ((~temp)&0x01) from (~temp) ,to get only last bit as accumulator is performing single bit operation

Note :not required in 'AND' operation as its already performing logical AND on entire byte so doesnt affect AND operation

*/
