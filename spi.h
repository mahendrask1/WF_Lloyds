/*        spi.h        */

#include <stdint.h>

/*---------------Function Declare----------------------------------*/
void SPI_Configure(void);
void PINSEL_Configure(void);
void PINSEL_Configure_Temp(void);
void UARTInit(void);
void SPI_Initialize(void);

uint8_t spi_write(unsigned char _data); //msk returns 0 on timeout 26-may-2026
char spi_read(void);
void spi_timeout_enable(uint8_t enable); //msk 26-may-2026

extern volatile uint8_t spi_bus_fault; //msk 26-may-2026
void PINSEL_Configure_IO(void);
