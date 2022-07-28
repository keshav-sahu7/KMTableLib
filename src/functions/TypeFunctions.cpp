#include "Core.hpp"
#include "FunctionStore.hpp"

#include <string>

namespace km
{
    namespace fnc
    {

        template <typename To_, typename From_>
        Variant ArithmeticConverter(const Variant *args)
        {
            static_assert((k_is_arithmetic<To_>::value && k_is_arithmetic<From_>::value),
                          "To_ and From_ must be either of KInt32, KInt6, KFloat32 or KFloat64!");
            return static_cast<To_>(args[0].as<From_>());
        }

        template <typename To_, bool optional_ = false>
        Variant StringToNumber(const Variant *args)
        {
            static_assert(k_is_arithmetic<To_>::value,
                          "To_ must be either of KInt32, KInt64, KFloat32 or KFloat64");

            char *end_ptr;
            Variant value;
            const KString &str = args[0].asString();

            if constexpr (k_is_same<To_, KInt32>::value)
            {
                value = static_cast<KInt32>(std::strtol(str.c_str(), &end_ptr, 10));
            }
            else if constexpr (k_is_same<To_, KInt64>::value)
            {
                value = static_cast<KInt64>(std::strtoll(str.c_str(), &end_ptr, 10));
                if (*end_ptr == 'l' && end_ptr != str.c_str())
                    ++end_ptr;
            }
            else if constexpr (k_is_same<To_, KFloat32>::value)
            {
                value = static_cast<KFloat32>(std::strtof(str.c_str(), &end_ptr));
                if (*end_ptr == 'f' && end_ptr != str.c_str())
                    ++end_ptr;
            }
            else //(k_is_same<To_, KFloat64>::value)
            {
                value = static_cast<KFloat64>(std::strtod(str.c_str(), &end_ptr));
            }

            if (str.empty() || *end_ptr != '\0')
            {
                if constexpr (optional_)
                {
                    value = args[1];
                }
                else
                {
                    value = To_(0);
                }
            }
            return value;
        }

        template <typename IntType_, bool optional_ = false>
        Variant ToIntXXBFromString(const Variant *args)
        {
            static_assert(k_is_integer<IntType_>::value, "IntType_ must be either of KInt32 or KInt64");

            char *end_ptr;
            Variant value;
            const KString &str = args[0].asString();
            if constexpr (k_is_same<IntType_, KInt32>::value)
            {
                value = static_cast<KInt32>(std::strtol(str.c_str(), &end_ptr, args[1].asInt32()));
            }
            else // KInt64
            {
                value = static_cast<KInt64>(std::strtoll(str.c_str(), &end_ptr, args[1].asInt32()));
                if (*end_ptr == 'l' && end_ptr != str.c_str())
                    ++end_ptr;
            }

            if (str.empty() || *end_ptr != '\0')
            {
                if constexpr (optional_)
                {
                    value = args[2];
                }
                else
                {
                    value = IntType_(0);
                }
            }
            return value;
        }

        template <typename From_>
        Variant numberToString(const Variant *args)
        {
            static_assert(k_is_arithmetic<From_>::value, "From_ must be either of KInt32, Kint64, KFloat32, KFloat4");
            return std::to_string(args[0].as<From_>());
        }

        template <typename To_, typename From_>
        Variant booleanConverter(const Variant *args)
        {
            static_assert(((k_is_same<To_, KBoolean>::value && k_is_integer<From_>::value) || (k_is_integer<To_>::value && k_is_same<From_, KBoolean>::value)), "To_ and From_ must be either of KInt32, KInt64 or KBoolean");
            return static_cast<To_>(args[0].as<From_>());
        }

        Variant boolToString(const Variant *args)
        {
            return KString(args[0].asBoolean() ? ("True") : ("False"));
        }

        Variant ToDate(const Variant *args)
        {
            return KDate{
                /* year = */ static_cast<uint16_t>(args[2].asInt32()),
                /*month = */ static_cast<uint8_t>(args[1].asInt32()),
                /*day = */ static_cast<uint8_t>(args[0].asInt32())};
        }

        Variant ToDateTime_6i(const Variant *args)
        {
            return KDateTime{
                /*.date = */ {
                    /*.year = */ static_cast<uint16_t>(args[2].asInt32()),
                    /*.month = */ static_cast<uint8_t>(args[1].asInt32()),
                    /*.day = */ static_cast<uint8_t>(args[0].asInt32()),
                },
                /*.time = */ {
                    /*.hour = */ static_cast<uint8_t>(args[3].asInt32()),
                    /*.minute = */ static_cast<uint8_t>(args[4].asInt32()),
                    /*.second = */ static_cast<uint8_t>(args[5].asInt32())}};
        }

