#include "Core.hpp"

#include <array>
#include <ostream>

namespace km
{

    namespace cmp
    {
        template <typename T>
        bool isEqual(const Variant &v1, const Variant &v2)
        {
            return v1.as<T>() == v2.as<T>();
        }

        template <typename T>
        bool isLess(const Variant &v1, const Variant &v2)
        {
            return v1.as<T>() < v2.as<T>();
        }

        template <typename T>
        bool isGreater(const Variant &v1, const Variant &v2)
        {
            return v1.as<T>() > v2.as<T>();
        }

    } // namespace cmp

    VariantComparator isEqualComparatorFor(DataType data_type) noexcept
    {
        const VariantComparator comparators[] = {
            cmp::isEqual<KInt32>,
            cmp::isEqual<KInt64>,
            cmp::isEqual<KFloat32>,
            cmp::isEqual<KFloat64>,
            cmp::isEqual<KString>,
            cmp::isEqual<KBoolean>,
            cmp::isEqual<KDate>,
            cmp::isEqual<KDateTime>,
            nullptr};
        return comparators[indexForDataType(data_type, 8)];
    }

    VariantComparator isLessComparatorFor(DataType data_type) noexcept
    {
        const VariantComparator comparators[] = {
            cmp::isLess<KInt32>,
            cmp::isLess<KInt64>,
            cmp::isLess<KFloat32>,
            cmp::isLess<KFloat64>,
            cmp::isLess<KString>,
            cmp::isLess<KBoolean>,
            cmp::isLess<KDate>,
            cmp::isLess<KDateTime>,
            nullptr};
        return comparators[indexForDataType(data_type, 8)];
    }

    VariantComparator isGreaterComparatorFor(DataType data_type) noexcept
    {
        const VariantComparator comparators[] = {
            cmp::isGreater<KInt32>,
            cmp::isGreater<KInt64>,
            cmp::isGreater<KFloat32>,
            cmp::isGreater<KFloat64>,
            cmp::isGreater<KString>,
            cmp::isGreater<KBoolean>,
            cmp::isGreater<KDate>,
            cmp::isGreater<KDateTime>,
            nullptr};
        return comparators[indexForDataType(data_type, 8)];
    }

}
