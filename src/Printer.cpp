#include "Printer.hpp"

#include <ostream>
#include "ErrorHandler.hpp"

namespace km
{
    namespace
    {

        template <typename Type_>
        void default_print_function(const Variant &data, std::ostream &stream)
        {
            stream << data.as<Type_>();
        }

        template <>
        void default_print_function<KBoolean>(const Variant &data, std::ostream &stream)
        {
            stream << ((data.asBoolean()) ? ("True") : ("False"));
        }

        void default_header_printer(const std::vector<std::string> &headers, std::ostream &stream, const char *del)
        {
            if (!headers.empty())
            {
                stream << headers[0];
                for (IndexType c_index = 1, c_count = headers.size(); c_index < c_count; ++c_index)
                {
                    stream << del << headers[c_index];
                }
            }
        }

    } // namespace

    std::function<void(const Variant &, std::ostream &)> getDefaultPrinter(DataType data_type)
    {
        using dt = DataType;
        switch (data_type)
        {
        case dt::INT32:
            return default_print_function<KInt32>;
        case dt::INT64:
            return default_print_function<KInt64>;
        case dt::FLOAT32:
            return default_print_function<KFloat32>;
        case dt::FLOAT64:
            return default_print_function<KFloat64>;
        case dt::STRING:
            return default_print_function<KString>;
        case dt::BOOLEAN:
            return default_print_function<KBoolean>;
        case dt::DATE:
            return default_print_function<KDate>;
        case dt::DATE_TIME:
            return default_print_function<KDateTime>;
        }
        return nullptr;
    }

    Printer::Printer(const AbstractTable *table)
        : m_table(table),
          m_header_printer(default_header_printer),
          m_print_table_name(true)
    {
        if (!m_table)
        {
            err::addLogMsg(err::LogMsg("Printer ~ NullPointer") << "Table passed to `Printer` is null.");
            throw std::invalid_argument(err::recentLog());
        }
        for (IndexType i = 0; i < m_table->columnCount(); ++i)
        {
            m_selected_columns.push_back(i);
            m_printers.push_back(getDefaultPrinter(m_table->columnAt(i).value().second));
        }
    }

    Printer::Printer(AbstractTable *table, const std::vector<std::string> &columns)
        : m_table(table),
          m_header_printer(default_header_printer),
          m_print_table_name(true)
    {
        if (!m_table)
        {
            err::addLogMsg(err::LogMsg("Printer ~ NullPointer") << "Table passed to `Printer` is null.");
            throw std::invalid_argument(err::recentLog());
        }
        for (const std::string &column_name : columns)
        {
            const auto find_column = m_table->findColumn(column_name);
            if (find_column)
            {
                const auto &[column_index, data_type] = find_column.value();
                m_selected_columns.push_back(column_index);
                m_printers.push_back(getDefaultPrinter(data_type));
            }
        }
    }

    bool Printer::print(std::ostream &stream, const char *del)
    {
        IndexType column_index = 0;
        try
        {
            if (m_print_table_name)
            {
                stream << m_table->getDecoratedName() << std::endl;
            }
            const SizeType column_count = m_selected_columns.size();
            const SizeType row_count = m_table->rowCount();

            std::vector<std::string> headers;
            headers.reserve(column_count);
            for (column_index = 0; column_index < column_count; ++column_index)
            {
                headers.push_back(m_table->columnAt(m_selected_columns[column_index]).value().first);
            }
            // print the headers
            m_header_printer(headers, stream, del);
            stream << std::endl;
            if (!row_count || !column_count)
                return true;
            //
            for (IndexType row_index = 0; row_index < row_count; ++row_index)
            {
                m_printers[0](m_table->getDataWC(row_index, m_selected_columns[0]), stream);
                for (column_index = 1; column_index < column_count; ++column_index)
                {
                    stream << del;
                    m_printers[column_index](m_table->getDataWC(row_index, m_selected_columns[column_index]), stream);
                }
                stream << '\n';
            }
            stream.flush();
            return true;
        }
        catch (std::bad_variant_access &e)
        {
            err::addLogMsg(err::LogMsg("Printer ~ InvalidArgs") << "Printer has encountered a bad variant access error for table `"
                                                                << m_table->getDecoratedName() << "`, possibly for column `"
                                                                << m_table->columnAt(m_selected_columns[column_index]).value().first << "`.");
            return false;
        }
    }

    void Printer::setHeaderPrinter(const HeaderPrinter_ &header_printer)
    {
        if (!header_printer)
            m_header_printer = default_header_printer;
        m_header_printer = header_printer;
    }

    bool Printer::setFunction(const std::string &column_name, ElementPrinter_ fnc)
    {
        const auto find_column = m_table->findColumn(column_name);

        if (find_column) // if column exists in the table
        {
            const auto &[column_index, column_tpye] = find_column.value();
            auto iterator = std::find(m_selected_columns.begin(), m_selected_columns.end(), column_index);
            if (!fnc)
                fnc = getDefaultPrinter(column_tpye);
            if (iterator != m_selected_columns.end()) // if exists in the printer
            {
                m_printers[std::distance(m_selected_columns.begin(), iterator)] = fnc;
            }
            else
            {
                m_selected_columns.push_back(column_index); // or add it to the printer
                m_printers.push_back(fnc);
            }
            return true;
        }
        return false;
    }

    void Printer::setFunction(const std::vector<std::string> &column_name_vec, ElementPrinter_ fnc)
    {
        for (const std::string &column : column_name_vec)
            setFunction(column, fnc);
    }

} // namespace km
