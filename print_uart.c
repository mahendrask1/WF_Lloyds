#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include "modbus.h"
# include "parameters.h"
#include "timer.h"
#include "modbus_dio.h"

#define BUFSIZE        0x40
#define IER_RBR        0x01
#define IER_THRE    0x02
#define IER_RLS        0x04

#define IIR_PEND    0x01
#define IIR_RLS        0x03
#define IIR_RDA        0x02
#define IIR_CTI        0x06
#define IIR_THRE    0x01

#define LSR_RDR        0x01
#define LSR_OE        0x02
#define LSR_PE        0x04
#define LSR_FE        0x08
#define LSR_BI        0x10
#define LSR_THRE    0x20
#define LSR_TEMT    0x40
#define LSR_RXFE    0x80

int  test;
volatile uint32_t UART0Status, UART1Status,UART3Status;
volatile uint8_t UART0TxEmpty = 1, UART1TxEmpty = 1, UART3TxEmpty = 1;
volatile uint8_t UART0Buffer[BUFSIZE], UART1Buffer[BUFSIZE],UART3Buffer[BUFSIZE];
volatile uint32_t UART0Count = 0, UART1Count = 0 , UART3Count = 0;

int ab = 1234;
uint32_t UARTInit( uint32_t portNum, uint32_t Baudrate );
void UART0_IRQHandler( void );
void UART1_IRQHandler( void );
void UARTSend( uint32_t portNum, uint8_t *BufferPtr, uint32_t Length );

extern int ModTxCount,ModTxLen,ModBusIdx;
extern unsigned char ModBusMessage[MAX_LENGTH],ModData[MAX_LENGTH];
extern uint32_t Modbus_Baud_Rate;
extern uint8_t Modbus_Parity ;

extern unsigned char DIO_ModBusMessage[MAX_LENGTH],DIO_ModData[MAX_LENGTH];
extern char DIO_ModTxLen, DIO_ModTxCount,DIO_ModBusIdx ;

extern unsigned char tx_over;
unsigned char DIO_ModBusMessage_1[30];
unsigned int par_count;
extern unsigned char download_menu_par[2900];
extern unsigned char Dwnd_flag;

extern unsigned char ABCC_RX_TX_Status;
extern unsigned int ABCC_RX_TX_Count,ABCC_RX_TX_Length,ABCC_RX_TX_Length_1,ABCC_RX_TX_Count_1;
extern unsigned char *TX_Data,*RX_Data;
extern uint16_t New_menu_parameters[105];

void UART_PrintExp()        //Config_UART2
{
LPC_UART_TypeDef *UARTx = (LPC_UART_TypeDef *)LPC_UART2;    //Initialize UART2

        /* UART Configuration structure variable */
        UART_CFG_Type UARTConfigStruct;
        /* UART FIFO configuration Struct variable */
        UART_FIFO_CFG_Type UARTFIFOConfigStruct;
        /* Pin configuration for UART0 */
        PINSEL_CFG_Type PinCfg;

        if((uint32_t)UARTx == (uint32_t)LPC_UART0) {
                /*
                 * Initialize UART0 pin connect
                 */
                PinCfg.Funcnum = 1;
                PinCfg.OpenDrain = 0;
                PinCfg.Pinmode = 0;
                PinCfg.Pinnum = 2;
                PinCfg.Portnum = 0;
                PINSEL_ConfigPin(&PinCfg);
                PinCfg.Pinnum = 3;
                PINSEL_ConfigPin(&PinCfg);
        }
        else if ((uint32_t)UARTx == (uint32_t)LPC_UART1) {
                /*
                 * Initialize UART1 pin connect
                 */
                PinCfg.Funcnum = 2;
                PinCfg.OpenDrain = 0;
                PinCfg.Pinmode = 0;
                PinCfg.Pinnum = 0;
                PinCfg.Portnum = 2;
                PINSEL_ConfigPin(&PinCfg);
                PinCfg.Pinnum = 1;
                PINSEL_ConfigPin(&PinCfg);
        }
                else if ((uint32_t)UARTx == (uint32_t)LPC_UART2) {
                /*
                 * Initialize UART1 pin connect
                 */
                PinCfg.Funcnum = 2;
                PinCfg.OpenDrain = 0;
                PinCfg.Pinmode = 0;
                PinCfg.Pinnum = 8;
                PinCfg.Portnum = 2;
                PINSEL_ConfigPin(&PinCfg);
                PinCfg.Pinnum = 9;
                PINSEL_ConfigPin(&PinCfg);
        }
        /* Initialize UART Configuration parameter structure to default state:
         * Baudrate = 9600bps
         * 8 data bit
         * 1 Stop bit
         * None parity
         */
        UART_ConfigStructInit(&UARTConfigStruct);

        /* Set Baudrate to 115200 */
        UARTConfigStruct.Baud_rate = 115200;

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

        /* Initialize FIFO for UART0 peripheral */
        UART_FIFOConfig(UARTx, &UARTFIFOConfigStruct);
    NVIC_EnableIRQ(UART2_IRQn);

        /*  Enable UART Transmit */
        UART_TxCmd(UARTx, ENABLE);
    LPC_UART2->IER = IER_RBR | IER_THRE | IER_RLS;    /* Enable UART1 interrupt */

   // UART_Send(UARTx, (uint8_t *)"serial ok\n\r", sizeof("serial ok"), BLOCKING);
//    UART_Send(UARTx, (uint8_t *)"hello", sizeof("hello"), BLOCKING);

}

