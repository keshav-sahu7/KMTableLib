/**
 * @file Parser2.hpp
 * @author Keshav Sahu
 * @date May 1st 2022
 * @brief This file contains classes and functions for parsing and executing formulae.
 */
#ifndef KMTABLELIB_KMT_PARSER2_HPP
#define KMTABLELIB_KMT_PARSER2_HPP

#include <vector>
#include <variant>
#include <string>

#include "Core.hpp"

namespace km
{

    class AbstractTable;

    namespace parse
    {
        struct function_info_t
        {
            Variant (*function)(const Variant *); ///< function
            SizeType argc;                        ///< argument count
            IndexType end_token;                  ///< ending token e.g. ')'
        };

        struct column_info_t
        {
            IndexType index; ///< column index
            DataType type;   ///< type of column
        };

        struct Token
        {
            std::string text;    ///< token text
            uint16_t token_type; ///< token type
            struct
            {
                function_info_t &asFncInfo() { return std::get<function_info_t>(e); }
                const function_info_t &asFncInfo() const { return std::get<function_info_t>(e); }
                column_info_t &asColInfo() { return std::get<column_info_t>(e); }
                const column_info_t &asColInfo() const { return std::get<column_info_t>(e); }
                Variant &asData() { return std::get<Variant>(e); }
                const Variant &asData() const { return std::get<Variant>(e); }
                template <typename T>
                void operator=(const T &obj) { e = obj; }

            private:
                std::variant<function_info_t, column_info_t, Variant> e;
            } element;
            Token(const std::string &text = "") : text(text), token_type(04000 /*Invalid*/) {}
            ~Token() = default;
        };

        using TokenContainer = std::vector<Token>;
        using TokenContainerRef = TokenContainer &;
        using ConstTokenContainerRef = const TokenContainer &;

        /**
         * @brief Parses string formula @a formula to tokens.
         *
         * It can be later lexed by other functions. Tokens will be stored in provided token
         * container @a token_vec . If formula is parsed successfully, true is returned.
         * If string formula is invalid and contains syntax error, it will return false
         * and log messages will be added to logs.
         */
        bool parseToTokens(std::string formula, TokenContainerRef token_vec);

        /**
         * @brief Compiles the parsed tokens.
         *
         * It parses string formula into tokens, if everything is fine, it finds reference to the columns and functions.
         * And converts it to executable tokens.
         *
         * @a formula is the formula to parse, @a token_vec will be filled with the executable tokens, @a table will be used
         * to refer the columns, it must be valid and shouldn't be nullptr. @a formula must generate a value/expression that
         * will have the data type provided by @a data_type. It optimizes out the formula, meaning
         * IF(isEqual(add(5,10),15),0,1) will be converted to 0 and will give fast execution.
         *
         * If it contains errors it will be written to logs and false will be returned. If everything is good then true will be returned.
         */
        bool getCheckedToken(const std::string &formula, TokenContainerRef token_vec, const AbstractTable *table, DataType data_type);

        /**
         * @brief Executes the compiled tokens.
         *
         * It executes compiled tokens provided by @a token_vec.
         * @a start_r and @a end_r are begining and ending of range of rows.
         * It will execute the formula for each row that falls in @b inclusive-range @b [start_r,end_r].
         * The ranges must be valid. The result will be stored of each evaluation in their respective rows
         * in the target column @a target_column .
         *
         * Preconditions:
         *      - @a token_vec must contain valid compiled tokens.
         *      - @a table must have at least end_r + 1 rows and should have setData implementation.
         *      - @a target_column must be a valid column index and should have the same data type as formula.
         *      - @a start_r must be <= @a end_r
         *
         * @warning If any of the condition fails then it may throw or will cause UB so be careful.
         *
         * It calls findColumn, setDataWC, getDataWC so you must provide all these implementation in the @a table.
         */
        void evaluateFormula(ConstTokenContainerRef token_vec,
                             AbstractTable *table,
                             IndexType target_column,
                             IndexType start_r,
                             IndexType end_r);

        /**
         * @brief Execute formula for single row.
         * This executes/evaluates compiled tokens provided by @a token_vec for row @a row_index in the table @a table
         * and returns the resultant value.
         *
         * @note It has same preconditions as above functions. @a row_index must be valid, else it would be UB.
         */
        km::Variant evaluateFormula(ConstTokenContainerRef token_vec, const AbstractTable *table, km::IndexType row_index);

        /**
         * @brief Filters the table with given boolean formula.
         *
         * It filters out all the rows of the table with given boolean formula. Any row in the table @a table,
         * that generates "True" for the given formula @a formula will be added to @a index_vec else row will be ignored.
         *
         * If everything is fine then it returns true. If formula contains any type of error or doesn't produce a boolean
         * result errors will be written to logs and it will return false.
         */
        bool filter(const std::string &formula, std::vector<IndexType> &index_vec, const AbstractTable *table);

        /**
         * @brief Overloaded function.
         *
         * It executes the precompiled tokens @a token_vec and if it evaluates to "True" for a row in the table @a table ,
         * it will add it to index_vec.
         * @warning @a token_vec must be valid.
         */
        void filter(ConstTokenContainerRef token_vec, std::vector<IndexType> &index_vec, const AbstractTable *table);

        /**
         * @brief Overloaded function.
         *
         * It executes the compiled tokens @a token_vec for row given by @a index in the table @a table .
         * Returns true if tokens evaluates to true, false if it evaluates to false.
         *
         * @warning @a token_vec and index must be valid.
         */
        bool filter(ConstTokenContainerRef token_vec, const AbstractTable *table, IndexType index);

    } // namespace parse

} // namespace km

#endif // KMTABLELIB_KMT_PARSER2_HPP
