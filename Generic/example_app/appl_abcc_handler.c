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
** This is an example implementation of the application abcc handler.
** It includes the following section:
** 1. ADI definition and default mappings
** 2. ABCC driver callback implementations (process data, default mapping ...)
** 3. State machine implementation for message sequencing, e.g. during user init
**    or reading exception state
** 4. ABCC handler main function to be called from the main loop, including a main
**    state machine for handling startup and restart of the ABCC.
********************************************************************************
********************************************************************************
*/

#include <stdio.h>

#include "abcc_td.h"
#include "abp.h"
#include "abcc.h"
#include "abcc_cmd_seq_if.h"
#include "ad_obj.h"
#include "abcc_port.h"

#include "ad_obj.h"         /* Application data object:   254                 */
#include "app_obj.h"        /* Application object:        255                 */
#include "appl_abcc_handler.h"
#include "abcc_port.h"
#include "etn_obj.h"
#include "opcua_obj.h"
#include "sync_obj.h"
#include "safe_obj.h"

#include "eip.h"
#include "prt.h"
#include "epl.h"
#include "dpv1.h"
#include "ect.h"
#include "dev.h"
#include "mod.h"
#include "cop.h"
#include "ccl.h"
#include "cfn.h"

#include "abcc_obj_cfg.h"
#include "appl_adi_config.h"

/*parameters.h*/
#ifndef PARAMETERS_H
#define PARAMETERS_H

#define Main_Menu_Max 15
#define Menu_String_Max_Size 20
#define Sel_par_size 10
#define Sel_string_size 20

/****************************** New_Parameters_For_Weigh_Feeder ********************************************/
#define	Display_Filter										*(New_menu_parameters+	0	)
#define	Prefeeder													*(New_menu_parameters+	1	)
#define	Units_Capacity										*(New_menu_parameters+	2	)
#define	Dec_Point_Capacity								*(New_menu_parameters+	3	)
#define	Capacity													*(New_menu_parameters+	4	) 	//WF_Capcity
#define	Dec_Pt_Totaliser1									*(New_menu_parameters+	5	)
#define	Units_Totaliser1									*(New_menu_parameters+	6	)
#define	Dec_Pt_Totaliser2									*(New_menu_parameters+	7	)
#define	Units_Totaliser2									*(New_menu_parameters+	8	)
#define	Tacho_Freq												*(New_menu_parameters+	9	) 	//Nominal Tacho Frequency
#define	Max_Load													*(New_menu_parameters+	10	)
#define	Min_Load													*(New_menu_parameters+	11	)
#define	Password													*(New_menu_parameters+	12	)
#define	Rev_Time													*(New_menu_parameters+	13	)
#define	Zero_Limit												*(New_menu_parameters+	14	)
#define	Belt_Track												*(New_menu_parameters+	15	)
#define	Run_Time													*(New_menu_parameters+	16	)
#define	Pulse1														*(New_menu_parameters+	17	)
#define	Pulse_Width												*(New_menu_parameters+	18	)
#define	Pulse2														*(New_menu_parameters+	19	)
#define	AO1_Value													*(New_menu_parameters+	20	)
#define	AO2_Value													*(New_menu_parameters+	21	)
#define	Load_Limit1												*(New_menu_parameters+	22	)
#define	Load_Limit2												*(New_menu_parameters+	23	)
#define	Load_Delay												*(New_menu_parameters+	24	)
#define	Clear_Totaliser2									*(New_menu_parameters+	25	)
#define	Load_Filter 											*(New_menu_parameters+	26	)
#define	Mode_Select												*(New_menu_parameters+	27	)
#define	DIO_Module												*(New_menu_parameters+	28	)
#define	SIO_Interface											*(New_menu_parameters+	29	)
#define	SIO_Baud_Rate											*(New_menu_parameters+	30	)
#define	SIO_Setup													*(New_menu_parameters+	31	)
#define	SIO_Address												*(New_menu_parameters+	32	)
#define	SIO_Monitor												*(New_menu_parameters+	33	)
#define	Int_Time													*(New_menu_parameters+	34	)
#define	Prop_Gain													*(New_menu_parameters+	35	)
#define	Dev_Limit													*(New_menu_parameters+	36	)
#define	Dev_Time													*(New_menu_parameters+	37	)
#define	Dead_Band													*(New_menu_parameters+	38	)
#define	Setpoint_Int											*(New_menu_parameters+	39	)
#define	Setpoint_Man											*(New_menu_parameters+	40	)
/*																													Date	41
																														Month	42
																														Year	43
																														Hour	44
																														Min	  45
																														Sec	  46*/
