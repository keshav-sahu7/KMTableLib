#include "LogMsg.hpp"

namespace km::err
{

    LogMsg &LogMsg::operator  << (const KDate &data)
    {
        char date_format[] = "00/00/0000";

        date_format[0] += data.day / 10;
        date_format[1] += data.day % 10;

        date_format[3] += data.month / 10;
        date_format[4] += data.month % 10;

        date_format[6] += data.year / 1000;
        date_format[7] += (data.year / 100) % 10;
        date_format[8] += (data.year / 10) % 10;
        date_format[9] += (data.year) % 10;
        m_msg += date_format;
        return *this;
    }

    LogMsg &LogMsg::operator  << (const KDateTime &data)
    {
        *this << data.date;
        char time_f[] = "00:00:00";
        time_f[0] = data.time.hour / 10;
        time_f[1] = data.time.hour % 10;
        time_f[3] = data.time.minute / 10;
        time_f[4] = data.time.minute % 10;
        time_f[6] = data.time.second / 10;
        time_f[7] = data.time.second % 10;
        m_msg += ' ';
        m_msg += time_f;
        return *this;
    }

    LogMsg &LogMsg::operator  << (const Variant &variant)
    {
        switch (variant.index())
        {
        case 0:
            return *this << variant.asInt32();
        case 1:
            return *this << variant.asInt64();
        case 2:
            return *this << variant.asFloat32();
        case 3:
            return *this << variant.asFloat64();
        case 4:
            return *this << variant.asString();
        case 5:
            return *this << variant.asBoolean();
        case 6:
            return *this << variant.asDate();
        case 7:
            return *this << variant.asDateTime();
        }
        return *this;
    }
} // namespace km
