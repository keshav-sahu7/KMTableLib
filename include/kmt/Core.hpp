#ifndef KMTABLELIB_KMT_CORE_HPP
#define KMTABLELIB_KMT_CORE_HPP

#include <variant>
#include <array>

#include "Types.hpp"
#include "TypeTraits.hpp"

/// disable copy constructor and copy assignment operator.
#define KM_DISABLE_COPY(class_name)               \
    class_name(const class_name &other) = delete; \
    class_name &operator=(const class_name &other) = delete;

/// disable move constructor and move assignment operator.
#define KM_DISABLE_MOVE(class_name)          \
    class_name(class_name &&other) = delete; \
    class_name &operator=(class_name &&other) = delete;

/// disable copy and move constructors and assignment operators.
#define KM_DISABLE_COPY_MOVE(class_name) \
    KM_DISABLE_COPY(class_name)          \
    KM_DISABLE_MOVE(class_name)

namespace km
{
    /**
     * @brief DataType is used to hold information about the type of data at run time.
     */
    enum class DataType : uint16_t
    {
        INT32 = 0x0001,    //> supports 32 bit integer.
        INT64 = 0x0002,    //> supports 64 bit integer.
        FLOAT32 = 0x0004,  //> supports at least 32 bit floating point number.
        FLOAT64 = 0x0008,  //> supports at least 64 bit floating point number.
        STRING = 0x0010,   //> to hold string/text.
        BOOLEAN = 0x0020,  //> to hold boolean value True and False.
        DATE = 0x0040,     //> to hold date information.
        DATE_TIME = 0x0080 //> to hold time as well as date information.
    };

