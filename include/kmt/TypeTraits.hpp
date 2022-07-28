#ifndef KMTABLELIB_KMT_TYPESTRAITS_HPP
#define KMTABLELIB_KMT_TYPESTRAITS_HPP

#include "Types.hpp"

namespace km
{
    /**
     * @brief It verifies wheather both types are same or not.
     *
     * If @b Type1_ and @b Type2_ are same then @b k_is_same<Type1_,Type_2>::value will be true, false otherwise.
     */
    template <typename Type1_, typename Type2_>
    struct k_is_same
    {
        enum
        {
            value = false
        };
    };

    template <typename Type_>
    struct k_is_same<Type_, Type_>
    {
        enum
        {
            value = true
        };
    };

    /**
     * @brief It is used to check if @b Type_ is in any of the next types.
     *
     * To know if @b Type_ matches any data type next to it we can use this function.
     *
     * @b k_is_in_list<int,char,int,float>::value evaluates true as int is in the list [char,int,float].
     */
    template <typename Type_, typename First_, typename... Second_>
    struct k_is_in_list
    {
        enum
        {
            value = k_is_same<Type_, First_>::value || k_is_in_list<Type_, Second_...>::value
        };
    };

    template <typename Type_, typename First_>
    struct k_is_in_list<Type_, First_>
    {
        enum
        {
            value = k_is_same<Type_, First_>::value
        };
    };

    /**
     * @brief It is used to check wheather @b Type_ is arithemetic or not. Valid arithemetic types
     *  are KInt32,KInt64,KFloat32, and KFloat64.
     */
    template <typename Type_>
    struct k_is_arithmetic
    {
        enum
        {
            value = k_is_in_list<Type_, KInt32, KInt64, KFloat32, KFloat64>::value
        };
    };

    /**
     * @brief It is used to check if @b Type_ is integer or not. Valid integer types are KInt32
     *  and KInt64.
     */
    template <typename Type_>
    struct k_is_integer
    {
        enum
        {
            value = k_is_in_list<Type_, KInt32, KInt64>::value
        };
    };

    /**
     * @brief It is used to check if @b Type_ is floating point type or not. Valid floating point types
     * are KFloat32 and KFloat64.
     */
    template <typename Type_>
    struct k_is_float
    {
        enum
        {
            value = k_is_in_list<Type_, KFloat32, KFloat64>::value
        };
    };

    /**
     * @brief checks if @b Type_ is a valid K' type or not.
     * All eight data types KInt32, KInt64, Float32, KFloat64, KString, KBoolean, KDate,
     * and KDateTime are valid K' types.
     */
    template <typename Type_>
    struct k_is_ktype
    {
        enum
        {
            value = k_is_in_list<Type_,
                                 KInt32, KInt64, KFloat32, KFloat64, KString, KBoolean, KDate, KDateTime>::value
        };
    };

} // namespace km

#endif // KMTABLELIB_KMT_TYPESTRAITS_HPP