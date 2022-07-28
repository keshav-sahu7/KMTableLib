#include "BasicView.hpp"

#include <stdexcept>
#include <algorithm>
#include <numeric> //std::iota

#include "UniqueNameContainer.h"
#include "ErrorHandler.hpp"
#include "KException.h"
#include "debug.h"

namespace km
{
    void BasicView::checkPreConditions(const std::string &view_name, AbstractTable *source_table, const std::string &formula)
    {
        if (source_table->isSortingPaused())
        {
            err::addLogMsg(err::LogMsg("BasicView ~ InvalidArgs") << "`" << source_table->getDecoratedName() << "` passed to create view is not in ready state.");
            throw KM_IA_EXCEPTION("BasicView ~ invalid table");
        }
        else if (source_table->columnCount() == 0)
        {
            err::addLogMsg(err::LogMsg("BasicView ~ NoColumn") << "`" << source_table->getDecoratedName() << "` passed to create view `" << view_name << "` is empty.");
            throw KM_IA_EXCEPTION("BasicView ~ empty table");
        }

        if (!isValidTableName(view_name))
        {
            err::addLogMsg(err::LogMsg("BasicView ~ Name") << "Invalid view name `" << view_name << "`.");
            throw KM_IA_EXCEPTION("BasicView ~ invalid name");
        }

        else if (!formula.empty())
        {
            err::LockLogFileHandler locker;
            if (!parse::getCheckedToken(formula, m_filtered_token, source_table, DataType::BOOLEAN))
            {
                locker.resume();
                err::addLogMsg(err::LogMsg("BasicView ~ FormulaEvaluation") << "Formula `" << formula << "` passed to filter the `"
                                                                            << source_table->getDecoratedName() << "` in view `" << view_name << "` is invalid.");
                throw KM_IA_EXCEPTION("BasicView ~ invalid formula");
            }
        }
    }

    static std::vector<IndexType> getIndicesOf(AbstractTable *table, const std::vector<std::string> &column_names, std::string &error_column)
    {
        std::vector<IndexType> ivec;
        if (column_names.empty())
        {
            for (IndexType i = 0, count = table->columnCount(); i < count; ++i)
                ivec.push_back(i);
        }
        else
        {
            UniqueNameContainer u_container(column_names);
            for (const auto &column_name : u_container.getUniqueList())
            {
                if (auto opt = table->findColumn(column_name))
                {
                    ivec.push_back(opt.value().first);
                }
                else
                {
                    error_column = column_name;
                    ivec.clear();
                    break;
                }
            }
        }
        return ivec;
    }

    BasicView::BasicView(const std::string &view_name, AbstractTable *source_table, const std::vector<std::string> &column_names, const std::string &formula, std::string sort_by, SortingOrder s_order)
        : AbstractView(view_name, "BasicView[" + view_name + "]", s_order),
          m_exp(formula)
    {
        checkPreConditions(view_name, source_table, formula);
        std::string non_existing_column;
        m_selected_columns = getIndicesOf(source_table, column_names, non_existing_column);
        if (m_selected_columns.empty())
        {
            err::addLogMsg(err::LogMsg("BasicView ~ InvalidArgs") << "Column `" << non_existing_column << "` does not exist in `" << source_table->getDecoratedName() << "`.");
            throw KM_IA_EXCEPTION("BasicView ~ Column doesn't exist");
        }

        if (!sort_by.empty())
        {
            auto index_sb = source_table->findColumn(sort_by);
            std::vector<IndexType>::iterator it;
            if (!index_sb || (it = std::find(m_selected_columns.begin(), m_selected_columns.end(), index_sb.value().first)) == m_selected_columns.end())
            {
                err::addLogMsg(err::LogMsg("BasicView ~ InvalidArgs") << "Column `" << sort_by << "` does not exist in selected columns in the view.");
                throw KM_IA_EXCEPTION("BasicView ~ Column doesn't exist");
            }
            setKeyColumn(std::distance(m_selected_columns.begin(), it));
        }
        else
        {
            sort_by = source_table->columnAt(m_selected_columns.front()).value().first;
            setKeyColumn(0);
        }

        setSourceTable(source_table);

        if (formula.empty()) // select all
        {
            m_indices.resize(source_table->rowCount());
            std::iota(m_indices.begin(), m_indices.end(), 0);
        }
        else
            parse::filter(m_filtered_token, m_indices, source_table);

        sortBy(sort_by, s_order);
    }

    bool BasicView::setViewName(const std::string &view_name)
    {
        if (!isValidTableName(view_name))
            return false;
        m_name = view_name;
        return true;
    }

    std::string BasicView::getFilterFormula() const
    {
        return m_exp;
    }

