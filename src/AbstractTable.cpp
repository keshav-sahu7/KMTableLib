#include "AbstractTable.hpp"
#include "AbstractView.hpp"

namespace km
{
    KM_SIGNAL void AbstractTable::dataUpdateEvent(IndexType row_index, IndexType column_index, const Variant &old_data)
    {
        if (shouldProcessEvent())
            for (auto &view : m_dependent_views)
                view->dataUpdated(row_index, column_index, old_data);
    }
    KM_SIGNAL void AbstractTable::rowInsertionEvent(IndexType row_index)
    {
        if (shouldProcessEvent())
            for (auto &view : m_dependent_views)
                view->rowInserted(row_index);
    }
    KM_SIGNAL void AbstractTable::rowDropEvent(IndexType row_index)
    {
        if (shouldProcessEvent())
            for (auto &view : m_dependent_views)
                view->rowDropped(row_index);
    }

    KM_SIGNAL void AbstractTable::refreshEvent()
    {
        if (shouldProcessEvent())
            for (auto &view : m_dependent_views)
                view->refresh();
    }

    KM_SIGNAL void AbstractTable::sourceReversedEvent()
    {
        if (shouldProcessEvent())
            for (auto &view : m_dependent_views)
                view->sourceReversed();
    }
    KM_SIGNAL void AbstractTable::sourceSortedEvent()
    {
        if (shouldProcessEvent())
            for (auto &view : m_dependent_views)
                view->sourceSorted();
    }
    KM_SIGNAL void AbstractTable::columnTransformedEvent(IndexType column_index)
    {
        if (shouldProcessEvent())
            for (auto &view : m_dependent_views)
                view->columnTransformed(column_index);
    }

    KM_SIGNAL void AbstractTable::aboutToDestruct()
    {
        for (auto &view : m_dependent_views)
            view->sourceAboutToBeDestructed();
    }
    void createColumn(AbstractColumnPtr_ &column_ptr, const std::string &column_name, const std::string &display_name, DataType data_type)
    {
        switch (data_type)
        {
        case DataType::INT32:
            column_ptr = new Column<KInt32>(column_name, display_name);
            break;
        case DataType::INT64:
            column_ptr = new Column<KInt64>(column_name, display_name);
            break;
        case DataType::FLOAT32:
            column_ptr = new Column<KFloat32>(column_name, display_name);
            break;
        case DataType::FLOAT64:
            column_ptr = new Column<KFloat64>(column_name, display_name);
            break;
        case DataType::STRING:
            column_ptr = new Column<KString>(column_name, display_name);
            break;
        case DataType::BOOLEAN:
            column_ptr = new Column<KBoolean>(column_name, display_name);
            break;
        case DataType::DATE:
            column_ptr = new Column<KDate>(column_name, display_name);
            break;
        case DataType::DATE_TIME:
            column_ptr = new Column<KDateTime>(column_name, display_name);
            break;
        default:
            column_ptr = nullptr;
        }
    }

    bool isValidColumnName(const std::string &column_name)
    {
        if (column_name.empty())
            return false;
        char c = column_name.front();
        if (!(std::isalpha(c) || c == '_'))
            return false;
        for (char c : column_name)
        {
            if (!(std::isalnum(c) || c == '_'))
                return false;
        }
        return true;
    }

    bool isValidTableName(const std::string &table_name)
    {
        if (table_name.empty())
            return false;
        char c = table_name.front();
        if (!(std::isalpha(c) || c == '_'))
            return false;
        for (char c : table_name)
        {
            if (!std::isprint(c) || (std::isspace(c) && c != ' ') || c == '/' || c == '\\')
                return false;
        }
        return true;
    }

}