void UART_MODBUS_CONFIG()        //Config_UART3 for Modbus-RTU/Modbus-Tcp/Profibus-DP
{
LPC_UART_TypeDef *UARTx = (LPC_UART_TypeDef *)LPC_UART3;    //Initialize UART3

        /* UART Configuration structure variable */
        UART_CFG_Type UARTConfigStruct;
        /* UART FIFO configuration Struct variable */
        UART_FIFO_CFG_Type UARTFIFOConfigStruct;
        /* Pin configuration for UART0 */
        PINSEL_CFG_Type PinCfg;

        if((uint32_t)UARTx == (uint32_t)LPC_UART0)
                    {
                /*
                 * Initialize UART0 pin connect
                 */
                PinCfg.Funcnum = 1;
                PinCfg.OpenDrain = 0;
                PinCfg.Pinmode = 0;
                PinCfg.Pinnum = 2;
                PinCfg.Portnum = 0;
                PINSEL_ConfigPin(&PinCfg);
                PinCfg.Pinnum = 3;
                PINSEL_ConfigPin(&PinCfg);
        }
        else if ((uint32_t)UARTx == (uint32_t)LPC_UART1)
                    {
                /*
                 * Initialize UART1 pin connect
                 */
                PinCfg.Funcnum = 2;
                PinCfg.OpenDrain = 0;
                PinCfg.Pinmode = 0;
                PinCfg.Pinnum = 0;
                PinCfg.Portnum = 2;
                PINSEL_ConfigPin(&PinCfg);
                PinCfg.Pinnum = 1;
                PINSEL_ConfigPin(&PinCfg);
        }
                else if ((uint32_t)UARTx == (uint32_t)LPC_UART2)
                    {
                /*
                 * Initialize UART1 pin connect
                 */
                PinCfg.Funcnum = 2;
                PinCfg.OpenDrain = 0;
                PinCfg.Pinmode = 0;
                PinCfg.Pinnum = 8;
                PinCfg.Portnum = 2;
                PINSEL_ConfigPin(&PinCfg);
                PinCfg.Pinnum = 9;
                PINSEL_ConfigPin(&PinCfg);
        }
            else if ((uint32_t)UARTx == (uint32_t)LPC_UART3)
                    {
                /*
                 * Initialize UART3 pin connect
                 */
                PinCfg.Funcnum = 3;
                PinCfg.OpenDrain = 0;
                PinCfg.Pinmode = 0;
                PinCfg.Pinnum = 28;
                PinCfg.Portnum = 4;
                PINSEL_ConfigPin(&PinCfg);
                PinCfg.Pinnum = 29;
                PINSEL_ConfigPin(&PinCfg);
          }

        /* Initialize UART Configuration parameter structure to default state:
         * Baudrate = 9600bps
         * 8 data bit
         * 1 Stop bit
         * None parity
         */
        UART_ConfigStructInit(&UARTConfigStruct);

        /* Set Baudrate to 115200 */
      //     UARTConfigStruct.Baud_rate = 9600 ;  // 9600 ;

                UARTConfigStruct.Baud_rate = Modbus_Baud_Rate;
                UARTConfigStruct.Parity    = Modbus_Parity;

                  //UARTConfigStruct.Databits  = UART_DATABIT_8;
                  //UARTConfigStruct.Stopbits  = UART_STOPBIT_1;

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

        /* Initialize FIFO for UART0 peripheral */
        UART_FIFOConfig(UARTx, &UARTFIFOConfigStruct);
                NVIC_EnableIRQ(UART3_IRQn);

        /*  Enable UART Transmit */
        UART_TxCmd(UARTx, ENABLE);
    LPC_UART3->IER = IER_RBR | IER_THRE | IER_RLS;    /* Enable UART0 interrupt */

}

