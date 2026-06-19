/***********************************************************************//**
 * @file                uart.c
 * @brief               Test systick work.
 * @version             1.0
 * @date                12. June. 2013
 * @author              jerry2012cn(tudengqian@gmail.com)
 **************************************************************************/

/** Includes--------------------------------------------------------------------*/
#include "board.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_gpio.h"

/************************** GLOBAL VARIABLES *************************/
uint8_t         rxBuf[16] = {0};                /** UART0 receive data buf */
uint32_t    rxCnt         = 0;
volatile Bool       rxFlag    = FALSE;

/************************** PRIVATE VARIABLES *************************/
static void PIN_Init(void);
static void UART0_Init(void);

/**
 * @brief systick test. timer 5ms , and 500ms flash LED
 */
void UartTest(void)
{
        /** System init */
        SystemInit();
        SystemCoreClockUpdate();

        /** LED init */
        CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCGPIO, ENABLE); /** Enable GPIO Power */
        GPIO_SetDir(LED_PORT, LED0, 1);                                 /** Set GPIO Output */
        LEDx_OFF(LED0);                                                                 /** OFF LED0 */

        /** UART init */
        PIN_Init();
        UART0_Init();

        /* print a string */
    UART_Send(LPC_UART0, (uint8_t *)"hello word", sizeof("hello word"), NONE_BLOCKING);

        while (1) {

                if (rxFlag == TRUE) {
                        rxFlag = FALSE;
                        LEDx_ON(LED0);
                        UART_Send(LPC_UART0, rxBuf, rxCnt, BLOCKING);

                } else {
                        LEDx_OFF(LED0);
                }
        }

}

static void PIN_Init(void)
{
        /* Pin configuration for UART0 */
        PINSEL_CFG_Type PinCfg;

        /*
         * Initialize UART0 pin connect
         */
        PinCfg.Funcnum   = 1;
        PinCfg.OpenDrain = 0;
        PinCfg.Pinmode   = 0;
        PinCfg.Portnum   = UART0_PORT;
        PinCfg.Pinnum    = UART0_TX;
        PINSEL_ConfigPin(&PinCfg);
        PinCfg.Pinnum    = UART0_RX;
        PINSEL_ConfigPin(&PinCfg);
}

static void UART0_Init(void)
{
        LPC_UART_TypeDef *UARTx = (LPC_UART_TypeDef *)LPC_UART0;

        /* UART Configuration structure variable */
        UART_CFG_Type UARTConfigStruct;
        /* UART FIFO configuration Struct variable */
        UART_FIFO_CFG_Type UARTFIFOConfigStruct;

        /* Initialize UART Configuration parameter structure to default state:
         * Baudrate = 9600bps
         * 8 data bit
         * 1 Stop bit
         * None parity
         */
        UART_ConfigStructInit(&UARTConfigStruct);

        /* Set Baudrate to 115200 */
//        UARTConfigStruct.Baud_rate = 9600;

        /* Initialize UART0 peripheral with given to corresponding parameter */
        UART_Init(UARTx, &UARTConfigStruct);

        /* Initialize FIFOConfigStruct to default state:
         *                              - FIFO_DMAMode = DISABLE
         *                              - FIFO_Level = UART_FIFO_TRGLEV0
         *                              - FIFO_ResetRxBuf = ENABLE
         *                              - FIFO_ResetTxBuf = ENABLE
         *                              - FIFO_State = ENABLE
         */
        UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
        UARTFIFOConfigStruct.FIFO_Level = UART_FIFO_TRGLEV2;    /* 8 Bytes */
        /* Initialize FIFO for UART0 peripheral */
        UART_FIFOConfig(UARTx, &UARTFIFOConfigStruct);

        /** config and enable interrupter */
        UART_IntConfig(UARTx, UART_INTCFG_RBR, ENABLE);

        /** NVIC enable interrupter , it's request??*/
        NVIC_EnableIRQ(UART0_IRQn);

        /*  Enable UART Transmit */
        UART_TxCmd(UARTx, ENABLE);
}

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/**
 * @brief UART0 interrupter
 */
void UART0_IRQHandler(void)
{
        uint8_t ReceiveValue;
        uint8_t i;
        uint32_t IIRValue;

        rxCnt = 0;
        IIRValue = UART_GetIntId(LPC_UART0);

        while ((IIRValue & 0x01) == 0){ /* if have another interrupter hand on */
                switch (IIRValue & 0x0E) {      /* flag */
                case UART_IIR_INTID_RDA :
                        for (i = 0; i < 8; i++) {
                                rxFlag = TRUE;
                                ReceiveValue = UART_ReceiveByte(LPC_UART0);
                                rxBuf[rxCnt++] = ReceiveValue;
                        }
                        break;
                case UART_IIR_INTID_CTI:
                        rxFlag = TRUE;
                        ReceiveValue = UART_ReceiveByte(LPC_UART0);
                        rxBuf[rxCnt++] = ReceiveValue;
                        break;
                default:
                        break;
                }
                IIRValue = UART_GetIntId(LPC_UART0);
        }
}

/*------- --------------------------End File------------------------------- */