#define	Pre_Hopper												*(New_menu_parameters+	47	)
#define	PI						  									*(New_menu_parameters+	48	)
#define	BL_Limit													*(New_menu_parameters+	49	)
#define	PF_Delay													*(New_menu_parameters+	50	)
#define	PF_StartSet												*(New_menu_parameters+	51	)
/*																			Capacity hopper high word 52 */
#define	BL_Band											      *(New_menu_parameters+	53	)
#define	PF_Limit													*(New_menu_parameters+	54	)
#define	Regulation										    *(New_menu_parameters+	55	)
#define	Reg_Tracking											*(New_menu_parameters+	56	)
#define Fixed_Frequency                   *(New_menu_parameters+	57	) 
#define	Hopper_Capacity										*(New_menu_parameters+	60	)
#define	No_Rev  													*(New_menu_parameters+	61	)
#define	AD_Conversion_Rate								*(New_menu_parameters+	66	)
#define	Filter_Order											*(New_menu_parameters+	67	)
#define	Cut_Off_Freq											*(New_menu_parameters+	68	)
#define LC_Filter                         *(New_menu_parameters+	69	)
#define Inv		                          	*(New_menu_parameters+	70	)
#define Inv_1		                          *(New_menu_parameters+	71	)
#define	AO3_Value													*(New_menu_parameters+	72	)
#define	AI_Nos														*(New_menu_parameters+	73	)
#define	AO_Nos														*(New_menu_parameters+	74	)
#define	Filter__Order											*(New_menu_parameters+	75	)
#define	Cut__Off_Freq		  								*(New_menu_parameters+	76	)
#define	LC__Filter												*(New_menu_parameters+	77	)
#define	Limit_Low											    *(New_menu_parameters+	78	)
#define	Limit_Nominal										  *(New_menu_parameters+	79	)
#define	Limit_Max  												*(New_menu_parameters+	80	)
#define	Limit_Delay        								*(New_menu_parameters+	81	)
#define	Min_Pulse  												*(New_menu_parameters+	82	)
#define	Correction_Factor									*(New_menu_parameters+	83	)
/*																			 Capacity hopper low word 84 */
#define	IP_Addr1											    *(New_menu_parameters+	85	)//added on 23/05/2018
#define	IP_Addr2												  *(New_menu_parameters+	86	)
#define	IP_Addr3												  *(New_menu_parameters+	87	)
#define	IP_Addr4												  *(New_menu_parameters+	88	)
#define	Profibus_Node_Addr								*(New_menu_parameters+	89	)
#define	Profibus_Baudrate									*(New_menu_parameters+	90	)
#define	Tacho_ip													*(New_menu_parameters+	91	) //added on 28/05/2018
/*
#define AI_calval													*(New_menu_parameters+	92	)	//6 values

#define AO_calz1													*(New_menu_parameters+	98	)	//after 6 int16 values
#define AO_cals1													*(New_menu_parameters+	99	)	//after 6 int16 values

#define AO_calz2													*(New_menu_parameters+	100	)	//after 6 int16 values
#define AO_cals2													*(New_menu_parameters+	101	)	//after 6 int16 values

#define AO_calz3													*(New_menu_parameters+	102	)	//after 6 int16 values
#define AO_cals3													*(New_menu_parameters+	103	)	//after 6 int16 values
*/
#define nextval														*(New_menu_parameters+	104)	//after 6 int16 values	

//uint16_t  AI_calb_val[6],AO_val[6];

#define Positive 0
#define Negative 1
#define Yes 0
#define No 	1
#define Mode0 0
#define Mode1	1
#define OFF 0
#define ON 1

typedef struct  {
	char mm_type; //mm for main menu
	int mm_no;
	char *mm_name[Menu_String_Max_Size];
	
}main_menu_struct;

typedef struct  {
	char mm_type; //mm for main menu
	int mm_no;
	char *mm_name[Menu_String_Max_Size];
	
}main_menu_2_struct;

typedef struct  {
	char par_type;
	int par_options;
	int variable_offset;

}sub_menu_struct;

#endif

/*
28/05/2018
Added Tacho ip parameter (if tacho got damaged then set Tacho ip = Inv so Inv output Freq is used as speed for calcculation )

23/05/2018
#define	IP_Addr1											    *(New_menu_parameters+	85	)
#define	IP_Addr2												  *(New_menu_parameters+	86	)
#define	IP_Addr3												  *(New_menu_parameters+	87	)
#define	IP_Addr4												  *(New_menu_parameters+	88	)

#define	Profibus_Node_Addr								*(New_menu_parameters+	89	)
#define	Profibus_Baudrate									*(New_menu_parameters+	90	)
*/

/*******************************************************************************
** Constants
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Maximum start up time when the module is upgrading its firmware
**------------------------------------------------------------------------------
*/
#define APPL_FW_UPGRADE_STARTUP_TIME_MS     ( 2 * 60 * (UINT32)1000 )

/*------------------------------------------------------------------------------
** Default IP configuration when using HW switches
**------------------------------------------------------------------------------
*/
#ifndef APPL_DEFAULT_IP_NETWORK_ADDRESS
#define APPL_DEFAULT_IP_NETWORK_ADDRESS         { 192, 168, 0, 0 }
#endif

#ifndef APPL_DEFAULT_NETMASK
#define APPL_DEFAULT_NETMASK                    { 255, 255, 255, 0 }
#endif

#ifndef APPL_DEFAULT_GATEWAY
#define APPL_DEFAULT_GATEWAY                    { 0, 0, 0, 0 }
#endif

#ifndef APPL_DEFAULT_DHCP_ENABLE
#define APPL_DEFAULT_DHCP_ENABLE                { FALSE }
#endif

/*******************************************************************************
** Typedefs
********************************************************************************
*/

extern UINT16 New_menu_parameters[91],OLD_IP_adrs1,OLD_IP_adrs2,OLD_IP_adrs3,OLD_IP_adrs4,OLD_Profibus_adrs,OLD_Profibus_Baud; ;

/*------------------------------------------------------------------------------
** The basic IPv4 settings.
**------------------------------------------------------------------------------
*/
typedef struct
{
   UINT16   iInstance;
   union
   {
      UINT8 abValue[ 4 ];
      BOOL  fValue;
   }
   uValue;
}
appl_IpElementType;

typedef struct
{
   appl_IpElementType  sAddress;
   appl_IpElementType  sNetmask;
   appl_IpElementType  sGateway;
   appl_IpElementType  sDhcp;
}
appl_IpSettingsType;

/*------------------------------------------------------------------------------
** ABCC Handler states
**------------------------------------------------------------------------------
*/
typedef enum appl_AbccHandlerState
{
   APPL_INIT,
   APPL_WAITCOM,
   APPL_RUN,
   APPL_SHUTDOWN,
   APPL_ABCCRESET,
   APPL_DEVRESET,
   APPL_HALT
}
appl_AbccHandlerStateType;

