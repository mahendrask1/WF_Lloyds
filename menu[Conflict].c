/*Menu.c*/

#include "menu.h"
#include "main_routine.h"
#include "OLED_2_7_12864.h"
#include "stdio.h"
#include "LPC17xx.h"
#include "cmsis_os.h"
#include "Board_Joystick.h"

//static main_menu_struct abc;
	char menu_counter,Sub_Menu_no;
	char menu_status, display_line1, display_line4;
	char start_display_pointer, end_display_pointer, display_cursor;
char direction;
 	char sub_menu_status;
	//char *pp;

//sub_menu_struct sub_1[3][10];//,*sub_2,*sub_3;
//sub_menu_struct sub_m1;
//sub_menu_struct *sm1 = &sub_m1;

//sm1 = &(sub_m1);
//const sub_menu_struct sub_menu =
sub_menu_struct sub_menu[7][15] ={
	{// 0 Measuring Parameters
		{'O',4,0},	//
		{'E',40,0},
		{'O',3,0},
		{'O',2,0},
		{'O',4,0},
		{'F',255,255}
	},
	{// 1 Configuration
		{'O',2,0},
		{'O',2,0},
		{'O',6,0},
		{'E',20,0},
		{'E',41,0},
		{'E',20,0},
		{'E',20,0},
		{'E',41,0},
		{'O',2,0},
		{'O',2,0},
		{'F',255,255}
	},
	{//2 Interface
		{'O',5,0},
		{'O',3,0},
		{'O',3,0},
		{'E',20,0},
		{'E',20,0},
		{'F',255,255}
	}
	
};

const main_menu_struct Main_menu[Main_Menu_Max]= {		//define the array of structures with max size Main_Menu_Max
	 /*Main_menu[0] = */	{'M',0,"Main Menu:"},//,&Main_menu[0].mm_name},
	/*Main_menu[1] = */	{'M',0,"1 Measuring Par."},//,&Main_menu[0].mm_name},
	/*Main_menu[2] = */	{'M',0,"2 Configuration"},
	/*Main_menu[3] = */	{'M',0,"3 Interface"},
	/*Main_menu[4] = */	{'M',0,"4 Calibration"},
	/*Main_menu[5] = */	{'M',0,"5 PLC Editor"},
	/*Main_menu[6] = */	{'M',0,"6 System Data"},
	/*Main_menu[7] = */	{'M',0,"7 Main Program"},
	
	/*Main_menu[8] = */	{'F',255,"8 FF"}
				};

char const *pp;
	char const t_menu[10][10][10][10]={
		{
	 	//0 Measuring parameters
					{{"M_Range"},{"1.00mV"},{"3.00mV"},{"6.00mV"},{"12.0mV"}},
					{{"Resolution"},{'E'}},
					{{"Digit_Step"},{"1"},{"2"},{"3"}},
					{{"Units_Cap"},{"kg"},{"t"}},
					{{"D_Pt_Total"},{"00000"},{"0000.0"},{"000.00"},{"00.000"}}
		},
		{
			//1 Configuration
			{{"Scale Type"},{"Batch"},{"LIW"}},
			{{"Mode"},{"Full"},{"Restricted"}},
			{{"No of Comp"},{"1"},{"2"},{"3"},{"4"},{"5"},{"6"}},
			{{"Scale Num"},{'E'}},
			{{"Zero Range"},{'E'}},
			{{"Sample Frq"},{'E'}},
			{{"Sample Num"},{'E'}},
			{{"Sample Tmr"},{'E'}},
			{{"I1 Mode"},{"0-20mA"},{"4-20mA"}},
			{{"I1 Value"},{"Net"},{"Gross"}}
		},
		{
				//2 Interface
			{{"No of DIO"},{"1"},{"2"},{"3"},{"4"}},
			{{"SIO Sel"},{"Off",},{"Printer"},{"Modbus"}},
			{{"SOP Baud"},{"4800"},{"9600"},{"19200"}},
			{{"SIO Addr"},{'E'}},
			{{"SIO Mon"},{'E'}}
		}
			

	};


