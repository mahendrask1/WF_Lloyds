/* adc12bit.c */
#include "adc.h"
#include "dac.h"
#include "GPIO_LPC17xx.h"
#include "lpc17xx_uart.h"
#include "LPC17xx.h"                    // Device header
#include "timer.h"
#include "OLED_2_7_12864.h"
#include "spi.h"
#include "parameters.h"  //#include "menu.h"
#include "functions.h"
#include "RAM512KB_Ext.h"
#include "math.h"

 uint16_t received_val,received_val_1,received_val_2,received_val_3;

 uint16_t AI_Value,AI1_Value,AI2_Value;
 //extern  uint16_t AI_avg,AI_reg[20],AI1_avg,AI1_reg[20],AI2_avg,AI2_reg[20]; //,testarray[16];

 unsigned char AI_pointer,AI_buffer=16,AI1_pointer,AI1_buffer=16,AI2_pointer,AI2_buffer=16;
 uint16_t AI_avg,AI_reg[16],AI1_avg,AI1_reg[16],AI2_avg,AI2_reg[16]; //,testarray[16];
 int AI_sum,AI1_sum,AI2_sum;

extern char* P_String;
extern char A_String[200];

void adc2_ltc1288_cs(int on)
{
  GPIO_PinWrite(adc2_ltc1288_cs_port,adc2_ltc1288_cs_pin, on);
}
void adc1_ltc1288_cs(int on)
{
  GPIO_PinWrite(adc1_ltc1288_cs_port,adc1_ltc1288_cs_pin, on);
}

void adc_ltc1288_cs(int on)
{
  GPIO_PinWrite(adc_ltc1288_cs_port,adc_ltc1288_cs_pin, on);
}

void adc_ltc1288_CLK(char on)
{
  GPIO_PinWrite (dac_CLK_port,dac_CLK_pin, on);

}

void adc_ltc1288_DOUT(char on)
{

    GPIO_PinWrite (dac_DOUT_port,dac_DOUT_pin, on);

}

char adc_ltc1288_DIN()
{
  char adc_pulse_val;
    adc_pulse_val= GPIO_PinRead (0,17);
    return adc_pulse_val;
}

int adc_ltc1288_read_bit_by_bit()                    //reads 16 bits of data from conversion register............
{
char i;

      PINSEL_Configure_Temp();  //configuring SPI pins as GPIO  for bit  by bit reading ADC value
      adc_ltc1288_cs(0); //Enable 12 bit adcGPIO_PinWrite (2,10,0);
super_delay_Temp(1300);

            adc_ltc1288_CLK(1);//clock
          super_delay_Temp(1300);                //super_delay(15);
            adc_ltc1288_DOUT(0);//Data in for DAC (MOSI)
    super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
          super_delay_Temp(1300);                //super_delay(15);
            adc_ltc1288_DOUT(0);//Data in for DAC (MOSI)
    super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(0);//Data in for DAC (MOSI)
            super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(1);//Data in for DAC (MOSI)
            super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(1);//Data in for DAC (MOSI)
            super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(0);//Data in for DAC (MOSI)
            super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(1);//Data in for DAC (MOSI)
            super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(0);//Data in for DAC (MOSI)
            super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            for(i=12;i>=1;i--)
                {
                    adc_ltc1288_CLK(1);//clock
                    super_delay_Temp(1300);    //Instead of after "received_val_1 = adc_ltc1288_DIN(); "delay is shitfed before "received_val_1 = adc_ltc1288_DIN(); "  on 24/1/2017
                    received_val_1 = adc_ltc1288_DIN();
                    received_val = (received_val ) | ( received_val_1 << (i));
                  received_val = received_val;
                    adc_ltc1288_CLK(0);//clock
                    super_delay_Temp(1300);
                }
                adc_ltc1288_cs(1); //disable 12bit adc to confirm before enabling 12bit_adc
                PINSEL_Configure();
                return  received_val ;
}

void Read_adc_ltc1288()      // Initialise and Read Adc Val
{
    static uint16_t tempadc0val;
    int i;
        GPIO_SetDir(adc_ltc1288_cs_port,adc_ltc1288_cs_pin,1);    //Initialize Chip Select for adc
        eeprom_cs(1);
        ram_cs(1);
      adc_cs(1); // disable 24bit adc
      dac1_cs(1); // disable 12bit dac
    dac2_cs(1); // disable 12bit dac
        dac3_cs(1); // disable 12bit dac
        adc2_ltc1288_cs(1); //disable 12bit adc to confirm before enabling 12bit_adc
        adc1_ltc1288_cs(1); //disable 12bit adc to confirm before enabling 12bit_adc
        adc_ltc1288_cs(1); //disable 12bit adc to confirm before enabling 12bit_adc
    received_val = adc_ltc1288_read_bit_by_bit(); //adc_ltc1288_read_bit_by_bit();// //save 12 bit value

    adc_ltc1288_cs(1); //disable 12bit adc

        i=(int)(tempadc0val - received_val);
        if(i <0){i=0-i;}        //get absolute value of i as diff. between last and new reading
        tempadc0val = received_val;
        if(i <= 12)
        {
            AI_Value = received_val;
        }

      super_delay(2);
      received_val = 0;
}