    std::optional<std::pair<IndexType, DataType>> BasicView::findColumn(const std::string &column_name) const
    {
        const SizeType column_count = columnCount();
        std::pair<std::string, DataType> col;
        for (IndexType i = 0; i < column_count; ++i)
        {
            col = getSourceTable()->columnAt(m_selected_columns[i]).value();
            if (col.first == column_name)
            {
                return std::make_pair(i, col.second);
            }
        }
        return {}; // column not found
    }

    const ColumnMetaData &BasicView::getColumnMetaData(IndexType column_index) const
    {
        return getSourceTable()->getColumnMetaData(m_selected_columns[column_index]);
    }

    SizeType BasicView::rowCount() const
    {
        return m_indices.size();
    }

    SizeType BasicView::columnCount() const
    {
        return m_selected_columns.size();
    }

    std::optional<std::pair<std::string, DataType>> BasicView::columnAt(IndexType column_index) const
    {
        if (column_index >= columnCount())
            return {};
        return getSourceTable()->columnAt(m_selected_columns[column_index]);
    }

    std::optional<Variant> BasicView::getData(IndexType row_index, IndexType column_index) const
    {
        const SizeType column_count = columnCount();
        const SizeType row_count = rowCount();

        if (row_index >= row_count || column_index >= column_count)
            return {};
        return getSourceTable()->getData(m_indices[row_index], m_selected_columns[column_index]);
    }

    Variant BasicView::getDataWC(IndexType row_index, IndexType column_index) const
    {
        return getSourceTable()->getDataWC(m_indices[row_index], m_selected_columns[column_index]);
    }

    void BasicView::sortBy(SortingOrder s_order)
    {
        if (m_sorder != s_order)
        {
            std::reverse(m_indices.begin(), m_indices.end());
            m_sorder = s_order;
            KM_EMIT sourceReversedEvent();
        }
    }

    void BasicView::sortBy(const std::string &column_name)
    {
        auto found = findColumn(column_name);
        if (!found)
            return;
        IndexType idx = found.value().first;
        DataType type = found.value().second;
        IndexType original_clm_index = m_selected_columns[idx];
        auto fnc = getSortingOrder() == SortingOrder::ASCENDING ? isLessComparatorFor(type) : isGreaterComparatorFor(type);
        auto source_table = getSourceTable();

        std::stable_sort(m_indices.begin(), m_indices.end(), [fnc, source_table, original_clm_index](IndexType index1, IndexType index2)
                         { return fnc(source_table->getDataWC(index1, original_clm_index), source_table->getDataWC(index2, original_clm_index)); });
        setKeyColumn(idx);
        KM_EMIT sourceSortedEvent();
    }

    void BasicView::sortBy(const std::string &column_name, SortingOrder s_order)
    {
        m_sorder = s_order;
        sortBy(column_name);
    }

    void BasicView::refresh()
    {
        m_indices.clear();
        if (m_exp.empty())
        {
            m_indices.resize(getSourceTable()->rowCount());
            std::iota(m_indices.begin(), m_indices.end(), 0);
        }
        else
            filter(m_filtered_token, m_indices, getSourceTable());

        pauseEventProcessing();
        if (auto column_info = columnAt(getKeyColumn()))
            sortBy(column_info.value().first, getSortingOrder());
        resumeEventProcessing();

        KM_EMIT refreshEvent();
    }

    IndexType BasicView::mapToLocal(IndexType src_row_index)
    {
        if (!getSourceTable())
            return INVALID_INDEX;
        return mapToLocal(src_row_index, getSourceTable()->getDataWC(src_row_index, m_selected_columns[getKeyColumn()]));
    }

    IndexType BasicView::mapToLocal(IndexType src_row_index, const Variant &key_data)
    {
        int64_t lower = 0, upper = static_cast<int64_t>(m_indices.size()) - 1;
        auto data_type = columnAt(getKeyColumn()).value().second;
        auto fnc = (getSortingOrder() == SortingOrder::ASCENDING) ? (isLessComparatorFor(data_type)) : (isGreaterComparatorFor(data_type));
        while (lower <= upper)
        {
            long long int mid = (lower + upper) / 2;
            if (m_indices[mid] == src_row_index)
                return mid;
            else if (fnc(key_data, getDataWC(mid, getKeyColumn())))
                upper = mid - 1;
            else
                lower = mid + 1;
        }
        return INVALID_INDEX;
    }

    IndexType BasicView::insertablePosition(const Variant &data)
    {
        if (m_indices.empty())
            return 0;
        auto data_type = columnAt(getKeyColumn()).value().second;
        auto source_clm_index = m_selected_columns[getKeyColumn()];
        auto fnc = getSortingOrder() == SortingOrder::ASCENDING ? isLessComparatorFor(data_type) : isGreaterComparatorFor(data_type);
        auto source_table = getSourceTable();
        auto it = std::upper_bound(m_indices.begin(), m_indices.end(), data, [fnc, source_clm_index, source_table](const Variant &value, IndexType middle)
                                   { return fnc(value, source_table->getDataWC(middle, source_clm_index)); });
        return std::distance(m_indices.begin(), it);
    }

