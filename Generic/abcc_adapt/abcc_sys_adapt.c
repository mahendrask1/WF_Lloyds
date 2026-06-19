/*******************************************************************************
********************************************************************************
**                                                                            **
** ABCC Starter Kit version 3.04.01 (2018-01-23)                              **
**                                                                            **
** Delivered with:                                                            **
**    ABP            7.55.01 (2017-12-20)                                     **
**    ABCC Driver    5.04.01 (2017-12-18)                                     **
**                                                                            */
/*******************************************************************************
********************************************************************************
** COPYRIGHT NOTIFICATION (c) 2015 HMS Industrial Networks AB                 **
**                                                                            **
** This code is the property of HMS Industrial Networks AB.                   **
** The source code may not be reproduced, distributed, or used without        **
** permission. When used together with a product from HMS, permission is      **
** granted to modify, reproduce and distribute the code in binary form        **
** without any restrictions.                                                  **
**                                                                            **
** THE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. HMS DOES NOT    **
** WARRANT THAT THE FUNCTIONS OF THE CODE WILL MEET YOUR REQUIREMENTS, OR     **
** THAT THE OPERATION OF THE CODE WILL BE UNINTERRUPTED OR ERROR-FREE, OR     **
** THAT DEFECTS IN IT CAN BE CORRECTED.                                       **
********************************************************************************
********************************************************************************
*/

#include "LPC17xx.h"
#include "GPIO_LPC17xx.h"
#include "lpc17xx_uart.h"

#include "abcc_drv_cfg.h"
#include "abcc_port.h"

#include "abcc_sys_adapt.h"
#include "abcc_sys_adapt_spi.h"
#include "abcc_sys_adapt_par.h"
#include "abcc_sys_adapt_ser.h"

extern unsigned char ABCC_RX_TX_Status;
extern unsigned int ABCC_RX_TX_Count,ABCC_RX_TX_Length,ABCC_RX_TX_Length_1,ABCC_RX_TX_Count_1;
extern unsigned char *TX_Data,*RX_Data;


BOOL ABCC_SYS_HwInit( void )
{
   /*
   ** Implement according to abcc_sys_adapt.h
   */
	
	return TRUE ;
}


BOOL ABCC_SYS_Init( void )
{
   /*
   ** Implement according to abcc_sys_adapt.h
   */
	return TRUE;
}


void ABCC_SYS_Close( void )
{
   /*
   ** Implement according to abcc_sys_adapt.h
   */
}


#if( ABCC_CFG_OP_MODE_SETTABLE )
void ABCC_SYS_SetOpmode( UINT8 bOpmode )
{
   /*
   ** Implement according to abcc_sys_adapt.h
   */
}
#endif


#if( ABCC_CFG_OP_MODE_GETTABLE )
UINT8 ABCC_SYS_GetOpmode( void )
{
   /*
   ** Implement according to abcc_sys_adapt.h
   */
}
#endif


void ABCC_SYS_HWReset( void )
{
   /*
   ** Implement according to abcc_sys_adapt.h
   */
	GPIO_PinWrite (1,1, 0);	//DIRN1
	
}


void ABCC_SYS_HWReleaseReset( void )
{
   /*
   ** Implement according to abcc_sys_adapt.h
   */
	GPIO_PinWrite (1,1, 1);//DIRN1 	
}


#ifndef ABCC_CFG_ABCC_MODULE_ID
UINT8 ABCC_SYS_ReadModuleId( void )
{
   /*
   ** Implement according to abcc_sys_adapt.h
   */
}
#endif


#if( ABCC_CFG_MOD_DETECT_PINS_CONN )
BOOL ABCC_SYS_ModuleDetect( void )
{
   /*
   ** Implement according to abcc_sys_adapt.h
   */
}
#endif

#if( ABCC_CFG_SYNC_ENABLE && ABCC_CFG_USE_ABCC_SYNC_SIGNAL )
void ABCC_SYS_SyncInterruptEnable( void )
{
   /*
   ** Implement according to abcc_sys_adapt.h
   */
}
#endif

#if( ABCC_CFG_SYNC_ENABLE && ABCC_CFG_USE_ABCC_SYNC_SIGNAL )
void ABCC_SYS_SyncInterruptDisable( void )
{
   /*
   ** Implement according to abcc_sys_adapt.h
   */
}
#endif


#if( ABCC_CFG_INT_ENABLED )
void ABCC_SYS_AbccInterruptEnable( void )
{
   /*
   ** Implement according to abcc_sys_adapt.h
   */
}


