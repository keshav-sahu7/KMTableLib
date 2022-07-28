#include "TableIO.hpp"

#include <fstream>
#include <string>
#include "ErrorHandler.hpp"

// change type of ptr to char*
#define MAKE_R(ptr) (reinterpret_cast<char *>(ptr))
// change type of ptr to const char*
#define MAKE_W(ptr) (reinterpret_cast<const char *>(ptr))

namespace km
{

    // if path is empty then return only file name (relative path)
    // else path/file

    namespace
    {

        std::string resolveFileName(std::string file_name, const std::string &path, const std::string fl_ext)
        {
            const IndexType fname_size = file_name.size();
            const IndexType fl_ext_size = fl_ext.size();
            if (fname_size <= (fl_ext_size + 1) || file_name.substr(fname_size - fl_ext_size, fl_ext_size) != fl_ext)
                file_name += "." + fl_ext;
            if (!path.empty())
                return (path + "/") + file_name;
            return file_name;
        }

        // writes table to path/column_name.clm

        template <typename Type_>
        bool writeColumnToStream(const Table *table, IndexType column_index, const std::string &path)
        {
            auto column_name = table->columnAt(column_index).value().first;
            std::string column_file_name = resolveFileName(column_name, path, "clm");
            std::ofstream c_ofs;
            c_ofs.open(column_file_name, std::ios_base::out | std::ios_base::binary);
            if (!c_ofs.is_open())
                return false;
            for (IndexType row_index = 0, row_count = table->rowCount(); row_index < row_count; ++row_index)
            {
                Variant data = table->getDataWC(row_index, column_index);
                c_ofs.write(MAKE_W(&data.as<Type_>()), sizeof(Type_));
            }
            c_ofs.close();
            return true;
        }

        // specialization for KString, as KString is a class type, so it only needs length.
        template <>
        bool writeColumnToStream<KString>(const Table *table, IndexType column_index, const std::string &path)
        {
            auto column_name = table->columnAt(column_index).value().first;
            std::string column_file_name = resolveFileName(column_name, path, "clm");
            std::ofstream c_ofs;
            c_ofs.open(column_file_name, std::ios_base::out | std::ios_base::binary);
            if (!c_ofs.is_open())
                return false;
            for (IndexType row_index = 0, row_count = table->rowCount(); row_index < row_count; ++row_index)
            {
                Variant data = table->getDataWC(row_index, column_index);
                const KString &str = data.asString();
                c_ofs.write(str.c_str(), str.length() + 1);
            }
            c_ofs.close();
            return true;
        }

        using StreamWriter_ = bool (*)(const Table *, IndexType, const std::string &);

        inline StreamWriter_ getColumnWriter(DataType data_type)
        {
            IndexType index = indexForDataType(data_type, 8); //[0..7] are valid indices, anything ]0..7[ is undefined.
            return std::array<StreamWriter_, 9>{
                writeColumnToStream<KInt32>,
                writeColumnToStream<KInt64>,
                writeColumnToStream<KFloat32>,
                writeColumnToStream<KFloat64>,
                writeColumnToStream<KString>,
                writeColumnToStream<KBoolean>,
                writeColumnToStream<KDate>,
                writeColumnToStream<KDateTime>,
                nullptr}[index];
        }

