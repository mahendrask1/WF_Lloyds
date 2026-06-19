/*        timer.h        */

#include <stdint.h>

/* Timer0 tick ~5 ms; used by SPI wait timeout in spi_write(). msk 26-may-2026 */
#define SPI_WAIT_TIMEOUT_TICKS  2U   //msk ~10 ms 26-may-2026

extern volatile uint32_t spi_wait_tick;

void super_delay(unsigned int delay);
void super_delay_Temp(unsigned int delay);
void OLED_uDelay( int del);
void OLED_uDelay2(void);
void TIMER0_IRQHandler (void);
void Timer0_init(void) ;
void Timer2_init(void);
void TIMER2_IRQHandler (void);
void Counter1_init(void);
void Timer3_init(void);
void TIMER3_IRQHandler (void);
