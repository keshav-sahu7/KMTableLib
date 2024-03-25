/**
 * @file Column.hpp
 * @author Keshav Sahu
 * @date May 1st 2022
 * @brief This file contains Column classes.
 */

#ifndef KMTABLELIB_KMT_COLUMN_HPP
#define KMTABLELIB_KMT_COLUMN_HPP

#include <string>
#include <vector>
#include <limits>

#include "Core.hpp"

namespace km
{
    /**
     * @brief ColumnMetaData is used to encapsulate column name, display name and data_type.
     *
     */
    struct ColumnMetaData
    {
        std::string column_name;            ///< column name/id used in formula
        std::string display_name;           ///< a description about column
        DataType data_type;                 ///< data type of the column

        /**
         * @brief Constructor 
         */
        ColumnMetaData(const std::string &c_name, DataType d_type) : column_name(c_name), data_type(d_type) {}
        /**
         * @brief Constructor
         */
        ColumnMetaData(const std::string &c_name, const std::string &d_name, DataType d_type) : column_name(c_name), display_name(d_name), data_type(d_type) {}
    };

    class AbstractColumn
    {
    protected:
        ColumnMetaData m_column_data; /// meta information of the column.

    public:
        /**
         * @brief Constructor.
         *
         * Constructs column with given @a column_name , @a display_name and @a column_datatype. It doesn't validates for column name.
         */
        AbstractColumn(const std::string &column_name, const std::string &display_name, DataType column_datatype);

        // disable copy and move constructors and assignments
        KM_DISABLE_COPY_MOVE(AbstractColumn)

        /**
         * @brief Returns name of the column.
         */
        const std::string &getName() const noexcept;

        /**
         * @brief Returns data type of the column.
         */
        DataType getDataType() const noexcept;

        /**
         * @brief Sets @a display_name as display name, if it is empty then column name is set as display name.
         */
        void setDisplayName(const std::string &display_name) noexcept;

        /**
         * @brief Returns display name of the column.
         */
        const std::string &getDisplayName() const noexcept;

        /**
         * @brief Returns column name, display name and data type in the form of ColumnMetaData.
         */
        const ColumnMetaData &getMetaData() const noexcept;

        /**
         * @brief Sets epsilon if column has type KFloat32 or KFloat64.
         *
         * If column has type KFloat32 or KFloat64 then it sets @a epsilon to epsilon on the column
         * else it does nothing. It is used for equality testing.
         *
         * @throws type of epsilon must match the type of column else it may throw std::bad_variant_access
         *
         * @see isEqual() and isEqualV().
         */
        virtual void setEpsilon(const Variant &epsilon);

        /**
         * @brief Creates column of the same type as this column.
         *
         * It is helpful for cloning or creating column based on other column without manually creating
         * it as it can have any data type so instead of testing the data type we can create this column
         * but it doesn't copy the data.
         */

        virtual AbstractColumn *getSameTypeColumn(const std::string &column_name) const = 0;

        /**
         * @brief Resizes the column.
         * If @a size is bigger then current data, then temporary values will be added,
         * if it is smaller then elements after size-1 will be deleted.
         */
        virtual void resize(SizeType size) = 0;

        /**
         * @brief Reserves memory for @a size elements for frequent insertion.
         */
        virtual void reserve(SizeType size) = 0;

        /**
         * @brief Sets data @a v at the index @a index.
         *
         * @a index must be a valid index else it would be UB.
         */
        virtual void setData(const Variant &v, IndexType index) = 0;

        /**
         * @brief Returns data at index @a index
         *
         * @a index must be a valid index else it would be UB.
         */
        virtual Variant getData(IndexType index) const noexcept = 0;

        /**
         * @brief Adds @a v at the end of column.
         */
        virtual void pushData(const Variant &v) = 0;

        /**
         * @brief Removes data from the end of column.
         */
        virtual void popData() = 0;

        /**
         * @brief Creates space for an empty element at the end of column.
         */
        virtual void createSpace() = 0;

