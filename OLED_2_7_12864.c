//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
#include "OLED_2_7_12864.h"
#include "string.h"
#include <stdlib.h>
#include "lpc17xx_gpio.h"
#include "main_routine.h"
#include "stdio.h"
#include "GPIO_LPC17xx.h"
#include "keys.h"
#include "parameters.h" //#include "menu.h"
#include "timer.h"

unsigned char cc;
unsigned int dd;
extern volatile char flag;
extern char lcd_flag;
extern int lcd_count_disp;
extern char* P_String;
extern char A_String[35];
char OLED_RAM[64][128];
extern uint16_t New_menu_parameters[105];
unsigned char START_ROW,END_ROW,START_COLUMN,END_COLUMN,COLUMN_POINTER;
char *clear_string ="                                ";
//extern int test_count2;
unsigned char counter=0;
extern unsigned char sub_display_cursor;
extern  char *unit_string_ptr,unit_string[2][5],unit_string_t1[2][5];
extern char unit_string1[1][5],unit_string2[1][5];
extern long setval;
unsigned int Brightness=0x40;
unsigned int *char_pointer=0;
unsigned char new_temp,i,j;
extern unsigned int calibri_10ptDescriptors_temp[97][3],calibri_10ptDescriptors[97][3],arial_16ptDescriptors[97][3],arial_32ptDescriptors[97][3];
extern unsigned char calibri_10ptBitmaps[],arial_32ptBitmaps[],arial_16ptBitmaps[];
extern uint32_t Key_Input;
char *chara="^";
extern char temp12;
char *unitstri[2][5]={{"kg/h\0"},{"t/h\0"}};
char Oled_flag,Rows_clear;

extern char Oled_flag_Temp ;
extern char IO_Online_flag,Check_Start ;

void Init_OLED_Pins()
{
//    GPIO_SetDir1(0,0x400078F0F,1);            //SET DIRECTION OF LCD PINS
//    GPIO_SetDir(0    ,0xFFFFFFFF,1);            //SET DIRECTION OF LCD PINS
    GPIO_SetDir(2,0,GPIO_DIR_OUTPUT);
    GPIO_SetDir(2,1,GPIO_DIR_OUTPUT);
    GPIO_SetDir(2,2,GPIO_DIR_OUTPUT);
    GPIO_SetDir(2,3,GPIO_DIR_OUTPUT);
    GPIO_SetDir(2,4,GPIO_DIR_OUTPUT);
    GPIO_SetDir(2,5,GPIO_DIR_OUTPUT);
    GPIO_SetDir(2,6,GPIO_DIR_OUTPUT);
    GPIO_SetDir(2,7,GPIO_DIR_OUTPUT);

    GPIO_SetDir(0,6,GPIO_DIR_OUTPUT);        //E
    GPIO_SetDir(0,7,GPIO_DIR_OUTPUT);        //R/W
    GPIO_SetDir(0,8,GPIO_DIR_OUTPUT);        //D/I
    GPIO_SetDir(0,9,GPIO_DIR_OUTPUT);        //CS_LCD
    GPIO_SetDir(0,25,GPIO_DIR_OUTPUT);    //RST

}

void cs(int on)        //CS_LCD
{
  GPIO_PinWrite (0,9, on);
}

void rs(int on)        //    D/I
{
  GPIO_PinWrite (0,8, on);

    }

    void rw( int on)
{
      GPIO_PinWrite (0,7, on);

    }
    void E1(int on)
{
      GPIO_PinWrite (0,6, on);

}

    void rst(int on)
{
  GPIO_PinWrite (0,25, on);

    }

void databus( long data)
    {
//long temp2 = 0 , temp3 = 0;

        GPIO_ClearValue(2,0xFF);        //clear previous data

//        temp2 = data & 0xFF;

        GPIO_SetValue(2,data);

    }

void Write_OLED_Ram()
{
oled_Command(0x5C);
}