void ABCC_SYS_AbccInterruptDisable( void )
{
   /*
   ** Implement according to abcc_sys_adapt.h
   */
}
#endif


#if( ABCC_CFG_POLL_ABCC_IRQ_PIN )
BOOL ABCC_SYS_IsAbccInterruptActive( void )
{
   /*
   ** Implement according to abcc_sys_adapt.h
   */
}
#endif


#if( ABCC_CFG_DRV_SPI )
void ABCC_SYS_SpiRegDataReceived( ABCC_SYS_SpiDataReceivedCbfType pnDataReceived  )
{
   /*
   ** Implement according to abcc_sys_adapt_spi.h
   */
}


void ABCC_SYS_SpiSendReceive( void* pxSendDataBuffer, void* pxReceiveDataBuffer, UINT16 iLength )
{
   /*
   ** Implement according to abcc_sys_adapt_spi.h
   */
}
#endif


#if( ( ABCC_CFG_DRV_PARALLEL || ABCC_CFG_DRV_PARALLEL_30 ) && !ABCC_CFG_MEMORY_MAPPED_ACCESS )
void ABCC_SYS_ParallelRead( UINT16 iMemOffset, void* pxData, UINT16 iLength )
{
   /*
   ** Implement according to abcc_sys_adapt_par.h
   */
}


#if( ABCC_CFG_DRV_PARALLEL_30 )
UINT8 ABCC_SYS_ParallelRead8( UINT16 iMemOffset )
{
   /*
   ** Implement according to abcc_sys_adapt_par.h
   */
}
#endif


#if( ABCC_CFG_DRV_PARALLEL )
UINT16 ABCC_SYS_ParallelRead16( UINT16 iMemOffset )
{
   /*
   ** Implement according to abcc_sys_adapt_par.h
   */
}
#endif


void ABCC_SYS_ParallelWrite( UINT16 iMemOffset, void* pxData, UINT16 iLength )
{
   /*
   ** Implement according to abcc_sys_adapt_par.h
   */
}


#if( ABCC_CFG_DRV_PARALLEL_30 )
void ABCC_SYS_ParallelWrite8( UINT16 iMemOffset, UINT8 pbData )
{
   /*
   ** Implement according to abcc_sys_adapt_par.h
   */
}
#endif


#if( ABCC_CFG_DRV_PARALLEL )
void ABCC_SYS_ParallelWrite16( UINT16 iMemOffset, UINT16 piData )
{
   /*
   ** Implement according to abcc_sys_adapt_par.h
   */
}
#endif

void* ABCC_SYS_ParallelGetRdPdBuffer( void )
{
   /*
   ** Implement according to abcc_sys_adapt_par.h
   */
}


void* ABCC_SYS_ParallelGetWrPdBuffer( void )
{
   /*
   ** Implement according to abcc_sys_adapt_par.h
   */
}
#endif
#if( ABCC_CFG_DRV_SERIAL )
/*
** Callback for Serial data ready indication.
*/
static ABCC_SYS_SerDataReceivedCbfType pnSerDataReadyCbf;
#endif

#if( ABCC_CFG_DRV_SERIAL )
void ABCC_SYS_SerRegDataReceived( ABCC_SYS_SerDataReceivedCbfType pnDataReceived  )
{
   /*
   ** Implement according to abcc_sys_adapt_ser.h
   */
	pnSerDataReadyCbf = pnDataReceived;
}



void ABCC_SYS_SerSendReceive( void* pxTxDataBuffer, void* pxRxDataBuffer, UINT16 iTxSize, UINT16 iRxSize )
{
	
//	if(UART_Send((LPC_UART_TypeDef *)LPC_UART0,pxTxDataBuffer,iTxSize,BLOCKING ) == 0)
//	{
//		return;
//	}
	
//if(UART_Receive((LPC_UART_TypeDef *)LPC_UART0,pxRxDataBuffer,iRxSize,BLOCKING ) == 0)
//	{
//		
//		return;
//	}
//	
	/*
   ** Implement according to abcc_sys_adapt_ser.h
   */
	if( pnSerDataReadyCbf )
   {
		 
      pnSerDataReadyCbf();
			ABCC_RX_TX_Status = 0;
   }
}


void ABCC_SYS_SerRestart( void )
{
   /*
   ** Implement according to abcc_sys_adapt_ser.h
   */
}
#endif
