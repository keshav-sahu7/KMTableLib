/**
 * @file Types.hpp
 * @author Keshav Sahu
 * @date May 1st 2022
 * @brief This file contains data type definitions that all the table and view class uses.
 */

#ifndef KMTABLELIB_KMT_TYPES_HPP
#define KMTABLELIB_KMT_TYPES_HPP

#include <ostream>
#include <string>
#include <cstdint>
namespace km
{

    inline namespace tp
    {
        /**
         * @brief Date type.
         * The date_t is a POD type and it can hold any date data. It is
         * memory efficient but it doesn't check if assigned date is valid or not.
         */
        struct date_t
        {
            uint16_t year;          ///< year
            uint8_t month;          ///< month
            uint8_t day;            ///< day
        };

        /**
         * @brief Date time type.
         * The date_time_t is a POD type and capable of holding both date and time. It is
         * memory efficient but it doesn't check if assigned time and date is valid or not.
         */
        struct date_time_t
        {
            date_t date;        ///< date
            struct time_t_
            {
                uint8_t hour;       ///< hour
                uint8_t minute;     ///< minute
                uint8_t second;     ///< second
            } time;
        };

        template <unsigned Size_>
        inline constexpr auto get_float_()
        {
            if constexpr (sizeof(float) >= Size_)
                return float();
            else if constexpr (sizeof(double) >= Size_)
                return double();
            else if constexpr (sizeof(long double) >= Size_)
                return (long double)(0);
            // else void
        }

        typedef decltype(get_float_<4>()) k_float_32_;
        static_assert(sizeof(k_float_32_) >= 4, "No > 4 bytes float type available.");
        typedef decltype(get_float_<8>()) k_float_64_;
        static_assert(sizeof(k_float_64_) >= 8, "No > 8 bytes float type available.");

        using KInt32 = int32_t;        ///< 4 byte integer
        using KInt64 = int64_t;        ///< 8 byte integer
        using KFloat32 = k_float_32_;  ///< floating point type of at least 4 bytes
        using KFloat64 = k_float_64_;  ///< floating point of at least 8 byte
        using KString = std::string;   ///< to hold text/strings
        using KBoolean = bool;         ///< to hold boolean values "True" and "False"
        using KDate = date_t;          ///< to hold date information
        using KDateTime = date_time_t; ///< to hold date and time

        using IndexType = std::size_t;
        using SizeType = std::size_t;
    }

    
    constexpr IndexType INVALID_INDEX = -1;     ///< indicates invalid index
    
    constexpr SizeType INVALID_SIZE = -1;       ///< indicates invalid size

    /**
     * @brief Converts @a date to string and @a sep is used as sperator.
     *
     * Returns a string in format dd/MM/yyyy where '/' is sep. User can provide '/' ' ' '.' or anything.
     */
    std::string toString(KDate date, char sep);

    /**
     * @brief Converts @a date_time to string and @a sep is used as sperator.
     *
     * Returns a string in format dd/MM/yyyy hh:mm:ss where '/' is sep. User can provide '/' ' ' '.' or anything.
     */
    std::string toString(KDateTime date_time, char sep);

    /**
     * @brief Writes KDate @a date to the stream @a stream.
     *
     * If @a date is invalid it may write anything.
     */
    inline std::ostream &operator<<(std::ostream &stream, const KDate &date)
    {
        stream << toString(date, '/');
        return stream;
    }

    /**
     * @brief Writes KDateTime @a date_time to the stream @a stream.
     *
     * If @a date is invalid it may write anything.
     */
    inline std::ostream &operator<<(std::ostream &stream, const KDateTime &date_time)
    {
        stream << toString(date_time, '/');
        return stream;
    }

    /**
     * @brief Converts time to seconds.
     */
    constexpr inline int32_t toSeconds(KDateTime::time_t_ t)
    {
        return t.hour * 3600 + t.minute * 60 + t.second;
    }

    /**
     * @brief Converts date into integer
     */
    constexpr inline int32_t integralRepresentationOf(KDate d)
    {
        return d.year * 10000 + d.month * 100 + d.day;
    }

    /**
     * @brief Checks if @a date1 and @a date2 are equal. If any of them is invalid then result may not be correct.
     */
    constexpr bool operator==(const KDate &date1, const KDate &date2)
    {
        return (date1.day == date2.day) && (date1.month == date2.month) && (date1.year == date2.year);
    }

