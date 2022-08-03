/**
 * @file Table.hpp
 * @author Keshav Sahu
 * @date May 1st 2022
 * @brief This file contains base Table class.
 */
#ifndef KMTABLELIB_KMT_TABLE_HPP
#define KMTABLELIB_KMT_TABLE_HPP

#include <algorithm>

#include "AbstractTable.hpp"
#include "ErrorHandler.hpp"
#include "Parser2.hpp"

namespace km
{
    /**
     * @brief Table allows us to create table with multiple columns and rows where each column can have their own data type.
     * Data types includes KInt32, KInt64, KFloat32, KFloat64, KString, KBoolean, KDate and KDateTime. The first column is
     * the primary column and is always sorted, it can have @b duplicate values. It provides insertion/dropping functions.
     * 
     * @code {.cpp}
     * Table student("student", {
     *                              {"name", "Name", DataType::STRING},
     *                              {"age",  "Age", DataType::INT32},
     *                              {"per", "Percentage", DataType::FLOAT32}
     *                          });
     * 
     * //insert some rows
     * student.insertRow({"Keshav Sahu",  25, 84.43f});         //index 1
     * student.insertRow({"Vastinel Jack", 26, 80.34f});        //index 3
     * student.insertRow({"Adil Hussain", 18, 81.39f});         //index 0
     * student.insertRow({"Parmindar Channi", 19, 81.08f});     //index 2
     * 
     * // student.getDataWC(1,0).asString() == "Keshav Sahu"
     * 
     * // adding a row saying "valid" if age > 20, "invalid" otherwise.
     * student.addColumnE({"is_valid", DataType::STRING}, "IF(isGreater($age,20), \"valid\" , \"invalid\")");
     * const IndexType index_of_is_valid = 3;
     * // student.getDataWC(1, index_of_is_valid).asString() == "valid"
     * 
     * @endcode
     */
    class Table : public AbstractTable
    {
        using Comparator_ = bool (AbstractColumn::*)(IndexType, IndexType) const;

    public:
        /**
         * @brief Constructor
         * 
         * Constructs a table with  given table name @a given_table, creates initial columns
         * provided by @a column_list and sets @a sorting_order as sorting order for the table.
         * 
         * @exception It throws std::invalid_argument exception if table name or columns names are not valid or
         * column names are not unique within the table or contains a data type which is altered.
         */
        Table(const std::string &table_name, const std::vector<ColumnMetaData> &column_list, SortingOrder sorting_order = SortingOrder::ASCENDING) KM_THROWS_EXCEPTION(std::invalid_argument);

        KM_DISABLE_COPY_MOVE(Table)

        /**
         * @brief Destructor destructs the object but notifies the dependent views before destructing.
         */
        ~Table();

        /**
         * @brief Insert a row into the column.
         *
         * Inserts the row, @a values.size() must match the rowCount() and all elements of @a values must
         * have same data type as the respective column.
         * If everything is fine then the index where row is inserted is returned. If any error occures,
         * it is written to logs and INVALID_INDEX is returned.
         * 
         * @note If row is inserted but isSortingPaused() evalutes to true then INVALID_INDEX is returned.
         */
        IndexType insertRow(const std::vector<Variant> &values) noexcept;

        /**
         * @brief Removes the row from the table.
         * 
         * If row_index is valid then row is removed and frees memory if necessary and true is
         * returned. If it is invalid then it returns false.
         */
        bool dropRow(IndexType row_index);

        /**
         * @brief Set @a size to the max free space tolerance size.
         *
         * When dropRow is called, it is marked as free space, when number of free rows
         * reach the @b m_mfst max free space tolerance size, it frees the space, and
         * shrinks memory to fit the table. Default is 64.
         */
        void setMaxFreeSpaceTolerance(SizeType size);

        /**
         * @brief get the max free space tolerance size (value of @b m_mfst ).
         */
        SizeType getMaxFreeSpaceTolerance() const;

        /**
         * @brief Appends a new column with formula.
         *
         * Appends the column to the table. If table has already some rows then instead of
         * filling them with any default value, evaluates @b formula for each row and fills
         * with the result of @a formula.
         *
         * If column_name is invalid ( see isValidColumnName() ), or column_name already exists
         * or formula contains any type of error, column is not added and false is returned. But if
         * table has no rows, then @b formula is not evaluated, thus if column name is valid and does
         * not exist in table, then it is added and true is returned.
         */
        bool addColumnE(const ColumnMetaData &column_data, const std::string &formula);

        /**
         * @brief appends the column at the end to the table.
         *
         * Appends the column to the table. If table has already some rows then those row elements are
         * filled with @b fill_with .
         *
         * If column_name is invalid ( see isValidColumnName() ), or column_name already exists
         * or data has different data type, error message is written to logs and column is not added and
         * false is returned.
         */
        bool addColumn(const ColumnMetaData &column, const Variant &fill_with);

        /**
         * @brief Append new column with a functor.
         *
         * Appends the column to the table. If it has some rows already then functor is used to generate the appropriate value.
         * Functor must return Variant with matching data type or directly value of matching type.
         * If column name is invalid or already exists in the table or  @b functor(index,args...) has incompatible rows, then it
         * is not added and error messages are written to logs, and false is returned. If table has no row then
         * @b functor(index,args...) is not even called, so even if it contains error, it wil be ignored. And if everything works
         * fine then column is added and true is returned.
         *
         * @code {.cpp}
         * table.addColumnF( "above_18", DataType::BOOLEAN, [](IndexType row_index){
         *      if(table.getDataWC(row_index,1).asInt32() >= 18)        //assuming column 1 represents "age" (int32)
         *          return true;
         *      else
         *          return false;
         * });
         * @endcode
         */
        template <typename Fnc, class... Args>
        bool addColumnF(const ColumnMetaData &column, Fnc functor, Args... args);

