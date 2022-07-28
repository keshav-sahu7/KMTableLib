/**
 * @file AbstractTable.hpp
 * @author Keshav Sahu
 * @date May 1st 2022
 * @brief This file contains base interface for all the table and view classes.
 */

#ifndef KMTABLELIB_KMT_ABSTRACTTABLE_HPP
#define KMTABLELIB_KMT_ABSTRACTTABLE_HPP

#include <string>
#include <vector>
#include <utility> //std::pair
#include <optional>
#include <algorithm>

#include "Core.hpp"
#include "Column.hpp"
#include "Parser2.hpp"

///< indicates that this function throws exception of kind `kind`
#define KM_THROWS_EXCEPTION(exception_type) noexcept(false)
#define KM_EMIT
#define KM_SIGNAL

namespace km
{

    /**
     * @brief SortingOrder will be used to set sorting order on table and view classes.
     *
     * Every table and views are sorted in either ascending or descending order for better
     * management of the data.
     */
    enum class SortingOrder
    {
        ASCENDING, /// ascending order
        DESCENDING /// descending order
    };

    using AbstractColumnPtr_ = AbstractColumn *;
    using ConstAbstractColumnPtr_ = const AbstractColumn *;

    class AbstractView;

    /**
     * @brief Base Abstract class for @ref Table and @ref AbstractView classes.
     *
     * It provides base for Table and View classes. It has some basic functionality that every table and view class
     * must have in order to work with other different classes (e.g. %Printer ) and evaluation functions that takes
     * AbstractTable pointer to work.
     *
     * For a custom class that inherits it must override these eight functions
     *
     * - @b findColumn(column_name) : To find a column by name.
     * - @b columnAt(column_index) : returns column at given column index.
     * - @b rowCount() : number of rows in the table.
     * - @b columnCount() : number of columns in the table.
     * - @b getData(row_index,column_index) : returns data with bound checking.
     * - @b getDataWC(row_index,column_index) : returns data without bound checking.
     *
     * @note AbstractTable::getDataWC is called from parse::evaluateFormula function. For
     * @ref parse::evaluateFormula support a class must also override @b setDataWC(row_index,column_index,data).
     */
    class AbstractTable
    {

    public:
        /**
         * @brief Constructor.
         *
         * This sets table name, decorated_name, sorting_order. Decorated name is used just to differ the base classes, for example
         * decorated name for a km::Table with name "Person" is Table[Person] and for a km::BasicView it will be BasicView[Person].
         *
         * This class doesn't provide any setter for table name, decorated_name and sorting order. Because some classes don't support it.
         */
        AbstractTable(const std::string &table_name, const std::string &decorated_name, SortingOrder sorting_order = SortingOrder::ASCENDING);

        /**
         * @brief Returns the name of the table.
         */
        const std::string &getName() const;

        /**
         * @brief Returns the sorting order.
         */
        SortingOrder getSortingOrder() const;

        /**
         * @brief Returns all views that is installed on this current table.
         */
        const std::vector<AbstractView *> &getDependentViews() const;

        /**
         * @brief Get whether sorting is paused.
         * If previously pauseSorting() was called then it is true, false otherwise.
         */
        bool isSortingPaused();

        /**
         * @brief Hints derived classes to pause sorting.
         *
         * Hints derived classes that no sorting should be done while insertion or setting
         * data, call resumeSorting() to resort the table. This function is designed
         * specifically for @ref Table class. In case if a huge number of insertion is to
         * be done and keeping the table sorted on each insertion can be costly. To overcome
         * it, this function should be used and after end of insertion resumeSorting() should
         * be called.
         *
         * @note It is not necessary to follow this rule for custom class, you can omit
         * it if you want. @ref BasicView class also doesn't follow it as there is no
         * insertion method in @ref BasicView class.
         */
        void pauseSorting();

        /**
         * @brief Resumes sorting.
         *
         * If sorting was paused by calling pauseSorting() then calling this function will
         * reset m_no_sorting to false and call the @b virtual sort() function.
         */
        void resumeSorting();

        /**
         * @brief Get the decorated name.
         *
         * It can be used to identify if an instance is a table or view. A @ref km::Table object
         * uses Table[table_name] format and a @ref km::BasicView object uses BasicView[view_name] format.
         */
        const std::string &getDecoratedName() const;

