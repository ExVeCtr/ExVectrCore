#include "ExVectrCore/time_units.hpp"

#include "string.h"
#include "stdio.h"


#define SECONDS_FROM_1970_TO_2000 946684800


VCTR::Core::TimeSpan::TimeSpan(int64_t t_ns) {timespan = t_ns;}

VCTR::Core::TimeSpan::TimeSpan(uint32_t ns, uint32_t us, uint32_t ms, uint32_t sec, uint32_t min, uint32_t hour, uint32_t day, uint32_t month, uint32_t year) {
    timespan = ns + us*MICROSECONDS + ms*MILLISECONDS + sec*SECONDS + min*MINUTES + hour*HOURS + day*DAYS + month*MONTHS + year*YEARS;
}


const uint8_t VCTR::Core::TimeDate::daysInMonth [] = { 31,28,31,30,31,30,31,31,30,31,30,31 };


VCTR::Core::TimeDate::TimeDate (uint32_t t) {
  t -= SECONDS_FROM_1970_TO_2000;    // bring to 2000 timestamp from 1970

    ss = t % 60;
    t /= 60;
    mm = t % 60;
    t /= 60;
    hh = t % 24;
    uint16_t days = t / 24;
    uint8_t leap;
    for (yOff = 0; ; ++yOff) {
        leap = isleapYear(yOff);
        if (days < 365 + leap)
            break;
        days -= 365 + leap;
    }
    for (m = 1; ; ++m) {
        uint8_t daysPerMonth = daysInMonth[m - 1];
        if (leap && m == 2)
            ++daysPerMonth;
        if (days < daysPerMonth)
            break;
        days -= daysPerMonth;
    }
    d = days + 1;
}

VCTR::Core::TimeDate::TimeDate (uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
    if (year >= 2000)
        year -= 2000;
    yOff = year;
    m = month;
    d = day;
    hh = hour;
    mm = min;
    ss = sec;
}

// supported formats are date "Mmm dd yyyy" and time "hh:mm:ss" (same as __DATE__ and __TIME__)
VCTR::Core::TimeDate::TimeDate(const char* date, const char* time) {
   static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
   static char buff[4] = {'0','0','0','0'};
   int y;
   sscanf(date, "%s %hhu %d", buff, &d, &y);
   yOff = y >= 2000 ? y - 2000 : y;
   m = (strstr(month_names, buff) - month_names) / 3 + 1;
   sscanf(time, "%hhu:%hhu:%hhu", &hh, &mm, &ss);
}

// UNIX time: IS CORRECT ONLY WHEN SET TO UTC!!!
uint32_t VCTR::Core::TimeDate::unixtime(void) const {
  uint32_t t;
  uint16_t days = date2days(yOff, m, d);
  t = time2long(days, hh, mm, ss);
  t += SECONDS_FROM_1970_TO_2000;  // seconds from 1970 to 2000

  return t;
}

uint16_t VCTR::Core::TimeDate::date2days(uint16_t y, uint8_t m, uint8_t d) {
    if (y >= 2000)
        y -= 2000;
    uint16_t days = d;
    for (uint8_t i = 1; i < m; ++i)
        days += daysInMonth[i - 1];
    if (m > 2 && isleapYear(y))
        ++days;
    return days + 365 * y + (y + 3) / 4 - 1;
}

long VCTR::Core::TimeDate::time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
    return ((days * 24L + h) * 60 + m) * 60 + s;
}

bool VCTR::Core::TimeDate::isleapYear(const uint8_t y) {
    if(y&3)//check if divisible by 4
        return false;
    //only check other, when first failed
    return (y % 100 || y % 400 == 0);
}


VCTR::Core::TimeSpan operator - (VCTR::Core::TimeDate const& A, VCTR::Core::TimeDate const& B) {

    return VCTR::Core::TimeSpan((int64_t) A.unixtime() * VCTR::Core::SECONDS - B.unixtime() * VCTR::Core::SECONDS);

}


VCTR::Core::TimeDate operator + (VCTR::Core::TimeDate const& A, VCTR::Core::TimeSpan const& B) {

    return VCTR::Core::TimeDate(A.unixtime() + B.nanosecondsTime() / VCTR::Core::SECONDS);

}


VCTR::Core::TimeDate operator - (VCTR::Core::TimeDate const& A, VCTR::Core::TimeSpan const& B) {

    return VCTR::Core::TimeDate(A.unixtime() - (B.nanosecondsTime() / VCTR::Core::SECONDS));

}


bool operator > (VCTR::Core::TimeDate const& A, VCTR::Core::TimeDate const& B) {

    return A.nanosecondsTime() > B.nanosecondsTime();

}


bool operator < (VCTR::Core::TimeDate const& A, VCTR::Core::TimeDate const& B) {

    return A.nanosecondsTime() < B.nanosecondsTime();

}