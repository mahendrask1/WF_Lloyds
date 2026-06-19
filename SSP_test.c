#include "lpc17xx_ssp.h"
#include "lpc17xx_libcfg_default.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

/************************** PRIVATE DEFINTIONS ************************/
/** Max buffer length */
#define BUFFER_SIZE    0x40

/* SSP Configuration structure variable */
SSP_CFG_Type SSP_ConfigStruct;

/* Tx buffer */
uint8_t Tx_Buf[BUFFER_SIZE];

/* Rx buffer */
uint8_t Rx_Buf[BUFFER_SIZE];

/************************** PRIVATE FUNCTIONS *************************/
void Buffer_Init(void);
void Error_Loop(void);
uint8_t Buffer_Verify(void);
uint32_t i, j;
int a,b;
int *c,*d;
unsigned long LED_PINS  =  ((uint32_t)1<<2)|((uint32_t)1<<4) ;

/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief               c_entry: Main SSP program body
 * @param[in]   None
 * @return              int
 **********************************************************************/
int ssp_master(void)
{
        PINSEL_CFG_Type PinCfg;
        SSP_DATA_SETUP_Type xferConfig;

        /*
         * Initialize SPI pin connect
         * P0.15 - SCK;
         * P0.16 - SSEL
         * P0.17 - MISO
         * P0.18 - MOSI
         */
        PinCfg.Funcnum = 2;
        PinCfg.OpenDrain = 0;
        PinCfg.Pinmode = 0;
        PinCfg.Portnum = 0;
        PinCfg.Pinnum = 15;
        PINSEL_ConfigPin(&PinCfg);
        PinCfg.Pinnum = 17;
        PINSEL_ConfigPin(&PinCfg);
        PinCfg.Pinnum = 18;
        PINSEL_ConfigPin(&PinCfg);
        PinCfg.Pinnum = 16;
        PINSEL_ConfigPin(&PinCfg);

        /* initialize SSP configuration structure to default */
        SSP_ConfigStructInit(&SSP_ConfigStruct);
        /* Initialize SSP peripheral with parameter given in structure above */
        SSP_Init(LPC_SSP0, &SSP_ConfigStruct);

        /* Enable SSP peripheral */
        SSP_Cmd(LPC_SSP0, ENABLE);

 //       Buffer_Init();

 //       xferConfig.tx_data = Tx_Buf;
 //       xferConfig.rx_data = Rx_Buf;
//       xferConfig.tx_data = c;
  //     xferConfig.rx_data = d;
    //    xferConfig.length = BUFFER_SIZE;
  //      SSP_ReadWrite(LPC_SSP0, &xferConfig, SSP_TRANSFER_POLLING);
//        SSP_SendData(LPC_SSP0, 0x50);
    //    b= SSP_ReceiveData(LPC_SSP0);

//        *c= 20;
    //    *d=10;
    //    GPIO_SetDir(2, LED_PINS, 1);
        // Verify buffer after transferring //
 /*       if (Buffer_Verify()== 1){
                while(1){
                        // Delay some time //
                        for(i=500; i>0; i--)
                                for(j=10000; j>0; j--) {
                                }
                        // Output low level  //
                        GPIO_ClearValue(2, 0x04);
                        // Delay some time //
                        for(i=500; i>0; i--)
                                for(j=10000; j>0; j--) {
                                }
                        // Output high level //
                        GPIO_SetValue(2, 0x04);
                    }
        }
        else{
                while(1){
                   // Delay some time //
                        for(i=500; i>0; i--)
                                for(j=10000; j>0; j--) {
                                }
                        // Output low level  //
                        GPIO_ClearValue(2, 0x10);
                        // Delay some time //
                        for(i=500; i>0; i--)
                                for(j=10000; j>0; j--) {
                                }
                        // Output high level //
                        GPIO_SetValue(2, 0x10);
                }
        }
    return 1;*/
}

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief               Initialize buffer
 * @param[in]   None
 * @return              None
 **********************************************************************/
void Buffer_Init(void)
{
        uint8_t i;

        for (i = 0; i < BUFFER_SIZE; i++) {
                Tx_Buf[i] = i;
                Rx_Buf[i] = 0;
        }
}

/*********************************************************************//**
 * @brief               Error Loop (called by Buffer_Verify() if any error)
 * @param[in]   none
 * @return              None
 **********************************************************************/
void Error_Loop(void)
{
        /* Loop forever */
        while (1);
}

/*********************************************************************//**
 * @brief               Verify buffer
 * @param[in]   none
 * @return              None
 **********************************************************************/
uint8_t Buffer_Verify(void)
{
        uint8_t i;
        uint8_t *src_addr = (uint8_t *) &Tx_Buf[0];
        uint8_t *dest_addr = (uint8_t *) &Rx_Buf[0];

    if(*c==*d)
        return 1;
    else
        return 0;
    //
    //    for ( i = 0; i < BUFFER_SIZE; i++ )
      //  {
        //        if ( *src_addr++ != *dest_addr++ )
          //      {
            //            /* Call Error Loop */
              //          return 0;
                //}
       // }
       // return 1;
}
