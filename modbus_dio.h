/* modbus_dio.h - DIO Modbus helper prototypes */
#ifndef MODBUS_DIO_H
#define MODBUS_DIO_H

void DIO_modbus(void);

void DIO_ComposeMessageRtu(unsigned char Slave_Addr,
                           unsigned char Futn_Code,
                           unsigned char Reg_Add_hi,
                           unsigned char Reg_Add_lo,
                           unsigned char Reg_val_hi,
                           unsigned char Reg_val_lo,
                           unsigned char Byte_Count);

void UART_DIO_MODBUS_CONFIG(void);
void DIO_DIRN_MAX487(char on);

unsigned int DIO_GetCRC16(unsigned char *puchMsg, int DataLen);
void DIO_GetCRC16_(unsigned char *puchMsg, int DataLen);

#endif
