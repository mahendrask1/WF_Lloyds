/* TIMER.c */

/**
  *****************************************************************************
  * @title   Timer 0 with ISR
  * @author  Leandro J. Aguierre
  * @date    Oct 26
  * @brief   The following example shows how to init timer 0 with ISR. The interrupt period: 1 seg.

  *******************************************************************************
  */
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_gpio.h"
#include "GPIO_LPC17xx.h"
#include "timer.h"
#include "spi.h"
#include "modbus.h"
#include "keys.h"
#include "parameters.h"
#include "Generic/abcc_drv/inc/abcc.h"

//int ModBusIdx =-1,rx_msg=0,ModDataLen,ModTxLen,ModTxCount;
///extern uint16_t Ram_display_Enable_counter;
extern int DIO_rx_msg;
extern char DIO_ModBusIdx;
int rx_msg=0;
int ModBusIdx = -1;

extern char* clear_string;
extern volatile uint8_t flag;

volatile uint32_t spi_wait_tick = 0; //msk SPI wait timeout tick 26-may-2026

char* P_String;
char A_String[100];
//extern int adc_conv_counter;
extern unsigned long rx_data;
extern unsigned int Tacho_Counter,Inv_Op_Freq;
extern unsigned int Count_Tacho;
extern uint16_t  sec_counter;
extern char Start_Stop_status;
extern unsigned long adc_sample_count;

extern uint16_t adc_count;
extern unsigned char adc_conv_counter_ch1,adc_conv_counter_ch2;
extern uint16_t New_menu_parameters[105];
extern uint16_t iocomcounts,response_time;
void super_delay(unsigned int delay)
{
    unsigned int i,j,k,l;
    for(i=0;i<delay;i++)
        for(j=0;j<delay;j++)
            for(k=0;k<delay;k++)
                for(l=0;l<delay;l++);
}
void super_delay_Temp(unsigned int delay)
{
    unsigned int i;
    for(i=0;i<delay;i++);
    for(i=0;i<(delay);i++);        //msk doubled the delay on 18-nov-2025
}

/* delay for OLED */
void OLED_uDelay( int del)
{
    int k=0,j=0;

    for (k=0; k<3;k++)
        for(j=0; j<del;j++);
}

void OLED_uDelay2(void)
{
    OLED_uDelay(20);
}

void TIMER0_IRQHandler (void);
uint8_t temp = 0;
uint16_t scan_count;

/*
 * @brief    Timer 0 interrupt
 * @param    None.
 * @return   None.
 */

void Timer0_init()
{

                TIM_TIMERCFG_Type TMR0_Cfg;
        TIM_MATCHCFG_Type TMR0_Match;

    scan_count = 0;
        flag=1;
        /* On reset, Timer0/1 are enabled (PCTIM0/1 = 1), and Timer2/3 are disabled (PCTIM2/3 = 0).*/
        /* Timer0 tick: PrescaleValue 1 us, MatchValue 5000 -> 5 ms interrupt */
        TMR0_Cfg.PrescaleOption = TIM_PRESCALE_USVAL;
        TMR0_Cfg.PrescaleValue = 1;    /* 1 us per timer count */
        /* Use channel 0, MR0 */
        TMR0_Match.MatchChannel = 0;
        /* Enable interrupt when MR0 matches the value in TC register */
        TMR0_Match.IntOnMatch = ENABLE;
        /* Enable reset on MR0: TIMER will reset if MR0 matches it */
        TMR0_Match.ResetOnMatch = TRUE;
        /* Don't stop on MR0 if MR0 matches it*/
        TMR0_Match.StopOnMatch = FALSE;
        /* Do nothing for external output pin if match (see cmsis help, there are another options) */
        TMR0_Match.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
        /* MatchValue 5000 x 1 us = 5000 us = 5 ms (scan_count/response_time tick) */
                TMR0_Match.MatchValue =  5000;//2500;//30000;//         /* was 2500 = 2.5 ms */
        //TMR0_Match.MatchValue = 1350;          /* 1.35 ms if MatchValue = 1350 */
        /* Set configuration for Tim_config and Tim_MatchConfig */
        TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &TMR0_Cfg);
        TIM_ConfigMatch(LPC_TIM0, &TMR0_Match);

        /* Preemption = 1, sub-priority = 1 */
        NVIC_SetPriority(TIMER0_IRQn, 0);//((0x01<<3)|0x01));
        /* Enable interrupt for timer 0 */
        NVIC_EnableIRQ(TIMER0_IRQn);
                        /* Start timer 0 */
        TIM_Cmd(LPC_TIM0, ENABLE);
        spi_timeout_enable(1U); //msk enable spi_write timeout after boot 26-may-2026
}