        /**
         * @brief Returns the index of sorting column set by setKeyColumn().
         */
        IndexType getKeyColumn() const;

        /**
         * @brief Set epsilon on a column.
         *
         * If column has type DataType::FLOAT32 or DataType::FLOAT64 then epsilon
         * will be set on the columnn and will be used in searching. If column doesn't exist or it has non
         * floating point type then it will have no effect. The default function doesn't do anything. It is
         * the derived class's responsibility to tackel it.
         *
         * @see AbstractColumn::setEpsilon
         */
        virtual void setEpsilon(const std::string &column_name, const Variant &epsilon);

        /**
         * @brief sorts the table.
         *
         * In @ref km::Table class it uses the first column as key and sorts the table according to the sorting order.
         * While in @ref km::BasicView class it uses the current key column and sorting order set by user. This default
         * function doesn't do anything. It is called by resumeSorting() function.
         */
        virtual void sort();

        /**
         * @brief reserves space for storing @a row_count rows.
         *
         * This is useful for insertion of large rows. The default implementation doesn't do anything.
         */
        virtual void reserve(SizeType row_count);

        /**
         * @brief sets data at [row_index,column_index] field of the table.
         *
         * The default implementation doesn't do anything.
         */
        virtual bool setData(IndexType row_index, IndexType column_index, const Variant &data);

        /**
         * @brief Sets @a display_name as display name of the column at @a column_index .
         *
         * The default implementation doesn't do anything.
         */
        virtual void setDisplayName(const std::string &display_name, IndexType column_index);

        /**
         * @brief Returns display name of the column at @a column_index .
         */
        virtual std::string getDisplayName(IndexType column_index) const;

        /**
         * @brief Finds column by the given @a column_name .
         *
         * Returns index and data type of the column if it exists else nothing.
         */
        virtual std::optional<std::pair<IndexType, DataType>> findColumn(const std::string &column_name) const = 0;

        /**
         * @brief Returns column name and data type of the column at @a column_index .
         *
         * If @a column_index is out of range it doesn't return anything.
         */
        virtual std::optional<std::pair<std::string, DataType>> columnAt(IndexType column_index) const = 0;

        /**
         * @brief Returns column meta data of the column at @a column_index .
         * @warning @a column_index must be valid for the table else it will be undefined behaviour.
         */
        virtual const ColumnMetaData &getColumnMetaData(IndexType column_index) const = 0;

        /**
         * @brief Returns number of rows in the table.
         */
        virtual SizeType rowCount() const = 0;

        /**
         * @brief Returns number of columns in the table.
         */
        virtual SizeType columnCount() const = 0;

        /**
         * @brief Returns data at @a [row_index,column_index] with bound checking.
         *
         * @a row_index and @a column_index are relative to the current class. If any of them is out of bound
         * then it is empty value.
         */
        virtual std::optional<Variant> getData(IndexType row_index, IndexType column_index) const = 0;

        /**
         * @brief Returns data at @a [row_index,column_index] with no bound checking.
         *
         * @a row_index and @a column_index are relative to the current class.
         *
         * @warning If @a row_index and/or @a column_index are out of bound then it is undefined behaviour.
         */
        virtual Variant getDataWC(IndexType row_index, IndexType column_index) const = 0;

        /**
         * @brief destructor.
         */
        virtual ~AbstractTable() = default;

    protected:
        /**
         * @brief Sets @a data at [ @a row_index , @a column_index ] without bound checking.
         *
         * If @a data has different data type then the column at @a column_index then
         * it will throw std::bad_variant_access.
         *
         * @warning @a row_index and/or @a column_index are out of bound then it is undefined behaviour.
         *
         * @throws std::bad_variant_access.
         */
        virtual void setDataWC(IndexType row_index, IndexType column_index, const Variant &data);

        /**
         * @brief Sets @a m_key_column to @a key_column.
         */
        void setKeyColumn(IndexType key_column);

        /**
         * @brief Returns whether events should be notified to dependent views or not.
         */
        bool shouldProcessEvent() const;

        /**
         * @brief Pauses event/signal processing (stops notifying dependent views about changes).
         */
        void pauseEventProcessing();

        /**
         * @brief Resumes event/signal processing.
         *
         * It is derived class's responsibility to emit events/signals to notify changes/modification
         * of this table to the dependent views. It can be done by calling Event functions.
         */
        void resumeEventProcessing();