void UART_DIO_MODBUS_CONFIG()        //Config_UART0 for DIO_RS485/Inverter-RS485
{
LPC_UART_TypeDef *UARTx = (LPC_UART_TypeDef *)LPC_UART0;    //Initialize UART0

        /* UART Configuration structure variable */
        UART_CFG_Type UARTConfigStruct;
        /* UART FIFO configuration Struct variable */
        UART_FIFO_CFG_Type UARTFIFOConfigStruct;
        /* Pin configuration for UART0 */
        PINSEL_CFG_Type PinCfg;

        if((uint32_t)UARTx == (uint32_t)LPC_UART0)
                    {
                /*
                 * Initialize UART0 pin connect
                 */
                PinCfg.Funcnum = 1;
                PinCfg.OpenDrain = 0;
                PinCfg.Pinmode = 0;
                PinCfg.Pinnum = 2;
                PinCfg.Portnum = 0;
                PINSEL_ConfigPin(&PinCfg);
                PinCfg.Pinnum = 3;
                PINSEL_ConfigPin(&PinCfg);
        }
        else if ((uint32_t)UARTx == (uint32_t)LPC_UART1)
                    {
                /*
                 * Initialize UART1 pin connect
                 */
                PinCfg.Funcnum = 2;
                PinCfg.OpenDrain = 0;
                PinCfg.Pinmode = 0;
                PinCfg.Pinnum = 0;
                PinCfg.Portnum = 2;
                PINSEL_ConfigPin(&PinCfg);
                PinCfg.Pinnum = 1;
                PINSEL_ConfigPin(&PinCfg);
        }
                else if ((uint32_t)UARTx == (uint32_t)LPC_UART2)
                    {
                /*
                 * Initialize UART1 pin connect
                 */
                PinCfg.Funcnum = 2;
                PinCfg.OpenDrain = 0;
                PinCfg.Pinmode = 0;
                PinCfg.Pinnum = 8;
                PinCfg.Portnum = 2;
                PINSEL_ConfigPin(&PinCfg);
                PinCfg.Pinnum = 9;
                PINSEL_ConfigPin(&PinCfg);
        }
           else if ((uint32_t)UARTx == (uint32_t)LPC_UART3)
                    {
                /*
                 * Initialize UART1 pin connect
                 */
                PinCfg.Funcnum = 3;
                PinCfg.OpenDrain = 0;
                PinCfg.Pinmode = 0;
                PinCfg.Pinnum = 28;
                PinCfg.Portnum = 4;
                PINSEL_ConfigPin(&PinCfg);
                PinCfg.Pinnum = 29;
                PINSEL_ConfigPin(&PinCfg);
          }
        /* Initialize UART Configuration parameter structure to default state:
         * Baudrate = 9600bps
         * 8 data bit
         * 1 Stop bit
         * None parity
         */
        UART_ConfigStructInit(&UARTConfigStruct);

        /* Set Baudrate to 115200 */
        UARTConfigStruct.Baud_rate =19200;// 19200;//9600;//19200;//19200;//115200;//9600 ;  // 9600 ;
              UARTConfigStruct.Parity    = UART_PARITY_NONE;//UART_PARITY_EVEN;//
                //  UARTConfigStruct.Databits  = UART_DATABIT_8;
                //  UARTConfigStruct.Stopbits  = UART_STOPBIT_1;
                ///UARTConfigStruct.Baud_rate = Modbus_Baud_Rate;
                ///UARTConfigStruct.Parity    = Modbus_Parity;
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

        /* Initialize FIFO for UART0 peripheral */
        UART_FIFOConfig(UARTx, &UARTFIFOConfigStruct);
                NVIC_EnableIRQ(UART0_IRQn);

        /*  Enable UART Transmit */
        UART_TxCmd(UARTx, ENABLE);
        LPC_UART0->IER = IER_RBR | IER_THRE | IER_RLS;    /* Enable UART0 interrupt */

}