void TIMER0_IRQHandler (void)
{
        /*  Clear Interrupt */
        TIM_ClearIntPending(LPC_TIM0,TIM_MR0_INT);
        /* Code...*/
                flag = 1;
                spi_wait_tick++; //msk 26-may-2026
//                    if(Ram_display_Enable_counter > 0)
//                        --Ram_display_Enable_counter;
                 if(++iocomcounts > 16000){iocomcounts=16000;}
                if(++response_time > 16000){response_time=16000;}

            scan_count++;
                if(scan_count >= 200)            /* 200 x 5 ms = 1 s */
                        {
                            adc_conv_counter_ch1=0;
                            adc_conv_counter_ch2=0;
                             sec_counter++;
                             scan_count=0;
                            if(Tacho_ip == 0)
                             Tacho_Counter = LPC_TIM1->TC;  //Read Timer counter register Value
                            else
                                {
                                    Tacho_Counter = Inv_Op_Freq;  //Read Timer counter register Value
                                }

                             LPC_TIM1->TCR = 0;  //Disable  Counter1
                             LPC_TIM1->TC = 0 ;  //Clear Timer Counter Register
                             LPC_TIM1->TCR = 1;  //Enable  Counter1
                            /************* Tacho Routine ***************/
                            //    sprintf(A_String,":%lu\n\r",adc_count);
                                //  P_String = A_String;
                                //  UART_Send((LPC_UART_TypeDef *)LPC_UART2, P_String, strlen(P_String), BLOCKING);
                              //  adc_count = 0;
                        }

    return;
}
void Timer2_init()
{

                TIM_TIMERCFG_Type TMR2_Cfg;
        TIM_MATCHCFG_Type TMR2_Match;
        /* On reset, Timer0/1 are enabled (PCTIM0/1 = 1), and Timer2/3 are disabled (PCTIM2/3 = 0).*/
        /* Initialize timer 0, prescale count time of 100uS */
        TMR2_Cfg.PrescaleOption = TIM_PRESCALE_USVAL;
        TMR2_Cfg.PrescaleValue = 1;    //100uS
        /* Use channel 0, MR0 */
        TMR2_Match.MatchChannel = 0;
        /* Enable interrupt when MR0 matches the value in TC register */
        TMR2_Match.IntOnMatch = ENABLE;
        /* Enable reset on MR0: TIMER will reset if MR0 matches it */
        TMR2_Match.ResetOnMatch = TRUE;
        /* Don't stop on MR0 if MR0 matches it*/
        TMR2_Match.StopOnMatch = FALSE;
        /* Do nothing for external output pin if match (see cmsis help, there are another options) */
        TMR2_Match.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
        /* Set Match value, count value of 10000 (10000 * 100uS = 1000000us = 1s --> 1 Hz) */
        TMR2_Match.MatchValue = 1500;          //1.7mS
        /* Set configuration for Tim_config and Tim_MatchConfig */
        TIM_Init(LPC_TIM2, TIM_TIMER_MODE, &TMR2_Cfg);
        TIM_ConfigMatch(LPC_TIM2, &TMR2_Match);

        /* Preemption = 1, sub-priority = 1 */
        NVIC_SetPriority(TIMER2_IRQn, 0);//((0x01<<3)|0x01));
        /* Enable interrupt for timer 0 */
        NVIC_EnableIRQ(TIMER2_IRQn);
            /* Start timer 2 */
        TIM_Cmd(LPC_TIM2, ENABLE);

}