/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*******************************************************************************
** Private Globals
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Current state of the ABCC
**------------------------------------------------------------------------------
*/
static appl_AbccHandlerStateType appl_eAbccHandlerState = APPL_INIT;

/*------------------------------------------------------------------------------
** Current anybus state
**------------------------------------------------------------------------------
*/
static volatile ABP_AnbStateType appl_eAnbState = ABP_ANB_STATE_SETUP;

/*------------------------------------------------------------------------------
** Network Node address.
**------------------------------------------------------------------------------
*/
static UINT8 appl_bNwNodeAddress;

/*------------------------------------------------------------------------------
** Network IP address settings
**------------------------------------------------------------------------------
*/
static appl_IpSettingsType appl_sIpSettings =
{
   { 3, { APPL_DEFAULT_IP_NETWORK_ADDRESS } },
   { 4, { APPL_DEFAULT_NETMASK } },
   { 5, { APPL_DEFAULT_GATEWAY } },
   { 6, { APPL_DEFAULT_DHCP_ENABLE } }
};

/*------------------------------------------------------------------------------
** Common address variables (used for both node address and IP address depending
** on network type).
**------------------------------------------------------------------------------
*/
static BOOL appl_fSetAddr = FALSE;
static BOOL appl_fSetAddrInProgress = FALSE;

/*------------------------------------------------------------------------------
** Network baud rate.
**------------------------------------------------------------------------------
*/
static UINT8 appl_bNwBaudRate;
static BOOL  appl_fSetBaudRate = FALSE;
static BOOL  appl_fSetBaudRateInProgress = FALSE;

/*------------------------------------------------------------------------------
** Flags to keep track of if the network type supports node ID/IP address
** and/or baud rate to be set.
**------------------------------------------------------------------------------
*/
static BOOL appl_fNwSupportsNodeId;
static BOOL appl_fNwSupportsBaudRate;

/*------------------------------------------------------------------------------
** Set to TRUE when an unexpected error occur. The main state machine will
** return APPL_MODULE_UNEXPECTED_ERROR when this flag is set.
**------------------------------------------------------------------------------
*/
static BOOL appl_fUnexpectedError = FALSE;

/*------------------------------------------------------------------------------
** Set to TRUE the user init sequence is done.
**------------------------------------------------------------------------------
*/
static BOOL appl_fUserInitDone = FALSE;

/*------------------------------------------------------------------------------
** Event flags used by application to invoke the corresponding
** ABCC_Trigger<event_action> function from the desired context. The flag will
** be set to TRUE in ABCC_CbfEvent().
**------------------------------------------------------------------------------
*/
static volatile BOOL appl_fMsgReceivedEvent = FALSE;
static volatile BOOL appl_fRdPdReceivedEvent = FALSE;
static volatile BOOL appl_fTransmitMsgEvent = FALSE;
static volatile BOOL appl_fAbccStatusEvent = FALSE;

/*------------------------------------------------------------------------------
** Forward declarations
**------------------------------------------------------------------------------
*/

static ABCC_CmdSeqRespStatusType HandleExceptionResp( ABP_MsgType* psMsg );
static ABCC_CmdSeqRespStatusType HandleExceptionInfoResp( ABP_MsgType* psMsg );
static ABCC_CmdSeqCmdStatusType ReadExeption( ABP_MsgType* psMsg );
static ABCC_CmdSeqCmdStatusType ReadExeptionInfo( ABP_MsgType* psMsg );
static ABCC_CmdSeqCmdStatusType UpdateNodeAddress( ABP_MsgType* psMsg );
static ABCC_CmdSeqCmdStatusType UpdateIpAddress( ABP_MsgType* psMsg );
static ABCC_CmdSeqCmdStatusType UpdateNetmask( ABP_MsgType* psMsg );
static ABCC_CmdSeqCmdStatusType UpdateGateway( ABP_MsgType* psMsg );
static ABCC_CmdSeqCmdStatusType UpdateDhcp( ABP_MsgType* psMsg );
static void UpdateAddressDone( void );
static ABCC_CmdSeqCmdStatusType UpdateBaudRate( ABP_MsgType* psMsg );
static void UpdateBaudRateDone( void );

/*------------------------------------------------------------------------------
** User init sequence. See abcc_cmd_seq_if.h
**------------------------------------------------------------------------------
*/
static const ABCC_CmdSeqType appl_asUserInitCmdSeq[] =
{
   ABCC_CMD_SEQ( UpdateIpAddress, NULL ),
   ABCC_CMD_SEQ( UpdateNetmask, NULL ),
   ABCC_CMD_SEQ( UpdateGateway, NULL ),
   ABCC_CMD_SEQ( UpdateDhcp, NULL ),
   ABCC_CMD_SEQ( UpdateNodeAddress, NULL ),
   ABCC_CMD_SEQ( UpdateBaudRate, NULL ),
   ABCC_CMD_SEQ_END()
};

/*------------------------------------------------------------------------------
** Set IP or node address.
** Sequence triggered when the address switch has changed value.
** See abcc_cmd_seq_if.h.
**------------------------------------------------------------------------------
*/
static const ABCC_CmdSeqType appl_asAddressChangedCmdSeq[] =
{
   ABCC_CMD_SEQ( UpdateIpAddress, NULL ),
   ABCC_CMD_SEQ( UpdateNodeAddress, NULL ),
   ABCC_CMD_SEQ_END()
};

/*------------------------------------------------------------------------------
** Set baud rate.
** Sequence triggered when the baud rate switch has changed value.
** See abcc_cmd_seq_if.h.
**------------------------------------------------------------------------------
*/
static const ABCC_CmdSeqType appl_asBaudRateChangedCmdSeq[] =
{
   ABCC_CMD_SEQ( UpdateBaudRate, NULL ),
   ABCC_CMD_SEQ_END()
};