/*****************************************************************************
** Function name:        UART1_IRQHandler
**
** Descriptions:        UART1 interrupt handler
**
** parameters:            None
** Returned value:        None
**
*****************************************************************************/
void UART1_IRQHandler (void)
{
  uint8_t IIRValue, LSRValue;
  uint8_t Dummy = Dummy;

  IIRValue = LPC_UART1->IIR;

  IIRValue >>= 1;            /* skip pending bit in IIR */
  IIRValue &= 0x07;            /* check bit 1~3, interrupt identification */
  if ( IIRValue == IIR_RLS )        /* Receive Line Status */
  {
    LSRValue = LPC_UART1->LSR;
    /* Receive Line Status */
    if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
    {
      /* There are errors or break interrupt */
      /* Read LSR will clear the interrupt */
      UART1Status = LSRValue;
      Dummy = LPC_UART1->RBR;        /* Dummy read on RX to clear
                                interrupt, then bail out */
      return;
    }
    if ( LSRValue & LSR_RDR )    /* Receive Data Ready */
    {
      /* If no error on RLS, normal ready, save into the data buffer. */
      /* Note: read RBR will clear the interrupt */
      if ( UART1Count < BUFSIZE )
      {
          UART1Buffer[UART1Count] = LPC_UART1->RBR;
          UART1Count++;
      }
      else
      {
          (void)LPC_UART1->RBR;  /* Discard byte, clear interrupt */
          UART1Count = 0;
      }
    }
  }
  else if ( IIRValue == IIR_RDA )    /* Receive Data Available */
  {
    /* Receive Data Available */
    if ( UART1Count < BUFSIZE )
    {
        UART1Buffer[UART1Count] = LPC_UART1->RBR;
//            Print_Variable(UART1Buffer[UART1Count], 2,2,2,0);
//            Print_Char(UART1Buffer[UART1Count] , 1,1);

//        Print_String(UART1Buffer[UART1Count], 2,2,2,0);
        UART1Count++;
    }
    else
    {
        (void)LPC_UART1->RBR;  /* Discard byte, clear interrupt */
        UART1Count = 0;
    }
  }
  else if ( IIRValue == IIR_CTI )    /* Character timeout indicator */
  {
    /* Character Time-out indicator */
    UART1Status |= 0x100;        /* Bit 9 as the CTI error */
  }
  else if ( IIRValue == IIR_THRE )    /* THRE, transmit holding register empty */
  {
    /* THRE interrupt */
    LSRValue = LPC_UART1->LSR;        /* Check status in the LSR to see if
                                valid data in U0THR or not */
    if ( LSRValue & LSR_THRE )
    {
      UART1TxEmpty = 1;
    }
    else
    {
      UART1TxEmpty = 0;
    }
  }

}

