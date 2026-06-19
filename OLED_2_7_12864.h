/* OLED_2_7_12864.h*/

#define font10  0
#define font16  1
#define font32  2

#define line1 0
#define line2 1
#define line3 2
#define line4 3
#define line5 4
#define line6 5

#define linex 100
#define P_LCD 1
#define P_RAM 0

void PRINT_LCD(void);
void oled_ram_Data(char data,char data_1);
void Clear_LCD_RAM(void);
void Print_Byte_Width2(char data, char width);
void Clear_single_Line(char Start_Row_no, char End_Row_no);

void oled_Command(unsigned char Data);
void oled_Data(unsigned char Data);
void Set_Column_Address_12864(unsigned char a, unsigned char b);
void Set_Row_Address_12864(unsigned char a, unsigned char b);
void Write_OLED_Ram(void);
void Set_Contrast_Current_12864(unsigned char d);
void Set_Current_Range_12864(unsigned char d);
void Set_Remap_Format_12864(unsigned char d);
void Set_Start_Line_12864(unsigned char d);
void Set_Display_Offset_12864(unsigned char d);
void Set_Display_Mode_12864(unsigned char d);
void Set_Multiplex_Ratio_12864(unsigned char d);
void Set_Master_Config_12864(unsigned char d);

void Clear_LCD(void);
void OLED_12864_Init(void);
void OLED_12864_ApplyInitCommands(void); /* msk29-may-26 */
void OLED_12864_ResetAndInit(void); /* msk29-may-26 */
void Lcd_ReinitForMenu(void); /* msk29-may-26 */
void Init_OLED_Pins(void);
void Print_Byte_Width(char data, char width);
//void clear_string(unsigned char start_row, unsigned char start_col, unsigned char font_size);
void Print_Char(char char_data, char font_size , char underline);
void Print_String(char **string, char start_row, char start_col, char font_size, char underline_no);
void Print_String2(const char **string, char start_row, char start_col, char font_size, char underline_no);

void Print_Variable(unsigned long variable, unsigned char start_row, unsigned char start_col, unsigned char font_size, unsigned char underline);
void Enter_Variable(unsigned long variable, unsigned char start_row, unsigned char start_col, unsigned char font_size);
long Enter_Variable2(unsigned long variable, unsigned char start_row, unsigned char start_col, unsigned char font_size,
    unsigned char no_of_digit, unsigned char decimal_pt);

long Enter_Variable4(unsigned long variable, unsigned char start_row, unsigned char start_col, unsigned char font_size,
    unsigned char no_of_digit, unsigned char decimal_pt);

long Enter_Variable3(unsigned long variable, unsigned char start_row, unsigned char start_col, unsigned char font_size,
    unsigned char no_of_digit, unsigned char decimal_pt);
void Clear_String(char **string, char start_row, char start_col, char font_size);
void Print_Variable2(unsigned long variable, unsigned char start_row, unsigned char start_col, unsigned char font_size,
    unsigned char underline, unsigned char dec_pos, unsigned char units_pos);
void Print_Variable3(unsigned long variable, unsigned char start_row, unsigned char start_col, unsigned char font_size,
    unsigned char underline, unsigned char dec_pos,unsigned char units_pos, char parameter_unit);

void Print_Variable1(unsigned long variable, unsigned char start_row, unsigned char start_col, unsigned char font_size, unsigned char underline);
void Print_Variablex(unsigned long variable, unsigned char start_row, unsigned char start_col, unsigned char font_size,
    unsigned char underline, unsigned char dec_pos,unsigned char units_pos);
