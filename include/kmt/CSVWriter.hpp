#ifndef KMTABLELIB_KMT_CSVWRITER_HPP
#define KMTABLELIB_KMT_CSVWRITER_HPP

#include "AbstractTable.hpp"

namespace km
{
    /**
     * @brief writes whole table in a csv file.
     *
     * It writes the table @a table in a csv file provided by @a file_name
     * (which is actually a full path / relative path to the file). If @a file_name
     * has no .csv extension it will be added. @a del is the delemeter to separate
     * the columns, default is a comma.
     *
     * It returns true on success, on failure it returns false and writes error message to logs.
     */
    bool writeAsCsv(const AbstractTable *table, std::string file_name, const char *del = ", ");

} // namespace km

#endif // KMTABLELIB_KMT_CSVWRITER_HPP