/*****************************************************************************
** Function name:        UART2_IRQHandler
**
** Descriptions:        UART2 interrupt handler
**
** parameters:            None
** Returned value:        None
**
*****************************************************************************/
//void UART2_IRQHandler (void)
//{
//  uint8_t IIRValue, LSRValue;
//  uint8_t Dummy = Dummy;
//
//  IIRValue = LPC_UART2->IIR;
//
//  IIRValue >>= 1;            /* skip pending bit in IIR */
//  IIRValue &= 0x07;            /* check bit 1~3, interrupt identification */
//  if ( IIRValue == IIR_RLS )        /* Receive Line Status */
//  {
//    LSRValue = LPC_UART2->LSR;
//    /* Receive Line Status */
//    if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
//    {
//      /* There are errors or break interrupt */
//      /* Read LSR will clear the interrupt */
//      UART1Status = LSRValue;
//      Dummy = LPC_UART2->RBR;        /* Dummy read on RX to clear
//                                interrupt, then bail out */
//      return;
//    }
//    if ( LSRValue & LSR_RDR )    /* Receive Data Ready */
//    {
//      /* If no error on RLS, normal ready, save into the data buffer. */
//      /* Note: read RBR will clear the interrupt */
//      UART1Buffer[UART1Count] = LPC_UART2->RBR;
//      UART1Count++;
//      if ( UART1Count == BUFSIZE )
//      {
//        UART1Count = 0;        /* buffer overflow */
//      }
//    }
//  }
//  else if ( IIRValue == IIR_RDA )    /* Receive Data Available */
//  {
//    /* Receive Data Available */
//    UART1Buffer[UART1Count] = LPC_UART2->RBR;
////            Print_Variable(UART1Buffer[UART1Count], 2,2,2,0);
////            Print_Char(UART1Buffer[UART1Count] , 1,1);

////        Print_String(UART1Buffer[UART1Count], 2,2,2,0);
//
//        UART1Count++;
//    if ( UART1Count == BUFSIZE )
//    {
//      UART1Count = 0;        /* buffer overflow */
//    }
//  }
//  else if ( IIRValue == IIR_CTI )    /* Character timeout indicator */
//  {
//    /* Character Time-out indicator */
//    UART1Status |= 0x100;        /* Bit 9 as the CTI error */
//  }
//  else if ( IIRValue == IIR_THRE )    /* THRE, transmit holding register empty */
//  {
//    /* THRE interrupt */
//    LSRValue = LPC_UART2->LSR;        /* Check status in the LSR to see if
//                                valid data in U0THR or not */
//    if ( LSRValue & LSR_THRE )
//    {
//      UART1TxEmpty = 1;
//    }
//    else
//    {
//      UART1TxEmpty = 0;
//    }
//  }

//}

//void UART2_IRQHandler (void)
//{
//  uint8_t IIRValue, LSRValue;
//  uint8_t Dummy = Dummy;
//    uint16_t    rx_data_;
//
//  IIRValue = LPC_UART2->IIR;
//
//  IIRValue >>= 1;            /* skip pending bit in IIR */
//  IIRValue &= 0x07;            /* check bit 1~3, interrupt identification */
//  if ( IIRValue == IIR_RLS )        /* Receive Line Status */
//  {
//    LSRValue = LPC_UART2->LSR;
//    /* Receive Line Status */
//    if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
//    {
//      /* There are errors or break interrupt */
//      /* Read LSR will clear the interrupt */
//      UART1Status = LSRValue;
//      Dummy = LPC_UART2->RBR;        /* Dummy read on RX to clear
//                                                               interrupt, then bail out */
//      return;
//    }
//    if ( LSRValue & LSR_RDR )    /* Receive Data Ready */
//    {
//      /* If no error on RLS, normal ready, save into the data buffer. */
//      /* Note: read RBR will clear the interrupt */
//      UART1Buffer[UART1Count] = LPC_UART2->RBR;
//      UART1Count++;
//      if ( UART1Count == BUFSIZE )
//      {
//            UART1Count = 0;        /* buffer overflow */
//      }
//    }
//  }
//  else if ( IIRValue == IIR_RDA )    /* Receive Data Available */
//        {
//            rx_data_    =    LPC_UART2->RBR;
//        if((rx_data_ == 0x0D)||(rx_data_ == 0x0A) )
//         {
//                    if( UART1Count > 550 )
//                             UART1Count = 0;        /* buffer overflow */
//                    rx_data_ = 0;
//         }
//        else
//        {
//            if(rx_data_     == '}')
//                {
//                    Dwnd_flag = 1;
//                    par_count = UART1Count ;
//                    rx_data_ = 0;
//                }
//                else
//                    {
//                            download_menu_par[UART1Count] = rx_data_;
//                            UART1Count++;
//                        if( UART1Count > 550 )
//                             UART1Count = 0;        /* buffer overflow */
//                        rx_data_ = 0;
//                    }
//         }
//
//     }
//  else if ( IIRValue == IIR_CTI )    /* Character timeout indicator */
//  {
//    /* Character Time-out indicator */
//    UART1Status |= 0x100;        /* Bit 9 as the CTI error */
//  }
//  else if ( IIRValue == IIR_THRE )    /* THRE, transmit holding register empty */
//  {
//    /* THRE interrupt */
//    LSRValue = LPC_UART2->LSR;        /* Check status in the LSR to see if
//                                valid data in U0THR or not */
//    if ( LSRValue & LSR_THRE )
//    {
//      UART1TxEmpty = 1;
//    }
//    else
//    {
//      UART1TxEmpty = 0;
//    }
//  }

