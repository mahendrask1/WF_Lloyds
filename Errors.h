/*parameters.h*/
#ifndef ERRORS_H
#define ERRORS_H

#include <stdint.h>

void Display_Error(void);
void ErrorLatch_ClearAll(void);
void ErrorLatch_ClearInvComm(void); /* msk29-may-26 */
void ErrorLatch_SetFault(unsigned char err); /* latch Err 14/15 from inv comm path */
uint16_t ErrorLatch_GetBitmapLow(void);
uint16_t ErrorLatch_GetBitmapHigh(void);
extern unsigned char INV_Error_Flag;
extern unsigned char INV2_Error_Flag;
extern unsigned char Modbus_Comm_Fault;
extern unsigned char AO_Comm_Fault;

/* Drive fault display / latch codes (Error_No) */
#define ERR_DRIVE_M019      8   /* Panel/PLC drive healthy (M019/M020 DIO) */
#define ERR_DRIVE_STSWD    18   /* VFD Modbus status word fault (M042+INV_Error_Flag) */

char DriveFaultErrorCode(void); /* pick ERR_DRIVE_M019 vs ERR_DRIVE_STSWD */

#endif
