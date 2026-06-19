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
** Main program
********************************************************************************
********************************************************************************
*/

#include "abcc_td.h"
#include "abcc.h"
#include "abcc_sys_adapt.h"
#include "ad_obj.h"
#include "appl_abcc_handler.h"
#include "modbus.h"

// Keep prototype explicit to avoid implicit-function warnings.
void Modbus_Monitoring_Time(void);


#define APPL_TIMER_MS         1
#define USE_TIMER_INTERRUPT   1

//#if( USE_TIMER_INTERRUPT )
//static void TimerIsr( void )
//{
//   ABCC_RunTimerSystem( APPL_TIMER_MS );
//}

//static void SetupTimerInterrupt( void )
//{
//}
//#else
//static void DelayMs( UINT32 lDelayMs )
//{
//}
//#endif

static void Reset( void )
{
	APPL_RestartAbcc();
}
APPL_AbccHandlerStatusType eAbccHandlerStatus = APPL_MODULE_NO_ERROR;

int ABCC_Init()
{
   /*
   ** Make sure the ABCC reset signal is kept low by the platform specific
   ** initialization to keep the ABCC module in reset until the driver releases
   ** it.
   */
//   APPL_AbccHandlerStatusType eAbccHandlerStatus = APPL_MODULE_NO_ERROR;
//add Reset low 1/6/2018 //
   if( ABCC_HwInit() != ABCC_EC_NO_ERROR )
		{
      return( 0 );
		}

//#if( USE_TIMER_INTERRUPT )
//   SetupTimerInterrupt();
//#endif
	return 1;
}

int ABCC_main()
{
	
	
  if( eAbccHandlerStatus == APPL_MODULE_NO_ERROR )
   {
      eAbccHandlerStatus = APPL_HandleAbcc();

//#if( !USE_TIMER_INTERRUPT )
//      ABCC_RunTimerSystem( APPL_TIMER_MS );
//      DelayMs( APPL_TIMER_MS );
//#endif
		 
      switch( eAbccHandlerStatus )
      {
      case APPL_MODULE_RESET:
         Reset();
         break;
			case APPL_MODULE_UNEXPECTED_ERROR  :
					Modbus_Monitoring_Time(); // To check time out period of Modbus comm failure	
				break;
      default:
         break;
      }
   }

   return( 0 );
}
/*
29/09/2018
Added
Modbus_Monitoring_Time(); // To check time out period of Modbus comm failure


*/
