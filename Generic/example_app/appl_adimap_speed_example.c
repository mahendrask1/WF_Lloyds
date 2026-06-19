///*******************************************************************************
//********************************************************************************
//**                                                                            **
//** ABCC Starter Kit version 3.04.01 (2018-01-23)                              **
//**                                                                            **
//** Delivered with:                                                            **
//**    ABP            7.55.01 (2017-12-20)                                     **
//**    ABCC Driver    5.04.01 (2017-12-18)                                     **
//**                                                                            */
///*******************************************************************************
//********************************************************************************
//** COPYRIGHT NOTIFICATION (c) 2015 HMS Industrial Networks AB                 **
//**                                                                            **
//** This code is the property of HMS Industrial Networks AB.                   **
//** The source code may not be reproduced, distributed, or used without        **
//** permission. When used together with a product from HMS, permission is      **
//** granted to modify, reproduce and distribute the code in binary form        **
//** without any restrictions.                                                  **
//**                                                                            **
//** THE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. HMS DOES NOT    **
//** WARRANT THAT THE FUNCTIONS OF THE CODE WILL MEET YOUR REQUIREMENTS, OR     **
//** THAT THE OPERATION OF THE CODE WILL BE UNINTERRUPTED OR ERROR-FREE, OR     **
//** THAT DEFECTS IN IT CAN BE CORRECTED.                                       **
//********************************************************************************
//********************************************************************************
//** Example of an ADI setup with two simple UINT16 ADIs simulating speed and
//** reference speed.
//**
//** In abcc_drv_cfg.h make sure that the following definitions are set to:
//** ABCC_CFG_STRUCT_DATA_TYPE     ( FALSE )
//** ABCC_CFG_ADI_GET_SET_CALLBACK ( FALSE )
//********************************************************************************
//********************************************************************************
//*/

//#include "appl_adi_config.h"
//#include "abcc.h"
//#include "plc.h"
//#include "modbus.h"

//#if ( APPL_ACTIVE_ADI_SETUP == APPL_ADI_SETUP_SPEED_EXAMPLE )

//#if (  ABCC_CFG_STRUCT_DATA_TYPE || ABCC_CFG_ADI_GET_SET_CALLBACK )
//   #error ABCC_CFG_ADI_GET_SET_CALLBACK must be set to FALSE and ABCC_CFG_STRUCT_DATA_TYPE set to FALSE in order to run this example
//#endif
///*******************************************************************************
//** Constants
//********************************************************************************
//*/

///*------------------------------------------------------------------------------
//** Access descriptor for the ADIs
//**------------------------------------------------------------------------------
//*/
//#define APPL_READ_MAP_READ_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |           \
//                                         ABP_APPD_DESCR_MAPPABLE_READ_PD )

//#define APPL_READ_MAP_WRITE_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |          \
//                                          ABP_APPD_DESCR_SET_ACCESS |          \
//                                          ABP_APPD_DESCR_MAPPABLE_READ_PD )

//#define APPL_WRITE_MAP_READ_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |          \
//                                          ABP_APPD_DESCR_MAPPABLE_WRITE_PD )

//#define APPL_NOT_MAP_READ_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS )

//#define APPL_NOT_MAP_WRITE_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |           \
//                                         ABP_APPD_DESCR_SET_ACCESS )

///*******************************************************************************
//** Typedefs
//********************************************************************************
//*/

///*******************************************************************************
//** Private Globals
//********************************************************************************
//*/
//extern volatile char DIO_modarray_rd[2],modarray_rd[MAX_LENGTH],modarray_wr[MAX_LENGTH],DIO_modarray_wr[2];;

//extern uint8_t ram_values[RAM_SIZE];	
//extern unsigned int Common_Setpoint;
//extern unsigned int Actual_Feedrate;

//extern unsigned char Display_Sign,Deviation_Sign;
//extern unsigned int External_Setpoint,
//										Common_Setpoint,
//										Current_Setpoint,
//										Deviation,
//										PF_Drive_Setpoint,
//										WF_Drive_Setpoint_per,
//										Belt_Speed,
//										PF_Drive_Setpoint_Per,
//										Hopper_val,
//										Gate_Postion,
//										HOpper_total,
//										FDR_Total,
//										CM_Difference;

//extern double Totaliser1,Totaliser2;
//extern unsigned int Totaliser1_Display,Totaliser2_Display;
//extern unsigned int Common_Setpoint;
//extern signed int Belt_Load;
//extern float WF_Drive_Setpoint,CM_Display;


///*------------------------------------------------------------------------------
//** Data holder for the ADI instances
//**------------------------------------------------------------------------------
//*/
//extern unsigned short  
//			M0_15_Read,
//			M16_31_Read,
//			M32_39_Read;