/*
char *sub_menu_options123[5][5][6][20]={
{	
	{{"1.00mV"},{"3.00mV"},{"6.00mV"},{"12,0mV"}},
	{{"1"},{"2"},{"3"}}
},	
{
	{	{"Batch"},{"LIW"}	},
	{	{"FULL"},{"RESTRICTED"} },
	{ {"1"},{"2"},{"3"},{"4"},{"5"}	}
}
};

*/

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Cursor_Location (Routine to display Cursor pointer)
//Step 1: Clear Cursor Location of all the 4 lines
//Step 2: Print Cursor('>') at the given line_no
//Note: Font size is fixed. i.e. font10
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Cursor_Location(char line_no)
{
	char *cor[1] = {">"};
	char *blank[1] = {" "};
	
				Print_String(blank,1, 0,font10,0);	//
				Print_String(blank,2, 0,font10,0);	//Clear Cursor locations
				Print_String(blank,3, 0,font10,0);	//
				Print_String(blank,4, 0,font10,0);	//

				Print_String(cor, line_no, 0,font10,0);
}

	
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Display Main Menu Routine
//Step 1: Clear LCD
//Step 2: Print MENU Heading on Line0
//Step 3: Print String at print_line1 address on line1
//Step 4: Print String at print_line1+1 address on line2
//Step 5: Print String at print_line1+2 address on line3
//Step 6: Print String at print_line1+3 address on line4
//Step 7: Print Notice string on line5
//Step 8: display cursor = (display_cursor - print_line1 + 1)
//Step 9: Call cursor routine
//Note: Font size is fixed. i.e. font10
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void display_main_menu_routine(char print_line1, char display_cursor)//,main_menu_struct *Main_menu)
			{
unsigned int disp_cursor;
char *notice[24] = {"Right-Lft,Up-Dwn"};
char *print_menu;
/*const char sub_menu_option_table[10][10][10] ={
{//0 Measuring Paramenters
		{{"1.00mV"},{"3.00mV"},{"6.00mV"},{"12.0mV"}},
		{{"Resolution"}},
		{{"1"},{"2"},{"3"}},
		{{"Kg"},{"t"}},
		{{"00000"},{"0000.0"},{"000.00"},{"00.000"}}
	}//,
	
//	{	//1 Configuration
		
};*/

 
				Clear_LCD();
				
//				Print_String((Main_menu[0].mm_name), line1, 0,font10,0);
				print_menu = *(Main_menu[0].mm_name);
				Print_String(&print_menu, line1, 0,font10,0);


//				Print_String((Main_menu[print_line1].mm_name), line2, 5,font10,0);
				print_menu = *(Main_menu[print_line1].mm_name);
				Print_String(&print_menu, line2,5 ,font10,0);
				
//				Print_String((Main_menu[print_line1+1].mm_name), line3, 5,font10,0);
				print_menu = *(Main_menu[print_line1+1].mm_name);
				Print_String(&print_menu, line3,5 ,font10,0);
				
//				Print_String((Main_menu[print_line1+2].mm_name), line4, 5,font10,0);
				print_menu = *(Main_menu[print_line1+2].mm_name);
				Print_String(&print_menu, line4,5 ,font10,0);

//				Print_String((Main_menu[print_line1+3].mm_name), line5, 5,font10,0);
				print_menu = *(Main_menu[print_line1+3].mm_name);
				Print_String(&print_menu, line5,5 ,font10,0);

				Print_String(notice, line6, 0,font10,0);



			disp_cursor = display_cursor - print_line1+1;
			Cursor_Location(disp_cursor);
			return ;	
			}
			
