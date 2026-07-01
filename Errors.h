/*parameters.h*/
#ifndef ERRORS_H
#define ERRORS_H

#include <stdint.h>

void Display_Error(void);
void ErrorLatch_ClearAll(void);
void ErrorLatch_ClearInvComm(void); /* msk29-may-26 */
void ErrorLatch_ClearWfDriveModbusFault(void); /* msk 19-Jun-2026: Err8/18 from M042 Modbus path */
void ErrorLatch_SetFault(unsigned char err); /* latch Err 14/15 from inv comm path */
uint16_t ErrorLatch_GetBitmapLow(void);
uint16_t ErrorLatch_GetBitmapHigh(void);
extern unsigned char INV_Error_Flag;
extern unsigned char INV2_Error_Flag;
extern unsigned char Modbus_Comm_Fault;
extern unsigned char AO_Comm_Fault;
extern unsigned char Inv1_Comm_Fault; /* msk 19-Jun-2026: Inv1 Modbus timeout/CRC — trip feeder */
extern unsigned char Inv2_Comm_Fault;

/* Drive fault display / latch codes (Error_No) */
#define ERR_DRIVE_M019      8   /* Panel/PLC drive healthy (M019/M020 DIO) */
#define ERR_DRIVE_STSWD    18   /* VFD Modbus status word fault (M042+INV_Error_Flag) */
#define ERR_ACS_EFB_COMM_INV1  20   /* msk 19-Jun-2026: ACS drive-reported EFB comm — no trip */
#define ERR_ACS_EFB_COMM_INV2  21   /* msk 19-Jun-2026: ACS INV2 EFB comm — no trip */

char DriveFaultErrorCode(void); /* pick ERR_DRIVE_M019 vs ERR_DRIVE_STSWD */

extern unsigned char acs_inv1_efb_latched; /* msk 19-Jun-2026: sticky Err20 until M018 */
extern unsigned char acs_inv2_efb_latched; /* msk 19-Jun-2026: sticky Err21 until M018 */
void AcsEfbComm_ClearAll(void);          /* msk 19-Jun-2026: streak + latch — power-on/M018 */

extern uint16_t vfd_fault_reg_latched; /* ACS fault reg 400090 latched at trip */
void VfdFault_LatchRegCode(uint16_t code);
void VfdFault_ClearRegCode(void);
void Error_PrintLine6(unsigned char err_no, char start_row, char start_col,
                      char font_size, char underline_no);

#endif
