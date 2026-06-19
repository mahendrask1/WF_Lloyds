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
** Example of an ADI setup with an array of 16 bit values
**
** In abcc_drv_cfg.h make sure that the following definitions are set to:
** ABCC_CFG_STRUCT_DATA_TYPE     ( FALSE )
** ABCC_CFG_ADI_GET_SET_CALLBACK ( FALSE )
********************************************************************************
********************************************************************************
*/

#include "appl_adi_config.h"

#if ( APPL_ACTIVE_ADI_SETUP == APPL_ADI_SETUP_SIMPLE_16 )

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

/*------------------------------------------------------------------------------
** Data holder for the ADI instances
**------------------------------------------------------------------------------
*/
static UINT16 appl_aiUint16[ 32 ];
static UINT16 appl_aiUint16_[ 32 ];

/*------------------------------------------------------------------------------
** Min, max and default value for appl_aiUint16
**------------------------------------------------------------------------------
*/
static AD_UINT16Type appl_sUint16Prop = { { 0, 0xFFFF, 0 } };

/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*------------------------------------------------------------------------------
** 32 16-bit values as an array
**------------------------------------------------------------------------------
*/

/*-------------------------------------------------------------------------------------------------------------
** 1. iInstance | 2. pabName | 3. bDataType | 4. bNumOfElements | 5. bDesc | 6. pxValuePtr | 7. pxValuePropPtr
**--------------------------------------------------------------------------------------------------------------
*/
const AD_AdiEntryType APPL_asAdiEntryList[] =
{
   {  0x1,  "ABP_UINT16_WRITE",   ABP_UINT16,   32, APPL_WRITE_MAP_READ_ACCESS_DESC, { { appl_aiUint16_, NULL} } },
   {  0x2,  "ABP_UINT16_READ",    ABP_UINT16,   32, APPL_READ_MAP_WRITE_ACCESS_DESC,  { { NULL, appl_aiUint16 } } }
};