        void writeTableInfo(const Table *table, std::ofstream &ofs)
        {
            const std::string &name = table->getName();
            SizeType name_length = name.length();
            SortingOrder s_order = table->getSortingOrder();
            SizeType column_count = table->columnCount();
            SizeType row_count = table->rowCount();

            // the order is important
            ofs.write(MAKE_W(&name_length), sizeof(SizeType));  // length of the name
            ofs.write(name.c_str(), name_length);               // name
            ofs.write(MAKE_W(&s_order), sizeof(s_order));       // sorting order
            ofs.write(MAKE_W(&column_count), sizeof(SizeType)); // number of columns
            ofs.write(MAKE_W(&row_count), sizeof(SizeType));    // number of rows
            // writes below information column times
            for (SizeType column_index = 0; column_index < column_count; ++column_index)
            {
                const auto [column_name, display_name, column_type] = table->getColumnMetaData(column_index);
                IndexType cn_length = column_name.length(), dn_length = display_name.length();
                ofs.write(MAKE_W(&column_type), sizeof(DataType)); // data type
                ofs.write(MAKE_W(&cn_length), sizeof(IndexType));  // length of the column name
                ofs.write(column_name.c_str(), cn_length);         // column name
                ofs.write(MAKE_W(&dn_length), sizeof(IndexType));  // length of the column name
                ofs.write(display_name.c_str(), dn_length);        // column name
            }
        }

        template <class T>
        Variant readStreamData(IndexType /*index*/, std::ifstream &ifs)
        {
            T data = {};
            ifs.read(MAKE_R(&data), sizeof(T));
            return data;
        }

        template <>
        Variant readStreamData<KString>(IndexType /*index*/, std::ifstream &ifs)
        {
            char bytes[256] = {}; // A max of 255 characters can be read
            ifs.getline(bytes, 256, '\0');
            return KString(bytes);
        }

        // reads string from [4 byte length][length size string]...
        inline std::ifstream &readLString(std::ifstream &ifs, std::string &str)
        {
            SizeType length = 0;
            ifs.read(MAKE_R(&length), sizeof(SizeType));
            auto cur_pos = ifs.tellg();
            ifs.seekg(0, std::ios::end);
            if (length <= static_cast<SizeType>((ifs.tellg() - cur_pos)))
            {
                ifs.seekg(cur_pos, std::ios::beg);
                str.resize(length);
                ifs.read(str.data(), length);
            }
            else
                ifs.setstate(std::ios::failbit);
            return ifs;
        }

        using StreamDataReader_ = Variant (*)(IndexType, std::ifstream &);

        inline StreamDataReader_ getStreamReader(DataType data_type)
        {
            IndexType index = indexForDataType(data_type, 8);
            return std::array<StreamDataReader_, 9>{
                readStreamData<KInt32>,
                readStreamData<KInt64>,
                readStreamData<KFloat32>,
                readStreamData<KFloat64>,
                readStreamData<KString>,
                readStreamData<KBoolean>,
                readStreamData<KDate>,
                readStreamData<KDateTime>,
                nullptr}[index];
        }

        bool insertData(Table *table, SizeType row_count, const std::vector<ColumnMetaData> &column_vec, const std::string &path)
        {
            for (IndexType c_index = 0, column_count = column_vec.size(); c_index < column_count; ++c_index)
            {
                const auto &column_name = column_vec[c_index].column_name;
                const auto &column_type = column_vec[c_index].data_type;
                const auto &display_name = column_vec[c_index].display_name;

                std::ifstream ifs;
                const std::string file_name = resolveFileName(column_name, path, "clm");
                ifs.open(file_name, std::ios_base::in | std::ios_base::binary);
                if (!ifs.is_open())
                {
                    err::addLogMsg(err::LogMsg("ReadTableFromFile ~ IO") << "Couldn't open the file `" << file_name << "` to read column `"
                                                                         << column_name << "` to create table `" << table->getName() << "`.");
                    return false;
                }
                StreamDataReader_ dataReader = getStreamReader(column_type);
                if (c_index == 0) // first column is already added, and needs to be inserted manually
                {
                    table->pauseSorting();
                    for (IndexType r_index = 0; r_index < row_count; ++r_index)
                        table->insertRow({dataReader(0, ifs)});
                    table->resumeSorting();
                }
                else
                {
                    table->addColumnF<StreamDataReader_, std::ifstream &>({column_name, display_name, column_type}, dataReader, ifs);
                }
                ifs.close();
            }
            return true;
        }

    } // namespace

