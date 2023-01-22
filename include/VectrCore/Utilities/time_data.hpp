#ifndef VECTRCORE_UTILITIES_DATETIME_H
#define VECTRCORE_UTILITIES_DATETIME_H

#include "time_definitions.hpp"

#include "stdint.h"

namespace VCTR
{

    
    class TimeSpan
    {
    private:
        int64_t timespan = 0;

    public:
        /**
         * @brief Construct a new Time Span object with nanoseconds
         *
         * @param t_ns Timespan in nanoseconds
         */
        TimeSpan(int64_t t_ns = 0);

        /**
         * @brief Construct a new Time Span object
         *
         * @param ns
         * @param us
         * @param ms
         * @param sec
         * @param min
         * @param hour
         * @param day
         * @param month
         * @param year
         */
        TimeSpan(uint32_t ns, uint32_t us, uint32_t ms = 0, uint32_t sec = 0, uint32_t min = 0, uint32_t hour = 0, uint32_t day = 0, uint32_t month = 0, uint32_t year = 0);

        uint16_t year() const { return static_cast<uint16_t>(timespan / YEARS); }
        uint8_t month() const { return timespan / MONTHS % 12; }
        // Day of month. Assumes all months have 30 days
        uint8_t day() const { return timespan / DAYS % 30; }
        uint8_t hour() const { return timespan / HOURS % 24; }
        uint8_t minute() const { return timespan / MINUTES % 60; }
        uint8_t second() const { return timespan / SECONDS % 60; }
        uint16_t millisecond() const { return timespan / MILLISECONDS % 1000; }
        uint16_t microsecond() const { return timespan / MICROSECONDS % 1000; }
        uint16_t nanosecond() const { return timespan % 1000; }

        /**
         * @brief total Time span in nanoseconds
         *
         */
        int64_t nanosecondsTime() const { return timespan; }
    };

    // Simple general-purpose date/time class (no TZ / DST / leap second handling!)
    class TimeDate
    {
    public:
        TimeDate(uint32_t t = 0);
        TimeDate(uint16_t year, uint8_t month, uint8_t day,
                 uint8_t hour = 0, uint8_t min = 0, uint8_t sec = 0);
        TimeDate(const char *date, const char *time);
        uint16_t year() const { return 2000 + yOff; }
        uint8_t month() const { return m; }
        uint8_t day() const { return d; }
        uint8_t hour() const { return hh; }
        uint8_t minute() const { return mm; }
        uint8_t second() const { return ss; }
        uint8_t dayOfTheWeek() const;

        /// @brief The start of the day at 00:00.
        /// @return TimeDate
        TimeDate today() const { return TimeDate(year(), month(), day()); }

        /**
         * @brief unixtime in nanoseconds.
         *
         */
        int64_t nanosecondsTime() const { return unixtime() * SECONDS; }
        // 32-bit times as seconds since 1/1/1970
        // THE ABOVE COMMENT IS CORRECT FOR LOCAL TIME; TO USE THIS COMMAND TO
        // OBTAIN TRUE UNIX TIME SINCE EPOCH, YOU MUST CALL THIS COMMAND AFTER
        // SETTING YOUR CLOCK TO UTC
        uint32_t unixtime(void) const;

    protected:
        uint8_t yOff, m, d, hh, mm, ss;

        static const uint8_t daysInMonth[];

        static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d);

        static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s);

        static bool isleapYear(const uint8_t y);
    };

    TimeSpan operator-(TimeDate const &A, TimeDate const &B);

    TimeDate operator+(TimeDate const &A, TimeSpan const &B);

    TimeDate operator-(TimeDate const &A, TimeSpan const &B);

    bool operator>(TimeDate const &A, TimeDate const &B);

    bool operator<(TimeDate const &A, TimeDate const &B);

}

#endif