/*------------------------------------------------------------------------------
** Map all adi:s in both directions
**------------------------------------------------------------------------------
** 1. AD instance | 2. Direction | 3. Num elements | 4. Start index |
**------------------------------------------------------------------------------
*/
const AD_DefaultMapType APPL_asAdObjDefaultMap[] =
{
   { 1, PD_WRITE, 1, 0 },
	 { 1, PD_WRITE, 1, 1 },
	 { 1, PD_WRITE, 1, 2 },
   { 1, PD_WRITE, 1, 3 },
   { 1, PD_WRITE, 1, 4 },
   { 1, PD_WRITE, 1, 5 },
   { 1, PD_WRITE, 1, 6 },
   { 1, PD_WRITE, 1, 7 },
   { 1, PD_WRITE, 1, 8 },
   { 1, PD_WRITE, 1, 9 },
   { 1, PD_WRITE, 1, 10 },
   { 1, PD_WRITE, 1, 11 },
   { 1, PD_WRITE, 1, 12 },
   { 1, PD_WRITE, 1, 13 },
   { 1, PD_WRITE, 1, 14 },
   { 1, PD_WRITE, 1, 15 },
   { 1, PD_WRITE, 1, 16 },
   { 1, PD_WRITE, 1, 17 },
   { 1, PD_WRITE, 1, 18 },
   { 1, PD_WRITE, 1, 19 },
   { 1, PD_WRITE, 1, 20 },
   { 1, PD_WRITE, 1, 21 },
   { 1, PD_WRITE, 1, 22 },
   { 1, PD_WRITE, 1, 23 },
   { 1, PD_WRITE, 1, 24 },
   { 1, PD_WRITE, 1, 25 },
   { 1, PD_WRITE, 1, 26 },
   { 1, PD_WRITE, 1, 27 },
   { 1, PD_WRITE, 1, 28 },
   { 1, PD_WRITE, 1, 29 },
   { 1, PD_WRITE, 1, 30 },
   { 1, PD_WRITE, 1, 31 },
   
	 
   { 2, PD_READ,  1, 0 },
	 { 2, PD_READ,  1, 1 },
	 { 2, PD_READ,  1, 2 },
	 { 2, PD_READ,  1, 3 },
	 { 2, PD_READ,  1, 4 },
	 { 2, PD_READ,  1, 5 },
	 { 2, PD_READ,  1, 6 },
	 { 2, PD_READ,  1, 7 },
	 { 2, PD_READ,  1, 8 },
	 { 2, PD_READ,  1, 9 },
	 { 2, PD_READ,  1, 10 },
	 { 2, PD_READ,  1, 11 },
	 { 2, PD_READ,  1, 12 },
	 { 2, PD_READ,  1, 13 },
	 { 2, PD_READ,  1, 14 },
	 { 2, PD_READ,  1, 15 },
	 { 2, PD_READ,  1, 16 },
	 { 2, PD_READ,  1, 17 },
	 { 2, PD_READ,  1, 18 },
	 { 2, PD_READ,  1, 19 },
	 { 2, PD_READ,  1, 20 },
	 { 2, PD_READ,  1, 21 },
	 { 2, PD_READ,  1, 22 },
	 { 2, PD_READ,  1, 23 },
	 { 2, PD_READ,  1, 24 },
	 { 2, PD_READ,  1, 25 },
	 { 2, PD_READ,  1, 26 },
	 { 2, PD_READ,  1, 27 },
	 { 2, PD_READ,  1, 28 },
	 { 2, PD_READ,  1, 29 },
	 { 2, PD_READ,  1, 30 },
	 { 2, PD_READ,  1, 31 },
	 
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
   /*
   ** This function is called when read and write data have been updated. It
   ** could for example be used for operations on the ADI data.
   ** Not used in this example.
   */
	if( ABCC_AnbState() == ABP_ANB_STATE_PROCESS_ACTIVE )
   {
      /*
      ** An example of ADI data handling.
		 
	
      */
      if( appl_aiUint16_[0]!=appl_aiUint16[0])
      {
         appl_aiUint16_[0]=appl_aiUint16[0];
      }
			if( appl_aiUint16_[1]!=appl_aiUint16[1])
      {
         appl_aiUint16_[1]=appl_aiUint16[1];
      }
			if( appl_aiUint16_[2]!=appl_aiUint16[2])
      {
         appl_aiUint16_[2]=appl_aiUint16[2];
      }
			if( appl_aiUint16_[3]!=appl_aiUint16[3])
      {
         appl_aiUint16_[3]=appl_aiUint16[3];
      }
			if( appl_aiUint16_[4]!=appl_aiUint16[4])
      {
         appl_aiUint16_[4]=appl_aiUint16[4];
      }
			if( appl_aiUint16_[5]!=appl_aiUint16[5])
      {
         appl_aiUint16_[5]=appl_aiUint16[5];
      }
			if( appl_aiUint16_[6]!=appl_aiUint16[6])
      {
         appl_aiUint16_[6]=appl_aiUint16[6];
      }
			if( appl_aiUint16_[7]!=appl_aiUint16[7])
      {
         appl_aiUint16_[7]=appl_aiUint16[7];
      }
			
			if( appl_aiUint16_[8]!=appl_aiUint16[8])
      {
         appl_aiUint16_[8]=appl_aiUint16[8];
      }
			if( appl_aiUint16_[9]!=appl_aiUint16[9])
      {
         appl_aiUint16_[9]=appl_aiUint16[9];
      }
			if( appl_aiUint16_[10]!=appl_aiUint16[10])
      {
         appl_aiUint16_[10]=appl_aiUint16[10];
      }
			if( appl_aiUint16_[11]!=appl_aiUint16[11])
      {
         appl_aiUint16_[11]=appl_aiUint16[11];
      }
			if( appl_aiUint16_[12]!=appl_aiUint16[12])
      {
         appl_aiUint16_[12]=appl_aiUint16[12];
      }
			if( appl_aiUint16_[13]!=appl_aiUint16[13])
      {
         appl_aiUint16_[13]=appl_aiUint16[13];
      }
			if( appl_aiUint16_[14]!=appl_aiUint16[14])
      {
         appl_aiUint16_[14]=appl_aiUint16[14];
      }
			if( appl_aiUint16_[15]!=appl_aiUint16[15])
      {
         appl_aiUint16_[15]=appl_aiUint16[15];
      }
			
			if( appl_aiUint16_[16]!=appl_aiUint16[16])
      {
         appl_aiUint16_[16]=appl_aiUint16[16];
      }
			if( appl_aiUint16_[17]!=appl_aiUint16[17])
      {
         appl_aiUint16_[17]=appl_aiUint16[17];
      }
			if( appl_aiUint16_[18]!=appl_aiUint16[18])
      {
         appl_aiUint16_[18]=appl_aiUint16[18];
      }
			if( appl_aiUint16_[19]!=appl_aiUint16[19])
      {
         appl_aiUint16_[19]=appl_aiUint16[19];
      }
			if( appl_aiUint16_[20]!=appl_aiUint16[20])
      {
         appl_aiUint16_[20]=appl_aiUint16[20];
      }
			if( appl_aiUint16_[21]!=appl_aiUint16[21])
      {
         appl_aiUint16_[21]=appl_aiUint16[21];
      }
			if( appl_aiUint16_[22]!=appl_aiUint16[22])
      {
         appl_aiUint16_[22]=appl_aiUint16[22];
      }
			if( appl_aiUint16_[23]!=appl_aiUint16[23])
      {
         appl_aiUint16_[23]=appl_aiUint16[23];
      }
			
			if( appl_aiUint16_[24]!=appl_aiUint16[24])
      {
         appl_aiUint16_[24]=appl_aiUint16[24];
      }
			if( appl_aiUint16_[25]!=appl_aiUint16[25])
      {
         appl_aiUint16_[25]=appl_aiUint16[25];
      }
			if( appl_aiUint16_[26]!=appl_aiUint16[26])
      {
         appl_aiUint16_[26]=appl_aiUint16[26];
      }
			if( appl_aiUint16_[27]!=appl_aiUint16[27])
      {
         appl_aiUint16_[27]=appl_aiUint16[27];
      }
			if( appl_aiUint16_[28]!=appl_aiUint16[28])
      {
         appl_aiUint16_[28]=appl_aiUint16[28];
      }
			if( appl_aiUint16_[29]!=appl_aiUint16[29])
      {
         appl_aiUint16_[29]=appl_aiUint16[29];
      }
			if( appl_aiUint16_[30]!=appl_aiUint16[30])
      {
         appl_aiUint16_[30]=appl_aiUint16[30];
      }
			if( appl_aiUint16_[31]!=appl_aiUint16[31])
      {
         appl_aiUint16_[31]=appl_aiUint16[31];
      }
   }
}

/*******************************************************************************
** Tasks
********************************************************************************
*/

#endif
