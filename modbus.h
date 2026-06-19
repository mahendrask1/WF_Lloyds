#ifndef __MODBUS__H
#define __MODBUS__H
//#include <at89c51cc03.h>

void serial_init(void);
unsigned int GetCRC16(unsigned char *puchMsg, int DataLen);
void ComposeMessageRtu(void);
void SendRtuCmdToModBus(int len);
void DIRN_MAX487(char on);
//sbit DIRN_MAX487=P1^7;
//sbit serial_int_en=IEN0^4;  //defination of serial interrupt
//sbit receive_en=SCON^4;	//defination of serial reception enable
void Baud_rate_change(void);
void modbus(void);
void Modbus_Monitoring_Time(void);
void UART_MODBUS_CONFIG(void);

#define MAX_LENGTH  100
//#define MAX_LENGTH  120

#endif
