/*RTC.c*/

#include "LPC17xx.h"
#include "lpc_types.h"
#include "lpc17xx_rtc.h"
#include "OLED_2_7_12864.h"
#include "main_routine.h"
#include "string.h"
#include "stdio.h"

RTC_TIME_Type rtc_read;

extern char* P_String;
extern char A_String[35];

void rtc_write_routine(RTC_TIME_Type rtc_write)
{

        /* Set current time for RTC
        * Current time is 11:51:50 AM, 2011-02-24
        */
        RTC_SetTime(LPC_RTC, RTC_TIMETYPE_SECOND, rtc_write.SEC);
        RTC_SetTime(LPC_RTC, RTC_TIMETYPE_MINUTE, rtc_write.MIN);
        RTC_SetTime(LPC_RTC, RTC_TIMETYPE_HOUR, rtc_write.HOUR);
        RTC_SetTime(LPC_RTC, RTC_TIMETYPE_MONTH, rtc_write.MONTH);
        RTC_SetTime(LPC_RTC, RTC_TIMETYPE_YEAR, rtc_write.YEAR);
        RTC_SetTime(LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, rtc_write.DOM);
        /* Get and print current time */
            }

            void rtc_read_routine()
            {
        RTC_GetFullTime(LPC_RTC, &rtc_read);

            }

void rtc_config()
{
    char* rtc_String_date;
  char* rtc_String_time;
    RTC_TIME_Type rtc_write1;

                /* RTC init module*/
        RTC_Init(LPC_RTC);
    /* Disable RTC interrupt */
    NVIC_DisableIRQ(RTC_IRQn);
    /* preemption = 1, sub-priority = 1 */
    NVIC_SetPriority(RTC_IRQn, ((0x01<<3)|0x01));
        /* Enable rtc (starts increase the tick counter and second counter register) */
        RTC_ResetClockTickCounter(LPC_RTC);
        RTC_Cmd(LPC_RTC, ENABLE);

/*
    rtc_write1.DOM = 26;
    rtc_write1.MONTH=8;
    rtc_write1.YEAR = 2015;

    rtc_write1.HOUR = 17;
    rtc_write1.MIN = 03;
    rtc_write1.SEC = 01;

rtc_write_routine(rtc_write1);
*/
/*
while(1)
{    super_delay(50);
    rtc_read_routine();
    */
/*
    sprintf(rtc_String_date,"%d / %d / %d",rtc_read.DOM,rtc_read.MONTH,rtc_read.YEAR);
    sprintf(rtc_String_time,"%d : %d : %d",rtc_read.HOUR,rtc_read.MIN,rtc_read.SEC);

                Print_String(&rtc_String_date, line1, 5,font32,0);
                Print_String(&rtc_String_time, line2, 5,font32,0);
*/
    /*
            sprintf(A_String,"SEC:%lu \n\r",rtc_read.SEC    );
        P_String = A_String;
        UART_Send((LPC_UART_TypeDef *)LPC_UART2, P_String, strlen(P_String), BLOCKING);

                Print_Variable(rtc_read.HOUR, line2, 0,font16,0);
                Print_Variable(rtc_read.MIN, line2, 20,font16,0);
                Print_Variable(rtc_read.SEC, line2, 40,font16,0);

                Print_Variable(rtc_read.DOM, line1, 0,font16,0);
                Print_Variable(rtc_read.MONTH, line1, 20,font16,0);
                Print_Variable(rtc_read.YEAR, line1, 40,font16,0);

    super_delay(50);
Clear_LCD();

}
*/

}
