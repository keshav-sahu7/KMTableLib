#include "Table.hpp"

#include <algorithm>
#include <stdexcept>
#include <memory>

#include "ErrorHandler.hpp"
#include "KException.h"
#include "debug.h"

namespace km
{
    namespace
    {
        struct column_error_t
        {
            int error_code = 0;
            union
            {
                struct
                {
                    IndexType first, second; // error_code = 1
                } dplc;
                IndexType inv_col_index; // error_code = 2
                IndexType inv_dt_index;  // error_code = 3
            };
        };

        column_error_t validateColumnList(const std::vector<ColumnMetaData> &vec)
        {
            column_error_t error;
            auto begin_it = vec.begin();
            auto end_it = vec.end();
            for (auto current_it = begin_it; current_it != end_it; ++current_it)
            {
                if (!isValidColumnName(current_it->column_name))
                {
                    error.error_code = 2;
                    error.inv_col_index = std::distance(begin_it, current_it);
                    break;
                }
                auto found_it = std::find_if(current_it + 1, end_it, [current_it](const auto &a)
                                             { return current_it->column_name == a.column_name; });
                if (found_it != end_it)
                {
                    error.error_code = 1;
                    error.dplc.first = std::distance(begin_it, current_it);
                    error.dplc.second = std::distance(begin_it, found_it);
                    break;
                }

                if (!isValidDataType(current_it->data_type))
                {
                    error.error_code = 3;
                    error.inv_dt_index = std::distance(begin_it, current_it);
                    break;
                }
            }
            return error;
        }
    } // namespace

    Table::Table(
        const std::string &table_name,
        const std::vector<ColumnMetaData> &column_list,
        SortingOrder sorting_order)
        : AbstractTable(table_name, "Table[" + table_name + "]", sorting_order),
          m_base_column(nullptr),
          m_comparator((sorting_order == SortingOrder::ASCENDING) ? &AbstractColumn::isLess : &AbstractColumn::isGreater),
          m_mfst(64)
    {
        if (!isValidTableName(table_name))
        {
            err::addLogMsg(err::LogMsg("Table ~ Name") << "Invalid table name `" << table_name << "`.");
            throw KM_IA_EXCEPTION("Table ~ invalid name");
        }

        auto val_col = validateColumnList(column_list);
        if (val_col.error_code)
        {
            if (val_col.error_code == 1) // duplicate column
            {
                err::addLogMsg(err::LogMsg("Table ~ Name") << "Duplicate column name `" << column_list[val_col.dplc.first].column_name << "` found at index "
                                                           << KInt64(val_col.dplc.first) << " and at index " << KInt64(val_col.dplc.second) << " given to create table `"
                                                           << table_name << "`.");
                throw KM_IA_EXCEPTION("Table ~ duplicate column name");
            }
            else if (val_col.error_code == 2) // invalid column name occured
            {
                err::addLogMsg(err::LogMsg("Table ~ Name") << "Invalid column name `" << column_list[val_col.inv_col_index].column_name
                                                           << "` at index " << KInt64(val_col.inv_col_index) << " given to create table `" << table_name << "`.");
                throw KM_IA_EXCEPTION("Table ~ invalid column name");
            }
            else if (val_col.error_code == 3) // invalid datatype
            {
                err::addLogMsg(err::LogMsg("Table ~ DataType") << "Column `" << column_list[val_col.inv_dt_index].column_name << "` passed to create table `"
                                                               << table_name << "` has invalid data type `undefined`.");
                throw KM_IA_EXCEPTION("Table ~ Invalid data type");
            }
        }

        for (const auto &[column_name, display_name, data_type] : column_list)
        {
            AbstractColumnPtr_ ptr = nullptr;
            createColumn(ptr, column_name, display_name, data_type);
            m_columns.push_back(ptr);
        }

        setKeyColumn(0);
        if (!m_columns.empty())
            m_base_column = m_columns.front();
    }

    Table::~Table()
    {
        KM_EMIT aboutToDestruct();
        for (AbstractColumnPtr_ ptr : m_columns)
            delete ptr;
        m_columns.clear();
    }