/*------------------------------------------------------------------------------
** Read exception info.
** Sequence triggered when the anybus state changes to exception state.
** See abcc_cmd_seq_if.h.
**------------------------------------------------------------------------------
*/
static const ABCC_CmdSeqType appl_asReadExeptionCmdSeq[] =
{
   ABCC_CMD_SEQ( ReadExeption,     HandleExceptionResp ),
   ABCC_CMD_SEQ( ReadExeptionInfo, HandleExceptionInfoResp ),
   ABCC_CMD_SEQ_END()
};

/*******************************************************************************
** Private Services
********************************************************************************
*/

/*------------------------------------------------------------------------------
**  Builds the command for reading the exception code (get attribute single).
**
**  This function is a part of a command sequence. See description of
**  ABCC_CmdSeqCmdHandler type in cmd_seq_if.h
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqCmdStatusType ReadExeption( ABP_MsgType* psMsg )
{
   ABCC_GetAttribute( psMsg, ABP_OBJ_NUM_ANB, 1, ABP_ANB_IA_EXCEPTION, ABCC_GetNewSourceId() );
   return ( ABCC_SEND_COMMAND );
}

/*------------------------------------------------------------------------------
**  Builds the command for reading the exception info code (get attribute
**  single).
**
**  This function is a part of a command sequence. See description of
**  ABCC_CmdSeqCmdHandler type in cmd_seq_if.h
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqCmdStatusType ReadExeptionInfo( ABP_MsgType* psMsg )
{
   ABCC_GetAttribute( psMsg, ABP_OBJ_NUM_NW, 1, ABP_NW_IA_EXCEPTION_INFO, ABCC_GetNewSourceId() );
   return ( ABCC_SEND_COMMAND );
}

/*------------------------------------------------------------------------------
**  Handles the exception code response (get attribute single).
**
**  This function is a part of a command sequence. See description of
**  ABCC_CmdSeqRespHandler type in cmd_seq_if.h
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqRespStatusType HandleExceptionResp( ABP_MsgType* psMsg )
{
   UINT8 bException;

   if( ABCC_VerifyMessage( psMsg ) != ABCC_EC_NO_ERROR )
   {
      APPL_UnexpectedError();
      return( ABCC_EXEC_NEXT_COMMAND );
   }

   ABCC_GetMsgData8( psMsg, &bException, 0 );
   ABCC_PORT_DebugPrint( ( "Exception Code:    %X:\n\n", bException ) );

   (void)bException;
   return ( ABCC_EXEC_NEXT_COMMAND );
}

/*------------------------------------------------------------------------------
**  Handles the exception info code response (get attribute single).
**
**  This function is a part of a command sequence. See description of
**  ABCC_CmdSeqRespHandler type in cmd_seq_if.h
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqRespStatusType HandleExceptionInfoResp( ABP_MsgType* psMsg )
{
   UINT8 bExceptionInfo;

   if( ABCC_VerifyMessage( psMsg ) != ABCC_EC_NO_ERROR )
   {
      APPL_UnexpectedError();
      return( ABCC_EXEC_NEXT_COMMAND );
   }

   ABCC_GetMsgData8( psMsg, &bExceptionInfo, 0 );
   ABCC_PORT_DebugPrint( ( "Exception Info:    %X:\n\n", bExceptionInfo ) );

   (void)bExceptionInfo;
   return( ABCC_EXEC_NEXT_COMMAND );
}

/*------------------------------------------------------------------------------
**  Notification that the address is updated.
**
**  This function is a part of a command sequence. See description of
**  ABCC_CmdSeqDoneHandler type in cmd_seq_if.h
**------------------------------------------------------------------------------
*/
static void UpdateAddressDone( void )
{
   appl_fSetAddrInProgress = FALSE;
}

