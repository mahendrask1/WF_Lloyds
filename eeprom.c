/*        eeprom.c        */
#include "eeprom.h"
#include <stdint.h>

#include "functions.h"
#include "GPIO_LPC17xx.h"
#include "lpc17xx_uart.h"
#include "LPC17xx.h"                    // Device header
#include "main_routine.h"
#include "spi.h"
#include <string.h>
#include "OLED_2_7_12864.h"
#include "lpc_types.h"
#include "parameters.h"

char Decimal_Point[5];
extern long calib_val[8];
extern uint16_t  Min_adc1_val,Min_adc2_val,Min_adc3_val,Max_adc1_val,Max_adc2_val,Max_adc3_val,AI_calb_val[6],AO_val[6];
extern unsigned long Present_Count,Present_Count_ch2,Count_DL,Count_DL_ch2,Span_Count,Span_Count_ch2,zero_load,zero_load_ch2,full_load,full_load_ch2;
uint16_t New_menu_parameters[105];
uint16_t AO_cal_Values[6];
uint32_t totalizer_parameters[2];
//extern int bytes_written;
extern unsigned char report_time[6];
extern uint8_t plc_temp[400];
int temp1,temp2;
extern char plc_inst[200][7];
extern int ass_counter, prog_counter;

extern unsigned long Min_Count_Channel1,Min_Count_Channel2,Min_Count_Channel3,Max_Count_Channel1,Max_Count_Channel2,Max_Count_Channel3;
extern unsigned long Dac1_SpanCounts,Dac2_SpanCounts,Dac3_SpanCounts;
//extern uint16_t Dac1_ScaleFactor, Dac2_ScaleFactor, Dac3_ScaleFactor; //These are macros in parameters.h
extern unsigned int Totaliser1_Display,Totaliser2_Display;

char calib_string1[20];
char *calib_string_ptr1 = calib_string1;

void eeprom_cs(int on)
{
  GPIO_PinWrite (eeprom_cs_port,eeprom_cs_pin, on);
}

char read_eeprom_status(void)
{
char data;
    eeprom_cs(0);
    spi_write(0x05);        //READ Status Register instruction
    data = spi_read();
    eeprom_cs(1);
    return (data);
}

//-----------------------------------------------------------

//------------------------------------------------
//-----------------------------------------------------------
void eeprom_write_array(unsigned int start_address, unsigned char *ram_address, unsigned int no_of_bytes)
{
    unsigned int start_address_lo, start_address_hi,bytecounter,local_start_add,Page_Counter,i;
    unsigned char data,eeprom_status;

    if(no_of_bytes <= 0){return;}

    bytecounter=0;        // bytes counter ptr
    local_start_add=start_address;

    Page_Counter=0;

    eeprom_status    = read_eeprom_status();
    if((eeprom_status & 0x01)==1)        //eeprom not Ready
    {    super_delay(30);}

    eeprom_status    = read_eeprom_status();
    if((eeprom_status & 0x01)==1)        //eeprom not Ready
    {
        calib_string_ptr1 = "            ";
        Print_String(&calib_string_ptr1,line1,0,font16,0);
        calib_string_ptr1 = "EPR ERROR..";
        Print_String(&calib_string_ptr1,line1,0,font16,0);        //EPR display msg removed msk 08-octo25
        super_delay(100);
        return;
    }
    eeprom_cs(1);
    calib_string_ptr1 = "            ";
    Print_String(&calib_string_ptr1,line1,0,font16,0);
    calib_string_ptr1 = "SAVING.....";
    Print_String(&calib_string_ptr1,line1,0,font16,0);        //EPR display msg removed msk 08-octo25
    super_delay(30);

    eeprom_cs(0);        //cs_adc=0;                    //select adc
    spi_write(0x06);        //WRITE ENABLE
    eeprom_cs(1);
    start_address_lo = local_start_add & 0xFF;
    start_address_hi = local_start_add & 0x1F00 ;
    start_address_hi = local_start_add >> 8;
    eeprom_cs(0);
    spi_write(0x02);                            //Write Instruction
    spi_write(start_address_hi);    //address High Byte
    spi_write(start_address_lo);    //address Low Byte
        while(bytecounter  < no_of_bytes)
        {
            data = *(ram_address+bytecounter);            //load data from RAM to be written
            spi_write(data);                    //write data to EEPROM
            bytecounter++;
            local_start_add++;

            if(((local_start_add & 0x1f) == 0x00) && (bytecounter  < no_of_bytes))    //if page boundry crossing and bytes are yet to be written
            {
                Page_Counter++;
                    eeprom_cs(1);
                    calib_string_ptr1 = "            ";
                    Print_String(&calib_string_ptr1,line1,0,font16,0);
                    calib_string_ptr1 = "SAVING.....";
                    Print_String(&calib_string_ptr1,line1,0,font16,0);        //EPR display msg removed msk 08-octo25
                    super_delay(35);

                    eeprom_status    = read_eeprom_status();

                    if((eeprom_status & 0x01)==1)        //eeprom not Ready
                    {
                        super_delay(5);
                        eeprom_status    = read_eeprom_status();
                        if((eeprom_status & 0x01)==1)        //eeprom not Ready
                        {
                            eeprom_cs(1);
                            calib_string_ptr1 = "            ";
                            Print_String(&calib_string_ptr1,line1,0,font16,0);
                            calib_string_ptr1 = "EPR ERROR..";
                            Print_String(&calib_string_ptr1,line1,0,font16,0);        //EPR display msg removed msk 08-octo25
                            super_delay(100);
                            return;
                        }
                    }
                    eeprom_cs(0);
                    spi_write(0x06);        //WRITE ENABLE
                    eeprom_cs(1);

                    start_address_lo = local_start_add & 0xFF;
                    start_address_hi = local_start_add & 0x1F00 ;
                    start_address_hi = local_start_add >> 8;

                    eeprom_cs(0);
                    spi_write(0x02);                            //Write Instruction
                    spi_write(start_address_hi);    //address High Byte
                    spi_write(start_address_lo);    //address Low Byte
                }//if((local_start_add & 0x1f) == 0x00)
        }//while(bytecounter  < no_of_bytes)
        eeprom_cs(1);
}
//------------------------------------------------

