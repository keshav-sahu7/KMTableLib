/**
 * @file BasicView.hpp
 * @author Keshav Sahu
 * @date May 1st 2022
 * @brief This file contains BasicView class.
 */

#ifndef KMTABLELIB_KMT_BASIC_VIEW_HPP
#define KMTABLELIB_KMT_BASIC_VIEW_HPP


#include "AbstractView.hpp"

namespace km
{
    /**
     * @brief BasicView is a view class that can be used to filter a table.
     * 
     * A view is virtual table. It simply filters the source table and creates a view. It can hold any number of available columns
     * in any order. Source table @b must not be empty (no columns). This view filters the table and keeps a set of records that
     * refers to the source table. It doesn't copy the actual data. It is not needed to refresh it manually. It is always refreshed
     * itself whenever source table is altered/modified.
     * 
     * @code {.cpp}
     * using dt = DataType;
     * Table table("student", {{"name", dt::STRING},{"marks",dt::INT32});
     * table.insertRow({"Akash",  80});
     * table.insertRow({"Simmon", 78});
     * table.insertRow({"Jimmy", 83});
     * table.insertRow({"Martin", 79});
     * // selects all students with marks >= 80 and sorts it by marks in descending order.
     * BasicView basic_view("topper students", &table, {"name","marks"}, "isGreaterOrEqual($marks,80)", "marks", SortingOrder::DESCENDING);
     * 
     * if(basic_view.rowCount() > 0)
     * {
     *      std::cout << "Rank 1 : Student " << basic_view.getDataWC(0,0).toString()
     *                << " with Marks " << basic_view.getDataWC(0,1).toInt() << std::endl;
     * }
     * 
     * //prints Rank 1 : Student Jimmy with Marks 83
     * @endcode
     * 
     * 
     */
    class BasicView final : public AbstractView
    {
        KM_DISABLE_COPY_MOVE(BasicView)
    public:
        /**
         * @brief Constructor
         * 
         * Constructs a view with the given view name @a view_name from source table @a source_table. Selects only columns provided by
         * @a column_names. The order of columns is determined from @a column_names and doesn't rely on source table. If @a column_names
         * is empty then it selects all columns in the same order as the source table.
         * 
         * @a formula is used to filter the table if it is empty then all rows are selected. It must be boolean expression (The final result
         * of the expression should be a boolean if not empty). @a sort_by and @a s_order are the inital sorting column and order. If @a sort_by
         * is empty then it will be sorted according to first column provided from @a column_names (source's first column if @a column_names
         * is empty).
         * 
         * @note If @a formula contains only whitespaces then it won't be treated as empty string. It must not contain '\n' and '\t' chars.
         * BasicView name follows same rule as @ref Table names.
         * 
         * @throws It throws std::invalid_argument if @a view_name is invalid, or if @a column_names holds duplicates or non existing columns.
         * It also throws if @a formula contains any error or return type is not boolean. It also throws if @a sort_by is non empty non existing
         * column in the @a column_names.
         */
        BasicView(const std::string &view_name, AbstractTable *source_table, const std::vector<std::string> &column_names, const std::string &formula = std::string(), std::string sort_by = std::string(), SortingOrder s_order = SortingOrder::ASCENDING) KM_THROWS_EXCEPTION(std::invalid_argument);
        
        /**
         * @brief Destructor
         */
        ~BasicView() = default;

        /**
         * @brief set view name.
         * 
         * Unlike Table, BasicView allows changing its name. If @a view_name is valid then it is changed and true is returned.
         * If @a view_name is invalid it discards it and false is returned.
         */
        bool setViewName(const std::string &view_name);

        // All these functions are implemented from AbstractTable and AbstractView.

        std::string getFilterFormula() const override;
        std::optional<std::pair<IndexType, DataType>> findColumn(const std::string &column_name) const override;
        std::optional<std::pair<std::string, DataType>> columnAt(IndexType column_index) const override;
        const ColumnMetaData &getColumnMetaData(IndexType column_index) const override;
        SizeType rowCount() const override;
        SizeType columnCount() const override;
        std::optional<Variant> getData(IndexType row_index, IndexType column_index) const override;
        Variant getDataWC(IndexType row_index, IndexType column_index) const override;

        /**
         * @brief returns insertable position for @a data.
         * 
         * It finds insertable position for the @a data in this view. @a data must have the data type according to current
         * sorting column. It uses binary search. It is used in internal functions but can be used for other purposes too.
         */
        IndexType insertablePosition(const Variant &data);

    public:
        // All these functions are implemented from AbstractView.

        void sortBy(SortingOrder s_order) override;
        void sortBy(const std::string &column_name) override;
        void sortBy(const std::string &column_name, SortingOrder s_order) override;
        IndexType mapToLocal(IndexType src_row_index) override;
        void refresh() override;

    protected:
        // All these  slot functions are implemented from AbstractView.
        KM_SLOT void dataUpdated(IndexType row_index, IndexType column_index, const Variant &old_data) override;
        KM_SLOT void rowInserted(IndexType row_index) override;
        KM_SLOT void rowDropped(IndexType row_index) override;
        KM_SLOT void sourceSorted() override;
        KM_SLOT void sourceReversed() override;
        KM_SLOT void columnTransformed(IndexType column_index) override;
        KM_SLOT void sourceRefreshed() override;
        KM_SLOT void sourceAboutToBeDestructed() override;

    private:
        void checkPreConditions(const std::string &view_name, AbstractTable *source_table, const std::string &formula);
        IndexType mapToLocal(IndexType src_row_index, const Variant &old_data);

    private:
        std::vector<IndexType> m_indices;
        std::vector<IndexType> m_selected_columns;
        parse::TokenContainer m_filtered_token;
        std::string m_exp;
    };
}

#endif // KMTABLELIB_KMT_BASIC_VIEW_HPP
