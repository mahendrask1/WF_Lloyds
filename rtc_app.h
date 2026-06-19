/* rtc_app.h - application RTC helpers */
#ifndef RTC_APP_H
#define RTC_APP_H

#include "lpc17xx_rtc.h"

void rtc_read_routine(void);
void rtc_write_routine(RTC_TIME_Type rtc_write);

#endif
