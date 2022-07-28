#include "Core.hpp"
#include "FunctionStore.hpp"

namespace km
{
    namespace fnc
    {

        template <typename Type_>
        Variant isLess_(const Variant *args)
        {
            return args[0].as<Type_>() < args[1].as<Type_>();
        }

        template <typename Type_>
        Variant isGreater_(const Variant *args)
        {
            return args[0].as<Type_>() > args[1].as<Type_>();
        }

        template <typename Type_>
        Variant isEqual_(const Variant *args)
        {
            return args[0].as<Type_>() == args[1].as<Type_>();
        }

        template <typename Type_>
        Variant isLessOrEqual_(const Variant *args)
        {
            return args[0].as<Type_>() <= args[1].as<Type_>();
        }

        template <typename Type_>
        Variant isGreaterOrEqual_(const Variant *args)
        {
            return args[0].as<Type_>() >= args[1].as<Type_>();
        }

    }

    void initComparatorFunctions()
    {
        using dt = DataType;
        using namespace fnc;

        FunctionStore &store = FunctionStore::store();

        store.addEntries(
            {{"isLess_ii", {isLess_<KInt32>, dt::BOOLEAN, 2}},
             {"isEqual_ii", {isEqual_<KInt32>, dt::BOOLEAN, 2}},
             {"isGreater_ii", {isGreater_<KInt32>, dt::BOOLEAN, 2}},
             {"isLessOrEqual_ii", {isLessOrEqual_<KInt32>, dt::BOOLEAN, 2}},
             {"isGreaterOrEqual_ii", {isGreaterOrEqual_<KInt32>, dt::BOOLEAN, 2}},

             {"isLess_II", {isLess_<KInt64>, dt::BOOLEAN, 2}},
             {"isEqual_II", {isEqual_<KInt64>, dt::BOOLEAN, 2}},
             {"isGreater_II", {isGreater_<KInt64>, dt::BOOLEAN, 2}},
             {"isLessOrEqual_II", {isLessOrEqual_<KInt64>, dt::BOOLEAN, 2}},
             {"isGreaterOrEqual_II", {isGreaterOrEqual_<KInt64>, dt::BOOLEAN, 2}},

             {"isLess_ff", {isLess_<KFloat32>, dt::BOOLEAN, 2}},
             {"isEqual_ff", {isEqual_<KFloat32>, dt::BOOLEAN, 2}},
             {"isGreater_ff", {isGreater_<KFloat32>, dt::BOOLEAN, 2}},
             {"isLessOrEqual_ff", {isLessOrEqual_<KFloat32>, dt::BOOLEAN, 2}},
             {"isGreaterOrEqual_ff", {isGreaterOrEqual_<KFloat32>, dt::BOOLEAN, 2}},

             {"isLess_FF", {isLess_<KFloat64>, dt::BOOLEAN, 2}},
             {"isEqual_FF", {isEqual_<KFloat64>, dt::BOOLEAN, 2}},
             {"isGreater_FF", {isGreater_<KFloat64>, dt::BOOLEAN, 2}},
             {"isLessOrEqual_FF", {isLessOrEqual_<KFloat64>, dt::BOOLEAN, 2}},
             {"isGreaterOrEqual_FF", {isGreaterOrEqual_<KFloat64>, dt::BOOLEAN, 2}},

             {"isLess_ss", {isLess_<KString>, dt::BOOLEAN, 2}},
             {"isEqual_ss", {isEqual_<KString>, dt::BOOLEAN, 2}},
             {"isGreater_ss", {isGreater_<KString>, dt::BOOLEAN, 2}},
             {"isLessOrEqual_ss", {isLessOrEqual_<KString>, dt::BOOLEAN, 2}},
             {"isGreaterOrEqual_ss", {isGreaterOrEqual_<KString>, dt::BOOLEAN, 2}},

             {"isLess_dd", {isLess_<KDate>, dt::BOOLEAN, 2}},
             {"isEqual_dd", {isEqual_<KDate>, dt::BOOLEAN, 2}},
             {"isGreater_dd", {isGreater_<KDate>, dt::BOOLEAN, 2}},
             {"isLessOrEqual_dd", {isLessOrEqual_<KDate>, dt::BOOLEAN, 2}},
             {"isGreaterOrEqual_dd", {isGreaterOrEqual_<KDate>, dt::BOOLEAN, 2}},

             {"isLess_DD", {isLess_<KDateTime>, dt::BOOLEAN, 2}},
             {"isEqual_DD", {isEqual_<KDateTime>, dt::BOOLEAN, 2}},
             {"isGreater_DD", {isGreater_<KDateTime>, dt::BOOLEAN, 2}},
             {"isLessOrEqual_DD", {isLessOrEqual_<KDateTime>, dt::BOOLEAN, 2}},
             {"isGreaterOrEqual_DD", {isGreaterOrEqual_<KDateTime>, dt::BOOLEAN, 2}}});
    }

} // namespace km