        /**
         * @brief Adds @a view to the dependent view list.
         *
         * After a view is installed on this table/view, the view will be notified for every changes in
         * this table/view. A View can have a child View so in View's slots, it should call these events
         * to notify their child views.
         */
        void installView(AbstractView *view);

        /**
         * @brief Finds @a view in the dependent view list and removes it.
         */
        void uninstallView(AbstractView *view);

        /**
         * @brief Notifies dependent views that any data in the table is altered.
         *
         * This function should be called from @ref setData, @a row_index and @a column_index are the same indices
         * passed to @ref setData function. @a old_data is the previous data.
         *
         * When data is changed, dependent views may need to restructure rows and columns. For example, if a view holds rows
         * only if age >= 18 and any element of age column is changed from 6 to 26, view will add this
         * row and update itself.
         *
         * @note This must be called after the data is updated. If @ref shouldProcessEvent() returns true
         * then it will call @ref AbstractView::dataUpdated function.
         */
        KM_SIGNAL void dataUpdateEvent(IndexType row_index, IndexType column_index, const Variant &old_data);

        /**
         * @brief Notifies dependent views that a row is inserted.
         *
         * This function should be called when a row is inserted (e.g. km::Table::insertRow function).
         * @a row_index is the index in the current table where the new column is inserted.
         *
         * @note This must be called after the row insertion is done. If @ref shouldProcessEvent() returns true
         * then it will call @ref AbstractView::rowInserted function.
         */
        KM_SIGNAL void rowInsertionEvent(IndexType row_index);

        /**
         * @brief Notifies dependent views that a row is dropped.
         *
         * This function should be called when a row is dropped (e.g. @ref km::Table::dropRow function).
         * @a row_index is the index in the current table where the new column is dropped.
         *
         * @note This must be called after the row dropping is done. If @ref shouldProcessEvent() returns true
         * then it will call @ref AbstractView::rowDropped function.
         */
        KM_SIGNAL void rowDropEvent(IndexType row_index);

        /**
         * @brief Notifies dependent views that the whole table is restructured.
         *
         * It notifies all the views that they need to be refreshed.
         *
         * @note If @ref shouldProcessEvent() returns true then it will call @ref AbstractView::refresh function.
         */
        KM_SIGNAL void refreshEvent();

        /**
         * @brief Notifies dependent views that table is sorted in reverse order but with same column.
         *
         * @note If @ref shouldProcessEvent() returns true then it will call @ref AbstractView::sourceReversed
         * function.
         */
        KM_SIGNAL void sourceReversedEvent();

        /**
         * @brief Notifies dependent views that table is sorted with different column and may be with opposit order.
         *
         * @note If @ref shouldProcessEvent() returns true then it will call @ref AbstractView::sourceSorted function.
         * function. Basically at current time it is same as calling refreshEvent for some reason.
         */
        KM_SIGNAL void sourceSortedEvent();

        /**
         * @brief Notifies dependent views that all elements of column are transformed.
         *
         * When whole column is changed it is better to call this function. It is used by
         * @ref km::Table::transformColumn function. @a column_index is the index of the column.
         *
         * @note This function must be called after transformation of the column. If @ref shouldProcessEvent()
         * returns true then it will call @ref AbstractView::columnTransformed function.
         */
        KM_SIGNAL void columnTransformedEvent(IndexType column_index);

        /**
         * @brief Notifies dependent views that source is destructed.
         *
         * This functions notifies all the dependent views that it is going to be destructed so all columns should
         * uninstall itself from it and should not refer to a destructed class.
         *
         * @note This function must be called @b before the destructing the elements. It should be called from base
         * class destructor. It doesn't depend on @ref shouldProcessEvent() function.
         */
        KM_SIGNAL void aboutToDestruct();

    protected:
        std::string m_name;           /// name of the table/view.
        std::string m_decorated_name; /// decorated name.
        SortingOrder m_sorder;        /// sorting order of table/view.

    private:
        bool m_no_sorting;                             /// sorting order of the table or view.
        bool m_process_event;                          /// holds information if event processing is paused.
        std::vector<AbstractView *> m_dependent_views; /// Views that depends on this table/view
        IndexType m_key_column;                        /// index of sorting column.

