#include <string>
#include <algorithm>
#include "Core.hpp"
#include "FunctionStore.hpp"

namespace km
{
    namespace fnc
    {

        Variant uppercase_s(const Variant *args)
        {
            KString str = args[0].asString();
            for (char &c : str)
            {
                if (c >= 'a' && c <= 'z')
                    c += 'A' - 'a';
            }
            return str;
        }

        Variant lowercase_s(const Variant *args)
        {
            KString str = args[0].asString();
            for (char &c : str)
            {
                if (c >= 'A' && c <= 'Z')
                    c += 'a' - 'A';
            }
            return str;
        }

        /*
            Variant capitalizeEach_s(const Variant *args)
            {
                KString str = args[0].asString();
                bool previous_was_space = true;
                for (char &c : str)
                {
                    if (previous_was_space && (c >= 'a' && c <= 'z'))
                        c += 'A' - 'a';
                    previous_was_space = (c == ' ');
                }
                return str;
            }

            Variant sentenseCase_s(const Variant *args)
            {
                KString s = args[0].asString();
                if (!s.empty() && (s[0] >= 'a' && s[0] <= 'z'))
                    s[0] += 'A' - 'a';
                return s;
            }
    */
        Variant length_s(const Variant *args)
        {
            return static_cast<KInt32>((args[0].asString()).length());
        }

        Variant countChar_ss(const Variant *args)
        {
            const KString &str = args[0].asString();
            const KString &tgt = args[1].asString();

            return tgt.empty() ? KInt32{0} : static_cast<KInt32>(std::count(str.cbegin(), str.cend(), tgt[0]));
        }

        Variant contains_ss(const Variant *args)
        {
            return args[0].asString().find(args[1].asString()) != KString::npos;
        }

        Variant containsAnyOf_ss(const Variant *args)
        {
            const KString &str = args[0].asString();
            KString gc = args[1].asString();
            std::sort(gc.begin(), gc.end());
            for (char c : str)
                if (std::binary_search(gc.begin(), gc.end(), c))
                    return KBoolean(true);
            return KBoolean(false);
        }

        template <typename Type_>
        Variant add_(const Variant *args);
    }

    void initStringFunctions()
    {
        using fm = FunctionStore;
        using dt = DataType;
        using namespace fnc;
        fm &store = fm::store();
        store.addEntries(
            {{"concatenate_ss", {add_<KString>, dt::STRING, 2}},
             {"contains_ss", {contains_ss, dt::BOOLEAN, 2}},
             {"containsAnyOf_ss", {containsAnyOf_ss, dt::BOOLEAN, 2}},
             {"countChar_ss", {countChar_ss, dt::INT32, 2}},
             {"length_s", {length_s, dt::INT32, 1}},
             {"lowerCase_s", {lowercase_s, dt::STRING, 1}},
             {"toLower_s", {lowercase_s, dt::STRING, 1}},
             {"toUpper_s", {uppercase_s, dt::STRING, 1}},
             {"upperCase_s", {uppercase_s, dt::STRING, 1}}});
    }
}