void Sub_Menu_Routine(char Sub_Menu_no)
{
	char sub_display_cursor=1;
	char par_val=0;
  uint32_t    key_ip;
//char *aa = "Config";

 /*
	pp= (t_menu[0][0][0]);
 Print_String((&pp),line6,10,font10,0);

	pp= (t_menu[1][2][2]);
 Print_String((&pp),line1,10,font10,0);
 
	pp= (t_menu[1][2][3]);
 Print_String((&pp),line2,10,font10,0);
*/



	sub_menu_status = 0;



	//*(sub_menu1[0].sm_name) = "config";
	//sub_menu1[0].par_options = 0;
	/*
		sub_jumpxx: 
key_ip = key_check_routine();
	if(sub_menu_status !=0)
			goto sub_jump1;
	sub_menu_status =1;
	Clear_LCD();
	
	sub_jump1:
			Print_String((Main_menu[Sub_Menu_no].mm_name), line1, 0,font16,0);
	if((config_menu[sub_display_cursor].par_type) =='O')	
	{
			Print_String((config_menu[sub_display_cursor].sm_name), line2, 5,font16,0);
			par_val= config_menu[sub_display_cursor].variable_offset;
//	if(config_menu[sub_display_cursor].par_options >= par_val)
//			Print_String((options1[sub_display_cursor].sm_options[par_val-1]),line3,10,font16,0);
		
//			Print_String((sub_menu_options123[display_cursor-1][sub_display_cursor][par_val-1]),line3,10,font16,0);

	}
*/
		
		
	
//	if
	//if(Sub_Menu_no == 2)
//	Print_main_menu(Menu_Config);
	
}
			
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Print Main Menu
/*	Algorithm 
JUMPXX: goto JUMPXX
				Call Key_check Routine
				if Menu_Status !=0, goto JUMP1	
				Menu_status=1									//1st time entry in Menu routine
				direction = 0									//direction variable for cursor direction UP/DOWN	
				Display_Cursor=1							//Address of 1st string
				Save Display_Cursor as Display_Line1	//Save address of 1st String
				Save Display_Line1+3 as Display_Line4	//Save address of 4th String
				call Display_Main_Menu_Routine
jump1:	If Key_Input != Ver, goto JUMP2		
				if Display_Cursor < Display_LIne4
				if direction ==0, Increment Display_Cursor, goto JUMP3
				if Display_Cursor == Display_LIne4, 
				if(Main_menu[Display_Cursor+1].mm_type == 'F'),direction = 1 ,go to JUMP6		//when last line, make direction=1 for UP cursor
				Inc Display_Line1						//Next line exists!
				Inc Display_Line4
				Inc Display_Cursor

jump6:	If Display_Cursor > Display_Line1, Dec Display_Cursor, goto JUMP3			//Next line does not exist!
				If Display_Cursor=Display_Line1,
				if Display_Line1 ==1			//Display already on 1st line,
				if direction ==1, Inc Display_Cursor, goto JUMP3	//when 1st line, make direction=0 for DOWN cursor
				Dec Display_Line1
				Dec Display_Line4
				Dec Display_Cursor
				goto JUMP3

jump2:  If Key_Input != Hor,  goto JUMPXX
				menu_status=2
				Sub_Menu_no=Display_cursor
				Call Sub_Menu_Routine
				goto JUMPXX

jump3:	Call Display_Main_Menu_Routine
				goto JUMPXX

*/
//Note: Font size is fixed. i.e. font10
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Print_main_menu()
{
//extern  main_menu_struct Main_menu[Main_Menu_Max];
  uint32_t    key_ip;
//char *aa = "Config";
	menu_status = 0;

	//*(sub_menu1[0].sm_name) = "config";
	//sub_menu1[0].par_options = 0;
	
	

		jumpxx: 
key_ip = key_check_routine();
	if(menu_status !=0)
			goto jump1;
		
			menu_status =1;
			direction = 0;		//cursor direction DOWN
			display_cursor = 1;
		if(Main_menu[display_cursor].mm_type != 'F')
		{
			display_line1 = display_cursor;
			display_line4 = display_cursor  +3;

			display_main_menu_routine(display_line1,display_cursor);
		}
		jump1:		//check if key pressed is Vertical or Horizontal
//		super_delay(50);
		if (key_ip != 11) 
			goto jump2;	//if NOT VERTICAL & HORIZONTAL 
	
		if(key_ip == 11)
		{
			if(display_cursor < display_line4)
				if(direction ==0)	//if DOWN cursor Direction
				{
				display_cursor++;
				goto jump3;
				}
			if(display_cursor == display_line4)
			{
				if(Main_menu[display_cursor+1].mm_type == 'F')
					direction = 1;	//if last line then change to UP cursor Direction
			}
				if(direction ==1)	//if UP cursor direction then JUMP6
					goto jump6;

				display_line1++;
			display_line4++;
			display_cursor++;
			
			goto jump3;
			
			jump3: display_main_menu_routine(display_line1,display_cursor);
			goto jumpxx;
			
			jump6: 
				if(display_cursor > display_line1)
				{
					display_cursor--;
					goto jump3;
				}

				if(display_cursor == display_line1)
					if(display_line1 == 1)		//if UP cursor Direction 
					{
						direction = 0;
						display_cursor++;
						goto jump3;
					}
				display_cursor--;
				display_line1--;
				display_line4--;
				goto jump3;
			}
			jump2: if(key_ip != 10)
				goto jumpxx;
			Sub_Menu_no=display_cursor;
			Print_Variable(Sub_Menu_no,line1, 60,font10,0);
			Sub_Menu_Routine(Sub_Menu_no);
			goto jumpxx;
}


	//main_menu_definition();					//call routine where list of menu is defined