    /**
     * @brief Operator & performs bitwise AND operations between a and b.
     */
    constexpr inline DataType operator&(DataType a, DataType b)
    {
        return static_cast<DataType>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b));
    }

    /**
     * @brief Operator | performs bitwise OR operations between a and b.
     */
    constexpr inline DataType operator|(DataType a, DataType b)
    {
        return static_cast<DataType>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
    }

    /**
     * @brief Returns [0..7] index for any valid data type.
     * 
     * It returns index (order) of the data type @a data_type defined in @ref DataType. If @a data_type is invalid then
     * the @a otherwise is returned. @a INVALID_INDEX is default for invalid data types.
     * 
     * pow(2U,index) == static_cast<unsigned int>(data_type)
     */
    constexpr inline IndexType indexForDataType(DataType data_type, IndexType otherwise = INVALID_INDEX)
    {
        for (IndexType i = 0; (uint16_t(1) << i) <= static_cast<uint16_t>(data_type) && i < 8; ++i)
        {
            if ((uint16_t(1) << i) == static_cast<uint16_t>(data_type))
                return i;
        }
        return otherwise;
    }

    /**
     * @brief Returns string representation of @a data_type.
     * 
     * If type is invalid then "undefiined" is returned.
     */
    constexpr inline const char *dataTypeToString(DataType data_type)
    {
        IndexType index = indexForDataType(data_type, 8); //[0..7] are valid indices, anything ]0..7[ is undefined.
        return std::array<const char *, 9>{
            "int32", "int64", "float32",
            "float64", "string", "boolean",
            "date", "date_time", "undefined"}[index];
    }

    /**
     * @brief Verifies if @a data_type is not altered and a valid data type.
     */
    constexpr inline bool isValidDataType(DataType data_type)
    {
        return indexForDataType(data_type) != INVALID_INDEX;
    }

    /**
     * @brief Converts integer @a index to data type.
     * 0 => INT32, 1 => INT64, 2 => FLOAT32, 3 = FLOAT64,
     * 4 = STRING, 5 => BOOLEAN, 6 = DATE, 7 = DATE_TIME
     */
    constexpr inline DataType toDataType(IndexType index)
    {
        return static_cast<DataType>(1U << index);
    }

    /**
     * @brief This is just a wrapper around std::variant<KInt32, KInt64, KFloat32, KFloat64, KString, KBoolean, KDate, KDateTime>
     * to make it more readable and usable. It provides some necessary functions that is frequently needed. Only all K Types are
     * supported (see @ref k_is_ktype).
     * 
     * @code
     * Variant var = KInt32(10);
     * //var.index() == 0
     * //var.asInt32() == 10
     * //var.asInt64() throws exception
     * var = KInt64(100);
     * //var.index() == 1
     * //var.asInt64() == 100
     * //var.asString() throws exception
     * @endcode
     * 
     */
    class Variant
    {
        using variant_t = std::variant<KInt32, KInt64, KFloat32, KFloat64, KString, KBoolean, KDate, KDateTime>;

    public:
        /**
         * @brief Default constructor.
         */
        constexpr Variant() = default;

        /**
         * @brief Constructor
         * 
         * Constructs variant with data of any K' Type.
         */
        template <typename Type_, class = typename std::enable_if<k_is_ktype<Type_>::value>::type>
        constexpr Variant(Type_ data) : m_data(data) {}

        /**
         * @brief Constructor
         * 
         * Sepecial constructor for KString to avoid string copy.
         */
        constexpr Variant(const KString &data) : m_data(data) {}

        /**
         * @brief Constructor
         * 
         * Constructs variant from rvalue KString.
         */
        constexpr Variant(KString &&data) : m_data(std::move(data)) {}

        /**
         * @brief Constructor
         * 
         * Explicit constructor for const char* as some compilers implicitly converts const char*
         * to bool instead of std::string. To prevent this bug it is defined explicitly.
         */
        Variant(const char *data) : m_data(KString(data)) {}

        /**
         * @brief Copy constructor.
         */
        Variant(const Variant &other) = default;

        /**
         * @brief Move constructor.
         */
        Variant(Variant &&other) = default;

        /**
         * @brief Assignment operator for data of any K' Type.
         */
        template <typename Type_, class = typename std::enable_if<k_is_ktype<Type_>::value>::type>
        Variant &operator=(Type_ data)
        {
            m_data = data;
            return *this;
        }

        /**
         * @brief Special assignment operator for KString.
         */
        Variant &operator=(const KString &data)
        {
            m_data = KString(data);
            return *this;
        }

        /**
         * @brief Special assignment operator for const char*
         */
        Variant &operator=(const char *data)
        {
            m_data = KString(data);
            return *this;
        }

        /**
         * @brief Special assignment operator for movable KString.
         */
        Variant &operator=(KString &&data)
        {
            m_data = std::move(data);
            return *this;
        }

        /**
         * @brief Defaulted copy assignment operator
         */
        Variant &operator=(const Variant &other) = default;

        /**
         * @brief Defaulted move assignment operator
         */
        Variant &operator=(Variant &&other) = default;

        /**
         * @brief Destructor.
         */
        ~Variant() = default;

        /**
         * @brief Returns const reference to @b Type_ to the underlying data.
         * 
         * @exception May throw std::bad_variant_access exception if variant holds different type.
         */
        template <class Type_>
        const Type_ &as() const
        {
            return std::get<Type_>(m_data);
        }

        /**
         * @brief Returns const reference to Kint32 to the underlying data.
         * 
         * @exception May throw std::bad_variant_access exception if variant holds different type.
         */
        const KInt32 &asInt32() const
        {
            return std::get<KInt32>(m_data);
        }

        /**
         * @brief Returns const reference to KInt64 to the underlying data.
         * 
         * @exception May throw std::bad_variant_access exception if variant holds different type.
         */
        const KInt64 &asInt64() const
        {
            return std::get<KInt64>(m_data);
        }

        /**
         * @brief Returns const reference to KFloat32 to the underlying data.
         * 
         * @exception May throw std::bad_variant_access exception if variant holds different type.
         */
        const KFloat32 &asFloat32() const
        {
            return std::get<KFloat32>(m_data);
        }

        /**
         * @brief Returns const reference to KFloat64 to the underlying data.
         * 
         * @exception May throw std::bad_variant_access exception if variant holds different type.
         */
        const KFloat64 &asFloat64() const
        {
            return std::get<KFloat64>(m_data);
        }

        /**
         * @brief Returns const reference to KString to the underlying data.
         * 
         * @exception May throw std::bad_variant_access exception if variant holds different type.
         */
        const KString &asString() const
        {
            return std::get<KString>(m_data);
        }

        /**
         * @brief Returns const reference to KBoolean to the underlying data.
         * 
         * @exception May throw std::bad_variant_access exception if variant holds different type.
         */
        const KBoolean &asBoolean() const
        {
            return std::get<KBoolean>(m_data);
        }

        /**
         * @brief Returns const reference to KDate to the underlying data.
         * 
         * @exception May throw std::bad_variant_access exception if variant holds different type.
         */
        const KDate &asDate() const
        {
            return std::get<KDate>(m_data);
        }

        /**
         * @brief Returns const reference to KDateTime to the underlying data.
         * 
         * @exception May throw std::bad_variant_access exception if variant holds different type.
         */
        const KDateTime &asDateTime() const
        {
            return std::get<KDateTime>(m_data);
        }

        /**
         * @brief Returns the underlying std::variant.
         */
        const variant_t &data() const noexcept
        {
            return m_data;
        }
        /**
         * @brief Returns index of data type of the data it currently has.
         */
        std::size_t index() const
        {
            return m_data.index();
        }

    private:
        variant_t m_data;       /// the actual data
    };

    /**
     * @brief Returns data type of data @a v currently holds.
    */
    inline DataType dataTypeOf(const Variant &v)
    {
        return toDataType(v.index());
    }

    /// A variant comparator function pointer.
    using VariantComparator = bool (*)(const Variant &v1, const Variant &v2);

    /**
     * @brief Returns equality testing function for variants.
     * 
     * It will return a function that can be used to check wheather two variants of same data type
     * are equal or not. @a data_type is the type of the Variants that we will compare.
     */
    VariantComparator isEqualComparatorFor(DataType data_type) noexcept;

    /**
     * @brief Returns a comparision (less) function.
     * 
     * It will return a function that can be used to check wheather a variant of the same data type
     * is less than the second one. @a data_type is the type of the Variants that we will compare.
     */
    VariantComparator isLessComparatorFor(DataType data_type) noexcept;

    /**
     * @brief Returns a comparision (greater) function.
     * 
     * It will return a function that can be used to check wheather a variant of the same data type
     * is greater than the second one. @a data_type is the type of the Variants that we will compare.
     */
    VariantComparator isGreaterComparatorFor(DataType data_type) noexcept;

} // end ofnamespace km

#endif // KMTABLELIB_KMT_CORE_HPP