/*------------------------------------------------------------------------------
**  Builds the command for setting the ip-address (set attribute).
**
**  This function is a part of a command sequence. See description of
**  ABCC_CmdSeqCmdHandler type in cmd_seq_if.h
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqCmdStatusType UpdateIpAddress( ABP_MsgType* psMsg )
{
   if( ( !appl_fNwSupportsNodeId ) &&
       ( appl_fSetAddr ) )
   {
      ABCC_SetMsgHeader( psMsg,
                         ABP_OBJ_NUM_NC,
                         appl_sIpSettings.sAddress.iInstance,
                         ABP_NC_VAR_IA_VALUE,
                         ABP_CMD_SET_ATTR,
                         4,
                         ABCC_GetNewSourceId() );

      ABCC_SetMsgString( psMsg, (char*)appl_sIpSettings.sAddress.uValue.abValue, 4, 0 );

      return( ABCC_SEND_COMMAND );
   }

   return( ABCC_SKIP_COMMAND );
}

/*------------------------------------------------------------------------------
**  Builds the command for setting the netmask (set attribute).
**
**  This function is a part of a command sequence. See description of
**  ABCC_CmdSeqCmdHandler type in cmd_seq_if.h
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqCmdStatusType UpdateNetmask( ABP_MsgType* psMsg )
{
   if( ( !appl_fNwSupportsNodeId ) &&
       ( appl_fSetAddr ) )
   {
      ABCC_SetMsgHeader( psMsg,
                         ABP_OBJ_NUM_NC,
                         appl_sIpSettings.sNetmask.iInstance,
                         ABP_NC_VAR_IA_VALUE,
                         ABP_CMD_SET_ATTR,
                         4,
                         ABCC_GetNewSourceId() );

      ABCC_SetMsgString( psMsg, (char*)appl_sIpSettings.sNetmask.uValue.abValue, 4, 0 );

      return( ABCC_SEND_COMMAND );
   }

   return( ABCC_SKIP_COMMAND );
}

/*------------------------------------------------------------------------------
**  Builds the command for setting the gateway (set attribute).
**
**  This function is a part of a command sequence. See description of
**  ABCC_CmdSeqCmdHandler type in cmd_seq_if.h
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqCmdStatusType UpdateGateway( ABP_MsgType* psMsg )
{
   if( ( !appl_fNwSupportsNodeId ) &&
       ( appl_fSetAddr ) )
   {
      ABCC_SetMsgHeader( psMsg,
                         ABP_OBJ_NUM_NC,
                         appl_sIpSettings.sGateway.iInstance,
                         ABP_NC_VAR_IA_VALUE,
                         ABP_CMD_SET_ATTR,
                         4,
                         ABCC_GetNewSourceId() );

      ABCC_SetMsgString( psMsg, (char*)appl_sIpSettings.sGateway.uValue.abValue, 4, 0 );

      return( ABCC_SEND_COMMAND );
   }

   return( ABCC_SKIP_COMMAND );
}

/*------------------------------------------------------------------------------
**  Builds the command for enable/disable dhcp (set attribute).
**
**  This function is a part of a command sequence. See description of
**  ABCC_CmdSeqCmdHandler type in cmd_seq_if.h
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqCmdStatusType UpdateDhcp( ABP_MsgType* psMsg )
{
   if ((!appl_fNwSupportsNodeId) &&
      (appl_fSetAddr))
   {
      ABCC_SetByteAttribute( psMsg, ABP_OBJ_NUM_NC,
                             appl_sIpSettings.sDhcp.iInstance,
                             ABP_NC_VAR_IA_VALUE,
                             appl_sIpSettings.sDhcp.uValue.fValue,
                             ABCC_GetNewSourceId() );

      return( ABCC_SEND_COMMAND );
   }
   return( ABCC_SKIP_COMMAND );
}
/*------------------------------------------------------------------------------
**  Builds the command for setting the node-address (set attribute).
**
**  This function is a part of a command sequence. See description of
**  ABCC_CmdSeqCmdHandler type in cmd_seq_if.h
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqCmdStatusType UpdateNodeAddress( ABP_MsgType* psMsg )
{
   if( ( appl_fNwSupportsNodeId ) &&
            ( appl_fSetAddr ) )
   {
      ABCC_SetByteAttribute( psMsg, ABP_OBJ_NUM_NC,
                             ABP_NC_INST_NUM_SW1,
                             ABP_NC_VAR_IA_VALUE,
                             appl_bNwNodeAddress,
                             ABCC_GetNewSourceId() );

      return( ABCC_SEND_COMMAND );
   }

   return( ABCC_SKIP_COMMAND );
}

/*------------------------------------------------------------------------------
**  Notification that the baud rate is updated.
**
**  This function is a part of a command sequence. See description of
**  ABCC_CmdSeqDoneHandler type in cmd_seq_if.h
**------------------------------------------------------------------------------
*/
static void UpdateBaudRateDone( void )
{
   appl_fSetBaudRateInProgress = FALSE;
}

/*------------------------------------------------------------------------------
**  Builds the command for setting the baud rate (set attribute).
**
**  This function is a part of a command sequence. See description of
**  ABCC_CmdSeqCmdHandler type in cmd_seq_if.h
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqCmdStatusType UpdateBaudRate( ABP_MsgType* psMsg )
{
   if( ( appl_fNwSupportsBaudRate ) &&
       ( appl_fSetBaudRate ) )
   {
      ABCC_SetByteAttribute( psMsg, ABP_OBJ_NUM_NC,
                             ABP_NC_INST_NUM_SW2,
                             ABP_NC_VAR_IA_VALUE,
                             appl_bNwBaudRate,
                             ABCC_GetNewSourceId() );

      return( ABCC_SEND_COMMAND );
   }
   return( ABCC_SKIP_COMMAND );
}


/*------------------------------------------------------------------------------
**  Notification that the user init sequence is done.
**
**  This function is a part of a command sequence. See description of
**  ABCC_CmdSeqDoneHandler type in cmd_seq_if.h
**------------------------------------------------------------------------------
*/
static void UserInitDone( void )
{
   appl_fUserInitDone = TRUE;
   ABCC_UserInitComplete();
}

/*******************************************************************************
** Public Services
********************************************************************************
*/