//}

//MODBUS-Rtu/Modbus-Tcp/Profibus-Dp
void UART3_IRQHandler (void) /**/
{
  uint8_t IIRValue, LSRValue;
  uint8_t Dummy = Dummy;

  IIRValue = LPC_UART3->IIR;

  IIRValue >>= 1;            /* skip pending bit in IIR */
  IIRValue &= 0x07;            /* check bit 1~3, interrupt identification */

    if ( IIRValue == IIR_RLS )        /* Receive Line Status */
        {
        LSRValue = LPC_UART3->LSR;
        /* Receive Line Status */
        if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
            {
                /* There are errors or break interrupt */
                /* Read LSR will clear the interrupt */
                UART3Status = LSRValue;
                Dummy = LPC_UART3->RBR;        /* Dummy read on RX to clear
                                        interrupt, then bail out */
                return;
            }
        if ( LSRValue & LSR_RDR )    /* Receive Data Ready */
            {
                /* If no error on RLS, normal ready, save into the data buffer. */
                /* Note: read RBR will clear the interrupt */
                if ( UART3Count < BUFSIZE )
                {
                    UART3Buffer[UART3Count] = LPC_UART3->RBR;
                    UART3Count++;
                }
                else
                {
                    (void)LPC_UART3->RBR;  /* Discard byte, clear interrupt */
                    UART3Count = 0;
                }
            }
        }
    if ( IIRValue == IIR_THRE )    /* Transmit FIFO Empty */
        {

            if(SIO_Interface==1)
                {
                if(ModTxCount<ModTxLen)
                    {
                        UART_SendByte((LPC_UART_TypeDef *)LPC_UART3,ModBusMessage[ModTxCount]);
                        ModTxCount++;
                    }
                else
                    {
                        DIRN_MAX487(0);                         //MAX487 IN RECEIVE MODE
                    }
                }
                else if ((SIO_Interface == 2) || (SIO_Interface == 3)) //msk ABCC TX only if pointers valid 26-may-2026
                    {
                        if ((TX_Data != 0) && (ABCC_RX_TX_Count < ABCC_RX_TX_Length))
                                {
                                 UART_SendByte((LPC_UART_TypeDef *)LPC_UART3,*TX_Data++);
                                 ABCC_RX_TX_Count++;
                                }

                    }
                else
                    {
                        DIRN_MAX487(0); //msk SIO off: no ABCC TX 26-may-2026
                    }
            }

    if ( IIRValue == IIR_RDA )    /* Receive Data Available */
        {

            if(SIO_Interface==1)
                {
                Timer2_init();    //re start timer1
                /* Receive Data Available */
                ModBusIdx++;
                if (ModBusIdx < 30)  // Bounds check to prevent buffer overrun (ModBusMessage is 30 bytes)
                {
                    ModBusMessage[ModBusIdx] = UART_ReceiveByte((LPC_UART_TypeDef *)LPC_UART3);
                }
                else
                {
                    // Buffer full - discard byte but clear UART
                    (void)UART_ReceiveByte((LPC_UART_TypeDef *)LPC_UART3);
                    ModBusIdx = 29;  // Keep at max valid index
                }
                UART3Count++;
                if ( UART3Count == BUFSIZE )
                    {
                        UART3Count = 0;        /* buffer overflow */
                    }
                }
            else if ((SIO_Interface == 2) || (SIO_Interface == 3)) //msk ABCC RX only if RX_Data set 26-may-2026
                {
                    if (RX_Data != 0)
                    {
                        *RX_Data++ = UART_ReceiveByte((LPC_UART_TypeDef *)LPC_UART3);
                        UART3Count++;
                        ABCC_RX_TX_Count_1++;
                        if ( UART3Count == ABCC_RX_TX_Length_1)
                            {
                                ABCC_RX_TX_Status = 2;
                                UART3Count= 0;
                            }
                    }
                    else
                    {
                        (void)UART_ReceiveByte((LPC_UART_TypeDef *)LPC_UART3); //msk discard if ABCC not armed 26-may-2026
                    }
                }
            else
                {
                    (void)UART_ReceiveByte((LPC_UART_TypeDef *)LPC_UART3); //msk SIO off: discard PLC UART3 traffic 26-may-2026
                }
        }

}