void TIMER2_IRQHandler (void)
{
        /*  Clear Interrupt */

    TIM_ClearIntPending(LPC_TIM2,TIM_MR2_INT);
        /* Code...*/
        TIM_Cmd(LPC_TIM2, DISABLE);         //Disable Timer2

    if(ModBusIdx>6)    //if timeout check modbus data
             {
                rx_msg=1;
                DIRN_MAX487(1);    //Put in Tx mode to prevent further bytes reception
            }
        else
             {
                ModBusIdx=-1;
                DIRN_MAX487(0);    //Put in Rx mode
            }

}

void Counter1_init()  //Init Timer1 as Counter For Counting Tacho Pulses On CAP1.1 on Rising Edge
{
LPC_PINCON->PINSEL3 |= 0x00C0 ;//Set PINSEL3 [7:6] = 11 for P1.19 as CAP1.1 Function
GPIO_SetDir(1,19,0);    // set pin P1.19 as input
LPC_SC->PCONP |= 2; // PCTIM0 Timer1/Counter1 Power On
//CLKPWR_ConfigPPWR (CLKPWR_PCONP_PCTIM1, ENABLE);
CLKPWR_SetPCLKDiv (CLKPWR_PCLKSEL_TIMER1, CLKPWR_PCLKSEL_CCLK_DIV_4);
LPC_TIM1->TCR = 2;// Counter Reset
LPC_TIM1->PR = 0x00;    /* set prescaler to zero */
LPC_TIM1->CTCR = 0x0005; //Counter Mode:TC incremented on rising edges on the CAP1.1 for timer1
LPC_TIM1->CCR = 0; //disable capture event feature
// Counter Enable. Start Counting in Tacho Routine
//LPC_TIM1->TCR = 1;
}

void Timer3_init()
{

                TIM_TIMERCFG_Type TMR3_Cfg;
        TIM_MATCHCFG_Type TMR3_Match;
        /* On reset, Timer0/1 are enabled (PCTIM0/1 = 1), and Timer2/3 are disabled (PCTIM2/3 = 0).*/
        /* Initialize timer 0, prescale count time of 100uS */
        TMR3_Cfg.PrescaleOption = TIM_PRESCALE_USVAL;
        TMR3_Cfg.PrescaleValue = 1;    //100uS
        /* Use channel 0, MR0 */
        TMR3_Match.MatchChannel = 0;
        /* Enable interrupt when MR0 matches the value in TC register */
        TMR3_Match.IntOnMatch = ENABLE;
        /* Enable reset on MR0: TIMER will reset if MR0 matches it */
        TMR3_Match.ResetOnMatch = TRUE;
        /* Don't stop on MR0 if MR0 matches it*/
        TMR3_Match.StopOnMatch = FALSE;
        /* Do nothing for external output pin if match (see cmsis help, there are another options) */
        TMR3_Match.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
        /* Set Match value, count value of 10000 (10000 * 100uS = 1000000us = 1s --> 1 Hz) */
        TMR3_Match.MatchValue = 50;          //1mS
        /* Set configuration for Tim_config and Tim_MatchConfig */
        TIM_Init(LPC_TIM3, TIM_TIMER_MODE, &TMR3_Cfg);
        TIM_ConfigMatch(LPC_TIM3, &TMR3_Match);

        /* Preemption = 1, sub-priority = 1 */
        NVIC_SetPriority(TIMER3_IRQn, 0);//((0x01<<3)|0x01));
        /* Enable interrupt for timer 0 */
        NVIC_EnableIRQ(TIMER3_IRQn);
            /* Start timer 2 */
        TIM_Cmd(LPC_TIM3, ENABLE);

}

void TIMER3_IRQHandler (void)
{
        /*  Clear Interrupt */
        TIM_ClearIntPending(LPC_TIM3,TIM_MR3_INT);
        /* Code...*/
        //TIM_Cmd(LPC_TIM3, DISABLE);         //Disable Timer2
                ABCC_RunTimerSystem( 1);
}

/*
30/05/2018
Tacho counter = LPC_TIM1->TC ; //If Tacho ip == Tacho && Fixed Freq == No ; Belt speed source is taken from tacho sensor.
and,
Tacho counter = Inv_Op_Freq ; //If Tacho ip == Inv && Fixed Freq == No ; Belt speed source is taken from Inverter op freq.
To display exact speed it will take around 40- 50 sec.

23/08/2018
Timer3 used for HMS Anybus Modules communication.

*/
