#include <cmath>
#include "Core.hpp"
#include "FunctionStore.hpp"

namespace km
{
    namespace fnc
    {

        template <typename Type_>
        Variant add_(const Variant *args)
        {
            return args[0].as<Type_>() + args[1].as<Type_>();
        }

        template <typename Type_>
        Variant subtract_(const Variant *args)
        {
            return args[0].as<Type_>() - args[1].as<Type_>();
        }

        template <typename Type_>
        Variant multiply_(const Variant *args)
        {
            return args[0].as<Type_>() * args[1].as<Type_>();
        }

        /* integer/integer returns integer so cast it to floating point*/
        template <typename Type_, class CastTo_ = Type_>
        Variant divide_(const Variant *args)
        {
            Type_ t1 = args[0].as<Type_>();
            Type_ t2 = args[1].as<Type_>();
            if constexpr (k_is_in_list<CastTo_, KInt32, KInt64>::value)
            {
                if (t2 == 0)
                    return CastTo_(0); // in case of integer division
            }
            return static_cast<CastTo_>(t1) / static_cast<CastTo_>(t2);
        }

        Variant modulous_ii(const Variant *args)
        {
            return args[0].asInt32() % args[1].asInt32();
        }

        Variant modulous_II(const Variant *args)
        {
            return args[0].asInt64() % args[1].asInt64();
        }

        Variant modulous_ff(const Variant *args)
        {
            return static_cast<KFloat32>(std::fmod(args[0].asFloat32(), args[1].asFloat32()));
        }

        Variant modulous_FF(const Variant *args)
        {
            return static_cast<KFloat64>(std::fmod(args[0].asFloat64(), args[1].asFloat64()));
        }

        template <typename Type_, class CastTo_ = Type_>
        Variant sqrt_(const Variant *args)
        {
            return std::sqrt(static_cast<CastTo_>(args[0].as<Type_>()));
        }

        template <typename Type_>
        Variant pow_(const Variant *args)
        {
            return static_cast<Type_>(std::pow(args[0].as<Type_>(), args[1].as<Type_>()));
        }

        template <typename Type_>
        Variant abs_(const Variant *args)
        {
            return std::abs(args[0].as<Type_>());
        }

        template <typename Type_>
        Variant floor_(const Variant *args)
        {
            return std::floor(args[0].as<Type_>());
        }

        template <typename Type_>
        Variant ceil_(const Variant *args)
        {
            return std::ceil(args[0].as<Type_>());
        }

        template <typename Type_>
        Variant inRange_(const Variant *args)
        {
            const Type_ &val_ = args[0].as<Type_>();
            const Type_ &start_ = args[1].as<Type_>();
            const Type_ &end_ = args[2].as<Type_>();
            return (val_ >= start_) && (val_ <= end_);
        }

    }

