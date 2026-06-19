#include "lpc17xx_spi.h"
#include "lpc17xx_pinsel.h"
#include "lpc_types.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_libcfg_default.h"
#include "spi.h"
#include "timer.h"
#include "functions.h"
/*---------------Macro Define--------------------------------------*/
#define BUFFER_SIZE 0x20

/*--------------------- Variable Declare--------------------------*/
SPI_CFG_Type spiInitialization;
volatile uint8_t spi_bus_fault = 0; //msk SPI hang detect 26-may-2026
static uint8_t spi_timeout_enabled = 0;

void spi_timeout_enable(uint8_t enable) //msk disable timeout during ADC init 26-may-2026
{
    spi_timeout_enabled = enable ? 1U : 0U;
}

uint8_t spi_write(unsigned char _data)
{
    uint32_t start_tick;

    SPI_SendData(LPC_SPI, _data);

    if (spi_timeout_enabled == 0U) {
        while (!((LPC_SPI->SPSR) & SPI_SPSR_SPIF)) {
            ;
        }
        return 1U;
    }

    start_tick = spi_wait_tick;
    while (!((LPC_SPI->SPSR) & SPI_SPSR_SPIF)) {
        if ((spi_wait_tick - start_tick) >= SPI_WAIT_TIMEOUT_TICKS) { //msk ~10 ms 26-may-2026
            spi_bus_fault = 1U;
            adc_spi_fault_recovery(); //msk re-init CS5532 26-may-2026
            return 0U;
        }
    }
    return 1U;
}

char spi_read(void)
{
    if (spi_write(0x00) == 0U) { //msk SPI timeout path 26-may-2026
        return (char)0xFF;
    }
    return (char)((unsigned char)SPI_ReceiveData(LPC_SPI));
}
/*
 * @brief    Simply test the SPI to ensure its available
 * @param    None
 * @return   None
 */

void SPI_Initialize()
{
        SPI_DATA_SETUP_Type xferConfig;

        SystemInit();

        PINSEL_Configure();
        SPI_Configure();

}
/*
 * @brief    Configure the SPI to make it work normally
 * @param    None
 * @return   None
 */
void SPI_Configure()
{
        PINSEL_Configure();

        SPI_ConfigStructInit(&spiInitialization);
        /* Initialize SPI peripheral with parameter given in structure above
         * CPHA - SPI_CPHA_FIRST
         * CPOL - SPI_CPOL_HI
         * ClockRate - 1000000
         * DataOrder - SPI_DATA_MSB_FIRST
         * DataBit - SPI_DATABIT_8
         * Mode - SPI_MASTER_MODE
         */
        SPI_Init(LPC_SPI,&spiInitialization);//SPI_IntCmd(LPC_SPI, ENABLE);
        /*Use the default value to configure the SPI*/
}

void SPI_IRQHandler (void)
{
        /*  Clear Interrupt */
        SPI_ClearIntPending(LPC_SPI);
        /* Code...*/

    return;
}

/*
 * @brief    Configure the PINSEL to valid mode
 * @param    None
 * @return   None
 */
void PINSEL_Configure()
{
        PINSEL_CFG_Type PinCfg1;
        PinCfg1.Funcnum = 3;
        PinCfg1.OpenDrain = 0;
        PinCfg1.Pinmode = 0;
        PinCfg1.Portnum = 0;
        PinCfg1.Pinnum = 15;
        PINSEL_ConfigPin(&PinCfg1);
        PinCfg1.Pinnum = 17;
        PINSEL_ConfigPin(&PinCfg1);
        PinCfg1.Pinnum = 18;
        PINSEL_ConfigPin(&PinCfg1);
  //      PinCfg.Pinnum = 16;
    //    PinCfg.Funcnum = 0;
    //    PINSEL_ConfigPin(&PinCfg);
}

void PINSEL_Configure_Temp()/////////////////////////temp for testing of dac disabling SPI Pins and setting them as GPIO
{
        PINSEL_CFG_Type PinCfg1;
        PinCfg1.Funcnum = 0;
        PinCfg1.OpenDrain = 0;
        PinCfg1.Pinmode = 0;
        PinCfg1.Portnum = 0;
        PinCfg1.Pinnum = 15;    //CLK
        PINSEL_ConfigPin(&PinCfg1);

        PinCfg1.Pinnum = 17;    //MISO
        PINSEL_ConfigPin(&PinCfg1);

        PinCfg1.Pinnum = 18;    //MOSI
        PINSEL_ConfigPin(&PinCfg1);
  //      PinCfg.Pinnum = 16;
    //    PinCfg.Funcnum = 0;
    //    PINSEL_ConfigPin(&PinCfg);
}

void PINSEL_Configure_IO()            ///////////////////////// IO Pins and setting  GPIO in PULL Down mode
{
        PINSEL_CFG_Type PinCfg1;
        PinCfg1.Funcnum = 0;
        PinCfg1.OpenDrain = 0;
        PinCfg1.Pinmode = PINSEL_PINMODE_PULLDOWN;
        PinCfg1.Portnum = 0;

                PinCfg1.Pinnum = 4;    //
        PINSEL_ConfigPin(&PinCfg1);
          PinCfg1.Pinnum = 5;    //
        PINSEL_ConfigPin(&PinCfg1);
                 PinCfg1.Pinnum = 19;
                PINSEL_ConfigPin(&PinCfg1);
                PinCfg1.Pinnum = 20;    //
        PINSEL_ConfigPin(&PinCfg1);
        PinCfg1.Pinnum = 21;    //
        PINSEL_ConfigPin(&PinCfg1);
          PinCfg1.Pinnum = 23;    //
        PINSEL_ConfigPin(&PinCfg1);
                 PinCfg1.Pinnum = 24;
                PINSEL_ConfigPin(&PinCfg1);
                PinCfg1.Pinnum = 27;    //
        PINSEL_ConfigPin(&PinCfg1);
                PinCfg1.Pinnum = 28;    //
        PINSEL_ConfigPin(&PinCfg1);

          PinCfg1.Portnum = 1;
                PinCfg1.Pinnum = 31;
                PINSEL_ConfigPin(&PinCfg1);
                PinCfg1.Pinnum = 21;    //
        PINSEL_ConfigPin(&PinCfg1);
                PinCfg1.Pinnum = 27;    //
        PINSEL_ConfigPin(&PinCfg1);

}