void AI_adc_avg_cal()
{
        if(AI_pointer >= AI_buffer )
            AI_pointer = 0;

            AI_sum = ( AI_sum +    AI_Value - AI_reg[AI_pointer] );
            AI_reg[AI_pointer] =  AI_Value;
            AI_pointer++;
            AI_avg = AI_sum / AI_buffer;
}

/**/
char adc1_ltc1288_DIN()
{
  char adc1_pulse_val;
    adc1_pulse_val= GPIO_PinRead (adc1_DOUT_port,adc1_DOUT_pin);
    return adc1_pulse_val;
}

int adc1_ltc1288_read_bit_by_bit()                    //reads 16 bits of data from conversion register............
{
char S_Count;

      PINSEL_Configure_Temp();  //configuring SPI pins as GPIO  for bit  by bit reading ADC value
      adc1_ltc1288_cs(0); //Enable 12 bit adcGPIO_PinWrite (2,10,0);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(0);//Data in for DAC (MOSI)
                        super_delay_Temp(1300);
    adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(0);//Data in for DAC (MOSI)
                        super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(0);//Data in for DAC (MOSI)
                        super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(1);//Data in for DAC (MOSI)
                        super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(1);//Data in for DAC (MOSI)
                        super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(0);//Data in for DAC (MOSI)
                        super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(1);//Data in for DAC (MOSI)
                        super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(0);//Data in for DAC (MOSI)
                        super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            for(S_Count=12;S_Count>=1;S_Count--)
                {
                    adc_ltc1288_CLK(1);//clock
                    super_delay_Temp(1300);
                    received_val_3 = adc1_ltc1288_DIN();
                    received_val_2 = (received_val_2 ) | ( received_val_3 << (S_Count));
                  received_val_2 = received_val_2;
                    adc_ltc1288_CLK(0);//clock
                    super_delay_Temp(1300);
                }

                adc1_ltc1288_cs(1); //disable 12bit adc to confirm before enabling 12bit_adc
                PINSEL_Configure();
                return  received_val_2 ;
}

void Read_adc1_ltc1288()      // Initialise and Read Adc Val
{
    static uint16_t tempadc1val;
    int i;
        GPIO_SetDir(adc1_ltc1288_cs_port,adc1_ltc1288_cs_pin,1);    //Initialize Chip Select for adc
        eeprom_cs(1);
        ram_cs(1);
      adc_cs(1); // disable 24bit adc
      dac1_cs(1); // disable 12bit dac
    dac2_cs(1); // disable 12bit dac
        dac3_cs(1); // disable 12bit dac
      adc2_ltc1288_cs(1); //disable 12bit adc to confirm before enabling 12bit_adc
        adc_ltc1288_cs(1); //disable 12bit adc to confirm before enabling 12bit_adc
        adc1_ltc1288_cs(1); //disable 12bit adc to confirm before enabling 12bit_adc

    received_val_2 = adc1_ltc1288_read_bit_by_bit(); //adc_ltc1288_read_bit_by_bit();// //save 12 bit value

        adc1_ltc1288_cs(1); //disable 12bit adc

        i=(int)(tempadc1val - received_val_2);
        if(i <0){i=0-i;}        //get absolute value of i as diff. between last and new reading
        tempadc1val = received_val_2;
    if(i <= 12)
    {
        AI1_Value = received_val_2;
    }

      super_delay(2);
        received_val = 0;
      received_val_2 = 0;
        received_val_3 = 0;
}

void AI_adc1_avg_cal()
{
        if(AI1_pointer >= AI1_buffer )
            AI1_pointer = 0;

            AI1_sum = ( AI1_sum +    AI1_Value - AI1_reg[AI1_pointer] );
            AI1_reg[AI1_pointer] =  AI1_Value;
            AI1_pointer++;
            AI1_avg = AI1_sum / AI1_buffer;

}

/**/
/**/
char adc2_ltc1288_DIN()
{
  char adc2_pulse_val;
    adc2_pulse_val= GPIO_PinRead (adc2_DOUT_port,adc2_DOUT_pin);
    return adc2_pulse_val;
}