        /**
         * @brief Transform/Refill the column.
         * 
         * It evaluates formula and refills the column provided by @a column_name with the results for each row.
         * It can refer itself and can use the previous value it had. Like to give bonus 5% to all student in the existing percentage
         * we will do
         * @code {.cpp}
         * student.transformColumn("per", "add($per,5.0f)");
         * @endcode
         * 
         */
        bool transformColumn(const std::string &column_name, const std::string &formula);

        /**
         * @brief Searches the data @a data in the given column.
         * 
         * It searches all rows for the data @a data in the given column @a column_name.
         * Returns rows that contains the data. In case of float32 and float64, epsilon set in those columns are used for accuracy.
         * If @a column_name is the key column then binary search is applied and caused O(logN) time complexity but in case of other
         * columns, linear search is applied hence causing O(N) complexity.
         */
        std::vector<IndexType> search(const std::string &column_name, const Variant &data) const;

        /**
         * @brief Searches the data @a data in the primary column.
         * 
         * It searches whole table for the data @a data in the given column @a column_name. Binary search is applied so it takes
         * O(logN) time complexity but also search linearly for the duplicates. In case of float32 and float64, epsilon set in those
         * columns are used for accuracy. Returns rows that contains the data.
         */
        std::vector<IndexType> searchInKeyColumn(const Variant &data) const;

        //pure virtual functions

        std::optional<std::pair<IndexType, DataType>> findColumn(const std::string &column_name) const override;
        std::optional<std::pair<std::string, DataType>> columnAt(IndexType column_index) const override;
        const ColumnMetaData &getColumnMetaData(IndexType column_index) const override;
        std::optional<Variant> getData(IndexType row_index, IndexType column_index) const override;
        Variant getDataWC(IndexType row_index, IndexType column_index) const override;
        SizeType rowCount() const override;
        SizeType columnCount() const override;
        
        // virtual functions
        
        /**
         * @brief Set data at the table.
         * 
         * If @a row_index and @a column_index is valid and data contains a value with the required data type
         * it will be set at that location and returns true, false otherwise. If @a column_index == 0 then it
         * doesn't change the data and returns false. Primary key not editable.
         */
        bool setData(IndexType row_index, IndexType column_index, const Variant &data) override;
        
        void setEpsilon(const std::string &column_name, const Variant &data) override;
        void reserve(SizeType row_count) override;
        void sort() override;
        void setDisplayName(const std::string &display_name, IndexType column_index) override;
        std::string getDisplayName(IndexType column_index) const;

    protected:

        void setDataWC(IndexType row_index, IndexType column_index, const Variant &data) override;

    // will be made private in next update.
    protected:
        std::vector<IndexType> m_indices;                   ///< index of the rows inserted in the columns
        std::vector<AbstractColumnPtr_> m_columns;          ///< list of columns
        std::vector<IndexType> m_free_space;                ///< contains indices which are marked free
        AbstractColumnPtr_ m_base_column;                   ///< the first column
        const Comparator_ m_comparator;                     ///< comparator for the primary column
        SizeType m_mfst;                                    ///< max free space tolerance

    private:
        
        /**
         * @brief Frees the space occupied by the column and shrinks it to fit the table.
         *
         * When rows are dropped, those rows are marked as free/deleted, if a new row is inserted it first checks for
         * free space. If free space/row is available then it overwrites old row with the new row.
         * But if many rows were dropped, it is just wasting the memory so to free those spaces, columns are recreated,
         * and all non deleted row elements are inserted in new column.
         */
        void freeSpace();

        /**
         * @brief Checks if @a column and @a data_type is valid or not.
         *
         * It checks whether @a column is a valid column name or not and if already exists in the table. It also checks if
         * @a data_type is valid or altered. If @a column is invalid name or duplicate or @a data_type is invalid then
         * error message is written to logs and false is returned. If everything is fine then true is returned.
         */
        bool validateForNewColumn(const std::string &column, DataType data_type);
    };

    inline void Table::setMaxFreeSpaceTolerance(SizeType size)
    {
        m_mfst = size;
    }

    inline SizeType Table::getMaxFreeSpaceTolerance() const
    {
        return m_mfst;
    }

    inline SizeType Table::rowCount() const
    {
        return m_indices.size();
    }

    inline SizeType Table::columnCount() const
    {
        return m_columns.size();
    }

    inline Variant Table::getDataWC(IndexType row_index, IndexType column_index) const
    {
        return m_columns[column_index]->getData(m_indices[row_index]);
    }

    template <typename Fnc, class... Args>
    bool Table::addColumnF(const ColumnMetaData &column, Fnc functor, Args... args)
    {
        if (!validateForNewColumn(column.column_name, column.data_type))
            return false;

        AbstractColumnPtr_ column_ptr = nullptr;
        createColumn(column_ptr, column.column_name, column.display_name, column.data_type);
        IndexType row_count = rowCount();
        try
        {
            column_ptr->resize(row_count + m_free_space.size());
            for (IndexType i = 0; i < row_count; ++i)
            {
                column_ptr->setData(functor(i, args...), m_indices[i]);
            }
        }
        catch (std::bad_variant_access &)
        {
            delete column_ptr;
            err::addLogMsg(err::LogMsg(getDecoratedName() + " ~ InvalidArgs") << "Bad argument is passed to add new column `" << column.column_name
                                                                              << "`. Ignoring it, column is not added.");
            return false;
        }
        m_columns.push_back(column_ptr);
        if (m_columns.size() == 1)
        {
            m_base_column = m_columns.front();
            sort();
        }
        return true;
    }
}
#endif // KMTABLELIB_KMT_TABLE_HPP
