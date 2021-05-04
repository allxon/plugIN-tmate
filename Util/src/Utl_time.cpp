#include "../include/Utl_time.h"

typedef struct tm* (*UTLTime_ConversionFunc)(const time_t* timer, struct tm* result); 

static void UTLTime_priv_GetCalendarTime(
    UTLTime_CalendarTime_t* calendarTime_out,
    UTLTime_ConversionFunc conversionFunc,
    struct timeval tempWithMicrosec)
{
    if (calendarTime_out != NULL) {
        struct tm temp;
        (IGNORE_RESULT) conversionFunc(&tempWithMicrosec.tv_sec, &temp);
        calendarTime_out->year = temp.tm_year + 1900;
        calendarTime_out->month = temp.tm_mon + 1;
        calendarTime_out->day = temp.tm_mday;
        calendarTime_out->hour = temp.tm_hour;
        calendarTime_out->min = temp.tm_min;
        calendarTime_out->sec = temp.tm_sec;
        calendarTime_out->msec = tempWithMicrosec.tv_usec / 1000;
        calendarTime_out->usec = tempWithMicrosec.tv_usec;
    }
}

void UTLTime_GetCalendarTimeLocal(UTLTime_CalendarTime_t* calendarTime_out)
{
    struct timeval tempWithMicrosec;
    (IGNORE_RESULT) gettimeofday(&tempWithMicrosec, NULL);
    UTLTime_priv_GetCalendarTime(calendarTime_out, &localtime_r, tempWithMicrosec); 

}