APPL_AbccHandlerStatusType APPL_HandleAbcc( void )
{
   static APPL_AbccHandlerStatusType eModuleStatus = APPL_MODULE_NO_ERROR;
   UINT32 lStartupTimeMs;
   ABCC_CommunicationStateType eAbccComState;

   switch( appl_eAbccHandlerState )
   {
   case APPL_INIT:

      eModuleStatus = APPL_MODULE_NO_ERROR;
      appl_fMsgReceivedEvent = FALSE;
      appl_fRdPdReceivedEvent = FALSE;
      appl_fTransmitMsgEvent = FALSE;
      appl_fAbccStatusEvent = FALSE;
      appl_fUserInitDone = FALSE;

      if( !ABCC_ModuleDetect() )
      {
         eModuleStatus = APPL_MODULE_NOT_DETECTED;
      }

      if( eModuleStatus == APPL_MODULE_NO_ERROR )
      {
         /*
         ** Init application data object
         */
         if( AD_Init( APPL_asAdiEntryList,
                      APPL_GetNumAdi(),
                      APPL_asAdObjDefaultMap ) != APPL_NO_ERROR )
         {
            eModuleStatus = APPL_MODULE_UNEXPECTED_ERROR ;
         }
      }

      if( eModuleStatus == APPL_MODULE_NO_ERROR )
      {
#if APP_OBJ_ENABLE
         if( APP_GetCandidateFwAvailable() == TRUE )
         {
            lStartupTimeMs = APPL_FW_UPGRADE_STARTUP_TIME_MS;
         }
         else
#endif
         {
            /*
            ** Default time will be used
            */
            lStartupTimeMs = 0;
         }

         if( ABCC_StartDriver( lStartupTimeMs ) == ABCC_EC_NO_ERROR )
         {
            ABCC_HWReleaseReset();
            appl_eAbccHandlerState = APPL_WAITCOM;
         }
         else
         {
            eModuleStatus = APPL_MODULE_NOT_ANSWERING;
         }
      }

      if( eModuleStatus != APPL_MODULE_NO_ERROR )
      {
         appl_eAbccHandlerState = APPL_HALT;
      }

      break;

   case APPL_WAITCOM:

      eAbccComState = ABCC_isReadyForCommunication();

      if( eAbccComState == ABCC_READY_FOR_COMMUNICATION )
      {
         appl_eAbccHandlerState = APPL_RUN;
      }
      else if( eAbccComState == ABCC_COMMUNICATION_ERROR )
      {
         appl_eAbccHandlerState = APPL_HALT;
         eModuleStatus = APPL_MODULE_NOT_ANSWERING;
      }
      break;

   case APPL_RUN:
      /*------------------------------------------------------------------------
      ** Handle events indicated in ABCC_CbfEvent()callback function.
      ** Note that these events could be handled from any chosen context but in
      ** in this application it is done from main loop context.
      **------------------------------------------------------------------------
      */
      if( appl_fRdPdReceivedEvent )
      {
         appl_fRdPdReceivedEvent = FALSE;
         ABCC_TriggerRdPdUpdate();
      }

      if( appl_fMsgReceivedEvent )
      {
         appl_fMsgReceivedEvent = FALSE;
         ABCC_TriggerReceiveMessage();
      }

      if( appl_fTransmitMsgEvent )
      {
         appl_fTransmitMsgEvent = FALSE;
         ABCC_TriggerTransmitMessage();
      }

      if( appl_fAbccStatusEvent )
      {
         appl_fAbccStatusEvent = FALSE;
         ABCC_TriggerAnbStatusUpdate();
      }
      /*
      ** End event handling.
      */

#if SYNC_OBJ_ENABLE
      if( SYNC_GetMode() == SYNC_MODE_NONSYNCHRONOUS )
      {
         ABCC_TriggerWrPdUpdate();
      }
#else
      /*
      ** Always update write process data
      */
      ABCC_TriggerWrPdUpdate();
#endif

      ABCC_RunDriver();

      APPL_CyclicalProcessing();

      break;

   case APPL_SHUTDOWN:

      ABCC_HWReset();
      eModuleStatus = APPL_MODULE_SHUTDOWN;
      appl_eAbccHandlerState = APPL_HALT;

      break;

   case APPL_ABCCRESET:

      ABCC_HWReset();
      appl_eAbccHandlerState = APPL_INIT;
      eModuleStatus = APPL_MODULE_NO_ERROR;
      break;

   case APPL_DEVRESET:

      ABCC_HWReset();
      eModuleStatus = APPL_MODULE_RESET;
      appl_eAbccHandlerState = APPL_HALT;

      break;

   case APPL_HALT:

      break;

   default:

      break;
   }

   if( appl_fUnexpectedError )
   {
      return( APPL_MODULE_UNEXPECTED_ERROR );
   }

   return( eModuleStatus );
}

void APPL_SetAddress( UINT8 bSwitchValue )
{
   appl_fSetAddr = TRUE;

   if( appl_fSetAddrInProgress )
   {
      /*
      ** Address updated next time
      */
      return;
   }

   if( appl_fUserInitDone == FALSE )
   {
      /*
      ** HW switch 1 will the last octet in the IP address
      ** for applicable networks ( 192.168.0.X )
		 appl_sIpSettings.sAddress.uValue.abValue[0] = 192
		 appl_sIpSettings.sAddress.uValue.abValue[1] = 168
		 appl_sIpSettings.sAddress.uValue.abValue[2] = 0
		 appl_sIpSettings.sAddress.uValue.abValue[3] = X
      */
		 
		 if(SIO_Interface==2)
		 {
      appl_sIpSettings.sAddress.uValue.abValue[ 0 ] = IP_Addr1;/*Changed here added [1] = 101 as to change 192.168.0.X */
			appl_sIpSettings.sAddress.uValue.abValue[ 1 ] = IP_Addr2;/*Changed here added [2] = 1 as to change 192.168.X.101 */
			appl_sIpSettings.sAddress.uValue.abValue[ 2 ] = IP_Addr3;/*Changed here added [1] = 101 as to change 192.168.0.X */
			appl_sIpSettings.sAddress.uValue.abValue[ 3 ] = IP_Addr4;/*Changed here added [2] = 1 as to change 192.168.X.101 */
			OLD_IP_adrs1=IP_Addr1;
			OLD_IP_adrs2=IP_Addr2;
			OLD_IP_adrs3=IP_Addr3;
			OLD_IP_adrs4=IP_Addr4;

		 }
      /*
      ** Switch 1 is node address for applicable networks
      */
		 if(SIO_Interface==3)
		 {
			 appl_bNwNodeAddress = Profibus_Node_Addr;
			 OLD_Profibus_adrs = Profibus_Node_Addr;
		 }

      /*
      ** Indicate to application object that the address is set by HW switches
      */
   #if APP_OBJ_ENABLE
      APP_HwConfAddress( TRUE );
   #endif
   }
   else if( appl_bNwNodeAddress != bSwitchValue )
   {
     /*
     ** Start command sequence to update address if the value has changed.
     */
		 if(SIO_Interface==2)
		 {
			appl_sIpSettings.sAddress.uValue.abValue[ 0 ] = IP_Addr1;/*Changed here added [0] */
			appl_sIpSettings.sAddress.uValue.abValue[ 1 ] = IP_Addr2;/*Changed here added [1] */
			appl_sIpSettings.sAddress.uValue.abValue[ 2 ] = IP_Addr3;/*Changed here added [2] */
			appl_sIpSettings.sAddress.uValue.abValue[ 3 ] = IP_Addr4;/*Changed here added [3] */
			OLD_IP_adrs1=IP_Addr1;
			OLD_IP_adrs2=IP_Addr2;
			OLD_IP_adrs3=IP_Addr3;
			OLD_IP_adrs4=IP_Addr4;
 
		 }
		 if(SIO_Interface==3)
		 {
			 appl_bNwNodeAddress = Profibus_Node_Addr;
			 OLD_Profibus_adrs = Profibus_Node_Addr;	
		 }
		 
		 //appl_sIpSettings.sAddress.uValue.abValue[ 3 ] = bSwitchValue;
     //appl_bNwNodeAddress = bSwitchValue;
     appl_fSetAddrInProgress = TRUE;
     ABCC_AddCmdSeq( appl_asAddressChangedCmdSeq, UpdateAddressDone );
		 appl_fSetAddrInProgress = FALSE;/*Added by vishal on 18/08/2018 to change address*/
		 
   }
}

