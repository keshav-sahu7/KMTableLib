#include "FunctionStore.hpp"

namespace km
{

    FunctionStore &FunctionStore::store()
    {
        static FunctionStore fmanager;
        return fmanager;
    }

    extern void initStringFunctions();
    extern void initArithmeticFunctions();
    extern void initLogicalFunctions();
    extern void initComparatorFunctions();
    extern void initTypeFunctions();
    extern void initDateFunctions();

    void initAllFnc()
    {
        initArithmeticFunctions();
        initLogicalFunctions();
        initStringFunctions();
        initComparatorFunctions();
        initTypeFunctions();
        initDateFunctions();
    }

}
