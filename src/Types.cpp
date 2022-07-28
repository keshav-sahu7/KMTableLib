#include "Types.hpp"

namespace km
{

    std::string toString(KDate date, char sep)
    {
        char date_format[] = "00 00 0000";

        date_format[0] += date.day / 10;
        date_format[1] += date.day % 10;

        date_format[2] = sep;

        date_format[3] += date.month / 10;
        date_format[4] += date.month % 10;

        date_format[5] = sep;

        date_format[6] += date.year / 1000;
        date_format[7] += (date.year / 100) % 10;
        date_format[8] += (date.year / 10) % 10;
        date_format[9] += (date.year) % 10;

        return date_format;
    }

    std::string toString(KDateTime date_time, char sep)
    {
        char time_format[] = " 00:00:00";
        time_format[1] += date_time.time.hour / 10;
        time_format[2] += date_time.time.hour % 10;
        time_format[4] += date_time.time.minute / 10;
        time_format[5] += date_time.time.minute % 10;
        time_format[7] += date_time.time.second / 10;
        time_format[8] += date_time.time.second % 10;
        
        return toString(date_time.date,sep) + time_format;
    }

} // namespace km