        /**
         * @brief Returns wheather data at @a index1 is greater than data at @a index2 or not.
         *
         * Indices must be valid else it would be UB.
         */
        virtual bool isGreater(IndexType index1, IndexType index2) const noexcept = 0;

        /**
         * @brief Returns wheather data at @a index1 is equal to data at @a index2 or not.
         *
         * Indices must be valid else it would be UB.
         *
         * @note In case of KFloat32 and KFloat64, epsilon is used for comparison e.g.
         * abs(data_vec[index1] - data_vec[index2]) < epsilon.
         */
        virtual bool isEqual(IndexType index1, IndexType index2) const noexcept = 0;

        /**
         * @brief Returns wheather data at @a index1 is less than data at @a index2 or not.
         *
         * Indices must be valid else it would be UB.
         */
        virtual bool isLess(IndexType index1, IndexType index2) const noexcept = 0;

        /**
         * @brief Returns wheather data at @a index1 is greater than data @a data or not.
         *
         * @a index must be a valid index else it would be UB.
         */
        virtual bool isGreaterV(IndexType index, const Variant &data) const = 0;

        /**
         * @brief Returns wheather data at @a index1 is equal to data @a data or not.
         *
         * @a index must be a valid index else it would be UB.
         *
         * @note in case of KFloat32 and KFloat64, epsilon is used for comparison e.g.
         * abs(data_vec[index] - data.as<KFloatXX>()) < epsilon.
         */
        virtual bool isEqualV(IndexType index, const Variant &data) const = 0;

        /**
         * @brief Returns wheather data at @a index1 is less than data @a data or not.
         *
         * @a index must be a valid index else it would be UB.
         */
        virtual bool isLessV(IndexType index1, const Variant &data) const = 0;

        /**
         * @brief destructor.
         */
        virtual ~AbstractColumn() = default;
    };

    inline AbstractColumn::AbstractColumn(const std::string &column_name, const std::string &display_name, DataType column_datatype)
        : m_column_data({column_name, (display_name.empty() ? column_name : display_name), column_datatype})
    {
        //
    }

    inline const std::string &AbstractColumn::getName() const noexcept
    {
        return m_column_data.column_name;
    }

    inline DataType AbstractColumn::getDataType() const noexcept
    {
        return m_column_data.data_type;
    }

    inline void AbstractColumn::setDisplayName(const std::string &display_name) noexcept
    {
        m_column_data.display_name = display_name.empty() ? m_column_data.column_name : display_name;
    }

    inline const std::string &AbstractColumn::getDisplayName() const noexcept
    {
        return m_column_data.display_name;
    }

    inline const ColumnMetaData &AbstractColumn::getMetaData() const noexcept
    {
        return m_column_data;
    }

    inline void AbstractColumn::setEpsilon([[maybe_unused]] const Variant &epsilon)
    {
        //
    }

    template <typename T>
    constexpr DataType dataTypeFor()
    {
        static_assert(sizeof(T) != sizeof(T), "No specialization for this data type!");
        return static_cast<DataType>(-1);
    }

    template <>
    constexpr DataType dataTypeFor<KInt32>()
    {
        return DataType::INT32;
    }

    template <>
    constexpr DataType dataTypeFor<KInt64>()
    {
        return DataType::INT64;
    }

    template <>
    constexpr DataType dataTypeFor<KFloat32>()
    {
        return DataType::FLOAT32;
    }

    template <>
    constexpr DataType dataTypeFor<KFloat64>()
    {
        return DataType::FLOAT64;
    }

    template <>
    constexpr DataType dataTypeFor<KString>()
    {
        return DataType::STRING;
    }

    template <>
    constexpr DataType dataTypeFor<KBoolean>()
    {
        return DataType::BOOLEAN;
    }

    template <>
    constexpr DataType dataTypeFor<KDate>()
    {
        return DataType::DATE;
    }

    template <>
    constexpr DataType dataTypeFor<KDateTime>()
    {
        return DataType::DATE_TIME;
    }