    KM_SLOT void BasicView::dataUpdated(IndexType src_row_index, IndexType src_column_index, const Variant &old_data)
    {
        bool should_filter = std::any_of(m_filtered_token.begin(), m_filtered_token.end(), [src_column_index](const parse::Token &token)
                                         { return (token.token_type == 0x0040 && token.element.asColInfo().index == src_column_index); });
        bool change_in_key_column = (src_column_index == m_selected_columns[getKeyColumn()]);
        IndexType local_row_index = (change_in_key_column) ? mapToLocal(src_row_index, old_data) : mapToLocal(src_row_index);
        bool row_exists = (local_row_index != INVALID_INDEX);
        bool filter_result = should_filter ? parse::filter(m_filtered_token, getSourceTable(), src_row_index) : false;

        // call nested event
        if ((!should_filter || filter_result) && row_exists && !change_in_key_column)
        {
            auto it = std::find(m_selected_columns.begin(), m_selected_columns.end(), src_column_index);
            if (it != m_selected_columns.end())
                KM_EMIT dataUpdateEvent(local_row_index, *it, old_data);
            return;
        }
        // return/skip
        if ((!should_filter || !filter_result) && !row_exists)
        {
            return;
        }
        // insert
        if (should_filter && filter_result && !row_exists)
        {
            Variant new_data = getSourceTable()->getDataWC(src_row_index, m_selected_columns[getKeyColumn()]);
            auto new_pos = insertablePosition(new_data);
            m_indices.insert(m_indices.begin() + new_pos, src_row_index);
            KM_EMIT rowInsertionEvent(new_pos);
            return;
        }
        // remove
        if (should_filter && !filter_result && row_exists)
        {
            m_indices.erase(m_indices.begin() + local_row_index);
            KM_EMIT rowDropEvent(local_row_index);
            return;
        }
        // insert/remove
        if ((!should_filter || filter_result) && row_exists && change_in_key_column)
        {
            m_indices.erase(m_indices.begin() + local_row_index);
            KM_EMIT rowDropEvent(local_row_index);
            Variant new_data = getSourceTable()->getDataWC(src_row_index, m_selected_columns[getKeyColumn()]);
            auto new_pos = insertablePosition(new_data);
            m_indices.insert(m_indices.begin() + new_pos, src_row_index);
            KM_EMIT rowInsertionEvent(new_pos);
            return;
        }
    }

    KM_SLOT void BasicView::rowInserted(IndexType row_index)
    {
        std::for_each(m_indices.begin(), m_indices.end(),
                      [row_index](IndexType &index)
                      {
                          if (index >= row_index)
                              ++index;
                      });
        if (!m_filtered_token.empty() && !parse::filter(m_filtered_token, getSourceTable(), row_index))
            return;
        Variant data = getSourceTable()->getDataWC(row_index, m_selected_columns[getKeyColumn()]);
        IndexType view_row_index = insertablePosition(data);
        m_indices.insert(m_indices.begin() + view_row_index, row_index);
        KM_EMIT rowInsertionEvent(view_row_index);
    }

    KM_SLOT void BasicView::rowDropped(IndexType row_index)
    {
        IndexType view_row_index = mapToLocal(row_index);
        if (view_row_index != INVALID_INDEX)
            m_indices.erase(m_indices.begin() + view_row_index);
        std::for_each(m_indices.begin(), m_indices.end(), [row_index](IndexType &index)
                      {
            if(index > row_index) --index; });
        KM_EMIT rowDropEvent(view_row_index);
    }

    KM_SLOT void BasicView::sourceSorted()
    {
        refresh();
    }

    KM_SLOT void BasicView::sourceReversed()
    {
        SizeType column_count = getSourceTable()->columnCount();
        for (auto &index : m_indices)
            index = column_count - 1 - index;
    }

    KM_SLOT void BasicView::columnTransformed(IndexType column_index)
    {
        if (std::find(m_selected_columns.begin(), m_selected_columns.end(), column_index) != m_indices.end()) // contains the column.
        {
            refresh(); // calls sourceRefreshEvent()
        }
        else if (m_selected_columns[getKeyColumn()] == column_index) // if it was the base column for the view
        {
            sortBy(columnAt(getKeyColumn()).value().first); // calls sourceSortedEvent()
        }
    }

    KM_SLOT void BasicView::sourceRefreshed()
    {
        refresh();
    }

    KM_SLOT void BasicView::sourceAboutToBeDestructed()
    {
        KM_EMIT aboutToDestruct();
        m_indices.clear();
        m_selected_columns.clear();
        setKeyColumn(INVALID_INDEX);
        setSourceTable(nullptr);
    }
} // namespace km