void APPL_SetBaudrate( UINT8 bSwitchValue )
{
   if( appl_fSetBaudRateInProgress )
   {
      /*
      ** Baud rate updated next time
      */
      return;
   }

   if( appl_fUserInitDone == FALSE )
   {
      appl_bNwBaudRate = bSwitchValue;
			OLD_Profibus_Baud = Profibus_Baudrate;
   }
   else if( appl_bNwBaudRate != bSwitchValue)
   {
      /*
      ** Start command sequence to update baud rate if the value has changed.
      */
      appl_bNwBaudRate = bSwitchValue;
      ABCC_AddCmdSeq( appl_asBaudRateChangedCmdSeq, UpdateBaudRateDone );
			OLD_Profibus_Baud = Profibus_Baudrate;
   }
   appl_fSetBaudRate = TRUE;
}

void APPL_UnexpectedError( void )
{
   appl_fUnexpectedError = TRUE;
}

void APPL_RestartAbcc( void )
{
   appl_eAbccHandlerState = APPL_ABCCRESET;
}

void APPL_Shutdown( void )
{
   appl_eAbccHandlerState = APPL_SHUTDOWN;
}


void APPL_Reset( void )
{
   appl_eAbccHandlerState = APPL_DEVRESET;
}

UINT16  ABCC_CbfAdiMappingReq( const AD_AdiEntryType**  const ppsAdiEntry,
                               const AD_DefaultMapType** const ppsDefaultMap )
{
   return AD_AdiMappingReq( ppsAdiEntry, ppsDefaultMap );
}

BOOL ABCC_CbfUpdateWriteProcessData( void* pxWritePd )
{

   /*
   ** AD_UpdatePdWriteData is a general function that updates all ADI:s according
   ** to current map.
   ** If the ADI mapping is fixed there is potential for doing that in a more
   ** optimized way, for example by using memcpy.
   */
   return( AD_UpdatePdWriteData( pxWritePd ) );
}

#if( ABCC_CFG_REMAP_SUPPORT_ENABLED )
void ABCC_CbfRemapDone( void )
{
   AD_RemapDone();
}
#endif

void ABCC_CbfNewReadPd( void* pxReadPd )
{
   /*
   ** AD_UpdatePdReadData is a general function that updates all ADI:s according
   ** to current map.
   ** If the ADI mapping is fixed there is potential for doing that in a more
   ** optimized way, for example by using memcpy.
   */
   AD_UpdatePdReadData( pxReadPd );
}

void ABCC_CbfDriverError( ABCC_SeverityType eSeverity, ABCC_ErrorCodeType iErrorCode, UINT32 lAddInfo )
{
   switch( eSeverity )
   {
      case ABCC_SEV_FATAL:
      case ABCC_SEV_WARNING:
         APPL_UnexpectedError();
         break;
      default:
         break;
   }

   (void)iErrorCode;
   (void)lAddInfo;
}

void ABCC_CbfReceiveMsg( ABP_MsgType* psReceivedMsg )
{
   switch(  ABCC_GetMsgDestObj( psReceivedMsg ) )
   {
#if OPCUA_OBJ_ENABLE
   case ABP_OBJ_NUM_OPCUA:

      OPCUA_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if SAFE_OBJ_ENABLE
   case ABP_OBJ_NUM_SAFE:

      SAFE_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if EPL_OBJ_ENABLE
   case ABP_OBJ_NUM_EPL:

      EPL_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if EIP_OBJ_ENABLE
   case ABP_OBJ_NUM_EIP:

      EIP_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if PRT_OBJ_ENABLE
   case ABP_OBJ_NUM_PNIO:

      PRT_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if DPV1_OBJ_ENABLE
   case ABP_OBJ_NUM_DPV1:

      DPV1_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if DEV_OBJ_ENABLE
   case ABP_OBJ_NUM_DEV:

      DEV_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if MOD_OBJ_ENABLE
   case ABP_OBJ_NUM_MOD:

      MOD_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if COP_OBJ_ENABLE
   case ABP_OBJ_NUM_COP:

      COP_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if ETN_OBJ_ENABLE
   case ABP_OBJ_NUM_ETN:

      ETN_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if ECT_OBJ_ENABLE
   case ABP_OBJ_NUM_ECT:

      ECT_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
   case ABP_OBJ_NUM_APPD:

      AD_ProcObjectRequest( psReceivedMsg );
      break;

#if APP_OBJ_ENABLE
   case ABP_OBJ_NUM_APP:

      APP_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if SYNC_OBJ_ENABLE
   case ABP_OBJ_NUM_SYNC:

      SYNC_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if CCL_OBJ_ENABLE
   case ABP_OBJ_NUM_CCL:

      CCL_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
#if CFN_OBJ_ENABLE
   case ABP_OBJ_NUM_CFN:

      CFN_ProcessCmdMsg( psReceivedMsg );
      break;
#endif
   default:

      /*
      ** We have received a command to an unsupported object.
      */
      ABP_SetMsgErrorResponse( psReceivedMsg, 1, ABP_ERR_UNSUP_OBJ );
      ABCC_SendRespMsg( psReceivedMsg );
      break;
   }
}

