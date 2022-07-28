#ifndef KMTABLELIB_KMT_PRINTER_HPP
#define KMTABLELIB_KMT_PRINTER_HPP

#include <functional> //using std::function so it can be used with std::bind

#include "AbstractTable.hpp"

namespace km
{

    /**
     * @brief The Printer class is used to print the table or view.
     * Uses std::function so we can bind some additional arguments
     * for formatting.
     */
    class Printer
    {
        using ElementPrinter_ = std::function<void(const Variant &, std::ostream &)>;
        using HeaderPrinter_ = std::function<void(const std::vector<std::string> &, std::ostream &, const char *)>;

    public:
        /**
         * @brief constructor.
         *
         * Selects the table to print. Selects all columns from the @b table for printing.
         *
         * @exception throws std::invalid_argument if table is nullptr.
         */
        Printer(const AbstractTable *table);

        /**
         * @brief constructor.
         *
         * Selects only provided columns @a columns from the table @a table for printing.
         * If any column does not exist in the table, it will be ignored. If all columns
         * are not found, it won't do anything.
         *
         * @exception throws std::invalid_argument if table is nullptr.
         */
        Printer(AbstractTable *table, const std::vector<std::string> &columns);

        /**
         * @brief Prints the table to given @a stream.
         * 
         * @a stream is the output stream to write data on. @a del is delemeter to separate columns.
         * Returns true on success. If any exception is thrown it writes it to logs and returns false.
         */
        bool print(std::ostream &stream, const char *del);

        /**
         * @brief Should the Printer print the table name.
         *
         * If true then table's decorated name will be printed else it won't. By
         * default is is set to true.
         */
        void printTableName(bool b = true);

        /**
         * @brief Sets the function @a fnc for the given column.
         *
         * If @a column_name exists in the Printer then its function will be
         * updated with @a fnc . If it doesn't exist then it will be appended and
         * true will be returned. If #column_name doesn't exist in the table then
         * false will be returned.
         *
         * @note function should match this prototype
         * @code
         * void function(const Variant &v, std::ostream &stream);
         * @endcode
         *  For other functions use std::bind.
         */
        bool setFunction(const std::string &column_name, ElementPrinter_ fnc);

        /**
         * @brief Sets the function @a fnc for the given column.
         *
         * If column names from @a column_name_vec exists in the Printer then its
         * function will be updated with @a fnc . If it doesn't exist then it will
         * be appended in the Printer's column list. If a column name doesn't exist
         * in the table then it will be ignored.
         *
         * @note function should match this prototype
         * @code
         * void function(const Variant &v, std::ostream &stream);
         * @endcode
         * 
         * For other functions use std::bind.
         */
        void setFunction(const std::vector<std::string> &column_name_vec, ElementPrinter_ fnc);

        /**
         * @brief Returns the current header printer function.
         */
        const HeaderPrinter_ &getHeaderPrinter() const;

        /**
         * @brief Sets the header printer function.
         *
         * If @a header_printer is null then the default is set. The default function
         * prints headers separated with the passed delemeter to the print()
         * function. Else user's given function is set.
         *
         * @note @a header_printer must have this prototype
         * 
         * @code
         * void function_name(const std::vector<std::string> &headers, std::ostream &stream, const char* del);
         * @endcode
         *
         * For other functions use std::bind.
         */
        void setHeaderPrinter(const HeaderPrinter_ &header_printer);

    private:
        std::vector<IndexType> m_selected_columns;
        const AbstractTable *m_table;
        std::vector<ElementPrinter_> m_printers;
        HeaderPrinter_ m_header_printer;
        bool m_print_table_name;
    };

    inline void Printer::printTableName(bool b)
    {
        m_print_table_name = b;
    }

    inline const Printer::HeaderPrinter_ &Printer::getHeaderPrinter() const
    {
        return m_header_printer;
    }

}

#endif // KMTABLELIB_KMT_PRINTER_HPP