        // friend functions and classes
        friend void ::km::parse::evaluateFormula(parse::ConstTokenContainerRef, AbstractTable *, IndexType, IndexType, IndexType);
        friend class ::km::AbstractView;
    };

    inline AbstractTable::AbstractTable(const std::string &table_name, const std::string &decorated_name, SortingOrder sorting_order)
        : m_name(table_name),
          m_decorated_name(decorated_name),
          m_sorder(sorting_order),
          m_no_sorting(false),
          m_process_event(true),
          m_key_column(0)
    {
        //
    }

    inline const std::string &AbstractTable::getName() const
    {
        return m_name;
    }

    inline void AbstractTable::sort()
    {
        //
    }

    inline SortingOrder AbstractTable::getSortingOrder() const
    {
        return m_sorder;
    }

    inline const std::string &AbstractTable::getDecoratedName() const
    {
        return m_decorated_name;
    }

    inline void AbstractTable::setEpsilon([[maybe_unused]] const std::string &column_name, [[maybe_unused]] const Variant &epsilon)
    {
    }

    inline void AbstractTable::reserve([[maybe_unused]] SizeType row_count)
    {
    }

    inline bool AbstractTable::setData([[maybe_unused]] IndexType row_index, [[maybe_unused]] IndexType column_index, [[maybe_unused]] const Variant &data)
    {
        return false;
    }

    inline void AbstractTable::setDataWC([[maybe_unused]] IndexType row_index, [[maybe_unused]] IndexType column_index, [[maybe_unused]] const Variant &data)
    {
    }

    inline const std::vector<AbstractView *> &AbstractTable::getDependentViews() const
    {
        return m_dependent_views;
    }

    inline bool AbstractTable::isSortingPaused()
    {
        return m_no_sorting;
    }

    inline void AbstractTable::pauseSorting()
    {
        pauseEventProcessing();
        m_no_sorting = true;
    }

    inline void AbstractTable::resumeSorting()
    {
        if (isSortingPaused())
        {
            resumeEventProcessing();
            sort();
            m_no_sorting = false;
        }
    }

    inline void AbstractTable::setDisplayName([[maybe_unused]] const std::string &display_name, [[maybe_unused]] IndexType column_index)
    {
        //
    }
    inline std::string AbstractTable::getDisplayName([[maybe_unused]] IndexType column_index) const
    {
        return {};
    }

    inline void AbstractTable::setKeyColumn(IndexType key_column)
    {
        m_key_column = key_column;
    }

    inline IndexType AbstractTable::getKeyColumn() const
    {
        return m_key_column;
    }

    inline bool AbstractTable::shouldProcessEvent() const
    {
        return m_process_event;
    }

    inline void AbstractTable::pauseEventProcessing()
    {
        m_process_event = false;
    }

    inline void AbstractTable::resumeEventProcessing()
    {
        m_process_event = true;
    }

    inline void AbstractTable::installView(AbstractView *view)
    {
        m_dependent_views.push_back(view);
    }

    inline void AbstractTable::uninstallView(AbstractView *view)
    {
        auto it = std::find(m_dependent_views.begin(), m_dependent_views.end(), view);
        if (it != m_dependent_views.end())
            m_dependent_views.erase(it);
    }

    void createColumn(AbstractColumnPtr_ &column_ptr, const std::string &column_name, const std::string &display_name, DataType data_type);

    /**
     * @brief Returns whether @b column_name can be used for column name or not.
     *
     * A valid column name must start with alphabets or underscore (_). Followed by zero or
     * more alphabets and digits. Spaces are not allowed. If @b column_name satisfies
     * all these conditions then true is returned, false otherwise.
     *
     * @note This is not nessessary to follow for your custom classes, but to work with dynamic
     * Formula, column name must satisfy these condition.
     */
    bool isValidColumnName(const std::string &column_name);

    /**
     * @brief Returns whether @b table_name can be used for table name or not.
     *
     * A valid table name must start with alphabets or underscore (_). Followed by zero or
     * more alphabets, digits and signs except '\' and '/'. Space is also allowed but other whitespace
     * characters such as tabs '\t' and newline '\n' are not allowed. If @b table_name satisfies
     * all these conditions then true is returned, false otherwise.
     */
    bool isValidTableName(const std::string &table_name);

}

#endif // KMTABLELIB_KMT_ABSTRACTTABLE_HPP