//extern unsigned short  
//			I0_15_Write,
//			I16_31_Write,
//			I32_47_Write,
//			O0_15_Write,
//			O16_31_Write,
//			M0_15_Write,
//			M16_31_Write,
//			M32_47_Write,
//			M48_63_Write,
//			M64_79_Write,
//			M80_95_Write,
//			Deviation_Comm,
//			Belt_Load_Comm;



//static UINT16 appl_iSpeed;
//static UINT16 appl_iRefSpeed;

///*------------------------------------------------------------------------------
//** Min, max and default value for appl_aiUint16
//**------------------------------------------------------------------------------
//*/
////static AD_UINT16Type appl_sUint16Prop = { { 0, 0xFFFF, 0 } };

///*******************************************************************************
//** Public Globals
//********************************************************************************
//*/

///*-------------------------------------------------------------------------------------------------------------
//** 1. iInstance | 2. pabName | 3. bDataType | 4. bNumOfElements | 5. bDesc | 6. pxValuePtr | 7. pxValuePropPtr
//**--------------------------------------------------------------------------------------------------------------
//*/
//const AD_AdiEntryType APPL_asAdiEntryList[] =
//{
///*1010*/	 	 {   1,  "Common_Setpoint", 			ABP_UINT16,   1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &Common_Setpoint, NULL} } },
///*1011*/	 	 {   2,  "M0...M15", 							ABP_UINT16,   1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &M0_15_Read, NULL} } },
///*1012*/	 	 {   3,  "M16...M31", 						ABP_UINT16,   1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &M16_31_Read, NULL} } },
///*1013*/	 	 {   4,  "M32...M39", 						ABP_UINT16,   1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &M32_39_Read, NULL} } },
//	 
///*1018*/	 	 {   8,  "I0...I15",     					ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &I0_15_Write,    NULL } } },
///*1019*/	   {   9,  "I16...I31",     				ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &I16_31_Write,    NULL } } },
///*101a*/	 	 {  10,  "I32...I39",     				ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &I32_47_Write,    NULL } } },
///*101b*/	 	 {  11,  "O0...O15",     					ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &O0_15_Write,    NULL } } },
///*101c*/	 	 {  12,  "O16...O31",     				ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &O16_31_Write,    NULL } } },
///*101d*/	 	 {  13,  "M0...M15",     					ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &M0_15_Write,    NULL } } },
///*101e*/	 	 {  14,  "M16...M31",     				ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &M16_31_Write,    NULL } } },
///*101f*/	 	 {  15,  "M32...M47",     				ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &M32_47_Write,    NULL } } },
///*1020*/	 	 {  16,  "M48...M63",     				ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &M48_63_Write,    NULL } } },
///*1021*/	 	 {  17,  "M64...M79",     				ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &M64_79_Write,    NULL } } },
///*1022*/	 	 {  18,  "M80...M95",     				ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &M80_95_Write,    NULL } } },
///*1023*/	 	 {  19,  "Actual Feedrate",     	ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &Actual_Feedrate,    NULL} } },
///*1024*/	 	 {  20,  "Totaliser 1",     			ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &(((uint16_t*)&(Totaliser1_Display))[1]),    NULL } } },
///*1025*/	 	 {  21,  "Totaliser 1",     			ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &(((uint16_t*)&(Totaliser1_Display))[0]),    NULL } } },
///*1026*/	 	 {  22,  "Totaliser 2",     			ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &(((uint16_t*)&(Totaliser2_Display))[1]),    NULL } } },
///*1027*/	 	 {  23,  "Totaliser 2",     			ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &(((uint16_t*)&(Totaliser2_Display))[0]),    NULL } } },
///*1028*/	 	 {  24,  "Deviation",     				ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &Deviation_Comm,    NULL } } },