    /**
     * @brief This class implements %AbstractColumn class for different
     * data types.
     * @tparam Type_ is the type of data this %Column class will hold.
     *
     * @note All columns must satisfy @ref k_is_ktype\<Type_\> in order to create column.
     */

    template <typename Type_>
    class Column final : public AbstractColumn
    {
        /**
         *  All columns must satisfy k_is_ktype<T> in order to create column.
         */
        static_assert(k_is_ktype<Type_>::value, "Type is not supported for column");

    private:
        std::vector<Type_> m_data_vec;

    public:
        /**
         * @brief Constructor
         * 
         * Constructs Column with given @a column_name and @a display_name and type is auto detected.
         */
        Column(const std::string &column_name, const std::string &display_name) : AbstractColumn(column_name, display_name, dataTypeFor<Type_>()) {}

        /**
         * @brief defaulted copy constructor.
         */
        Column(const Column &) = default;

        /**
         * @brief defaulted move constructor.
         */
        Column(Column &&) = default;

        /**
         * @brief defaulted copy assignment.
         */
        Column &operator=(const Column &) = default;

        /**
         * @brief defaulted move assignment.
         */
        Column &operator=(Column &&) = default;

        void reserve(SizeType size) override
        {
            m_data_vec.reserve(size);
        }
        void resize(SizeType size) override
        {
            m_data_vec.resize(size);
        }
        AbstractColumn *getSameTypeColumn(const std::string &column_name) const override
        {
            return new Column<Type_>(column_name, getDisplayName());
        }
        void setData(const Variant &data, IndexType index) override
        {
            m_data_vec[index] = data.as<Type_>();
        }

        Variant getData(IndexType index) const noexcept override
        {
            return m_data_vec[index];
        }
        void pushData(const Variant &data) override
        {
            m_data_vec.push_back(data.as<Type_>());
        }
        void popData() override
        {
            if (!m_data_vec.empty())
                m_data_vec.pop_back();
        }
        void createSpace() override
        {
            m_data_vec.emplace_back(Type_());
        }
        bool isGreater(IndexType index1, IndexType index2) const noexcept override
        {
            return m_data_vec[index1] > m_data_vec[index2];
        }
        bool isEqual(IndexType index1, IndexType index2) const noexcept override
        {
            return m_data_vec[index1] == m_data_vec[index2];
        }
        bool isLess(IndexType index1, IndexType index2) const noexcept override
        {
            return m_data_vec[index1] < m_data_vec[index2];
        }
        bool isGreaterV(IndexType index, const Variant &data) const override
        {
            return m_data_vec[index] > data.as<Type_>();
        }
        bool isEqualV(IndexType index, const Variant &data) const override
        {
            return m_data_vec[index] == data.as<Type_>();
        }
        bool isLessV(IndexType index, const Variant &data) const override
        {
            return m_data_vec[index] < data.as<Type_>();
        }

        ~Column() override = default;
    };

    /// @cond "false" specialization for KFloat32, added support for epsilon
    template <>
    class Column<KFloat32> final : public AbstractColumn
    {
    private:
        std::vector<KFloat32> m_data_vec;
        KFloat32 m_epsilon;

    public:
        Column(const std::string &column_name, const std::string &display_name) : AbstractColumn(column_name, display_name, dataTypeFor<KFloat32>()), m_epsilon(std::numeric_limits<KFloat32>::epsilon()) {}

        Column(const Column &) = default;

        Column(Column &&) = default;