    IndexType Table::insertRow(const std::vector<Variant> &values) noexcept
    {
        if (m_columns.empty() || values.size() != m_columns.size())
        {
            err::addLogMsg(err::LogMsg(getDecoratedName() + " ~ InvalidArgs") << "Invalid number of values are given to insert.");
            return INVALID_INDEX;
        }
        try
        {
            IndexType index = 0;
            if (!m_free_space.empty())
            {
                index = m_free_space.back();
                for (IndexType i = 0; i < m_columns.size(); ++i)
                {
                    m_columns[i]->setData(values[i], index); // this may throw
                }
                m_free_space.pop_back();
            }
            else
            {
                index = m_indices.size();
                for (IndexType i = 0, size = m_columns.size(); i < size; ++i)
                {
                    try
                    {
                        m_columns[i]->pushData(values[i]);
                    }
                    catch (const std::exception &e)
                    {
                        for (IndexType j = i; j-- > 0;)
                            m_columns[j]->popData(); // clean the data
                        if (const std::bad_variant_access *ex = dynamic_cast<const std::bad_variant_access *>(&e))
                            throw *ex;
                        else
                            throw e;
                    }
                }
            }
            if (!isSortingPaused())
            {
                auto iterator = std::upper_bound(m_indices.begin(), m_indices.end(), 0,
                                                 [this, index](IndexType /**/, IndexType mid)
                                                 {
                                                     return (m_base_column->*m_comparator)(index, mid);
                                                 });
                IndexType insertion_index = iterator - m_indices.begin();
                m_indices.insert(iterator, index);
                KM_EMIT rowInsertionEvent(insertion_index);
                return insertion_index;
            }
            else
            {
                m_indices.push_back(index);
                return m_indices.size() - 1;
            }
        }
        catch (const std::bad_variant_access & /*e*/)
        {
            err::addLogMsg(err::LogMsg(getDecoratedName() + " ~ DataType") << "Couldn't insert the row, insertion failed due to `type mismatch`.");
        }
        catch (const std::exception &e)
        {
            err::addLogMsg(err::LogMsg(getDecoratedName() + " ~ UnknownException") << "Unknown excepton caught `" << e.what() << "`.");
        }
        return INVALID_INDEX;
    }

    bool Table::dropRow(IndexType row_index)
    {
        const IndexType row_count = rowCount();
        if (row_index >= row_count)
            return false;
        m_free_space.push_back(m_indices[row_index]);
        m_indices.erase(m_indices.begin() + row_index);
        KM_EMIT rowDropEvent(row_index);
        if (m_mfst <= m_free_space.size())
            freeSpace();
        return true;
    }

    bool Table::addColumnE(const ColumnMetaData &column, const std::string &formula)
    {
        if (!validateForNewColumn(column.column_name, column.data_type))
            return false;

        AbstractColumnPtr_ column_ptr = nullptr;
        createColumn(column_ptr, column.column_name, column.display_name, column.data_type);
        // store it in advance, like if it doesn't enter the if block, it will be still part of the column
        m_columns.push_back(column_ptr);
        IndexType row_count = rowCount();
        if (row_count)
        {
            column_ptr->resize(row_count + m_free_space.size());
            std::vector<parse::Token> tokens;
            err::LockLogFileHandler locker;
            if (!parse::getCheckedToken(formula, tokens, this, column.data_type))
            {
                locker.resume();
                err::addLogMsg(err::LogMsg(getDecoratedName() + " ~ ExpressionEvaluator")
                               << "Given formula `" << formula << "` to add new column `" << column.column_name << "` is invalid.");
                delete column_ptr;
                m_columns.pop_back();
                return false;
            }
            parse::evaluateFormula(tokens, this, m_columns.size() - 1, 0, row_count - 1);
        }
        if (m_columns.size() == 1)
        {
            m_base_column = m_columns.front();
            sort();
        }
        return true;
    }

