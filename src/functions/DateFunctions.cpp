#include "Core.hpp"
#include "FunctionStore.hpp"

namespace km
{
    namespace fnc
    {
        Variant day_d(const Variant *args) { return KInt32(args[0].asDate().day); }
        Variant day_D(const Variant *args) { return KInt32(args[0].asDateTime().date.day); }
        Variant month_d(const Variant *args) { return KInt32(args[0].asDate().month); }
        Variant month_D(const Variant *args) { return KInt32(args[0].asDateTime().date.month); }
        Variant year_d(const Variant *args) { return KInt32(args[0].asDate().year); }
        Variant year_D(const Variant *args) { return KInt32(args[0].asDateTime().date.year); }

        Variant hour_D(const Variant *args) { return KInt32(args[0].asDateTime().time.hour); }
        Variant minute_D(const Variant *args) { return KInt32(args[0].asDateTime().time.minute); }
        Variant second_D(const Variant *args) { return KInt32(args[0].asDateTime().time.second); }

        Variant isLeapYear_i(const Variant *args)
        {
            KInt32 year = args[0].asInt32();
            return KBoolean((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
        }

    }

    void initDateFunctions()
    {
        using dt = DataType;
        using namespace fnc;
        FunctionStore &store = FunctionStore::store();
        store.addEntries(
            {{"day_d", {day_d, dt::INT32, 1}},
             {"month_d", {month_d, dt::INT32, 1}},
             {"year_d", {year_d, dt::INT32, 1}},

             {"day_D", {day_D, dt::INT32, 1}},
             {"month_D", {month_D, dt::INT32, 1}},
             {"year_D", {year_D, dt::INT32, 1}},
             {"hour_D", {hour_D, dt::INT32, 1}},
             {"minute_D", {minute_D, dt::INT32, 1}},
             {"second_D", {second_D, dt::INT32, 1}},

             {"isLeapYear_i", {isLeapYear_i, dt::BOOLEAN, 1}}});
    }

}