        Variant ToDateTime_1d3i(const Variant *args)
        {
            return KDateTime{
                /*.date = */ (args[0].asDate()),
                /*.time = */ {
                    /*.hour = */ static_cast<uint8_t>(args[1].asInt32()),
                    /*.minute = */ static_cast<uint8_t>(args[2].asInt32()),
                    /*.second = */ static_cast<uint8_t>(args[3].asInt32())}};
        }
    }

    // for more information see "FunctionStore.hpp"
    void initTypeFunctions()
    {
        /**
         *     text(string to be converted), base(base in which number will be interpreted),
         *     optional (if text has invalid characters)
         *
         *     ToInt32_s       => (text, base = 10, optional = 0) -> int32
         *     ToInt32_si      => (text, base = 10, optional)     -> int32
         *     ToInt32B_si     => (text, base, optional = 0)      -> int32
         *     ToInt32B_sii    => (text, base, optional)          -> int32
         *     ToInt64B_siI    => (text, base : int32, optional)  -> int64
         *
         **/
        using dt = DataType;
        using namespace fnc;
        FunctionStore &store = FunctionStore::store();
        store.addEntries(
            {
                // arithmetic types to int
                {"toInt32_I", {ArithmeticConverter<KInt32, KInt64>, dt::INT32, 1}},
                {"toInt32_f", {ArithmeticConverter<KInt32, KFloat32>, dt::INT32, 1}},
                {"toInt32_F", {ArithmeticConverter<KInt32, KFloat64>, dt::INT32, 1}},
                {"toInt32_b", {booleanConverter<KInt32, KBoolean>, dt::INT32, 1}},
                // string to int32
                {"toInt32_s", {StringToNumber<KInt32>, dt::INT32, 1}},
                {"toInt32_si", {StringToNumber<KInt32, true>, dt::INT32, 2}},
                {"toInt32B_si", {ToIntXXBFromString<KInt32>, dt::INT32, 2}},
                {"toInt32B_sii", {ToIntXXBFromString<KInt32, true>, dt::INT32, 3}},

                {"toInt64_i", {ArithmeticConverter<KInt64, KInt32>, dt::INT64, 1}},
                {"toInt64_f", {ArithmeticConverter<KInt64, KFloat32>, dt::INT64, 1}},
                {"toInt64_F", {ArithmeticConverter<KInt64, KFloat64>, dt::INT64, 1}},
                {"toInt64_b", {booleanConverter<KInt64, KBoolean>, dt::INT64, 1}},
                // string to int64
                {"toInt64_s", {StringToNumber<KInt64>, dt::INT64, 1}},
                {"toInt64_sI", {StringToNumber<KInt64, true>, dt::INT64, 2}},
                {"toInt64B_si", {ToIntXXBFromString<KInt64>, dt::INT64, 2}},
                {"toInt64B_siI", {ToIntXXBFromString<KInt64, true>, dt::INT64, 3}},

                {"toFloat32_i", {ArithmeticConverter<KFloat32, KInt32>, dt::FLOAT32, 1}},
                {"toFloat32_I", {ArithmeticConverter<KFloat32, KInt64>, dt::FLOAT32, 1}},
                {"toFloat32_F", {ArithmeticConverter<KFloat32, KFloat64>, dt::FLOAT32, 1}},
                // string to float32
                {"toFloat32_s", {StringToNumber<KFloat32>, dt::FLOAT32, 1}},
                {"toFloat32_sf", {StringToNumber<KFloat32, true>, dt::FLOAT32, 2}},

                {"toFloat64_i", {ArithmeticConverter<KFloat64, KInt32>, dt::FLOAT64, 1}},
                {"toFloat64_I", {ArithmeticConverter<KFloat64, KInt64>, dt::FLOAT64, 1}},
                {"toFloat64_f", {ArithmeticConverter<KFloat64, KFloat32>, dt::FLOAT64, 1}},
                // string to float64
                {"toFloat64_s", {StringToNumber<KFloat64>, dt::FLOAT64, 1}},
                {"toFloat64_sF", {StringToNumber<KFloat64, true>, dt::FLOAT64, 2}},

                {"toString_i", {numberToString<KInt32>, dt::STRING, 1}},
                {"toString_I", {numberToString<KInt64>, dt::STRING, 1}},
                {"toString_f", {numberToString<KFloat32>, dt::STRING, 1}},
                {"toString_F", {numberToString<KFloat64>, dt::STRING, 1}},
                {"toString_b", {boolToString, dt::STRING, 1}},

                {"toBoolean_i", {booleanConverter<KBoolean, KInt32>, dt::BOOLEAN, 1}},
                {"toBoolean_I", {booleanConverter<KBoolean, KInt64>, dt::BOOLEAN, 1}},

                {"toDate_iii", {ToDate, dt::DATE, 3}},
                {"toDateTime_iiiiii", {ToDateTime_6i, dt::DATE_TIME, 6}},
                {"toDateTime_diii", {ToDateTime_1d3i, dt::DATE_TIME, 4}},

            });
    }
}