    bool Table::addColumn(const ColumnMetaData &column, const Variant &fill_with)
    {
        if (!validateForNewColumn(column.column_name, column.data_type))
        {
            return false;
        }
        else if ((1U << fill_with.index()) != static_cast<unsigned int>(column.data_type))
        {
            err::addLogMsg(err::LogMsg(getDecoratedName() + " ~ InvalidArgs")
                           << "Couldn't add column `" << column.column_name << "` due to type mismatch. Note passed datatype is `"
                           << column.data_type << "` and passed data `" << fill_with << "` has type `" << dataTypeOf(fill_with) << "`.");
            return false;
        }

        AbstractColumnPtr_ column_ptr = nullptr;
        createColumn(column_ptr, column.column_name, column.display_name, column.data_type);
        IndexType row_count = rowCount();
        column_ptr->resize(row_count + m_free_space.size());
        for (IndexType i : m_indices)
        {
            column_ptr->setData(fill_with, i);
        }
        m_columns.push_back(column_ptr);
        if (m_columns.size() == 1)
        {
            m_base_column = m_columns.front();
            sort();
        }
        return true;
    }

    bool Table::transformColumn(const std::string &column_name, const std::string &formula)
    {
        const auto found_column = findColumn(column_name);
        if (!found_column)
        {
            err::addLogMsg(err::LogMsg(getDecoratedName() + " ~ Name") << "Given column name `" << column_name
                                                                       << "` to transform doesn't exist in this table.");
            return false;
        }
        const auto &[column_index, column_datatype] = found_column.value();
        std::vector<parse::Token> token_vec;
        err::LockLogFileHandler locker;
        if (!parse::getCheckedToken(formula, token_vec, this, column_datatype))
        {
            locker.resume();
            err::addLogMsg(err::LogMsg(getDecoratedName() + " ~ ExpressionEvaluator")
                           << "Given formula `" << formula << "` to transform column `" << column_name << "` is invalid.");
            return false;
        }
        parse::evaluateFormula(token_vec, this, column_index, 0, rowCount() - 1);
        if (column_index == 0)
            sort();
        else
            KM_EMIT columnTransformedEvent(column_index);
        return true;
    }

    std::vector<IndexType> Table::search(const std::string &column_name, const Variant &data) const
    {
        if (!rowCount())
            return {};

        auto found_column = findColumn(column_name);
        if (!found_column || data.index() != indexForDataType(found_column.value().second))
            return {};
        IndexType column_index = found_column.value().first;

        if (column_index == 0) // first column, sorted so binary search will be applied
        {
            return searchInKeyColumn(data);
        }
        else
        {
            std::vector<IndexType> result_indices;
            const SizeType row_count = rowCount();
            AbstractColumnPtr_ column_ptr = m_columns[column_index];
            for (IndexType index = 0; index < row_count; ++index)
            {
                if (column_ptr->isEqualV(m_indices[index], data))
                    result_indices.push_back(index);
            }
            return result_indices;
        }
    }

    std::vector<IndexType> Table::searchInKeyColumn(const Variant &data) const
    {
        if (!rowCount() || data.index() != indexForDataType(m_base_column->getDataType()))
            return {};
        std::vector<IndexType> result_indices;
        // search in key column
        auto comparator = (m_sorder == SortingOrder::ASCENDING) ? &AbstractColumn::isLessV : &AbstractColumn::isGreaterV;
        auto itr = std::lower_bound(m_indices.begin(), m_indices.end(), data, [this, comparator](IndexType index, const Variant &data)
                                    { return (m_base_column->*comparator)(index, data); });
        IndexType index = std::distance(m_indices.begin(), itr);
        if (index == rowCount() && !m_base_column->isEqualV(m_indices[--index], data)) // index points the last possible index
            return {};
        IndexType start_index = index - 1, end_index = index;
        while (start_index != INVALID_INDEX && m_base_column->isEqualV(m_indices[start_index], data))
            --start_index;
        while (end_index < rowCount() && m_base_column->isEqualV(m_indices[end_index], data))
            ++end_index;
        for (; ++start_index != end_index;)
            result_indices.push_back(start_index);
        return result_indices;
    }

    std::optional<std::pair<IndexType, DataType>> Table::findColumn(const std::string &column_name) const
    {
        const SizeType column_count = m_columns.size();
        for (IndexType i = 0; i < column_count; ++i)
        {
            if (m_columns[i]->getName() == column_name)
                return std::make_pair(i, m_columns[i]->getDataType());
        }
        return {};
    }