    /**
     * @brief Checks if @a date1 and @a date2 are not equal. If any of them is invalid then result may not be correct.
     */
    constexpr bool operator!=(const KDate &date1, const KDate &date2)
    {
        return (date1.day != date2.day) || (date1.month != date2.month) || (date1.year != date2.year);
    }

    /**
     * @brief Checks if @a date1 is greater than @a date2. If any of them is invalid then result may not be correct.
     */
    constexpr bool operator>(const KDate &date1, const KDate &date2)
    {
        const auto value1 = integralRepresentationOf(date1);
        const auto value2 = integralRepresentationOf(date2);
        return value1 > value2;
    }

    /**
     * @brief Checks if @a date1 is less than @a date2. If any of them is invalid then result may not be correct.
     */
    constexpr bool operator<(const KDate &date1, const KDate &date2)
    {
        const auto value1 = integralRepresentationOf(date1);
        const auto value2 = integralRepresentationOf(date2);
        return value1 < value2;
    }

    /**
     * @brief Checks if @a date1 is greater than or equal to @a date2. If any of them is invalid then result may not be correct.
     */
    constexpr bool operator>=(const KDate &date1, const KDate &date2)
    {
        const auto value1 = integralRepresentationOf(date1);
        const auto value2 = integralRepresentationOf(date2);
        return value1 >= value2;
    }

    /**
     * @brief Checks if @a date1 is less than or equal to @a date2 . If any of them is invalid then result may not be correct.
     */

    constexpr bool operator<=(const KDate &date1, const KDate &date2)
    {
        const auto value1 = integralRepresentationOf(date1);
        const auto value2 = integralRepresentationOf(date2);
        return value1 <= value2;
    }

    /**
     * @brief Checks if @a date_time_1 and @a date_time_2 are equal. If any of them is invalid then result may not be correct.
     */
    constexpr bool operator==(const KDateTime &date_time1, const KDateTime &date_time2)
    {
        return (date_time1.date == date_time2.date) && (date_time1.time.hour == date_time2.time.hour) && (date_time1.time.minute == date_time2.time.minute) && (date_time1.time.second == date_time2.time.second);
    }

    /**
     * @brief Checks if @a date_time_1 and @a date_time_2 are not equal. If any of them is invalid then result may not be correct.
     */
    constexpr bool operator!=(const KDateTime &date_time1, const KDateTime &date_time2)
    {
        return (date_time1.time.hour != date_time2.time.hour) || (date_time1.time.minute != date_time2.time.minute) || (date_time1.time.second != date_time2.time.second) || (date_time1.date != date_time2.date);
    }

    /**
     * @brief Checks if @a date_time_1 is greater than @a date_time_2. If any of them is invalid then result may not be correct.
     */
    constexpr bool operator>(const KDateTime &date_time1, const KDateTime &date_time2)
    {
        if (date_time1.date > date_time2.date)
            return true;
        else if (date_time1.date < date_time2.date)
            return false;
        const auto value1 = toSeconds(date_time1.time);
        const auto value2 = toSeconds(date_time2.time);
        return value1 > value2;
    }

    /**
     * @brief Checks if @a date_time_1 is less than @a date_time_2. If any of them is invalid then result may not be correct.
     */
    constexpr bool operator<(const KDateTime &date_time1, const KDateTime &date_time2)
    {
        if (date_time1.date < date_time2.date)
            return true;
        else if (date_time1.date > date_time2.date)
            return false;
        const auto value1 = toSeconds(date_time1.time);
        const auto value2 = toSeconds(date_time2.time);
        return value1 < value2;
    }

    /**
     * @brief Checks if @a date_time_1 is less than or equal to @a date_time_2. If any of them is invalid then result may not be correct.
     */
    constexpr bool operator<=(const KDateTime &date1, const KDateTime &date2)
    {
        return !(date1 > date2);
    }

    /**
     * @brief Checks if @a date_time_1 is greater than or equal to @a date_time_2. If any of them is invalid then result may not be correct.
     */
    constexpr bool operator>=(const KDateTime &date1, const KDateTime &date2)
    {
        return !(date1 < date2);
    }

} // namespace km

#endif // KMTABLELIB_KMT_TYPES_HPP