///*1029*/	   {  25,  "Current_Setpoint",     	ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &Current_Setpoint,    NULL } } },
///*102a*/	 	 {  26,  "Belt_Load",     				ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &Belt_Load_Comm,    NULL } } },
///*102b*/	 	 {  27,  "Belt_Speed",     				ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &Belt_Speed,    NULL } } },
///*102c*/	 	 {  28,  "WF_Drive_Setpoint_per", ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &WF_Drive_Setpoint_per,    NULL } } },
///*102d*/	 	 {  29,  "PF_Drive_Setpoint_Per", ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &PF_Drive_Setpoint_Per,    NULL } } },
///*102e*/	 	 {  30,  "Hopper_val",     				ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &(((uint16_t*)&(Hopper_val))[1]),    NULL } } },
///*102f*/	 	 {  31,  "Hopper_val",     				ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &(((uint16_t*)&(Hopper_val))[0]),    NULL } } },
///*1030*/	 	 {  32,  "Gate_Postion",     			ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &Gate_Postion,    NULL } } },
///*1031*/	 	 {  33,  "HOpper_total",     			ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &(((uint16_t*)&(HOpper_total))[1]),    NULL } } },
///*1032*/	 	 {  34,  "HOpper_total",     			ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &(((uint16_t*)&(HOpper_total))[0]),    NULL } } },
///*1033*/	 	 {  35,  "FDR_Total",     				ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &(((uint16_t*)&(FDR_Total))[1]),    NULL } } },
///*1034*/	 	 {  36,  "FDR_Total",     				ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &(((uint16_t*)&(FDR_Total))[0]),    NULL } } },
///*1035*/	 	 {  37,  "CM_Difference",     		ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &CM_Difference,    NULL } } },
///*1036*/	 	 {  38,  "CM_Display",     				ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &CM_Display,    NULL } } },
//	 
//	 
//	 	 
//	 
//   
//};

///*------------------------------------------------------------------------------
//** Map all adi:s in both directions
//**------------------------------------------------------------------------------
//** 1. AD instance | 2. Direction | 3. Num elements | 4. Start index |
//**------------------------------------------------------------------------------
//*/
//const AD_DefaultMapType APPL_asAdObjDefaultMap[] =
//{
//      { 1, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 2, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 3, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 4, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			
//			{  8, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{  9, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 10, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 11, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 12, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 13, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 14, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 15, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 16, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 17, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 18, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 19, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 20, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 21, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 22, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 23, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 24, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 25, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 26, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 27, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 28, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 29, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 30, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 31, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 32, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 33, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 34, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 35, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 36, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 37, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 38, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			
//   { AD_DEFAULT_MAP_END_ENTRY }
//};

///*******************************************************************************
//** Private Services
//********************************************************************************
//*/

///*******************************************************************************
//** Public Services
//********************************************************************************
//*/

//UINT16 APPL_GetNumAdi( void )
//{
//   return( sizeof( APPL_asAdiEntryList ) / sizeof( AD_AdiEntryType ) );
//}

//void APPL_CyclicalProcessing( void )
//{
//   if( ABCC_AnbState() == ABP_ANB_STATE_PROCESS_ACTIVE )
//   {
//      /*
//      ** An example of ADI data handling.
//      */
//      if( appl_iSpeed > appl_iRefSpeed )
//      {
//         /*
//         ** Do something that lowers speed.
//         */
//         appl_iSpeed -= 1;
//      }
//      else if( appl_iSpeed < appl_iRefSpeed )
//      {
//         /*
//         ** Do something that increases speed.
//         */
//         appl_iSpeed += 1;
//      }
//   }
//   else
//   {
//      /*
//      ** We are not in process active, the default should be that the motor
//      ** should not run.
//      */
//      appl_iSpeed = 0;
//   }
//}

///*******************************************************************************
//** Tasks
//********************************************************************************
//*/

//#endif

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
** Example of an ADI setup with two simple UINT16 ADIs simulating speed and
** reference speed.
**
** In abcc_drv_cfg.h make sure that the following definitions are set to:
** ABCC_CFG_STRUCT_DATA_TYPE     ( FALSE )
** ABCC_CFG_ADI_GET_SET_CALLBACK ( FALSE )
********************************************************************************
********************************************************************************
*/


#include "appl_adi_config.h"
#include "abcc.h"
#include "modbus.h"
#include "plc.h"
typedef unsigned          char uint8_t;

#if ( APPL_ACTIVE_ADI_SETUP == APPL_ADI_SETUP_SPEED_EXAMPLE )

#if (  ABCC_CFG_STRUCT_DATA_TYPE || ABCC_CFG_ADI_GET_SET_CALLBACK )
   #error ABCC_CFG_ADI_GET_SET_CALLBACK must be set to FALSE and ABCC_CFG_STRUCT_DATA_TYPE set to FALSE in order to run this example
#endif
/*******************************************************************************
** Constants
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Access descriptor for the ADIs
**------------------------------------------------------------------------------
*/
#define APPL_READ_MAP_READ_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |           \
                                         ABP_APPD_DESCR_MAPPABLE_READ_PD )

#define APPL_READ_MAP_WRITE_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |          \
                                          ABP_APPD_DESCR_SET_ACCESS |          \
                                          ABP_APPD_DESCR_MAPPABLE_READ_PD )

#define APPL_WRITE_MAP_READ_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |          \
                                          ABP_APPD_DESCR_MAPPABLE_WRITE_PD )

#define APPL_NOT_MAP_READ_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS )

#define APPL_NOT_MAP_WRITE_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |           \
                                         ABP_APPD_DESCR_SET_ACCESS )

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*******************************************************************************
** Private Globals
********************************************************************************
*/
extern volatile char DIO_modarray_rd[2],modarray_rd[MAX_LENGTH],modarray_wr[MAX_LENGTH],DIO_modarray_wr[2];;

extern uint8_t ram_values[RAM_SIZE];	
extern unsigned int Common_Setpoint;
extern unsigned int Actual_Feedrate;

extern unsigned char Display_Sign,Deviation_Sign;
extern unsigned int External_Setpoint,
										Common_Setpoint,
										Current_Setpoint,
										Deviation,
										PF_Drive_Setpoint,
										WF_Drive_Setpoint_per,
										Belt_Speed,
										PF_Drive_Setpoint_Per,
										Hopper_val,
										Gate_Postion,
										HOpper_total,
										FDR_Total,
										CM_Difference;

extern double Totaliser1,Totaliser2;
extern unsigned int Totaliser1_Display,Totaliser2_Display;
extern unsigned int Common_Setpoint;
extern signed int Belt_Load;
extern float WF_Drive_Setpoint,CM_Display;


/*------------------------------------------------------------------------------
** Data holder for the ADI instances
**------------------------------------------------------------------------------
*/
extern unsigned short  
			M0_15_Read,
			M16_31_Read,
			M32_39_Read;



static UINT16 appl_iSpeed;
static UINT16 appl_iRefSpeed;


unsigned short  ALL_Data[35];
/*------------------------------------------------------------------------------
** Min, max and default value for appl_aiUint16
**------------------------------------------------------------------------------
*/
//static AD_UINT16Type appl_sUint16Prop = { { 0, 0xFFFF, 0 } };

/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*-------------------------------------------------------------------------------------------------------------
** 1. iInstance | 2. pabName | 3. bDataType | 4. bNumOfElements | 5. bDesc | 6. pxValuePtr | 7. pxValuePropPtr
**--------------------------------------------------------------------------------------------------------------
*/
const AD_AdiEntryType APPL_asAdiEntryList[] =
{
/*1011*/	 	 {   1,  "Common_Setpoint", 			ABP_UINT16,   1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &Common_Setpoint, NULL} } },
///*1031*/	 	 {   2,  "M0...M15", 							ABP_UINT16,   1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &M0_15_Read, NULL} } },
///*1051*/	 	 {   3,  "M16...M31", 						ABP_UINT16,   1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &M16_31_Read, NULL} } },
///*1071*/	 	 {   4,  "M32...M39", 						ABP_UINT16,   1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &M32_39_Read, NULL} } },
	 
/*1111*/	 	 {   9,  "Write_Data",     					ABP_UINT16,   32, APPL_WRITE_MAP_READ_ACCESS_DESC, { { ALL_Data,    NULL } } },
	 
	 
	 	 
	 
   
};

/*------------------------------------------------------------------------------
** Map all adi:s in both directions
**------------------------------------------------------------------------------
** 1. AD instance | 2. Direction | 3. Num elements | 4. Start index |
**------------------------------------------------------------------------------
*/
const AD_DefaultMapType APPL_asAdObjDefaultMap[] =
{
      { 1, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 2, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 3, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 4, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
			
			{  9, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{  9, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 10, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 11, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 12, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 13, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 14, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 15, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 16, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 17, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 18, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 19, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 20, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 21, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 22, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 23, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 24, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 25, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 26, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 27, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 28, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 29, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 30, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 31, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 32, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 33, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 34, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 35, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 36, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 37, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
//			{ 38, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
			
   { AD_DEFAULT_MAP_END_ENTRY }
};

/*******************************************************************************
** Private Services
********************************************************************************
*/

/*******************************************************************************
** Public Services
********************************************************************************
*/

UINT16 APPL_GetNumAdi( void )
{
   return( sizeof( APPL_asAdiEntryList ) / sizeof( AD_AdiEntryType ) );
}

void APPL_CyclicalProcessing( void )
{
   if( ABCC_AnbState() == ABP_ANB_STATE_PROCESS_ACTIVE )
   {
      /*
      ** An example of ADI data handling.
      */
      if( appl_iSpeed > appl_iRefSpeed )
      {
         /*
         ** Do something that lowers speed.
         */
         appl_iSpeed -= 1;
      }
      else if( appl_iSpeed < appl_iRefSpeed )
      {
         /*
         ** Do something that increases speed.
         */
         appl_iSpeed += 1;
      }
   }
   else
   {
      /*
      ** We are not in process active, the default should be that the motor
      ** should not run.
      */
      appl_iSpeed = 0;
   }
}

/*******************************************************************************
** Tasks
********************************************************************************
*/

#endif

