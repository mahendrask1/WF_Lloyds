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
#include "RAM512KB_Ext.h"

unsigned char read_val_ram[65],Data_Write[66]={0x13,0x12,0x11,0x10,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,
                                                                                             0x26,0x25,0x24,0x23,0x22,0x21,0x20,0x19,0x18,0x17,0x16,0x15,0x14,
                                                                                             0x39,0x38,0x37,0x36,0x35,0x34,0x33,0x32,0x31,0x30,0x29,0x28,0x27,
                                                                                             0x52,0x51,0x50,0x49,0x48,0x47,0x46,0x45,0x44,0x43,0x42,0x41,0x40,
                                                                                             0x65,0x64,0x63,0x62,0x61,0x60,0x59,0x58,0x57,0x56,0x55,0x54,0x53};

//void read_sequential( );
//void write_sequential();
void ram_cs(char);

void ram_cs(char on)
{
  GPIO_PinWrite(Ram__cs_port,Ram__cs_pin, on);
}
/*
void read_sequential()
{
    unsigned int i;
    unsigned char start_address_hi,start_address_lo;

    start_address_hi = 0x00;
    start_address_lo = 0x00;

    ram_cs(1);
    ram_cs(0);
    ram_cs(1);
    adc_ltc1288_cs(1);
    dac1_cs(1);
    adc_cs(1);
    eeprom_cs(1);

    ram_cs(0);                                       //Enable ram
    spi_write(0x03);              //Read instruction
    spi_write(start_address_hi);    //address High Byte
    spi_write(start_address_lo);    //address Low Byte
    for(i=0;i<65;i++)
    {
     read_val_ram[i] =  spi_read();     //Read data into variable read_val
    }
    ram_cs(1);                                        //Disable ram

}

void write_sequential()
{
    unsigned int i;
    unsigned char start_address_hi,start_address_lo;
    start_address_hi = 0x00;
    start_address_lo = 0x00;

    ram_cs(1);
    ram_cs(0);
    ram_cs(1);

    adc_ltc1288_cs(1);
    dac1_cs(1);
    adc_cs(1);
    eeprom_cs(1);

    ram_cs(0);                                        //Enable ram
    spi_write(0x02);                             //Write instruction
    spi_write(start_address_hi);    //address High Byte
    spi_write(start_address_lo);    //address Low Byte

    for(i=0;i<65;i++)
    {
        spi_write(Data_Write[i]);             //Write data
    }
    ram_cs(1);                                         //Disable ram

}
*/