//--------------------------------------------------
void eeprom_read_block(int start_address, unsigned char *ram_address, int no_of_bytes)
{
    unsigned char aaa,b,c,d,e,f;
    int start_address_lo, start_address_hi,i;
    start_address_lo = start_address & 0xFF;
    start_address_hi = start_address & 0x1F00 ;
    start_address_hi = start_address_hi >> 8;
    super_delay(30);
eeprom_cs(0);
    for(b=0; b<100; b++){;}
spi_write(0x03);                                //read instruction

spi_write(start_address_hi);        //high byte address
spi_write(start_address_lo);        //low byte address

    temp1 = start_address_hi;
    temp2 = start_address_lo;

    for(i=0;i<no_of_bytes;i++)
    //    *(ram_address+i) = spi_read();        //read SPI reg
    {
        aaa=spi_read();
    //(char *)(&ram_address)[i]=a;
        ram_address[i]=aaa;
    }
//
//    b=spi_read();
//c=spi_read();
//d=spi_read();
//e=spi_read();
//

eeprom_cs(1);

}
//--------------------------------------------------------------
//Save Menu Parameters in EEPROM(2-Bytes Each)
void save_par_eeprom(void)
{
//    if(bytes_written == 0)
//    {
        //eeprom_write_block(bytes_written,(char*)New_menu_parameters,324,bytes_written,2);
        eeprom_write_array(0, (unsigned char *)New_menu_parameters , 324 );
//        bytes_written=1000;
//    }
}
//-------------------------------------------------------------------------------------
void read_par_eeprom(void)
{
    //eeprom_read_block(0,(char*)menu_parameters,320);
    eeprom_read_block(0,(unsigned char*)New_menu_parameters,324);
    return;
}
//-------------------------------------------------------------------------------------
void     save_calib_val(void)
{
//    bytes_written=0;
//    calib_val[2] = Count_DL;//save_calib_val();
    //eeprom_write_block(512,(char*)calib_val,80,0,2);
    eeprom_write_array(512, (unsigned char *)calib_val , 80 );
//    bytes_written=324;
}
void read_calib_val(void)
{

    eeprom_read_block(512,(unsigned char*)calib_val,80);

  full_load = calib_val[0];
    Span_Count = calib_val[1];
  zero_load = calib_val[2];

    full_load_ch2  =     calib_val[3];
    Span_Count_ch2 =  calib_val[4];
  zero_load_ch2  =  calib_val[5];

}

void     save_AO_calib_val(void)
{
//    bytes_written=0;
//    eeprom_write_block(1616,(char*)AO_val,12,0,2);
    eeprom_write_array(1600,(unsigned char*)AO_cal_Values,12);

    //ytes_written=320;//initialize to 320 so that EEPROM does not write

}