void ABCC_CbfWdTimeout( void )
{
   ABCC_PORT_DebugPrint( ( "ABCC watchdog timeout" ) );
}

void ABCC_CbfWdTimeoutRecovered( void )
{
   ABCC_PORT_DebugPrint( ( "ABCC watchdog recovered" ) );
}

#if ABCC_CFG_SYNC_ENABLE
void ABCC_CbfSyncIsr( void )
{
   /*
   ** Call application specific handling of sync event
   */
   APPL_SyncIsr();
}
#endif

void ABCC_CbfEvent( UINT16 iEvents )
{

   /*
   ** Set flag to indicate that an event has occurred and the corresponding
   ** ABCC_Trigger<event_action> must be called. In the sample code the the
   ** trigger function is called from main loop context.
   */
   if( iEvents & ABCC_ISR_EVENT_RDPD )
   {
      appl_fRdPdReceivedEvent = TRUE;
   }

   if( iEvents & ABCC_ISR_EVENT_RDMSG )
   {
      appl_fMsgReceivedEvent = TRUE;
   }

   if( iEvents & ABCC_ISR_EVENT_WRMSG )
   {
      appl_fTransmitMsgEvent = TRUE;
   }

   if( iEvents & ABCC_ISR_EVENT_STATUS  )
   {
      appl_fAbccStatusEvent = TRUE;
   }
}

void ABCC_CbfAnbStateChanged( ABP_AnbStateType eNewAnbState )
{
   static const char* AnbStateString[ 8 ] = { "ABP_ANB_STATE_SETUP",
                                              "ABP_ANB_STATE_NW_INIT",
                                              "ABP_ANB_STATE_WAIT_PROCESS",
                                              "ABP_ANB_STATE_IDLE",
                                              "ABP_ANB_STATE_PROCESS_ACTIVE",
                                              "ABP_ANB_STATE_ERROR",
                                              "",
                                              "ABP_ANB_STATE_EXCEPTION" };
   (void)AnbStateString[ 0 ];

   appl_eAnbState = eNewAnbState;
   ABCC_PORT_DebugPrint( ( "ANB_STATUS: %s \n" ,
                           AnbStateString[ appl_eAnbState ] ) );

   switch( appl_eAnbState )
   {
   case ABP_ANB_STATE_PROCESS_ACTIVE:

      ABCC_TriggerWrPdUpdate();
      break;

   case ABP_ANB_STATE_EXCEPTION:

      /* Trigger message sequence for reading exception data */
      ABCC_AddCmdSeq( appl_asReadExeptionCmdSeq, NULL );
      break;

   case ABP_ANB_STATE_ERROR:

      /* Trigger message sequence for reading exception data */
      break;

   default:

      break;
   }
}

void ABCC_CbfUserInitReq( void )
{
   UINT16 iModuleType;
   UINT16 iNetworkType;

   iModuleType = ABCC_ModuleType();
   iNetworkType = ABCC_NetworkType();

   if( ( iNetworkType == ABP_NW_TYPE_DEV ) ||
       ( iNetworkType == ABP_NW_TYPE_PDPV0 ) ||
       ( iNetworkType == ABP_NW_TYPE_PDPV1 ) ||
       ( iNetworkType == ABP_NW_TYPE_COP ) ||
       ( iNetworkType == ABP_NW_TYPE_CNT ) ||
       ( iNetworkType == ABP_NW_TYPE_CCL ) ||
       ( iNetworkType == ABP_NW_TYPE_CFN ) ||
       ( iNetworkType == ABP_NW_TYPE_CPN ) ||
       ( iNetworkType == ABP_NW_TYPE_ECT ) ||
       ( iNetworkType == ABP_NW_TYPE_EPL ) )
   {
      appl_fNwSupportsNodeId = TRUE;
   }
   else
   {
      appl_fNwSupportsNodeId = FALSE;
   }

   if( ( iNetworkType == ABP_NW_TYPE_DEV ) ||
       ( iNetworkType == ABP_NW_TYPE_COP ) ||
       ( iNetworkType == ABP_NW_TYPE_CCL ) )
   {
      appl_fNwSupportsBaudRate = TRUE;
   }
   else
   {
      appl_fNwSupportsBaudRate = FALSE;
   }

   if( ( iNetworkType == ABP_NW_TYPE_SRC3 ) && ( iModuleType == ABP_MODULE_TYPE_ABCC ) )
   {
      appl_sIpSettings.sAddress.iInstance = 4;
      appl_sIpSettings.sNetmask.iInstance = 5;
      appl_sIpSettings.sGateway.iInstance = 6;
      appl_sIpSettings.sDhcp.iInstance = 7;
   }
   else if( ( iNetworkType == ABP_NW_TYPE_BIP ) && ( iModuleType == ABP_MODULE_TYPE_ABCC ) )
   {
      appl_sIpSettings.sAddress.iInstance = 6;
      appl_sIpSettings.sNetmask.iInstance = 7;
      appl_sIpSettings.sGateway.iInstance = 8;
      appl_sIpSettings.sDhcp.iInstance = 9;
   }
   else
   {
      /*
      ** Default vales from initialization (3,4,5,6)
      */
   }
   /*
   ** Start user init command sequence
   */
   ABCC_AddCmdSeq( appl_asUserInitCmdSeq, UserInitDone );

}