        Column &operator=(const Column &) = default;
        Column &operator=(Column &&) = default;
        void setEpsilon(const Variant &epsilon) override
        {
            m_epsilon = epsilon.asFloat32();
        }
        void reserve(SizeType size) override
        {
            m_data_vec.reserve(size);
        }
        void resize(SizeType size) override
        {
            m_data_vec.resize(size);
        }
        AbstractColumn *getSameTypeColumn(const std::string &column_name) const override
        {
            return new Column<KFloat32>(column_name, getDisplayName());
        }
        void setData(const Variant &data, IndexType index) override
        {
            m_data_vec[index] = data.asFloat32();
        }
        Variant getData(IndexType index) const noexcept override
        {
            return m_data_vec[index];
        }
        void pushData(const Variant &data) override
        {
            m_data_vec.push_back(data.asFloat32());
        }
        void popData() override
        {
            if (!m_data_vec.empty())
                m_data_vec.pop_back();
        }
        void createSpace() override
        {
            m_data_vec.emplace_back(KFloat32());
        }
        bool isGreater(IndexType index1, IndexType index2) const noexcept override
        {
            return m_data_vec[index1] > m_data_vec[index2];
        }
        bool isEqual(IndexType index1, IndexType index2) const noexcept override
        {
            return std::abs(m_data_vec[index1] - m_data_vec[index2]) < m_epsilon;
        }
        bool isLess(IndexType index1, IndexType index2) const noexcept override
        {
            return m_data_vec[index1] < m_data_vec[index2];
        }
        bool isGreaterV(IndexType index, const Variant &data) const override
        {
            return m_data_vec[index] > data.asFloat32();
        }
        bool isEqualV(IndexType index, const Variant &data) const override
        {
            return std::abs(m_data_vec[index] - data.asFloat32()) < m_epsilon;
        }
        bool isLessV(IndexType index, const Variant &data) const override
        {
            return m_data_vec[index] < data.asFloat32();
        }
        ~Column() override = default;
    };

    // specialization for KFloat64, added support for epsilon
    template <>
    class Column<KFloat64> final : public AbstractColumn
    {
    private:
        std::vector<KFloat64> m_data_vec;
        KFloat64 m_epsilon;

    public:
        Column(const std::string &column_name, const std::string &display_name) : AbstractColumn(column_name, display_name, dataTypeFor<KFloat64>()), m_epsilon(std::numeric_limits<KFloat64>::epsilon()) {}

        Column(const Column &) = default;
        Column(Column &&) = default;
        Column &operator=(const Column &) = default;
        Column &operator=(Column &&) = default;

        void setEpsilon(const Variant &epsilon) override
        {
            m_epsilon = epsilon.asFloat64();
        }
        void reserve(SizeType size) override
        {
            m_data_vec.reserve(size);
        }
        void resize(SizeType size) override
        {
            m_data_vec.resize(size);
        }
        AbstractColumn *getSameTypeColumn(const std::string &column_name) const override
        {
            return new Column<KFloat64>(column_name, getDisplayName());
        }
        void setData(const Variant &data, IndexType index) override
        {
            m_data_vec[index] = data.asFloat64();
        }
        Variant getData(IndexType index) const noexcept override
        {
            return m_data_vec[index];
        }
        void pushData(const Variant &data) override
        {
            m_data_vec.push_back(data.asFloat64());
        }
        void popData() override
        {
            if (!m_data_vec.empty())
                m_data_vec.pop_back();
        }
        void createSpace() override
        {
            m_data_vec.emplace_back(KFloat64());
        }
        bool isGreater(IndexType index1, IndexType index2) const noexcept override
        {
            return m_data_vec[index1] > m_data_vec[index2];
        }
        bool isEqual(IndexType index1, IndexType index2) const noexcept override
        {
            return std::abs(m_data_vec[index1] - m_data_vec[index2]) < m_epsilon;
        }
        bool isLess(IndexType index1, IndexType index2) const noexcept override
        {
            return m_data_vec[index1] < m_data_vec[index2];
        }
        bool isGreaterV(IndexType index, const Variant &data) const override
        {
            return m_data_vec[index] > data.asFloat64();
        }
        bool isEqualV(IndexType index, const Variant &data) const override
        {
            return std::abs(m_data_vec[index] - data.asFloat64()) < m_epsilon;
        }
        bool isLessV(IndexType index, const Variant &data) const override
        {
            return m_data_vec[index] < data.asFloat64();
        }
        ~Column() override = default;
    };
    ///@endcond
}
#endif // KMTABLELIB_KMT_COLUMN_HPP