void read_AO_calib_val(void)
{
    eeprom_read_block(1600,(unsigned char*)&AO_cal_Values[0],12);
    Min_Count_Channel1=AO_calz1;
    Max_Count_Channel1=AO_cals1;
    Dac1_SpanCounts = (Max_Count_Channel1-Min_Count_Channel1);

    Min_Count_Channel2=AO_calz2;
    Max_Count_Channel2=AO_cals2;
    Dac2_SpanCounts = (Max_Count_Channel2-Min_Count_Channel2);

    Min_Count_Channel3=AO_calz3;
    Max_Count_Channel3=AO_cals3;
    Dac3_SpanCounts = (Max_Count_Channel3-Min_Count_Channel3);

    //Initialize DAC scale factors to 10000 (100.00%) if invalid
    if(Dac1_ScaleFactor < 5000 || Dac1_ScaleFactor > 20000) Dac1_ScaleFactor = 10000;
    if(Dac2_ScaleFactor < 5000 || Dac2_ScaleFactor > 20000) Dac2_ScaleFactor = 10000;
    if(Dac3_ScaleFactor < 5000 || Dac3_ScaleFactor > 20000) Dac3_ScaleFactor = 10000;
}
/*
void read_AO_calib_val_ch1(void)    //msk 17-aug-25
{
    eeprom_read_block(1600,(char*)&AO_val[0],12);

  Min_Count_Channel1 = AO_val[0];
    Max_Count_Channel1 = AO_val[1];
//  Min_Count_Channel2 = AO_val[2];
//    Max_Count_Channel2 = AO_val[3];
//    Min_Count_Channel3 = AO_val[4];
//    Max_Count_Channel3 = AO_val[5];

}

void read_AO_calib_val_ch2(void)    //msk 17-aug-25
{
    eeprom_read_block(1600,(char*)AO_val,12);

//  Min_Count_Channel1 = AO_val[0];
//    Max_Count_Channel1 = AO_val[1];
  Min_Count_Channel2 = AO_val[2];
    Max_Count_Channel2 = AO_val[3];
//    Min_Count_Channel3 = AO_val[4];
//    Max_Count_Channel3 = AO_val[5];
}
void read_AO_calib_val_ch3(void)    //msk 17-aug-25
{
    eeprom_read_block(1600,(char*)AO_val,12);

 // Min_Count_Channel1 = AO_val[0];
    //Max_Count_Channel1 = AO_val[1];
//  Min_Count_Channel2 = AO_val[2];
//    Max_Count_Channel2 = AO_val[3];
    Min_Count_Channel3 = AO_val[4];
    Max_Count_Channel3 = AO_val[5];
}
*/

void     save_AI_calib_val(void)
{
//    bytes_written=0;
    eeprom_write_array(1642,(unsigned char*)AI_calb_val,12);
//    bytes_written=324; ////initialize to 320 so that EEPROM does not overwrite into menu parameters
}
void read_AI_calib_val(void)
{

            eeprom_read_block(1642,(unsigned char*)AI_calb_val,12);
          Min_adc1_val = AI_calb_val[0];
            Max_adc1_val = AI_calb_val[1];
            Min_adc2_val = AI_calb_val[2];
            Max_adc2_val = AI_calb_val[3];
            Min_adc3_val = AI_calb_val[4];
            Max_adc3_val = AI_calb_val[5];
}

void save_ass_eeprom(void)
{
//    bytes_written=0;
//    while(bytes_written<400)
//    {
        eeprom_write_array(1024,(unsigned char*)plc_temp,400);
//    }
//    bytes_written = 324;                //initialize to 320 so that EEPROM does not write
}

void read_ass_eeprom(void)
{
    int i;
    eeprom_read_block(1024,(unsigned char*)plc_temp,400);
}

void save_plc_edit_eeprom(void)
{
//bytes_written=0;
    //while(bytes_written<1400)
    //{
        eeprom_write_array(2048,(unsigned char*)plc_inst,1400);
    //}
//        bytes_written = 324;                //initialize to 320 so that EEPROM does not write
}

void read_plc_edit_eeprom(void)
{
    int i;
    eeprom_read_block(2048,(unsigned char*)plc_inst,1400);
    return;

}

/*
Date:07/04/2018
save_plc_edit_eeprom();
read_plc_edit_eeprom();
No of bytes to write is increased from 400 to 1400 as 400 is able to save only 55 no of instuctions and actually it sholud be 200.

*/