    void initArithmeticFunctions()
    {
        using dt = DataType;
        using namespace fnc;
        FunctionStore &store = FunctionStore::store();
        store.addEntries(
            {
                /*name              function                return type     argc*/
                // add
                {"add_ii", {add_<KInt32>, dt::INT32, 2}},
                {"add_II", {add_<KInt64>, dt::INT64, 2}},
                {"add_ff", {add_<KFloat32>, dt::FLOAT32, 2}},
                {"add_FF", {add_<KFloat64>, dt::FLOAT64, 2}},
                {"add_ss", {add_<KString>, dt::STRING, 2}}, // only add function is supported on strings.

                // subtract
                {"subtract_ii", {subtract_<KInt32>, dt::INT32, 2}},
                {"subtract_II", {subtract_<KInt64>, dt::INT64, 2}},
                {"subtract_ff", {subtract_<KFloat32>, dt::FLOAT32, 2}},
                {"subtract_FF", {subtract_<KFloat64>, dt::FLOAT64, 2}},
                {"sub_ii", {subtract_<KInt32>, dt::INT32, 2}},
                {"sub_II", {subtract_<KInt64>, dt::INT64, 2}},
                {"sub_ff", {subtract_<KFloat32>, dt::FLOAT32, 2}},
                {"sub_FF", {subtract_<KFloat64>, dt::FLOAT64, 2}},

                // multiply
                {"multiply_ii", {multiply_<KInt32>, dt::INT32, 2}},
                {"multiply_II", {multiply_<KInt64>, dt::INT64, 2}},
                {"multiply_ff", {multiply_<KFloat32>, dt::FLOAT32, 2}},
                {"multiply_FF", {multiply_<KFloat64>, dt::FLOAT64, 2}},
                {"mul_ii", {multiply_<KInt32>, dt::INT32, 2}},
                {"mul_II", {multiply_<KInt64>, dt::INT64, 2}},
                {"mul_ff", {multiply_<KFloat32>, dt::FLOAT32, 2}},
                {"mul_FF", {multiply_<KFloat64>, dt::FLOAT64, 2}},

                // divide
                {"divide_ii", {divide_<KInt32, KFloat32>, dt::FLOAT32, 2}},
                {"divide_II", {divide_<KInt64, KFloat64>, dt::FLOAT64, 2}},
                {"divide_ff", {divide_<KFloat32>, dt::FLOAT32, 2}},
                {"divide_FF", {divide_<KFloat64>, dt::FLOAT64, 2}},
                {"div_ii", {divide_<KInt32, KFloat32>, dt::FLOAT32, 2}},
                {"div_II", {divide_<KInt64, KFloat64>, dt::FLOAT64, 2}},
                {"div_ff", {divide_<KFloat32>, dt::FLOAT32, 2}},
                {"div_FF", {divide_<KFloat64>, dt::FLOAT64, 2}},

                {"intDiv_ii", {divide_<KInt32>, dt::INT32, 2}},
                {"intDiv_II", {divide_<KInt64>, dt::INT64, 2}},

                // modulous
                {"mod_ii", {modulous_ii, dt::INT32, 2}},
                {"mod_II", {modulous_II, dt::INT64, 2}},
                {"mod_ff", {modulous_ff, dt::FLOAT32, 2}},
                {"mod_FF", {modulous_FF, dt::FLOAT64, 2}},

                // sqrt
                {"sqrt_i", {sqrt_<KInt32, KFloat32>, dt::FLOAT32, 1}},
                {"sqrt_I", {sqrt_<KInt64, KFloat64>, dt::FLOAT64, 1}},
                {"sqrt_f", {sqrt_<KFloat32>, dt::FLOAT32, 1}},
                {"sqrt_F", {sqrt_<KFloat64>, dt::FLOAT64, 1}},

                // pow
                {"pow_ii", {pow_<KInt32>, dt::INT32, 2}},
                {"pow_II", {pow_<KInt64>, dt::INT64, 2}},
                {"pow_ff", {pow_<KFloat32>, dt::FLOAT32, 2}},
                {"pow_FF", {pow_<KFloat64>, dt::FLOAT64, 2}},

                // abs
                {"abs_i", {abs_<KInt32>, dt::INT32, 1}},
                {"abs_I", {abs_<KInt64>, dt::INT64, 1}},
                {"abs_f", {abs_<KFloat32>, dt::FLOAT32, 1}},
                {"abs_F", {abs_<KFloat64>, dt::FLOAT64, 1}},

                // floor
                {"floor_f", {floor_<KFloat32>, dt::FLOAT32, 1}},
                {"floor_F", {floor_<KFloat64>, dt::FLOAT64, 1}},

                // ceil
                {"ceil_f", {ceil_<KFloat32>, dt::FLOAT32, 1}},
                {"ceil_F", {ceil_<KFloat64>, dt::FLOAT64, 1}},

                // in range
                {"isInRange_iii", {inRange_<KInt32>, dt::BOOLEAN, 3}},
                {"isInRange_III", {inRange_<KInt64>, dt::BOOLEAN, 3}},
                {"isInRange_fff", {inRange_<KFloat32>, dt::BOOLEAN, 3}},
                {"isInRange_FFF", {inRange_<KFloat64>, dt::BOOLEAN, 3}},
                {"isInRange_sss", {inRange_<KString>, dt::BOOLEAN, 3}},
                {"isInRange_ddd", {inRange_<KDate>, dt::BOOLEAN, 3}},
                {"isInRange_DDD", {inRange_<KDateTime>, dt::BOOLEAN, 3}},
            });
    }
}
