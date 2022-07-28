#include <fstream>
#include "CSVWriter.hpp"
#include "Printer.hpp"
#include "AbstractTable.hpp"
#include "ErrorHandler.hpp"

namespace km
{

    namespace
    {

        void writeQuotedString(const Variant &v, std::ostream &stream)
        {
            stream << '"' << v.asString() << '"';
        }

    } // namespace

    bool writeAsCsv(const AbstractTable *table, std::string file_name, const char *del)
    {
        if (!table)
        {
            err::addLogMsg(err::LogMsg("CSVWriter ~ NullPointer") << "Given table is null");
            return false;
        }

        if (file_name.length() < 5 || file_name.substr(file_name.length() - 4, 4) != ".csv")
            file_name += ".csv";

        std::ofstream ofs(file_name);
        if (!ofs.is_open())
        {
            err::addLogMsg(err::LogMsg("CSVWriter ~ IO") << "Error when writing `" << file_name << "` file.");
            return false;
        }
        Printer printer(table);
        printer.printTableName(false);
        for (IndexType i = 0; i < table->columnCount(); ++i)
        {
            const auto [column_name, data_type] = table->columnAt(i).value();
            if (data_type == DataType::STRING)
            {
                printer.setFunction(column_name, writeQuotedString);
            }
        }
        bool res = printer.print(ofs, del);
        ofs.close();
        return res;
    }

} // namespace km
