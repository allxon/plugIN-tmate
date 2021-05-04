#ifndef __UTL_TIME_H__
#define __UTL_TIME_H__
#include "Comm.h"

#define UTLTIME_NANOSEC_PER_MICROSEC  1000
#define UTLTIME_MICROSEC_PER_SEC      1000000
#define UTLTIME_NANOSEC_PER_SEC       1000000000
#define IGNORE_RESULT  void

#define UTL_TIMEOUT_NONE  UINT64_MAX

typedef uint64_t UTLTime_t;

typedef struct UTLTime_CalendarTime
{
    int32_t year;  ///< Year (starting at 0)
    int32_t month; ///< Month (starting with January) [1, 12]
    int32_t day;   ///< Day of the month [1, 31]
    int32_t hour;  ///< Hours since midnight [0, 23]
    int32_t min;   ///< Minutes after the hour [0, 59]
    int32_t sec;   ///< Seconds after the minute [0, 59 (or up to 61 for leap seconds)]
    int32_t msec;  ///< Milliseconds after the second [0,999]
    int32_t usec;  ///< Microseconds after the second [0,999999]
} UTLTime_CalendarTime_t;

static inline UTLTime_t UTLTime_FromSec(uint64_t seconds) { return seconds * UTLTIME_MICROSEC_PER_SEC; }
static inline UTLTime_t UTLTime_FromNanosec(uint64_t ns) { return ns / UTLTIME_NANOSEC_PER_MICROSEC; }
static inline uint64_t UTLTime_ToSec(UTLTime_t vplTime) { return vplTime / UTLTIME_MICROSEC_PER_SEC; }
static inline uint64_t UTLTime_ToNanosec(UTLTime_t vplTime) { return vplTime * UTLTIME_NANOSEC_PER_MICROSEC; }


void UTLTime_GetCalendarTimeLocal(UTLTime_CalendarTime_t* calendarTime_out);


#endif

