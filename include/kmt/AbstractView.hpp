#ifndef KMTABLELIB_KMT_ABSTRACT_VIEW_HPP
#define KMTABLELIB_KMT_ABSTRACT_VIEW_HPP

#include "Core.hpp"
#include "AbstractTable.hpp"

#define KM_SLOT

namespace km
{

    /**
     * @brief AbstractView is the base class for all view classes.
     *
     * To make view classes get updates from source classes it must inherit AbstractView class.
     * It provides some slots (function that will be called from source classes to notify changes)
     * and extra functions that view classes must define.
     *
     * Use setSourceTable() to set the source table. It will install itself on the source table to
     * recieve updates and will automatically uninstall when destructing. That means you must set
     * it to nullptr if source table is destructed.
     *
     */
    class AbstractView : public AbstractTable
    {
    public:
        /**
         * @brief Constructor.
         *
         * Does nothing, just initializes AbstractTable with given parameters.
         */
        AbstractView(const std::string &table_name, const std::string &decorated_name, SortingOrder s_order);
        /**
         * @brief Destructor.
         *
         * Uninstalls itself from the source table if it exists.
         */
        virtual ~AbstractView();

        /**
         * @brief Change sorting order.
         *
         * This function sorts view with previously set column but @a s_order may be different.
         */
        virtual void sortBy(SortingOrder s_order) = 0;

        /**
         * @brief Change sorting column.
         *
         * This function sorts view by @a column_name (if exists) with the current sorting order.
         */
        virtual void sortBy(const std::string &column_name) = 0;

        /**
         * @brief Change both sorting order and sorting column.
         *
         * This function sorts view by @a column_name with given sorting order @a s_order.
         */
        virtual void sortBy(const std::string &column_name, SortingOrder s_order) = 0;

        /**
         * @brief Maps to source table's row index to view's row index.
         *
         * @a src_row_index is the row index in source table. If row doesn't exist It will return km::INVALID_INDEX.
         */
        virtual IndexType mapToLocal(IndexType src_row_index) = 0;

        /**
         * @brief Refresh the view.
         *
         * It is called by source's @ref AbstractTable::refreshEvent() function. The view must call @ref AbstractTable::refreshEvent()
         * function to notify nested view objects.
         */
        virtual void refresh() = 0;

        /**
         * @brief Get the filter formula.
         *
         * If view is based on some kind of filter formula then it will be returned else empty string should be returned.
         */
        virtual std::string getFilterFormula() const;

        /**
         * @brief Get the source table.
         */
        const AbstractTable *getSourceTable() const;

        /**
         * @brief Get the source table.
         */
        AbstractTable *getSourceTable();

    protected:
        /**
         * @brief Set source_table as source table.
         *
         * Sets @a source_table as source table. Installs @b this as dependency to get updates from source table.
         * It uninstalls previous source table and sets @a source_table as new source table. It can be a nullptr.
         */
        void setSourceTable(AbstractTable *source_table);

        /**
         * @brief a call back function called when a single element in the table is changed.
         *
         * This function is called by source table when a single element is changed. @a row_index and @a column_index
         * are indices of the data relative to *source table*. Because this function is called after the element is set,
         * but may be the view is sorted in this column so to find this column @a old_data must be provided.
         *
         * @note This function should call appropriate signal functions to notify its dependent views (aka nested view).
         */
        KM_SLOT virtual void dataUpdated(IndexType row_index, IndexType column_index, const Variant &old_data) = 0;

        /**
         * @brief Call back function called when a row is inserted in the source table.
         *
         * This function is called, when a row is inserted in the source table. @a row_index is the index of the row relative
         * to source table.
         *
         * @note This function should call appropriate singal functions to notify its dependent views (aka nested view).
         */
        KM_SLOT virtual void rowInserted(IndexType row_index) = 0;

        /**
         * @brief Call back function called when a row is dropped from the source table.
         *
         * This function is called, when a row is dropped from the source table. @a row_index is the index of the row relative
         * to source table.
         *
         * @note This function should call appropriate singal functions to notify its dependent views (aka nested view).
         */
        KM_SLOT virtual void rowDropped(IndexType row_index) = 0;

        /**
         * @brief Call back function called source table is resorted.
         *
         * This function is called when source table is sorted with different column or resorts.
         *
         * @note This function should call appropriate singal functions to notify its dependent views (aka nested view).
         */
        KM_SLOT virtual void sourceSorted() = 0;

        /**
         * @brief Call back function called when sorting order of the source table is changed but not the sorting column.
         *
         * @note This function usually do not need to call any other functions as it structures itself such that it stays
         * same for outer world.
         */
        KM_SLOT virtual void sourceReversed() = 0;

        /**
         * @brief Call back function called when source table's column is transformed.
         *
         * @a column_index is the index of the column relative to the source table.
         * @note This function should call appropriate singal functions to notify its dependent views (aka nested view).
         */
        KM_SLOT virtual void columnTransformed(IndexType column_index) = 0;

        /**
         * @brief Call back function callded when source table is refreshed/restructured.
         *
         * @note this function should call appropriate singal functions to notify its dependent views (aka nested view).
         */
        KM_SLOT virtual void sourceRefreshed() = 0;

        /**
         * @brief Call back function called before the destruction of the source table.
         *
         * @note This function must call the aboutToDestruct() function before destructing itself to notify the dependent views.
         * So that they the last nested view will destruct itslef first. Here destructing means, settings itself in a state that
         * its destructor should not crash and calling certain functions must not generate UB except some functions which can
         * generate UB even with a valid table. It should call the @b setSourceTable(nullptr) to uninstall itself from the view
         * and to prevent @ref AbstractView from uninstalling @b this from a dangling pointer.
         */
        KM_SLOT virtual void sourceAboutToBeDestructed() = 0;

    private:
        AbstractTable *m_source_table;    /// the target source table
        friend class ::km::AbstractTable; /// friend class
    };

    inline AbstractView::AbstractView(const std::string &table_name, const std::string &decorated_name, SortingOrder s_order)
        : AbstractTable(table_name, decorated_name, s_order), m_source_table(nullptr)
    {
        ;
    }
    inline std::string AbstractView::getFilterFormula() const
    {
        return std::string();
    }
    inline const AbstractTable *AbstractView::getSourceTable() const
    {
        return m_source_table;
    }

    inline AbstractTable *AbstractView::getSourceTable()
    {
        return m_source_table;
    }
}

#endif // KMTABLELIB_KMT_ABSTRACT_VIEW_HPP