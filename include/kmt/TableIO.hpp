#ifndef KMTABLELIB_KMT_TABLEIO_HPP
#define KMTABLELIB_KMT_TABLEIO_HPP

#include <string>

#include "Table.hpp"

namespace km
{
    /**
     * @brief Writes the table to the given file.
     * 
     * @a path is the path where file will be saved. The created file would
     * be path/table_name.kmt and other data will be saved in the same directory.
     * If everything works it will  return true. If it fails logs will be written
     * to log handlers and false will be returned.
     */
    bool writeTableTo(const Table *table, const std::string &path);

    /**
     * @brief Reads table from the file.
     * 
     * Reads the table from given file path/file_name. @a file_name should have .kmt format
     * else it would be added. If it reads without any error then table will be returned.
     * If it fails logs will be written to log hanlders and false will be returned.
     */
    Table *readTableFrom(const std::string &file_name, const std::string &path);
}

#endif // KMTABLELIB_KMT_TABLEIO_HPP
