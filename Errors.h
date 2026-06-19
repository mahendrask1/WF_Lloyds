/*parameters.h*/
#ifndef ERRORS_H
#define ERRORS_H

#include <stdint.h>

void Display_Error(void);
void ErrorLatch_ClearAll(void);
void ErrorLatch_ClearInvComm(void); /* msk29-may-26 */
uint16_t ErrorLatch_GetBitmapLow(void);
uint16_t ErrorLatch_GetBitmapHigh(void);
extern unsigned char INV_Error_Flag;
extern unsigned char INV2_Error_Flag;
extern unsigned char Modbus_Comm_Fault;
extern unsigned char AO_Comm_Fault;

#endif
