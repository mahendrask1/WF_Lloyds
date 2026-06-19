//Dac.c//
#include "Dac.h"
#include "adc.h"
#include "GPIO_LPC17xx.h"
#include "lpc17xx_spi.h"
#include "lpc17xx_uart.h"
#include "LPC17xx.h"                    // Device header
#include "timer.h"
#include "OLED_2_7_12864.h"
#include "spi.h"
#include "parameters.h"  //#include "menu.h"
#include "functions.h"

//    DAC_CHIP SELECT
void dac1_cs(int on)
{
  GPIO_PinWrite (dac_1_cs_port,dac_1_cs_pin, on);
}

void dac_CLK(char on)
{
  GPIO_PinWrite (dac_CLK_port,dac_CLK_pin, on);
}

void dac_DOUT(char on)
{
    GPIO_PinWrite (dac_DOUT_port,dac_DOUT_pin, on);
}

void dac2_cs(int on)
{
    GPIO_PinWrite (dac_2_cs_port,dac_2_cs_pin, on);

}
void dac3_cs(int on)
{
    GPIO_PinWrite (dac_3_cs_port,dac_3_cs_pin, on);

}

void init_dac()
{
    eeprom_cs(1);
    adc_cs(1);
    dac1_cs(1);
    dac2_cs(1);
    dac3_cs(1);
    adc_ltc1288_cs(1);

    PINSEL_Configure_Temp();
    GPIO_SetDir (dac_CLK_port,dac_CLK_pin,1);    //CLK
    GPIO_SetDir (dac_DOUT_port,dac_DOUT_pin,1);    //DOUT
    dac_CLK(0);//clock
    dac_DOUT(0);// Data in for DAC (MOSI)
}
//--------------------------------------------------
void sort_desc(unsigned int *arr, unsigned int n)
{
    int i,max_idx,j;
  for ( i = 0; i < n; ++i)
    {
         max_idx = i;
        for (j = i + 1; j < n; ++j) {
            if (arr[j] > arr[max_idx]) {
                max_idx = j;
            }
        }
        if (max_idx != i) {
            unsigned int tmp = arr[i];
            arr[i] = arr[max_idx];
            arr[max_idx] = tmp;
        }
    }
}
//---------------------------------------------------
void dac1_write(unsigned int dac1_Count)
{
    /*
    char i,start,end,total,addcount;
     unsigned int  Temp1,Temp2;
    static unsigned int dacopary[12];    //must be atleast 10 elements long array
    static unsigned int tmp1,dcounter=0;
    unsigned int daccnt;

    // Switch SPI pins to GPIO mode for bit-bang
    PINSEL_Configure_Temp();

    daccnt=dac1_Count;
    total = sizeof(dacopary) / sizeof dacopary[0];    //determine nos of elements in array
    start=total/4;        //ommit m nos of high descending elements and m nos of loweer descending elements. means for 12 size array , ignore first 3 and last 3 elements in descending order
    end=total-start;
    //if((total - start)< start){end=start;}else{end=total-start;}

    if(++dcounter >=total){dcounter=0;}
    dacopary[dcounter]=daccnt;
    sort_desc(dacopary, total);                            // sort the array in descending order

    Temp1=0;addcount=0;
    for (i = start; i < end; ++i) // exclude first and last element of an array means exclude Max and Min value of array elements
    {
            Temp1+=dacopary[i];
            addcount++;
    }

    Temp1/=addcount;    // since first and last values are omitted from averaging. (Min and Max values is omitted)

    // CRITICAL FIX: Clear pending interrupts BEFORE disabling to prevent pending IRQs from firing
    NVIC_ClearPendingIRQ(TIMER0_IRQn);
    NVIC_ClearPendingIRQ(TIMER1_IRQn);
    NVIC_ClearPendingIRQ(TIMER2_IRQn);
    NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_ClearPendingIRQ(UART1_IRQn);
    NVIC_ClearPendingIRQ(UART2_IRQn);
    NVIC_ClearPendingIRQ(UART3_IRQn);
    NVIC_ClearPendingIRQ(SPI_IRQn);

    // Now disable all interrupts that could interfere with DAC bit-bang
    NVIC_DisableIRQ(TIMER0_IRQn);
    NVIC_DisableIRQ(TIMER1_IRQn);
    NVIC_DisableIRQ(TIMER2_IRQn);
    NVIC_DisableIRQ(UART0_IRQn);
    NVIC_DisableIRQ(UART1_IRQn);
    NVIC_DisableIRQ(UART2_IRQn);
    NVIC_DisableIRQ(UART3_IRQn);
    NVIC_DisableIRQ(SPI_IRQn);

    dac_CLK(0);//clock low
    super_delay_Temp(1800);        ///msk added this delay 02-nov-2025

    dac2_cs(1);
    dac3_cs(1);
    dac1_cs(0);

        for(i=1;i<=8;i++)
            {
                Temp2 = Temp1 << i;
                Temp2 = Temp2 & 0x1000;
                Temp2=Temp2 >> 12;
                dac_CLK(0);//clock
                super_delay_Temp(1800);        ///msk added this delay 02-nov-2025
                dac_DOUT(Temp2);// Data in for DAC (MOSI)
                super_delay_Temp(1800);
                dac_CLK(1);//clock
                super_delay_Temp(1800);
                //dac_CLK(0);//clock
            }
        for(i=1;i<=4;i++)
            {
                Temp2 = Temp1 << i;
                Temp2 = Temp2 & 0x0010;
                Temp2 = Temp2 >> 4;
                dac_CLK(0);      //clock
                super_delay_Temp(1800);
                dac_DOUT(Temp2);// Data in for DAC (MOSI)
                super_delay_Temp(1800);
                dac_CLK(1);//clock
                super_delay_Temp(1800);
            }
//                dac_CLK(1);//clock
//                super_delay_Temp(2200);
//                dac_DOUT(0);// Data in for DAC (MOSI)
//                super_delay_Temp(2200);
//                dac_CLK(0);//clock
//                super_delay_Temp(2200);

    dac2_cs(1);
    dac1_cs(1);
    dac3_cs(1);
    dac_CLK(0);      //clock

//    __disable_irq();  // disable all interrupts
    __enable_irq();  // enable all interrupts

    // Restore SPI mode for ADC hardware use
    PINSEL_Configure();

        NVIC_EnableIRQ(TIMER0_IRQn);
    NVIC_EnableIRQ(TIMER1_IRQn);
        NVIC_EnableIRQ(TIMER2_IRQn);
        NVIC_EnableIRQ(UART0_IRQn);
    NVIC_EnableIRQ(UART1_IRQn);
        NVIC_EnableIRQ(UART2_IRQn);
        NVIC_EnableIRQ(UART3_IRQn);
    NVIC_EnableIRQ(SPI_IRQn);
        */
}
//--------------------------------------------------------------------
void dac2_write(unsigned int dac2_Count)
{
    /*
    char i,start,end,total,addcount;
    unsigned int  Temp1,Temp2;
    static unsigned int dacopary[12];    //must be atleast 10 elements long array
    static unsigned int tmp1,dcounter=0;
    unsigned int daccnt;

    // Switch SPI pins to GPIO mode for bit-bang
    PINSEL_Configure_Temp();    daccnt=dac2_Count;
    total = sizeof(dacopary) / sizeof dacopary[0];    //determine nos of elements in array
    start=total/4;        //ommit m nos of high descending elements and m nos of loweer descending elements. means for 12 size array , ignore first 3 and last 3 elements in descending order
    end=total-start;
    //if((total - start)< start){end=start;}else{end=total-start;}

    if(++dcounter >=total){dcounter=0;}
    dacopary[dcounter]=daccnt;
    sort_desc(dacopary, total);                            // sort the array in descending order

    Temp1=0;addcount=0;
    for (i = start; i < end; ++i) // exclude first and last element of an array means exclude Max and Min value of array elements
    {
            Temp1+=dacopary[i];
            addcount++;
    }

    Temp1/=addcount;    // since first and last values are omitted from averaging. (Min and Max values is omitted)

    // CRITICAL FIX: Clear pending interrupts BEFORE disabling to prevent pending IRQs from firing
    NVIC_ClearPendingIRQ(TIMER0_IRQn);
    NVIC_ClearPendingIRQ(TIMER1_IRQn);
    NVIC_ClearPendingIRQ(TIMER2_IRQn);
    NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_ClearPendingIRQ(UART1_IRQn);
    NVIC_ClearPendingIRQ(UART2_IRQn);
    NVIC_ClearPendingIRQ(UART3_IRQn);
    NVIC_ClearPendingIRQ(SPI_IRQn);

    // Now disable all interrupts that could interfere with DAC bit-bang
    NVIC_DisableIRQ(TIMER0_IRQn);
    NVIC_DisableIRQ(TIMER1_IRQn);
    NVIC_DisableIRQ(TIMER2_IRQn);
    NVIC_DisableIRQ(UART0_IRQn);
    NVIC_DisableIRQ(UART1_IRQn);
    NVIC_DisableIRQ(UART2_IRQn);
    NVIC_DisableIRQ(UART3_IRQn);
    NVIC_DisableIRQ(SPI_IRQn);

    __disable_irq();  // disable all interrupts for critical DAC bit-bang section

    dac_CLK(0);//clock
    super_delay_Temp(1800);

    dac1_cs(1);
    dac3_cs(1);
    dac2_cs(0);

                for(i=1;i<=8;i++)
                {
                    Temp2 = Temp1 << i;
                    Temp2 = Temp2 & 0x1000;
                    Temp2=Temp2 >> 12;
                    dac_CLK(0);//clock
                    super_delay_Temp(1800);
                    dac_DOUT(Temp2);// Data in for DAC (MOSI)
                    super_delay_Temp(1800);
                    dac_CLK(1);//clock
                    super_delay_Temp(1800);
                }

                for(i=1;i<=4;i++)
                {
                    Temp2 = Temp1 << i;
                    Temp2 = Temp2 & 0x0010;
                    Temp2 = Temp2 >> 4;
                    dac_CLK(0);//clock
                    super_delay_Temp(1800);
                    dac_DOUT(Temp2);// Data in for DAC (MOSI)
                    super_delay_Temp(1800);
                    dac_CLK(1);//clock
                    super_delay_Temp(1800);
                }
//                dac_CLK(1);//clock
//                super_delay_Temp(2200);
//                dac_DOUT(0);// Data in for DAC (MOSI)
//                super_delay_Temp(2200);
//                dac_CLK(0);//clock
//                super_delay_Temp(2200);

            dac1_cs(1);
            dac2_cs(1);
            dac3_cs(1);
                dac_CLK(0);      //clock

    __enable_irq();  // enable all interrupts

    // Restore SPI mode for ADC hardware use
    PINSEL_Configure();

      NVIC_EnableIRQ(TIMER0_IRQn);
    NVIC_EnableIRQ(TIMER1_IRQn);
        NVIC_EnableIRQ(TIMER2_IRQn);
        NVIC_EnableIRQ(UART0_IRQn);
    NVIC_EnableIRQ(UART1_IRQn);
        NVIC_EnableIRQ(UART2_IRQn);
        NVIC_EnableIRQ(UART3_IRQn);
    NVIC_EnableIRQ(SPI_IRQn);
        */
}