int adc2_ltc1288_read_bit_by_bit()                    //reads 16 bits of data from conversion register............
{
char S_Count;

      PINSEL_Configure_Temp();  //configuring SPI pins as GPIO  for bit  by bit reading ADC value
      adc2_ltc1288_cs(0); //Enable 12 bit adcGPIO_PinWrite (2,10,0);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(0);//Data in for DAC (MOSI)
                            super_delay_Temp(1300);
        adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(0);//Data in for DAC (MOSI)
                        super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1000);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(0);//Data in for DAC (MOSI)
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(1);//Data in for DAC (MOSI)
                        super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(1);//Data in for DAC (MOSI)
                        super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(0);//Data in for DAC (MOSI)
                        super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(1);//Data in for DAC (MOSI)
                        super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

            adc_ltc1288_CLK(1);//clock
            super_delay_Temp(1300);                //super_delay(15);
          adc_ltc1288_DOUT(0);//Data in for DAC (MOSI)
                        super_delay_Temp(1300);
            adc_ltc1288_CLK(0);//clock
            super_delay_Temp(1300);                //super_delay(15);

                for(S_Count=12;S_Count>=1;S_Count--)
                {
                    adc_ltc1288_CLK(1);//clock
                    super_delay_Temp(1300);
                    received_val_3 = adc2_ltc1288_DIN();
                    received_val_2 = (received_val_2 ) | ( received_val_3 << (S_Count));
                  received_val_2 = received_val_2;
                    adc_ltc1288_CLK(0);//clock
                    super_delay_Temp(1300);
                }

                adc2_ltc1288_cs(1); //disable 12bit adc to confirm before enabling 12bit_adc
                PINSEL_Configure();
                return  received_val_2 ;
}

void Read_adc2_ltc1288()      // Initialise and Read Adc Val
{
    static uint16_t tempadc2val;

    int i;

        GPIO_SetDir(adc2_ltc1288_cs_port,adc2_ltc1288_cs_pin,1);    //Initialize Chip Select for adc
        eeprom_cs(1);
        ram_cs(1);
      adc_cs(1); // disable 24bit adc
      dac1_cs(1); // disable 12bit dac
    dac2_cs(1); // disable 12bit dac
        dac3_cs(1); // disable 12bit dac
        adc_ltc1288_cs(1); //disable 12bit adc to confirm before enabling 12bit_adc
        adc1_ltc1288_cs(1); //disable 12bit adc to confirm before enabling 12bit_adc
    adc2_ltc1288_cs(1); //disable 12bit adc to confirm before enabling 12bit_adc

        received_val_2 = adc2_ltc1288_read_bit_by_bit(); //adc_ltc1288_read_bit_by_bit();// //save 12 bit value
        adc2_ltc1288_cs(1); //disable 12bit adc

        i=(int)(tempadc2val - received_val_2);
        if(i <0){i=0-i;}        //get absolute value of i as diff. between last and new reading
        tempadc2val = received_val_2;
    if(i <= 12)
    {
        AI2_Value = received_val_2;
    }
      super_delay(2);
        received_val = 0;
      received_val_2 = 0;
        received_val_3 = 0;
}

void AI_adc2_avg_cal()
{
        if(AI2_pointer >= AI2_buffer )
            AI2_pointer = 0;

            AI2_sum = ( AI2_sum +    AI2_Value - AI2_reg[AI2_pointer] );
            AI2_reg[AI2_pointer] =  AI2_Value;
            AI2_pointer++;
            AI2_avg = AI2_sum / AI2_buffer;

}

/* msk 19-Jun-2026: AIO comm fault — clear AI averages so setpoint is not last good read */
void AiBuffers_ClearAll(void)
{
    unsigned char k;

    AI_Value = 0;
    AI1_Value = 0;
    AI2_Value = 0;
    AI_avg = 0;
    AI1_avg = 0;
    AI2_avg = 0;
    AI_sum = 0;
    AI1_sum = 0;
    AI2_sum = 0;
    AI_pointer = 0;
    AI1_pointer = 0;
    AI2_pointer = 0;
    for (k = 0; k < AI_buffer; k++) {
        AI_reg[k] = 0;
        AI1_reg[k] = 0;
        AI2_reg[k] = 0;
    }
}

/*
changes:
//Instead of after "received_val_1 = adc_ltc1288_DIN(); "delay is shitfed before "received_val_1 = adc_ltc1288_DIN(); "  on 24/1/2017
//Did above in all ADC12bit
31/03/2018
//Previous was X6A=CH3 & X6B=CH2; Changed AI & AO Channel 2 port/pin to channel 3 & vica versa according to  X6A=CH2 & X6B=CH3

*/