    std::optional<std::pair<std::string, DataType>> Table::columnAt(IndexType column_index) const
    {
        if (column_index >= columnCount())
            return {};
        else
            return std::make_pair(m_columns[column_index]->getName(), m_columns[column_index]->getDataType());
    }

    const ColumnMetaData &Table::getColumnMetaData(IndexType column_index) const
    {
        return m_columns[column_index]->getMetaData();
    }

    std::optional<Variant> Table::getData(IndexType row_index, IndexType column_index) const
    {
        if (row_index >= rowCount() || column_index >= columnCount())
        {
            return {};
        }
        return m_columns[column_index]->getData(m_indices[row_index]);
    }

    bool Table::setData(IndexType row_index, IndexType column_index, const Variant &data)
    {
        if (column_index == 0 || row_index >= rowCount() || column_index >= columnCount() || DataType(1U << data.index()) != m_columns[column_index]->getDataType())
            return false;
        Variant old_data = m_columns[column_index]->getData(m_indices[row_index]);
        m_columns[column_index]->setData(data, m_indices[row_index]);
        KM_EMIT dataUpdateEvent(row_index, column_index, old_data);
        return true;
    }

    void Table::setEpsilon(const std::string &column_name, const Variant &epsilon)
    {
        const auto found_column = findColumn(column_name);
        if (found_column)
            m_columns[found_column.value().first]->setEpsilon(epsilon);
    }

    void Table::reserve(SizeType row_count)
    {
        m_indices.reserve(row_count);
        for (AbstractColumnPtr_ ptr : m_columns)
            ptr->reserve(row_count);
    }

    void Table::sort()
    {
        std::stable_sort(m_indices.begin(), m_indices.end(), [this](IndexType index1, IndexType index2)
                         { return (m_base_column->*m_comparator)(index1, index2); });
        KM_EMIT refreshEvent();
    }

    void Table::setDisplayName(const std::string &display_name, IndexType column_index)
    {
        m_columns[column_index]->setDisplayName(display_name);
    }

    std::string Table::getDisplayName(IndexType column_index) const
    {
        return column_index < columnCount() ? m_columns[column_index]->getDisplayName() : std::string();
    }

    void Table::setDataWC(IndexType row_index, IndexType column_index, const Variant &data)
    {
        m_columns[column_index]->setData(data, m_indices[row_index]);
    }

    void Table::freeSpace()
    {
        SizeType row_count = rowCount();
        SizeType column_count = columnCount();

        for (IndexType column_index = 0; column_index < column_count; ++column_index)
        {
            AbstractColumnPtr_ column_ptr = m_columns[column_index];
            // create clone of type (it doesn't hold any data, it holds only column name)
            AbstractColumnPtr_ tmp_column = column_ptr->getSameTypeColumn(column_ptr->getName());
            tmp_column->reserve(row_count);
            for (IndexType i = 0; i < row_count; ++i)
            {
                tmp_column->pushData(column_ptr->getData(m_indices[i]));
            }
            delete m_columns[column_index];
            m_columns[column_index] = tmp_column;
        }
        m_base_column = m_columns.front();
        for (IndexType i = 0; i < row_count; ++i)
            m_indices[i] = i;
        m_free_space.clear();
    }

    bool Table::validateForNewColumn(const std::string &column, DataType data_type)
    {
        if (!isValidColumnName(column))
        {
            err::addLogMsg(err::LogMsg(getDecoratedName() + " ~ Name")
                           << "Invalid column name `" << column << "` is passed to add new column to the table. Ignoring it, column is not added.");
            return false;
        }
        else if (findColumn(column).has_value())
        {
            err::addLogMsg(err::LogMsg(getDecoratedName() + " ~ Name")
                           << "Column name `" << column << "` passed to add new column, already exists in this table. Ignoring it, column is not added.");
            return false;
        }
        else if (!isValidDataType(data_type))
        {
            err::addLogMsg(err::LogMsg(getDecoratedName() + " ~ DataType")
                           << "DataType passed to add new column is altered and not a valid type");
            return false;
        }
        return true;
    }

}