//---------------------------------------------------------------------------
void dac3_write(unsigned int dac3_Count)
{
    /*
    char i,start,end,total,addcount;
     unsigned int  Temp1,Temp2;

    static unsigned int dacopary[12];    //must be atleast 10 elements long array
    static unsigned int tmp1,dcounter=0;
    unsigned int daccnt;

    daccnt=dac3_Count;
    total = sizeof(dacopary) / sizeof dacopary[0];    //determine nos of elements in array
    start=total/4;        //ommit m nos of high descending elements and m nos of loweer descending elements. means for 12 size array , ignore first 3 and last 3 elements in descending order
    end=total-start;
    //if((total - start)< start){end=start;}else{end=total-start;}

    if(++dcounter >=total){dcounter=0;}
    dacopary[dcounter]=daccnt;
    sort_desc(dacopary, total);                            // sort the array in descending order

    Temp1=0;addcount=0;
    for (i = start; i < end; ++i) // exclude first and last few element of an array means exclude Max and Min value of array elements
    {
            Temp1+=dacopary[i];
            addcount++;
    }

    Temp1/=addcount;    // since first and last values are omitted from averaging. (Min and Max values is omitted)

    // CRITICAL FIX: Clear pending interrupts BEFORE disabling to prevent pending IRQs from firing
    NVIC_ClearPendingIRQ(TIMER0_IRQn);
    NVIC_ClearPendingIRQ(TIMER1_IRQn);
    NVIC_ClearPendingIRQ(TIMER2_IRQn);
    NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_ClearPendingIRQ(UART1_IRQn);
    NVIC_ClearPendingIRQ(UART2_IRQn);
    NVIC_ClearPendingIRQ(UART3_IRQn);
    NVIC_ClearPendingIRQ(SPI_IRQn);

    // Now disable all interrupts that could interfere with DAC bit-bang
    NVIC_DisableIRQ(TIMER0_IRQn);
    NVIC_DisableIRQ(TIMER1_IRQn);
    NVIC_DisableIRQ(TIMER2_IRQn);
    NVIC_DisableIRQ(UART0_IRQn);
    NVIC_DisableIRQ(UART1_IRQn);
    NVIC_DisableIRQ(UART2_IRQn);
    NVIC_DisableIRQ(UART3_IRQn);
    NVIC_DisableIRQ(SPI_IRQn);
    //__disable_irq();  // disable all interrupts - COMMENTED: already using NVIC_DisableIRQ above

    dac_CLK(0);//clock
    super_delay_Temp(1800);

    dac1_cs(1);
    dac2_cs(1);
    dac3_cs(0);

        for(i=1;i<=8;i++)
        {
            Temp2 = Temp1 << i;
            Temp2 = Temp2 & 0x1000;
            Temp2=Temp2 >> 12;
            dac_CLK(0);//clock
            super_delay_Temp(1800);
            dac_DOUT(Temp2);// Data in for DAC (MOSI)
            super_delay_Temp(1800);
            dac_CLK(1);//clock
            super_delay_Temp(1800);
        }

        for(i=1;i<=4;i++)
        {
            Temp2 = Temp1 << i;
            Temp2 = Temp2 & 0x0010;
            Temp2 = Temp2 >> 4;
            dac_CLK(0);//clock
            super_delay_Temp(1800);        //delay after clock added msk 02-nov-2025
            dac_DOUT(Temp2);// Data in for DAC (MOSI)
            super_delay_Temp(1800);
            dac_CLK(1);//clock
            super_delay_Temp(1800);
        }

//        dac_CLK(1);//clock
//        super_delay_Temp(2200);
//        dac_DOUT(0);// Data in for DAC (MOSI)
//        super_delay_Temp(2200);
//        dac_CLK(0);//clock
//        super_delay_Temp(2200);

        dac1_cs(1);
        dac2_cs(1);
        dac3_cs(1);
        dac_CLK(0);      //clock

    __enable_irq();  // enable all interrupts

    // Restore SPI mode for ADC hardware use
    PINSEL_Configure();

      NVIC_EnableIRQ(TIMER0_IRQn);
    NVIC_EnableIRQ(TIMER1_IRQn);
        NVIC_EnableIRQ(TIMER2_IRQn);
        NVIC_EnableIRQ(UART0_IRQn);
    NVIC_EnableIRQ(UART1_IRQn);
        NVIC_EnableIRQ(UART2_IRQn);
        NVIC_EnableIRQ(UART3_IRQn);
    NVIC_EnableIRQ(SPI_IRQn);
        */
}
