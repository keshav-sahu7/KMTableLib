#include "Core.hpp"
#include "FunctionStore.hpp"

namespace km
{
    namespace fnc
    {
        Variant AND_bb(const Variant *args)
        {
            return args[0].asBoolean() && args[1].asBoolean();
        }

        Variant OR_bb(const Variant *args)
        {
            return args[0].asBoolean() || args[1].asBoolean();
        }

        Variant NOT_b(const Variant *args)
        {
            return !args[0].asBoolean();
        }

        Variant XOR_bb(const Variant *args)
        {
            return args[0].asBoolean() != args[1].asBoolean();
        }

        // IF(cond, expr1, expr2)
        Variant IF_(const Variant *args)
        {
            return args[0].asBoolean() ? (args[1]) : (args[2]);
        }

    }

    void initLogicalFunctions()
    {
        using dt = DataType;
        using namespace fnc;

        FunctionStore &store = FunctionStore::store();

        store.addEntries(
            {/*name          function   return type   argc*/
             {"AND_bb", {AND_bb, dt::BOOLEAN, 2}},
             {"OR_bb", {OR_bb, dt::BOOLEAN, 2}},
             {"NOT_b", {NOT_b, dt::BOOLEAN, 1}},
             {"XOR_bb", {XOR_bb, dt::BOOLEAN, 2}},
             {"IF_bii", {IF_, dt::INT32, 3}},
             {"IF_bII", {IF_, dt::INT64, 3}},
             {"IF_bff", {IF_, dt::FLOAT32, 3}},
             {"IF_bFF", {IF_, dt::FLOAT64, 3}},
             {"IF_bss", {IF_, dt::STRING, 3}},
             {"IF_bbb", {IF_, dt::BOOLEAN, 3}},
             {"IF_bdd", {IF_, dt::DATE, 3}},
             {"IF_bDD", {IF_, dt::DATE_TIME, 3}}});
    }
}