void oled_Command(unsigned char Data)
{
//    latch_control(1);

databus(Data);    //GPIO_Write(GPIOB, Data);//8080 mode

cs(0);                    //GPIO_ResetBits(GPIOC, CS1);
rs(0);                    //    GPIO_ResetBits(GPIOC, RS);
E1(1);                    //GPIO_SetBits(GPIOC, E1);RD
rw(0);                //GPIO_ResetBits(GPIOC, RW);
//OLED_uDelay(1);
rw(1);                //GPIO_SetBits(GPIOC, RW);
//OLED_uDelay(5);
E1(1);                //GPIO_ResetBits(GPIOC, E1);
cs(1);                //GPIO_SetBits(GPIOC, CS1);
rs(1);                //GPIO_SetBits(GPIOC, RS);
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void oled_Data(unsigned char Data)
{//latch_control(1);
//setval = setval & 0xFFF870FF;        //reset data bits

databus(Data);    //GPIO_Write(GPIOB, Data);//8080 mode

cs(0);                    //GPIO_ResetBits(GPIOC, CS1);
rs(1);                    //    GPIO_ResetBits(GPIOC, RS);
E1(1);                    //GPIO_SetBits(GPIOC, E1);
rw(0);                //GPIO_ResetBits(GPIOC, RW);
//OLED_uDelay(1);
rw(1);                //GPIO_SetBits(GPIOC, RW);
//OLED_uDelay(5);
E1(1);                //GPIO_ResetBits(GPIOC, E1);
cs(1);                //GPIO_SetBits(GPIOC, CS1);
rs(0);                //GPIO_SetBits(GPIOC, RS);
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Instruction Setting
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Set_Column_Address_12864(unsigned char a, unsigned char b)
{
    oled_Command(0x15);            // Set Column Address
    oled_Data(a+28);            //   Default => 0x00
    oled_Data(b+28);            //   Default => 0x3F (Total Columns Devided by 2)
}

void Set_Row_Address_12864(unsigned char a, unsigned char b)
{
    oled_Command(0x75);            // Set Row Address
    oled_Data(a);            //   Default => 0x00
    oled_Data(b);            //   Default => 0x4F
}

void Set_Contrast_Current_12864(unsigned char d)
{
    oled_Command(0x81);            // Set Contrast Value
    oled_Command(0x9F);            //   Default => 0x40
}

void Set_Current_Range_12864(unsigned char d)
{
    oled_Command(0x84|d);            // Set Current Range
                        //   Default => 0x84
                        //     0x84 (0x00) => Quarter Current Range
                        //     0x85 (0x01) => Half Current Range
                        //     0x86 (0x02) => Full Current Range
}

void Set_Remap_Format_12864(unsigned char d)
{
    oled_Command(0xA0);            // Set Re-Map & Data Format
    oled_Data(0x16);            //   Default => 0x00
    oled_Data(0x11);
                        //     Column Address 0 Mapped to SEG0
                        //     Disable Nibble Re-Map
                        //     Horizontal Address Increment
                        //     Scan from COM0 to COM[N-1]
                        //     Disable COM Split Odd Even
}

void Set_Start_Line_12864(unsigned char d)
{
    oled_Command(0xA1);            // Set Display Start Line
    oled_Command(d);            //   Default => 0x00
}

void Set_Display_Offset_12864(unsigned char d)
{
    oled_Command(0xA2);            // Set Display Offset
    oled_Command(d);            //   Default => 0x00
}

void Set_Display_Mode_12864(unsigned char d)
{
    oled_Command(0xA6|d);            // Set Display Mode
                        //   Default => 0xA4
                        //     0xA4 (0x00) => Normal Display
                        //     0xA5 (0x01) => Entire Display On, All Pixels Turn On at GS Level 15
                        //     0xA6 (0x02) => Entire Display Off, All Pixels Turn Off
                        //     0xA7 (0x03) => Inverse Display
}

void Set_Multiplex_Ratio_12864(unsigned char d)
{
    oled_Command(0xCA);            // A8 Set Multiplex Ratio
    oled_Command(d);            //   Default => 0x5F
}

void Set_Master_Config_12864(unsigned char d)
{
    oled_Command(0xAD);            // Set Master Configuration
    oled_Command(0x02|d);            //   Default => 0x03
                        //     0x02 (0x00) => Select External VCC Supply
                        //     0x03 (0x01) => Select Internal DC/DC Voltage Converter
}

void Set_Display_On_Off_12864(unsigned char d)
{
    oled_Command(0xAE|d);            // Set Display On/Off
                        //   Default => 0xAE
                        //     0xAE (0x00) => Display Off
                        //     0xAF (0x01) => Display On
}

void Set_Phase_Length_12864(unsigned char d)
{
    oled_Command(0xB1);            // Phase 1 & 2 Period Adjustment
    oled_Command(d);            //   Default => 0x53 (5 Display Clocks [Phase 2] / 3 Display Clocks [Phase 1])
                        //     D[3:0] => Phase 1 Period in 1~15 Display Clocks
                        //     D[7:4] => Phase 2 Period in 1~15 Display Clocks
}

void Set_Frame_Frequency_12864(unsigned char d)
{
    oled_Command(0xB2);            // Set Frame Frequency (Row Period)
    oled_Command(d);            //   Default => 0x25 (37 Display Clocks)
}

void Set_Display_Clock_12864(unsigned char d)
{
    oled_Command(0xB3);            // Display Clock Divider/Osciallator Frequency
    oled_Command(d);            //   Default => 0x41
                        //     D[3:0] => Display Clock Divider
                        //     D[7:4] => Oscillator Frequency
}

void Set_Precharge_Compensation_12864(unsigned char a, unsigned char b)
{
    oled_Command(0xB4);            // Set Pre-Charge Compensation Level
    oled_Command(b);            //   Default => 0x00 (No Compensation)

    if(a == 0x20)
    {
        oled_Command(0xB0);        // Set Pre-Charge Compensation Enable
        oled_Command(0x08|a);        //   Default => 0x08
                        //     0x08 (0x00) => Disable Pre-Charge Compensation
                        //     0x28 (0x20) => Enable Pre-Charge Compensation
    }
}

void Set_Precharge_Voltage_12864(unsigned char d)
{
    oled_Command(0xBB);            // Set Pre-Charge Voltage Level
    oled_Command(0x1F);            //   Default => 0x10 (Connect to VCOMH)
}

void Set_VCOMH_12864(unsigned char d)
{
    oled_Command(0xBE);            // Set Output Level High Voltage for COM Signal
    oled_Command(d);            //   Default => 0x1D (0.81*VREF)
}

void Set_VSL_12864(unsigned char d)
{
    oled_Command(0xB4);            // Set Segment Low Voltage Level
    oled_Command(0xA0);            //   Default => 0x0E
    oled_Command(0xFD);            //   Default => 0x0E
                        //     0x02 (0x00) => Keep VSL Pin Floating
                        //     0x0E (0x0C) => Connect a Capacitor between VSL Pin & VSS
}

void GA_Option_12864(unsigned char d)
{
    oled_Command(0x23);            // Graphic Acceleration Command Options
    oled_Command(d);            //   Default => 0x01
                        //     Enable Fill Rectangle
                        //     Disable Wrap around in Horizontal Direction During Copying & Scrolling
                        //     Disable Reverse Copy
}

void Draw_Rectangle_12864(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e)
{
    oled_Command(0x24);            // Draw Rectangle
    oled_Command(a);            //   Column Address of Start
    oled_Command(c);            //   Row Address of Start
    oled_Command(b);            //   Column Address of End (Total Columns Devided by 2)
    oled_Command(d);            //   Row Address of End
    oled_Command(e);            //   Gray Scale Level
    OLED_uDelay(200);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Full Screen)
//
//    a: Two Pixels Data
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_RAM_12864(unsigned char a)
{
    GA_Option_12864(0x01);
    Draw_Rectangle_12864(0x00,0x3F,0x00,0x5F,a);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Partial or Full Screen)
//
//    a: Column Address of Start
//    b: Column Address of End (Total Columns Devided by 2)
//    c: Row Address of Start
//    d: Row Address of End
//    e: Two Pixels Data
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_Block_12864(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e)
{
    GA_Option_12864(0x01);
    Draw_Rectangle_12864(a,b,c,d,e);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Clear Screen (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Clear_LCD()
{
unsigned char c,d;

    for(c=0;c<64;c++)
        for(d=0;d<128;d++)
            OLED_RAM[c][d]=0;
             PRINT_LCD();
}

//void PRINT_LCD_Temp(void)
//{
//unsigned char c,d;
//    Set_Column_Address_12864(0x00,0x3F);
//    Set_Row_Address_12864(0x00,0x5F);
//    Set_Remap_Format_12864(0x50);
//  lcd_flag=2;

//    for(c=0;c<2;c++)
//    {
//        for(d=0;d<64;d++)
//         {
//            oled_Data(OLED_RAM[c][d]);
//       }
//    }
//    }

void Clear_LCD_Temp()
{
unsigned char c,d;
    if(Oled_flag == 1)
        {
            Set_Column_Address_12864(0x00,0x3F);
            Set_Row_Address_12864(0x000,0x3F);
            Set_Remap_Format_12864(0x50);
            Write_OLED_Ram();
            Oled_flag = 2;
        }
    if(Oled_flag == 2)
        {
            for(c=Rows_clear;c<(Rows_clear+1);c++)
                for(d=0;d<128;d++)
                    {
                        OLED_RAM[c][d]= 0x00;
                        oled_Data(OLED_RAM[c][d]);
                    }
                        Oled_flag = 2;
                        Rows_clear++;
                    if( ( Rows_clear ) == 60 )
                        {
                            Oled_flag = 3;
                            Rows_clear = 0;
                        }

            return;
        }
}

//void PRINT_LCD_Single_Line(void)
//{
//unsigned char c,d;
//    Set_Column_Address_12864(0x00,0x3F);
//    Set_Row_Address_12864(20,30);
//    //Set_Remap_Format_12864(0x50);
//    //lcd_flag=2;

//    for(c=20;c<30;c++)
//        {
//            for(d=0;d<64;d++)
//            {
//                oled_Data(OLED_RAM[c][d]);
//            }
//    }
//}

//void Clear_single_Line()
//{
//unsigned char c,d;

//    for(c=20;c<30;c++)
//        for(d=0;d<64;d++)
//            OLED_RAM[c][d]=0;
//             PRINT_LCD_Single_Line();
//}

void Clear_single_Line(char Start_Row_no,char End_Row_no)
{
unsigned char c,d;
    for(c = Start_Row_no/*20*/;c < End_Row_no/*30*/; c++)
        for(d=0;d<128;d++)
            OLED_RAM[c][d]=0;
}

//void Clear_single_Line()
//{
//unsigned char c,d;
//    if(Oled_flag_Temp== 1)
//        {
//            Set_Column_Address_12864(0,63);
//            Set_Row_Address_12864(20,30);
//            Set_Remap_Format_12864(0x50);
//            Oled_flag_Temp= 2;
//        }
//    if(Oled_flag_Temp == 2)
//        {
//            for(c=Rows_clear;c<(Rows_clear+1);c++)
//                for(d=0;d<64;d++)
//                    {
//                        OLED_RAM[c][d]= 0x00;
//                        oled_Data(OLED_RAM[c][d]);
//                    }
//                        Oled_flag_Temp = 2;
//                        Rows_clear++;
//                    if( ( Rows_clear ) == 6 )
//                        {
//                            Oled_flag_Temp = 3;
//                            Rows_clear = 0;
//                        }
//
//            return;
//        }
//}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Clear Screen (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Clear_LCD_RAM()
{
unsigned char c,d;
    for(c=0;c<64;c++)
        for(d=0;d<128;d++)
            OLED_RAM[c][d]=0;
//PRINT_LCD();
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Clear Screen (Online Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Clear_LCD_RAM_Half_Online()
{
unsigned char c,d;
    for(c=0;c<30;c++)
        for(d=0;d<128;d++)
            OLED_RAM[c][d]=0;
//PRINT_LCD();
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Clear Screen (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Clear_LCD_RAM_Half()
{
unsigned char c,d;
    for(c=30;c<60;c++)
        for(d=0;d<128;d++)
            OLED_RAM[c][d]=0;
//PRINT_LCD();
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Gray Scale Bar (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Grayscale_12864()
{
    //   Level 16 => Column 1~8
        Fill_Block_12864(0x00,0x03,0x00,0x3F,0xFF);

    //   Level 15 => Column 9~16
        Fill_Block_12864(0x04,0x07,0x00,0x3F,0xEE);

    //   Level 14 => Column 17~24
        Fill_Block_12864(0x08,0x0B,0x00,0x3F,0xDD);

    //   Level 13 => Column 25~32
        Fill_Block_12864(0x0C,0x0F,0x00,0x3F,0xCC);

    //   Level 12 => Column 33~40
        Fill_Block_12864(0x10,0x13,0x00,0x3F,0xBB);

    //   Level 11 => Column 41~48
        Fill_Block_12864(0x14,0x17,0x00,0x3F,0xAA);

    //   Level 10 => Column 49~56
        Fill_Block_12864(0x18,0x1B,0x00,0x3F,0x99);

    //   Level 9 => Column 57~64
        Fill_Block_12864(0x1C,0x1F,0x00,0x3F,0x88);

    //   Level 8 => Column 65~72
        Fill_Block_12864(0x20,0x23,0x00,0x3F,0x77);

    //   Level 7 => Column 73~80
        Fill_Block_12864(0x24,0x27,0x00,0x3F,0x66);

    //   Level 6 => Column 81~88
        Fill_Block_12864(0x28,0x2B,0x00,0x3F,0x55);

    //   Level 5 => Column 89~96
        Fill_Block_12864(0x2C,0x2F,0x00,0x3F,0x44);

    //   Level 4 => Column 97~104
        Fill_Block_12864(0x30,0x33,0x00,0x3F,0x33);

    //   Level 3 => Column 105~112
        Fill_Block_12864(0x34,0x37,0x00,0x3F,0x22);

    //   Level 2 => Column 113~120
        Fill_Block_12864(0x38,0x3B,0x00,0x3F,0x11);

    //   Level 1 => Column 121~128
        Fill_Block_12864(0x3C,0x3F,0x00,0x3F,0x00);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Vertical Scrolling (Full Screen)
//
//    a: Scrolling Direction
//       "0x00" (Upward)
//       "0x01" (Downward)
//    b: Set Numbers of Row Scroll per Step
//    c: Set Time Interval between Each Scroll Step
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Set_Gray_Scale_Table_12864()
{
    oled_Command(0xB8);            // Set Gray Scale Table
    oled_Command(0x01);            //   Gray Scale Level 1
    oled_Command(0x11);            //   Gray Scale Level 3 & 2
    oled_Command(0x22);            //   Gray Scale Level 5 & 4
    oled_Command(0x32);            //   Gray Scale Level 7 & 6
    oled_Command(0x43);            //   Gray Scale Level 9 & 8
    oled_Command(0x54);            //   Gray Scale Level 11 & 10
    oled_Command(0x65);            //   Gray Scale Level 13 & 12
    oled_Command(0x76);            //   Gray Scale Level 15 & 14
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Initialization
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
/*void OLED_12864_Init()
{
unsigned char i;
//unsigned char Max_Column = 0x64;            // 128/2-1 (Total Columns Devided by 2)
//unsigned char Max_Row = 0x64;                // 64-1

    Init_OLED_Pins();

rst(0);                //    GPIO_ResetBits(GPIOC, RES);
    for(i=0;i<200;i++)
    {
        OLED_uDelay(200);
    }
rst(1);                //    GPIO_SetBits(GPIOC, RES);

    for(i=0;i<200;i++)
    {
        OLED_uDelay(200);
    }

    Set_Display_On_Off_12864(0x00);        // Display Off (0x00/0x01)

    Set_Display_Clock_12864(0x91);        // Set Clock as 135 Frames/Sec
    Set_Multiplex_Ratio_12864(0x3F);        // 1/64 Duty (0x0F~0x5F)
    Set_Display_Offset_12864(0x00);        // Shift Mapping RAM Counter (0x00~0x5F)
    oled_Command(0xAB);            // Set Gray Scale Table
    oled_Command(0x01);            // Set Gray Scale Table

    //Set_Start_Line_12864(0x00);            // Set Mapping RAM Display Start Line (0x00~0x5F)
    //Set_Master_Config_12864(0x00);        // Disable Embedded DC/DC Converter (0x00/0x01)
    Set_Remap_Format_12864(0x16);            // Set Column Address 0 Mapped to SEG0
                        //     Disable Nibble Remap
                        //     Horizontal Address Increment
                        //     Scan from COM[N-1] to COM0
                        //     Enable COM Split Odd Even
    Set_Current_Range_12864(0x02);        // Set Full Current Range
    Set_Gray_Scale_Table_12864();            // Set Pulse Width for Gray Scale Table

    Set_Contrast_Current_12864(Brightness);    // Set Scale Factor of Segment Output Current Control

    Set_Frame_Frequency_12864(0x51);        // Set Frame Frequency
    Set_Phase_Length_12864(0xF2);            // Set Phase 1 as 5 Clocks & Phase 2 as 5 Clocks
    Set_Precharge_Voltage_12864(0x10);        // Set Pre-Charge Voltage Level
    //Set_Precharge_Compensation_12864(0x20,0x02);    // Set Pre-Charge Compensation

    Set_VSL_12864(0x0D);                // Set Low Voltage Level of SEG Pin
    Set_VCOMH_12864(0x04);            // Set High Voltage Level of COM Pin
    Set_Display_Mode_12864(0x00);            // Normal Display Mode (0x00/0x01/0x02/0x03)

    Fill_RAM_12864(0x00);                // Clear Screen

    Set_Display_On_Off_12864(0x01);        // Display On (0x00/0x01)

}

*/

/* msk29-may-26: OLED register init only (no GPIO_SetDir); shared by boot and menu re-init */
void OLED_12864_ApplyInitCommands(void)
{
    oled_Command(0xAE); /* display OFF */
    oled_Command(0xB3);
    oled_Data(0x91);
    oled_Command(0xCA);
    oled_Data(0x3F);
    oled_Command(0xA2);
    oled_Data(0x00);
    oled_Command(0xAB);
    oled_Data(0x01);
    oled_Command(0xA0);
    oled_Data(0x16);
    oled_Data(0x11);
    oled_Command(0xC7);
    oled_Data(0x0F);
    oled_Command(0xC1);
    oled_Data(0x9F);
    oled_Data(0xB1);
    oled_Data(0xF2);
    oled_Command(0xBB);
    oled_Data(0x1F);
    oled_Command(0xB4);
    oled_Data(0xA0);
    oled_Data(0xFD);
    oled_Command(0xBE);
    oled_Data(0x04);
    oled_Command(0xA6);
    oled_Command(0xAF); /* display ON */
}

/* msk29-may-26: hardware RES pulse then register init (vibration / loose connector recovery) */
void OLED_12864_ResetAndInit(void)
{
    rst(0);
    for (i = 0; i < 200; i++)
        OLED_uDelay(200);
    rst(1);
    for (i = 0; i < 200; i++)
        OLED_uDelay(200);
    OLED_12864_ApplyInitCommands();
}

/* msk29-may-26: SET key -> menu; brief blank then menu draw */
void Lcd_ReinitForMenu(void)
{
    Clear_LCD_RAM();
    OLED_12864_ResetAndInit();
    Clear_LCD();
    Oled_flag = 0;
    lcd_flag = 0;
}

void OLED_12864_Init(void)
{
    Init_OLED_Pins();
    OLED_12864_ResetAndInit(); /* msk29-may-26: boot uses pins + reset/commands */
}

#if 0 /* msk29-may-26: legacy duplicate init removed */
void OLED_12864_Init_legacy_unused(void){
//digitalWrite(RES, LOW); //pull /RES (pin #16) low
//delayUS(200); //keep /RES low for minimum 200�s
//digitalWrite(RES, HIGH); //pull /RES high
//delayUS(200); //wait minimum 200�s before sending commands
    Init_OLED_Pins();

rst(0);                //    GPIO_ResetBits(GPIOC, RES);
    for(i=0;i<200;i++)
    {
        OLED_uDelay(200);
    }
rst(1);                //    GPIO_SetBits(GPIOC, RES);

    for(i=0;i<200;i++)
    {
        OLED_uDelay(200);
    }

oled_Command(0xAE); //display OFF
oled_Command(0xB3); //set CLK div. & OSC freq.
oled_Data(0x91);
oled_Command(0xCA); //set MUX ratio
oled_Data(0x3F);
oled_Command(0xA2); //set offset
oled_Data(0x00);
oled_Command(0xAB); //function selection
oled_Data(0x01);
oled_Command(0xA0); //set re-map
oled_Data(0x16);
oled_Data(0x11);
oled_Command(0xC7); //master contrast current
oled_Data(0x0F);
oled_Command(0xC1); //set contrast current
oled_Data(0x9F);
oled_Data(0xB1); //set phase length
oled_Data(0xF2);
oled_Command(0xBB); //set pre-charge voltage
oled_Data(0x1F);
oled_Command(0xB4); //set VSL
oled_Data(0xA0);
oled_Data(0xFD);
oled_Command(0xBE); //set VCOMH
oled_Data(0x04);
oled_Command(0xA6); //set display mode
oled_Command(0xAF); //display ON
}
#endif

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Print the Give Byte of Given Width
//
//Step 1: Load the Byte in a temperory register
//Step 2: if Bit7 is 1 save MSB as 0x0F else save MSB as 0x00
//Step 3: if Bit6 is 1 save LSB as 0xF0 else save MSB as 0x00
//Step 4: ADD MSB & LSB
//Step 5: Print the Result of addition
//Step 6: Repeat the above till the given width
//        -END-
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Print_Byte_Width(char data, char width)
{
    char MSB,LSB,temp;
    MSB=0;
LSB=0;
//temp_pointer = 0x40;
//temp = temp_pointer;
temp = data;
//test_count2++;
    if(width >=1)
        {
        if(temp&0x80)
            MSB=0xFF;
        else
            MSB=0x00;

        if(temp&0x40)
            LSB=0xFF;
        else
            LSB=0x00;

        //MSB = MSB + LSB;
        oled_Data(MSB);
        oled_Data(LSB);
        }

    if(width >=3)
        {
        if(temp&0x20)
            MSB=0xFF;
        else
            MSB=0x00;

        if(temp&0x10)
            LSB=0xFF;
        else
            LSB=0x00;

        //MSB = MSB + LSB;
        oled_Data(MSB);
        oled_Data(LSB);
    }

    if(width >=5)
        {
        if(temp&0x08)
            MSB=0xFF;
        else
            MSB=0x00;

        if(temp&0x04)
            LSB=0xFF;
        else
            LSB=0x00;

        //MSB = MSB + LSB;
        oled_Data(MSB);
        oled_Data(LSB);
    }

    if (width >= 7)
        {
        if(temp&0x02)
            MSB=0xFF;
        else
            MSB=0x00;

        if(temp&0x01)
            LSB=0xFF;
        else
            LSB=0x00;

        //MSB = MSB + LSB;
        oled_Data(MSB);
        oled_Data(LSB);
    }

}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Print the Give Byte of Given Width
//
//Step 1: Load the Byte in a temperory register
//Step 2: if Bit7 is 1 save MSB as 0x0F else save MSB as 0x00
//Step 3: if Bit6 is 1 save LSB as 0xF0 else save MSB as 0x00
//Step 4: ADD MSB & LSB
//Step 5: Print the Result of addition
//Step 6: Repeat the above till the given width
//        -END-
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Print_Byte_Width2(char data, char width)
{
    char MSB,LSB,temp;
    MSB=0;
LSB=0;
//temp_pointer = 0x40;
//temp = temp_pointer;
temp = data;
//test_count2++;
    if(width >=1)
        {
        if(temp&0x80)
            MSB=0xFF;
        else
            MSB=0x00;

        if(temp&0x40)
            LSB=0xFF;
        else
            LSB=0x00;

        //MSB = MSB + LSB;
        oled_ram_Data(MSB,LSB);
        //oled_ram_Data(LSB);
        }

    if(width >=3)
        {
        if(temp&0x20)
            MSB=0xFF;
        else
            MSB=0x00;

        if(temp&0x10)
            LSB=0xFF;
        else
            LSB=0x00;

        //MSB = MSB + LSB;
        oled_ram_Data(MSB,LSB);
        //oled_ram_Data(LSB);
    }

    if(width >=5)
        {
        if(temp&0x08)
            MSB=0xFF;
        else
            MSB=0x00;

        if(temp&0x04)
            LSB=0xFF;
        else
            LSB=0x00;

        //MSB = MSB + LSB;
        oled_ram_Data(MSB,LSB);
        //oled_ram_Data(LSB);
    }

    if (width >= 7)
        {
        if(temp&0x02)
            MSB=0xFF;
        else
            MSB=0x00;

        if(temp&0x01)
            LSB=0xFF;
        else
            LSB=0x00;
//MSB = MSB + LSB;
        oled_ram_Data(MSB,LSB);
        //oled_ram_Data(LSB);
    }

}

void Clear_String(char **string, char start_row, char start_col, char font_size)
{
unsigned char width,height,start_col2;
unsigned int char_address;
unsigned int *value_print;
unsigned char val;
unsigned char i;
unsigned char str_len;
char* P_String1;// = "ABRAR";
//unsigned int Descriptors[97][3];
unsigned int *arial_Descriptors;
unsigned char start_row_temp;

start_col2 = start_col;
P_String1 = *string;
    str_len = strlen(P_String1);

    start_row_temp=start_row;

switch(font_size)
{
    default:
    case 0:arial_Descriptors = &calibri_10ptDescriptors[0][0];
                    if(start_row_temp > 5)
                        start_row_temp = 5;
                    start_row_temp = start_row_temp*10;
                    break;
    case 1:arial_Descriptors = &arial_16ptDescriptors[0][0];
                    if(start_row_temp > 3)
                        start_row_temp = 3;
                    start_row_temp = start_row_temp*16;
                    break;
    case 2:arial_Descriptors = &arial_32ptDescriptors[0][0];
                    if(start_row_temp > 1)
                        start_row_temp = 1;
                    start_row_temp = start_row_temp*31;
                    break;

}
    for(i=0;i<str_len;i++)
//    for(i=0;i<=5;i++)
{
    val = *P_String1;
//    value_print = &calibri_10ptDescriptors[(val-32)][0];
    value_print =arial_Descriptors+(((val-32))*3);
    width = *value_print;

    value_print++;
    height = *value_print;

    value_print++;
    char_address = *value_print;

Set_Column_Address_12864(start_col2,((start_col2+(width+1)/2)-1));
Set_Row_Address_12864(start_row_temp,(start_row_temp+height-1));
Set_Remap_Format_12864(0x50);
Write_OLED_Ram();
    Print_Char(val,font_size,0);

P_String1++;
start_col2=start_col2+(width+1)/2;
}

}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//Print the Give Character of Given Width, Height & Address from the Look-up table calibri_8ptDescriptors
//
//Step 1: Go to Pointer in the Look-up table for the given Character Data
//Step 2: Save the Value at the Pointer as Width(char)
//Step 3: Increment the pointer
//Step 4: Save the Value at the Pointer as Height(char)
//Step 5: Increment the pointer
//Step 6: Save the Value at the Pointer as Address(int)
//Step 7: Call the function Print_Byte_Width for 'height' number of times.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Print_Char2(char char_data, char font_size , char underline)
{
unsigned char width,height,height_1;
unsigned int char_address,*arial_Descriptors;
unsigned char *Bitmaps;

switch(font_size)
{
    default:
    case 0:arial_Descriptors = &calibri_10ptDescriptors[0][0];
                    Bitmaps = &calibri_10ptBitmaps[0];
                    height_1 = 1;
                    break;
    case 1:arial_Descriptors = &arial_16ptDescriptors[0][0];
                    Bitmaps = &arial_16ptBitmaps[0];
                    height_1 = 3;
                    break;
    case 2:arial_Descriptors = &arial_32ptDescriptors[0][0];
                    Bitmaps = &arial_32ptBitmaps[0];
                    height_1=4;
                    break;

}

//    char_pointer = &calibri_10ptDescriptors[(char_data-32)][0];
//    char_pointer = arial_Descriptors+char_data;
    char_pointer = arial_Descriptors+((char_data-32)*3);
    width = *char_pointer;
  if((char_data == 0x2E)&&(IO_Online_flag ==1 ))
        width = 5;

    char_pointer++;
    height = *char_pointer;

    char_pointer++;
    char_address = *char_pointer;

//temp123 = *char_pointer;

//Set_Column_Address_12864(1,2);
    height_1 = height-height_1;
for(i=0;i<height;i++)
{
    if(width > 24)
        j=i*4;
    else if(width > 16)
        j=i*3;
    else if(width > 8)
        j=i*2;
    else
        j=i;
//    char_pointer = arial_Descriptors+((char_data-32)*3);

        if((i==height_1) && (underline ==1))
            new_temp = 0xff;
        else
            new_temp = *(Bitmaps +char_address+j);

    Print_Byte_Width2(new_temp,width);
    if(width >8)
    {
        if((i==height_1) && (underline ==1))
            new_temp = 0xff;
        else
            new_temp = *(Bitmaps +char_address+j+1);
    Print_Byte_Width2(new_temp,(width-8));
    }
    if(width >16)
    {
        if((i==height_1) && (underline ==1))
            new_temp = 0xff;
        else
            new_temp = *(Bitmaps +char_address+j+2);
    Print_Byte_Width2(new_temp,(width-16));
    }
    if(width >24)
    {
//new_temp = calibri_10ptBitmaps[char_address+j+3];
        if((i==height_1) && (underline ==1))
            new_temp = 0xff;
        else
            new_temp = *(Bitmaps +char_address+j+3);
    Print_Byte_Width2(new_temp,(width-24));
    }
//PRINT_LCD();
//super_delay(100);
}
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//Print the Give Character of Given Width, Height & Address from the Look-up table calibri_8ptDescriptors
//
//Step 1: Go to Pointer in the Look-up table for the given Character Data
//Step 2: Save the Value at the Pointer as Width(char)
//Step 3: Increment the pointer
//Step 4: Save the Value at the Pointer as Height(char)
//Step 5: Increment the pointer
//Step 6: Save the Value at the Pointer as Address(int)
//Step 7: Call the function Print_Byte_Width for 'height' number of times.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Print_Char(char char_data, char font_size , char underline)
{
unsigned char width,height,height_1;
unsigned int char_address,*arial_Descriptors;
unsigned char *Bitmaps;

switch(font_size)
{
    default:
    case 0:arial_Descriptors = &calibri_10ptDescriptors[0][0];
                    Bitmaps = &calibri_10ptBitmaps[0];
                    height_1 = 1;
                    break;
    case 1:arial_Descriptors = &arial_16ptDescriptors[0][0];
                    Bitmaps = &arial_16ptBitmaps[0];
                    height_1 = 3;
                    break;
    case 2:arial_Descriptors = &arial_32ptDescriptors[0][0];
                    Bitmaps = &arial_32ptBitmaps[0];
                    height_1=4;
                    break;

}

//    char_pointer = &calibri_10ptDescriptors[(char_data-32)][0];
    char_pointer = arial_Descriptors+((char_data-32)*3);
    width = *char_pointer;

    char_pointer++;
    height = *char_pointer;

    char_pointer++;
    char_address = *char_pointer;

//temp123 = *char_pointer;

//Set_Column_Address_12864(1,2);
    height_1 = height-height_1;
for(i=0;i<height;i++)
{
    if(width > 24)
        j=i*4;
    else if(width > 16)
        j=i*3;
    else if(width > 8)
        j=i*2;
    else
        j=i;
//    char_pointer = arial_Descriptors+((char_data-32)*3);

        if((i==height_1) && (underline ==1))
            new_temp = 0xff;
        else
            new_temp = *(Bitmaps +char_address+j);

    Print_Byte_Width(new_temp,width);
    if(width >8)
    {
        if((i==height_1) && (underline ==1))
            new_temp = 0xff;
        else
            new_temp = *(Bitmaps +char_address+j+1);
            Print_Byte_Width(new_temp,(width-8));
    }
    if(width >16)
    {
        if((i==height_1) && (underline ==1))
            new_temp = 0xff;
        else
            new_temp = *(Bitmaps +char_address+j+2);
    Print_Byte_Width(new_temp,(width-16));
    }
    if(width >24)
    {
//new_temp = calibri_10ptBitmaps[char_address+j+3];
        if((i==height_1) && (underline ==1))
            new_temp = 0xff;
        else
            new_temp = *(Bitmaps +char_address+j+3);
    Print_Byte_Width(new_temp,(width-24));
    }
}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Print an ASCII Character at a given Address
//
//Step 1: Go to Pointer in the Look-up table for the given Character Data
//Step 2: Save the Value at the Pointer as Width(char)
//Step 3: Increment the pointer
//Step 4: Save the Value at the Pointer as Height(char)
//Step 5: Increment the pointer
//Step 6: Save the Value at the Pointer as Address(int)
//Step 7: Set the Column Address starting from the given address till the width of the character
//Step 8: Set the Row Address starting from the given address till the height of the character
//Step 9: Remap format to Auto Increment horizontally.

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Print_ascii_char(char char_data, char start_row, char start_col, char font_size)
{

unsigned char width,height;
unsigned int char_address;

    char_pointer = &calibri_10ptDescriptors[(char_data-32)][0];
    width = *char_pointer;

    char_pointer++;
    height = *char_pointer;

    char_pointer++;
    char_address = *char_pointer;

Set_Column_Address_12864(start_col,((start_col+(width+1)/2)-1));
Set_Row_Address_12864(start_row,(start_row+height-1));

Set_Remap_Format_12864(0x50);
Write_OLED_Ram();
Print_Char(char_data,font_size,1);

}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Print an String from the given Address to the address

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//start_row = line1,line2,line3,line4,line5,line6            for font12 (6-lines)
//start_row = line1,line2,line3,line4                                 for font16 (4-lines)
//start_row = line1,line2,linex                                                            for font24 (2-lines)
void Print_String2(const char **string, char start_row, char start_col, char font_size, char underline_no)
{
unsigned char width,height,start_col2;
unsigned int char_address;
unsigned int *value_print;
unsigned char val1,val;
unsigned char i;
unsigned char str_len;
const char* P_String1;// = "ABRAR";

unsigned int *arial_Descriptors;
unsigned char start_row_temp;

start_col2= start_col*2;
P_String1 = *string;
    str_len = strlen(P_String1);

    start_row_temp=start_row;

switch(font_size)
{
    default:
    case 0:arial_Descriptors = &calibri_10ptDescriptors[0][0];
                    if(start_row_temp > 5)
                        start_row_temp = 5;
                    start_row_temp = start_row_temp*10;
                    //start_row_temp = (start_row_temp<<3)+(start_row_temp<<1);
                    break;
    case 1:arial_Descriptors = &arial_16ptDescriptors[0][0];
                    if(start_row_temp > 3)
                        start_row_temp = 3;
                    start_row_temp = start_row_temp<<4;
                    break;
    case 2:arial_Descriptors = &arial_32ptDescriptors[0][0];
                    if(start_row_temp ==linex)
                        start_row_temp=15;
//                        start_row_temp=18;
                    else{
                    if(start_row_temp > 1)
                        start_row_temp = 1;
                    start_row_temp = (start_row_temp<<5)-start_row_temp;
                }
                    break;

}
    for(i=0;i<str_len;i++)
    {
    val = *P_String1;

    value_print =arial_Descriptors+(((val-32))*3);
//    value_print =arial_Descriptors+val;

    width = *value_print;
  if(((val == 0x2E)&&(IO_Online_flag ==1 ))||((val == 32)&&(Check_Start==20)))
        width = 5;

    value_print++;
    height = *value_print;

    value_print++;
    char_address = *value_print;

    START_COLUMN=start_col2;
    COLUMN_POINTER = START_COLUMN;
    END_COLUMN=(start_col2+(width)-1);//END_COLUMN=(start_col2+((width+1)/2)-1);
    START_ROW = start_row_temp;
    END_ROW = start_row_temp+height;

//Print_Char(0xFF,font_size,1);        //underline character
if(i ==(underline_no-1))
    Print_Char2(val,font_size,1);        //underline character
else
    Print_Char2(val,font_size,0);

P_String1++;
start_col2=start_col2+((width+1));

}

}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Print an String from the given Address to the address

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//start_row = line1,line2,line3,line4,line5,line6            for font12 (6-lines)
//start_row = line1,line2,line3,line4                                 for font16 (4-lines)
//start_row = line1,line2,linex                                                            for font24 (2-lines)
void Print_String(char **string, char start_row, char start_col, char font_size, char underline_no)
{
unsigned char width,height,start_col2;
unsigned int char_address;
unsigned int *value_print;
unsigned char val;
unsigned char i;
unsigned char str_len;
char* P_String1;// = "ABRAR";
unsigned int *arial_Descriptors;
unsigned char start_row_temp;

start_col2= start_col;
P_String1 = *string;
    str_len = strlen(P_String1);

    start_row_temp=start_row;

switch(font_size)
{
    default:
    case 0:arial_Descriptors = &calibri_10ptDescriptors[0][0];
                    if(start_row_temp > 5)
                        start_row_temp = 5;
                    start_row_temp = start_row_temp*10;
                    break;
    case 1:arial_Descriptors = &arial_16ptDescriptors[0][0];
                    if(start_row_temp > 3)
                        start_row_temp = 3;
                    start_row_temp = start_row_temp*16;
                    break;
    case 2:arial_Descriptors = &arial_32ptDescriptors[0][0];
                    if(start_row_temp ==linex)
                        start_row_temp=12;
                    else{
                    if(start_row_temp > 1)
                        start_row_temp = 1;
                    start_row_temp = start_row_temp*31;
                }
                    break;

}
    for(i=0;i<str_len;i++)
{
    val = *P_String1;
//value_print = &calibri_10ptDescriptors[(val-32)][0];
    value_print =arial_Descriptors+(((val-32))*3);
    width = *value_print;

    value_print++;
    height = *value_print;

    value_print++;
    char_address = *value_print;

Set_Column_Address_12864(start_col2,((start_col2+(width+1)/2)-1));
Set_Row_Address_12864(start_row_temp,(start_row_temp+height-1));
Set_Remap_Format_12864(0x50);
Write_OLED_Ram();

if(i ==(underline_no-1))
    Print_Char(val,font_size,1);        //underline character
else
    Print_Char(val,font_size,0);

P_String1++;
start_col2=start_col2+(width+1)/2+1;
}

}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Print a Variable to the address
//Step 1: Convert Long to String
//Step 2: Call Print_String() routine
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Print_Variable(unsigned long variable, unsigned char start_row, unsigned char start_col, unsigned char font_size, unsigned char underline)
{
char new_string[10];
char *new_string_print;

    sprintf(new_string,"%lu",variable);
    new_string_print = new_string;

Print_String(&new_string_print, start_row, start_col,font_size,underline);
}

/*
void Print_Variable_Temp(double variable, unsigned char start_row, unsigned char start_col, unsigned char font_size, unsigned char underline)
{
unsigned char new_string[16];
unsigned char *new_string_print;

    sprintf(new_string,"%e",variable);
    new_string_print = new_string;

Print_String(&new_string_print, start_row, start_col,font_size,underline);
}
*/
 void Print_Variable1(unsigned long variable, unsigned char start_row, unsigned char start_col, unsigned char font_size, unsigned char underline)
{
 char new_string[10];
char *new_string_print;

    sprintf(new_string,"%lu",variable);
    new_string_print = new_string;

Print_String2(&new_string_print, start_row, start_col,font_size,underline);
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Enter  a Variable routine with Coursor Blink or
//Step 1: Convert Long to String
//Step 2: Calculate the String length
//Step 3: Calculate the font Size lookup addess & start_row address
//Step 4: Calculate the height
//Step 5: Calculate the start_row_temp (row where the line is to be printed)
//Step 6: Calculate the width
//Step 7: Print the String
//Step 8: Draw Rect below string in row start_row_temp; col from start_col_temp to start_col_temp2(start_col__temp + width)
//Step 9: Calculate the start col address of the next char
//Step 10:Loop back to 'Step 6' with count as string length
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Enter_Variable(unsigned long variable, unsigned char start_row, unsigned char start_col, unsigned char font_size)
{
char new_string[10];
 char *new_string_print;

unsigned char width,start_col_temp,start_col_temp2;
unsigned int *arial_Descriptors;
unsigned int *char_pointer1;
    unsigned char start_row_temp;
char str_len2=0,ht;
int val;
unsigned char j;

    sprintf(new_string,"%lu",variable);
    new_string_print = new_string;

    str_len2 = strlen(new_string);

    switch(font_size)
{
        default:
    case 0:arial_Descriptors = &calibri_10ptDescriptors[0][0];
                    if(start_row > 5)
                        start_row = 5;
                    start_row_temp = (start_row)*10;
                    ht = 10;
                    break;
    case 1:arial_Descriptors = &arial_16ptDescriptors[0][0];
                    if(start_row > 3)
                        start_row = 3;
                    start_row_temp = (start_row)*16;
                    ht = 16;
                    break;
    case 2:arial_Descriptors = &arial_32ptDescriptors[0][0];
                    if(start_row > 1)
                        start_row = 1;
                    start_row_temp = (start_row)*31;
                    ht = 31;
                    break;
}

//    char_pointer = &calibri_10ptDescriptors[(char_data-32)][0];
start_row_temp = start_row_temp + ht-1;

start_col_temp= start_col;

counter = 0;

/*
start_col2= start_col;
P_String1 = *string;
    str_len = strlen(P_String1);

    start_row_temp=start_row;
*/

for(j=0;j<str_len2;j++)
{

    val = new_string_print[j];
//    value_print = &calibri_10ptDescriptors[(val-32)][0];
    char_pointer1 =arial_Descriptors+(((val-32))*3);

    width = *char_pointer1;

        Print_String(&new_string_print, start_row, start_col,font_size,1);

//OK//start_col_temp2 = start_col_temp + (width+1)/2-1;

    start_col_temp2 = start_col_temp + (width+1)/2-1;

        Draw_Rectangle_12864(start_col_temp, start_col_temp2,(start_row_temp),(start_row_temp), 0xFF);

//OK//        start_col_temp = start_col_temp+ (width+1)/2 + 1;
        start_col_temp = start_col_temp+ (width+1)/2 + 1;

        super_delay(80);
counter ++;
    }
}
//-------------------------------------------------------------------------------
long Enter_Variable2(unsigned long variable, unsigned char start_row, unsigned char start_col, unsigned char font_size,
    unsigned char no_of_digit, unsigned char decimal_pt)
{
    long VARI=0;
char new_string[15]={0};
char print_new_string[15]={0};
char old_string[15]={0};
 char *new_string_print,*old_string_ptr;
char str_len1=0,str_len2=0;
unsigned char j=0,k=0,l=0,j2=0;
unsigned char str_loop=0;
signed char diff=0;
char dec_flag=0;
//variable = 2;

if(decimal_pt == 9)
    decimal_pt = Dec_Point_Capacity;
new_string_print= new_string;
    old_string_ptr=old_string;
    sprintf(old_string,"%lu",variable);
    str_len1 = strlen(old_string);
    diff = no_of_digit - str_len1;
//    Print_Variable(diff,10,30,1,0);

if(diff<0)
{
    variable = 0x0;
    sprintf(old_string,"%lu",variable);
    str_len1 = strlen(old_string);

    diff = no_of_digit - str_len1;
}

    if(diff>0)
    {
for(j=0;j<diff;j++)
    new_string[j]='0';

        if(variable<10)
        {            new_string[j]=variable+48;
        new_string[j+1]='\0';
        }
        else
strcat(new_string,old_string);
    }
if(diff ==0)
    strcpy(new_string,old_string);

    new_string_print = print_new_string;
    str_len2 = strlen(new_string);

strcpy(print_new_string,new_string);

str_loop=str_len2-decimal_pt;

////////
        for(k=str_loop;k<(str_len2)+1;k++)
        print_new_string[k+1] = new_string[k];

    print_new_string[str_loop] = '.';
////////

    for(j2=0;j2<=str_len2;j2++)
{
    j=j2;
if(    print_new_string[j]=='.')
    dec_flag = 1;

if(dec_flag ==1)
    j++;

Clear_String(&clear_string, start_row, 0,font_size);
Print_String(&new_string_print, start_row, start_col,font_size,j+1);
//Print_Variable(str_len2,start_row+1,start_col,font_size,j+1);
Key_Input = NO_KEY;

if(j2<str_len2)
{
    back1:
    if(flag==0)
        goto back1;
    flag=0;
    Key_Input=NO_KEY;
while(Key_Input>9 && Key_Input!=K_DEC && Key_Input!=ESC&& Key_Input!=K_INC)
{    super_delay(39);
    Key_Input=NO_KEY;

//    while(Key_Input==NO_KEY)
        key_check_routine();
//Print_Variable(Key_Input,start_row+1,start_col+10,font_size,j+1);
}

if(Key_Input<=9)
{
    new_string_print[j] = Key_Input+48;// (char)Key_Input;//new_string_print[j] +1;
    new_string[j2] = new_string_print[j];//(char)Key_Input;//new_string[j2] +1;
//Print_Variable(Key_Input,start_row+1,start_col+10,font_size,j+1);

}
else if(Key_Input == K_DEC || Key_Input == ESC|| Key_Input == K_INC)
{
    goto exit_and_return;
}
else
    goto back1;
    //goto exit_and_return ;
//    super_delay(70);
}

}
exit_and_return:
//Print_Variable(sub_display_cursor,line4,start_col+30,font_size,j+1);

VARI = atoi(new_string);
return(VARI);
}
//-----------------------------------------------------------------------

//-------------------------------------------------------------------------------
long Enter_Variable4(unsigned long variable, unsigned char start_row, unsigned char start_col, unsigned char font_size,
    unsigned char no_of_digit, unsigned char decimal_pt)
{
    long VARI=0;
char new_string[15]={0};
char print_new_string[15]={0};
char old_string[15]={0};
 char *new_string_print,*old_string_ptr;
char str_len1=0,str_len2=0;
unsigned char j=0,k=0,l=0,j2=0;
unsigned char str_loop=0;
signed char diff=0;
char dec_flag=0;
//variable = 2;

if(decimal_pt == 9)
    decimal_pt = Dec_Point_Capacity;
new_string_print= new_string;
    old_string_ptr=old_string;
    sprintf(old_string,"%lu",variable);
    str_len1 = strlen(old_string);
    diff = no_of_digit - str_len1;
//    Print_Variable(diff,10,30,1,0);

if(diff<0)
{
    variable = 0x0;
    sprintf(old_string,"%lu",variable);
    str_len1 = strlen(old_string);

    diff = no_of_digit - str_len1;
}

    if(diff>0)
    {
for(j=0;j<diff;j++)
    new_string[j]='0';

        if(variable<10)
        {            new_string[j]=variable+48;
        new_string[j+1]='\0';
        }
        else
strcat(new_string,old_string);
    }
if(diff ==0)
    strcpy(new_string,old_string);

    new_string_print = print_new_string;
    str_len2 = strlen(new_string);

strcpy(print_new_string,new_string);

str_loop=str_len2-decimal_pt;

////////
        for(k=str_loop;k<(str_len2)+1;k++)
        print_new_string[k+1] = new_string[k];

    print_new_string[str_loop] = '.';
////////

    while(1)  // Loop until HOR or CL is pressed
    {
    for(j2=0;j2<=str_len2;j2++)
{
    j=j2;
if(    print_new_string[j]=='.')
    dec_flag = 1;

if(dec_flag ==1)
    j++;

Clear_String(&clear_string, start_row, 0,font_size);
Print_String(&new_string_print, start_row, start_col,font_size,j+1);
//Print_Variable(str_len2,start_row+1,start_col,font_size,j+1);
Key_Input = NO_KEY;

if(j2<str_len2)
{
    back1:
    if(flag==0)
        goto back1;
    flag=0;
    Key_Input=NO_KEY;
//while(Key_Input>9 && Key_Input!=K_DEC && Key_Input!=ESC&& Key_Input!=K_INC)
while(Key_Input>9 && Key_Input!=CL && Key_Input!=HOR)
{    super_delay(39);
    Key_Input=NO_KEY;

//    while(Key_Input==NO_KEY)
        key_check_routine();
//Print_Variable(Key_Input,start_row+1,start_col,font_size,j+1);
}

if(Key_Input<=9)
{
    new_string_print[j] = Key_Input+48;// (char)Key_Input;//new_string_print[j] +1;
    new_string[j2] = new_string_print[j];//(char)Key_Input;//new_string[j2] +1;
//Print_Variable(Key_Input,start_row+1,start_col+10,font_size,j+1);

}
else if(Key_Input == HOR)// || Key_Input == ESC|| Key_Input == K_INC)
{
    goto exit_and_return4;
}
else if(/*Key_Input == ESC||*/ Key_Input == CL)
{
    return(variable);
}
else
    goto back1;
    //goto exit_and_return4 ;
//    super_delay(70);
}

}
        dec_flag = 0;  // Reset for next round
    }
exit_and_return4:
//Print_Variable(sub_display_cursor,line4,start_col+30,font_size,j+1);

VARI = atoi(new_string);
return(VARI);
}
//-----------------------------------------------------------------------

//-------------------------------------------------------------------------
long Enter_Variable3(unsigned long variable, unsigned char start_row, unsigned char start_col, unsigned char font_size,
    unsigned char no_of_digit, unsigned char decimal_pt)
{
    long VARI=0;
char new_string[15]={0};
char print_new_string[15]={0};
char old_string[15]={0};
 char *new_string_print,*old_string_ptr;
char str_len1=0,str_len2=0;
unsigned char j=0,k=0,l=0,j2=0;
unsigned char str_loop=0;
signed char diff=0;
char dec_flag=0;
//variable = 2;

if(decimal_pt == 9)
    decimal_pt = Dec_Point_Capacity;
new_string_print= new_string;
    old_string_ptr=old_string;
    sprintf(old_string,"%lu",variable);
    str_len1 = strlen(old_string);
    diff = no_of_digit - str_len1;
//    Print_Variable(diff,10,30,1,0);

if(diff<0)
{
    variable = 0x0;
    sprintf(old_string,"%lu",variable);
    str_len1 = strlen(old_string);

    diff = no_of_digit - str_len1;
}

    if(diff>0)
    {
for(j=0;j<diff;j++)
    new_string[j]='0';

        if(variable<10)
        {            new_string[j]=variable+48;
        new_string[j+1]='\0';
        }
        else
strcat(new_string,old_string);
    }
if(diff ==0)
    strcpy(new_string,old_string);

    new_string_print = print_new_string;
    str_len2 = strlen(new_string);

strcpy(print_new_string,new_string);

str_loop=str_len2-decimal_pt;

////////
        for(k=str_loop;k<(str_len2)+1;k++)
        print_new_string[k+1] = new_string[k];

    print_new_string[str_loop] = '.';
////////

    for(j2=0;j2<=str_len2;j2++)
{
    j=j2;
if(    print_new_string[j]=='.')
    dec_flag = 1;

if(dec_flag ==1)
    j++;

Clear_String(&clear_string, start_row, 0,font_size);
Print_String(&new_string_print, start_row, start_col,font_size,j+1);
Print_String(unitstri[Units_Capacity],line3,45,font16,0);
  // unit_string_ptr= *(unit_string + parameter_unit);       //Feedrate units
    //Print_String2(&unit_string_ptr, start_row, 45,font_size,0);
//Print_Variable(str_len2,start_row+1,start_col,font_size,j+1);
Key_Input = NO_KEY;
if(j2<str_len2)
{
    back1:
    if(flag==0)
        goto back1;
    flag=0;
    Key_Input=NO_KEY;
while(Key_Input>9 && Key_Input!=K_DEC && Key_Input!=K_INC && Key_Input!=ESC)
{    super_delay(39);
    Key_Input=NO_KEY;

//    while(Key_Input==NO_KEY)
        key_check_routine();
//Print_Variable(Key_Input,start_row+1,start_col+10,font_size,j+1);
}

if(Key_Input<=9)
{
    new_string_print[j] = Key_Input+48;// (char)Key_Input;//new_string_print[j] +1;
    new_string[j2] = new_string_print[j];//(char)Key_Input;//new_string[j2] +1;
//Print_Variable(Key_Input,start_row+1,start_col+10,font_size,j+1);

}
else if(Key_Input == ESC)
{
     temp12=0;
    goto exit_and_return;
}
else if(Key_Input == K_DEC )
{
    temp12=1;
    goto exit_and_return;
}
else if(Key_Input == K_INC )
{
    temp12=0;
    goto exit_and_return;
}
else
    goto back1;
//    super_delay(70);
}

}
exit_and_return:
//Print_Variable(sub_display_cursor,line4,start_col+30,font_size,j+1);

VARI = atoi(new_string);
return(VARI);
}
//------------------------------------------------------------------------------------------------------------
void Print_Variable2(unsigned long variable, unsigned char start_row, unsigned char start_col, unsigned char font_size,
                                            unsigned char underline, unsigned char dec_pos,unsigned char units_pos)
{
 char new_string[10];
 char *new_string_print;
float temp1;
char temp12;
    temp12=sizeof(variable);
    if(font_size==font16)
        temp12=temp12+temp12;

    if(dec_pos == 0)
    {
        temp1 = (float)variable;
    sprintf(new_string,"%.f",temp1);
    }
    else if(dec_pos == 1)
    {
        temp1 = (float)variable / 10;
    sprintf(new_string,"%.1f",temp1);
    }
    else if(dec_pos == 2)
    {
        temp1 = (float)variable / 100;
    sprintf(new_string,"%.2f",temp1);
    }
    else if(dec_pos == 3)
    {
        temp1 = (float)variable / 1000;
    sprintf(new_string,"%.3f",temp1);
    }
    else
    {
        temp1 = (float)variable / 10000;
    sprintf(new_string,"%.4f",temp1);
    }

//    strcat((char*)new_string,(char*)unit_string_ptr);

    new_string_print = new_string;

    unit_string_ptr= *(unit_string + Units_Capacity);
 Print_String2(&new_string_print, start_row, start_col,font_size,underline);

if(units_pos<10)
{
    if(start_row ==linex)
     Print_String2(&unit_string_ptr, start_row, 50,font_size,underline);
  else
     Print_String2(&unit_string_ptr, start_row, 50,font_size,underline);
}

}

/****************************************/

void Print_Variable3(unsigned long variable, unsigned char start_row, unsigned char start_col, unsigned char font_size,
                                            unsigned char underline, unsigned char dec_pos,unsigned char units_pos, char parameter_unit)
{
char new_string[10];
char *new_string_print;
float temp1;
char temp12;
    temp12=sizeof(variable);
    if(font_size==font16)
        temp12=temp12+temp12;

    if(dec_pos == 0)
    {
        temp1 = (float)variable;
    sprintf(new_string,"%.f",temp1);
    }
    else if(dec_pos == 1)
    {
        temp1 = (float)variable / 10;
    sprintf(new_string,"%.1f",temp1);
    }
    else if(dec_pos == 2)
    {
        temp1 = (float)variable / 100;
    sprintf(new_string,"%.2f",temp1);
    }
    else if(dec_pos == 3)
    {
        temp1 = (float)variable / 1000;
    sprintf(new_string,"%.3f",temp1);
    }
    else
    {
        temp1 = (float)variable / 10000;
    sprintf(new_string,"%.4f",temp1);
    }

    switch(parameter_unit)
    {
        case '0' : unit_string_ptr= *(unit_string1 + 0);                                //
                    break;
        case '1' : unit_string_ptr= *(unit_string_t1 + Units_Totaliser1);  //Totaliser1 units
                             break;
        case '2' : unit_string_ptr= *(unit_string_t1+ Units_Totaliser2);     //Totaliser2 units
                             break;
        case '3' : unit_string_ptr= *(unit_string2 + 0);                                //Tacho_Freq
                             break;
        case 'X' : unit_string_ptr= *(unit_string + Units_Capacity);       //Feedrate units
                             break;
        case '4' : unit_string_ptr= *(unit_string2 + 1);                                //Current
                             break;

        default:
                     break;

    };

 new_string_print = new_string;
 Print_String2(&new_string_print, start_row, start_col,font_size,underline);

if(units_pos<10)
{
    if(start_row ==linex)
    Print_String2(&unit_string_ptr, start_row, 50,font_size,underline);
else
    Print_String2(&unit_string_ptr, start_row, 50,font_size,underline);
}

}

/****************************************/

void PRINT_ONLINE(void)
{
    if(lcd_flag==3)
        return;
    if(lcd_flag==1)
    {
        Set_Column_Address_12864(0x00,0x3F);
        Set_Row_Address_12864(0x00,0x3f);
        Set_Remap_Format_12864(0x50);
        Write_OLED_Ram();
        cc=0;
        lcd_count_disp=0;
        lcd_flag=2;
    }
        if(lcd_flag==2)
        {
            {
                jump_lcd:
                    for(dd=0;dd<128;dd++)   /*to reduce time reduce dd count and increment lcd_count_disp when dd becomes 64*/
                      {
                            oled_Data(OLED_RAM[lcd_count_disp][dd]);//lsb

                        }
                        lcd_count_disp++;
                            if(lcd_count_disp==64)
                            {
                                lcd_flag=3;
                                lcd_count_disp=0;
                                return;
                            }
                    return;
            }
        }
}
void PRINT_LCD(void)
{
unsigned char c,d;
    Set_Column_Address_12864(0x00,63);
  Set_Row_Address_12864(0x00,63);
    Set_Remap_Format_12864(0x50);
    lcd_flag=2;
    Write_OLED_Ram();

    for(c=0;c<64;c++)
        {
            for(d=0;d<128;d++)
            {
                //oled_Data(0xff);
                oled_Data(OLED_RAM[c][d]);
         //super_delay(100);
            }
//        if(c==5)
//            d=0;
        }
    }

/*
void oled_ram_Data(char data)
{
    //[64][64]
    OLED_RAM[START_ROW][COLUMN_POINTER]=data;

    if(COLUMN_POINTER<END_COLUMN)
        COLUMN_POINTER++;
//    if(COLUMN_POINTER > END_COLUMN)
    else    if(START_ROW<=END_ROW)
    {
        START_ROW++;
        COLUMN_POINTER=START_COLUMN;
    }

//oled_Data(data);
}
*/

void oled_ram_Data(char data,char data_1)
{
    //[64][64]
    OLED_RAM[START_ROW][COLUMN_POINTER]=data;
    COLUMN_POINTER++;
    OLED_RAM[START_ROW][COLUMN_POINTER]=data_1;
    if(COLUMN_POINTER<END_COLUMN)
        COLUMN_POINTER++;
    else
    {
        START_ROW++;
        COLUMN_POINTER=START_COLUMN;
    }
     if(START_ROW>=END_ROW)
    {
        START_ROW++;
        COLUMN_POINTER=START_COLUMN;
    }
    /*
    else    if(START_ROW<=END_ROW)
    {
        START_ROW++;
        COLUMN_POINTER=START_COLUMN;
    }
*/

//oled_Data(data);
}

void Print_Variablex(unsigned long variable, unsigned char start_row, unsigned char start_col, unsigned char font_size,
                                            unsigned char underline, unsigned char dec_pos,unsigned char units_pos)
{
unsigned char new_string[10];
unsigned char *new_string_print;
float temp1;
char temp12;
    temp12=sizeof(variable);
    if(font_size==font16)
        temp12=temp12+temp12;

    if(dec_pos == 0)
    {
        temp1 = (float)variable;
    sprintf(new_string,"%.f",temp1);
    }
    else if(dec_pos == 1)
    {
        temp1 = (float)variable / 10;
    sprintf(new_string,"%.1f",temp1);
    }
    else if(dec_pos == 2)
    {
        temp1 = (float)variable / 100;
    sprintf(new_string,"%.2f",temp1);
    }
    else if(dec_pos == 3)
    {
        temp1 = (float)variable / 1000;
    sprintf(new_string,"%.3f",temp1);
    }
    else
    {
        temp1 = (float)variable / 10000;
    sprintf(new_string,"%.4f",temp1);
    }

//    strcat((char*)new_string,(char*)unit_string_ptr);

    new_string_print = new_string;

    unit_string_ptr= *(unit_string + Units_Capacity);

 Print_String(&new_string_print, start_row, start_col,font_size,underline);

if(units_pos<10)
{
    if(start_row ==linex)
    Print_String(&unit_string_ptr, start_row, 45,font_size,underline);
else
    Print_String(&unit_string_ptr, start_row, start_col+4*temp12+3,font_size,underline);
}

}

//long Enter_String(unsigned char variable[10], unsigned char start_row, unsigned char start_col, unsigned char font_size,
//    unsigned char str_size)
//{
//    char *clear_string ="                    ";

//    long VARI=0;
//char new_string[15]={0};
//char print_new_string[15]={0};
//char old_string[15]={0};
// char *new_string_print,*old_string_ptr;
////char str_len1=0,str_len2=0;
//unsigned char j=0,k=0,l=0,j2=0;
//char key_entered;
//char dec_flag=0;
//char F_status=0;
//new_string_print= new_string;        //New String
//    old_string_ptr=old_string;        //Old String
//    sprintf(old_string,"%s",variable);        //Load the function Parameter in Old String
////    str_len1 = strlen(old_string);        //Lenght of Old String

//strcpy(new_string,old_string);    //copy Old strin in New String

//    new_string_print = print_new_string;
////    str_len2 = strlen(new_string);

//strcpy(print_new_string,new_string);

//    for(j2=0;j2<str_size;j2++)        //String Length for Plc is 5
//{
//    j=j2;
//Clear_String(&clear_string, start_row, start_col,font_size);
//Print_String(&new_string_print, start_row, start_col,font_size,j+1);
////Print_Variable(str_len2,start_row+1,start_col,font_size,j+1);
//Key_Input = NO_KEY;
//    back1:
//    if(flag==0)
//        goto back1;
//    flag=0;
//    Key_Input=NO_KEY;
////while(Key_Input>9 && Key_Input!=VER && Key_Input!=ESC && Key_Input!=F_Key)
//while(Key_Input==NO_KEY)
//{
//back2:
//        Key_Input=NO_KEY;
////    while(Key_Input==NO_KEY)
//        key_check_routine();
////    Print_Variable(Key_Input,start_row+1,start_col+10,font_size,j+1);
//}
//        super_delay(50);

//if(Key_Input == k_SET && F_status == 0)
//{
//    F_status= 1;
//    chara="^";
//    Print_String(&chara,3,3,font16,0);
//    goto back2;
//}
//else if(j2>=2 && Key_Input<=9 && F_status ==0)
//{
//    new_string_print[j] = Key_Input+48;                // (char)Key_Input;//new_string_print[j] +1;
//    new_string[j2] = new_string_print[j];            //(char)Key_Input;//new_string[j2] +1;
////Print_Variable(Key_Input,start_row+1,start_col+10,font_size,j+1);
//}
//else if(F_status ==1)
//{
//    F_status=0;
//    chara=" ";
//    Print_String(&chara,3,3,font16,0);
//    Print_String(&chara,3,4,font16,0);

//    if(Key_Input==1)
//        new_string_print[j] = 'A';                // F+1 = A;
//    else if(Key_Input==2)
//        new_string_print[j] = 'C';                // F+2 = C;
//    else if(Key_Input==3)
//        new_string_print[j] = 'D';                // F+3 = D;
//    else if(Key_Input==4)
//        new_string_print[j] = 'I';                // F+4 = I;
//    else if(Key_Input==5)
//        new_string_print[j] = 'L';                // F+5 = L;
//    else if(Key_Input==6)
//        new_string_print[j] = 'M';                // F+6 = M;
//    else if(Key_Input==7)
//        new_string_print[j] = 'N';                // F+7 = N;
//    else if(Key_Input==8)
//        new_string_print[j] = 'O';                // F+8 = O;
//    else if(Key_Input==9)
//        new_string_print[j] = 'P';                // F+9 = P;
//    else if(Key_Input==0)
//        new_string_print[j] = 'R';                // F+0 = R;
//    else if(Key_Input==INT_DE_INT)
//        new_string_print[j] = '*';                // F+net_Gross = *;
//    else if(Key_Input==Start_Stop)
//        new_string_print[j] = 'T';                // F+Start_Stop = T;
//    else if(Key_Input==Auto_Man)
//        new_string_print[j] = '=';                // F+Auto = =;
//    else if(Key_Input==k_SET)
//        new_string_print[j] = 'S';                // F+F = S;
//    else if(Key_Input==HOR)
//        new_string_print[j] = ' ';                // F+HOR = ' '(space);
//    else
//    {
//        F_status=0;
//        goto back2;
//    }

//    new_string[j2] = new_string_print[j];            //(char)Key_Input;//new_string[j2] +1;
//}
//else if(Key_Input == K_DEC || Key_Input == ESC)
//{
//    goto exit_and_return;
//}else
//    goto back1;
////    super_delay(70);

//}
//exit_and_return:
////Print_Variable(sub_display_cursor,line4,start_col+30,font_size,j+1);
//
//VARI = atoi(new_string+2);
//return(VARI);
//}