    bool writeTableTo(const Table *table, const std::string &path)
    {
        std::string file_name = resolveFileName(table->getName(), path, "kmt");
        std::ofstream ofs;
        ofs.open(file_name, std::ios_base::out | std::ios_base::binary);
        if (!ofs.is_open())
        {
            err::addLogMsg(err::LogMsg("WriteTableTo ~ IO") << "While trying to write the table `" << table->getName()
                                                            << "`, couldn't write `" << file_name << "`.");
            return false;
        }
        writeTableInfo(table, ofs);
        ofs.close();
        for (IndexType column_index = 0, column_count = table->columnCount(); column_index < column_count; ++column_index)
        {
            auto [column_name, column_type] = table->columnAt(column_index).value();
            StreamWriter_ writer = getColumnWriter(column_type);
            if (!writer(table, column_index, path))
            {
                err::addLogMsg(err::LogMsg("WriteTableTo ~ IO") << "While trying to write the column `" << column_name
                                                                << "` of table `" << table->getName() << "`, couldn't write `"
                                                                << resolveFileName(column_name, path, "clm") << "`.");
                return false;
            }
        }
        return true;
    }

    Table *readTableFrom(const std::string &file_name, const std::string &path)
    {
        std::ifstream ifs;
        std::string f_file_name = resolveFileName(file_name, path, "kmt");

        ifs.open(f_file_name, std::ios_base::in | std::ios_base::out);
        if (!ifs.is_open())
        {
            err::addLogMsg(err::LogMsg("ReadTableFrom ~ IO") << "Couldn't open `" << f_file_name << "`.");
            return nullptr;
        }

        std::string table_name;
        SortingOrder s_order;
        SizeType row_count = 0, column_count = 0;

        bool _b_read_name = static_cast<bool>(readLString(ifs, table_name));
        bool _b_read_sorder = static_cast<bool>(ifs.read(MAKE_R(&s_order), sizeof(s_order)));
        bool _b_read_columnc = static_cast<bool>(ifs.read(MAKE_R(&column_count), sizeof(SizeType)));
        bool _b_read_rowc = static_cast<bool>(ifs.read(MAKE_R(&row_count), sizeof(SizeType)));
        if (!(_b_read_name && _b_read_sorder && _b_read_columnc && _b_read_rowc))
        {
            err::addLogMsg(err::LogMsg("ReadTableFrom ~ IO") << "Reading failed `" << file_name << "`.");
            return nullptr;
        }

        std::vector<ColumnMetaData> column_vec;
        for (SizeType c = 0; c < column_count; ++c)
        {
            DataType column_type;
            std::string column_name;
            std::string display_name;
            if (!(ifs.read(MAKE_R(&column_type), sizeof(DataType)) && readLString(ifs, column_name) && readLString(ifs, display_name)))
            {
                err::addLogMsg(err::LogMsg("ReadTableFrom ~ IO") << "Reading failed `" << file_name << "`.");
                return nullptr;
            }
            column_vec.push_back({column_name, display_name, column_type});
        }

        try
        {
            err::LockLogFileHandler lock;
            (void)lock;
            if (!column_count) // no columns, empty table
                return new Table(table_name, {}, s_order);
            if (!row_count) // no rows but columns exist
                return new Table(table_name, column_vec, s_order);

            // else have both columns and rows
            // create table with single column.
            Table *table = new Table(table_name, {column_vec.front()}, s_order);
            if (!insertData(table, row_count, column_vec, path))
            {
                delete table;
                throw 0;
            }
            return table;
        }
        catch (...)
        {
            err::addLogMsg(err::LogMsg("ReadTableFrom ~ Table") << " Error while creating table `"
                                                                << table_name << "` from `" << f_file_name << "`.");
            return nullptr;
        }
    }

}

/**
    name.length
    name.data
    sortingOrder
    columnCount
    rowCount
        columnType
        columnName.length
        columnName.data
    data_vec
  **/