/*DIO MODULE */
void UART0_IRQHandler (void)
{
    int i;
  uint8_t IIRValue, LSRValue;
  uint8_t Dummy = Dummy;

  IIRValue = LPC_UART0->IIR;

  IIRValue >>= 1;            /* skip pending bit in IIR */
  IIRValue &= 0x07;            /* check bit 1~3, interrupt identification */

    if ( IIRValue == IIR_RLS )        /* Receive Line Status */
  {
    LSRValue = LPC_UART0->LSR;
    /* Receive Line Status */
    if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
    {
      /* There are errors or break interrupt */
      /* Read LSR will clear the interrupt */
      UART0Status = LSRValue;
      Dummy = LPC_UART0->RBR;        /* Dummy read on RX to clear
                                interrupt, then bail out */
      return;
    }
    if ( LSRValue & LSR_RDR )    /* Receive Data Ready */
    {
      /* If no error on RLS, normal ready, save into the data buffer. */
      /* Note: read RBR will clear the interrupt */
      if ( UART0Count < BUFSIZE )
      {
          UART0Buffer[UART0Count] = LPC_UART0->RBR;
          UART0Count++;
      }
      else
      {
          (void)LPC_UART0->RBR;  /* Discard byte, clear interrupt */
          UART0Count = 0;
      }
    }
  }

    if ( IIRValue == IIR_THRE )    /* Transmit FIFO Empty */
     {
        if (DIO_ModTxCount<DIO_ModTxLen)
            {
             UART_SendByte((LPC_UART_TypeDef *)LPC_UART0,DIO_ModBusMessage[DIO_ModTxCount]);
           DIO_ModTxCount++;
            }
      else
            {
                tx_over = 1;
                for(i=0;i<2000;i++){;}
                DIO_DIRN_MAX487(0);                         //MAX487 IN RECEIVE MODE
            }
      }

    if ( IIRValue == IIR_RDA )    /* Receive Data Available */
        {
                /* Receive Data Available */
                DIO_ModBusIdx++;
                if (DIO_ModBusIdx < 34)  // Bounds check to prevent buffer overrun corrupting other variables
                {
                    DIO_ModBusMessage[DIO_ModBusIdx] = UART_ReceiveByte((LPC_UART_TypeDef *)LPC_UART0);
                }
                else
                {
                    // Buffer full - discard byte but clear UART to prevent interrupt storm
                    (void)UART_ReceiveByte((LPC_UART_TypeDef *)LPC_UART0);
                    DIO_ModBusIdx = 33;  // Keep at max valid index
                }
                UART0Count++;
                    if ( UART0Count == BUFSIZE )
                    {
                        UART0Count = 0;        /* buffer overflow */
                    }
        }

}

/*
23/08/2018

Added if else condition in UART3 ISR for modbus-Tcp and HMS Module

*/
