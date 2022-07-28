#include <vector>
#include <string>
#include <map>
#include <regex>
#include <stack>

#include "Core.hpp"
#include "AbstractTable.hpp"
#include "Table.hpp"
#include "ErrorHandler.hpp"
#include "FunctionStore.hpp"

namespace km
{

    namespace parse
    {

        enum TType : uint16_t
        {
            INT32 = 0x0001,
            INT64 = 0x0002,
            FLOAT32 = 0x0004,
            FLOAT64 = 0x0008,
            STRING = 0x0010,
            BOOLEAN = 0x0020,

            COLUMN = 0x0040,
            FUNCTION = 0x080,

            COMMA = 0x0100,
            P_OPEN = 0x0200,
            P_CLOSE = 0x0400,
            INVALID = 0x0800,
        };

        constexpr uint16_t TT_DATA = (INT32 | INT64 | FLOAT32 | FLOAT64 | STRING | BOOLEAN);
        constexpr uint16_t TT_DATAC = (TT_DATA | COLUMN);

        using TokenRef = Token &;
        using ConstTokenRef = const Token &;

        static void resolveTokenType(TokenRef token)
        {
            static const std::vector<std::pair<TType, std::regex>> regex_map{
                {INT32, std::regex("\\-?\\d+")},
                {INT64, std::regex("\\-?\\d+(l|L)?")},
                {FLOAT32, std::regex("\\-?\\d+\\.(\\d+)?f")},
                {FLOAT64, std::regex("\\-?\\d+\\.(\\d+)?")},
                {STRING, std::regex("\".*\"")},
                {BOOLEAN, std::regex("(True)|(False)")},
                {COLUMN, std::regex("\\$[A-Za-z_]\\w*")},
                {FUNCTION, std::regex("[A-Za-z]\\w*")},
                {P_OPEN, std::regex("\\(")},
                {P_CLOSE, std::regex("\\)")},
                {COMMA, std::regex(",")},
                {INVALID, std::regex(".*")}};
            for (const auto &ref : regex_map)
            {
                if (std::regex_match(token.text, ref.second))
                {
                    token.token_type = ref.first;
                    return;
                }
            }
        }

        bool parseToTokens(std::string formula, TokenContainerRef token_vec)
        {
            token_vec.clear();
            std::string current_token;
            bool is_string = false; // to check if current token is string
            formula += ' ';
            for (char c : formula)
            {
                if (is_string)
                {
                    current_token.push_back(c);
                    if (c == '\"')
                    {
                        token_vec.push_back({current_token});
                        current_token.clear();
                        is_string = false;
                    }
                }
                else if (c == '\"')
                {
                    if (!current_token.empty())
                    {
                        token_vec.push_back({current_token});
                        current_token.clear();
                    }
                    current_token.push_back('\"');
                    is_string = true;
                }
                else if (c == ' ' || c == ',')
                {
                    if (!current_token.empty())
                    {
                        token_vec.push_back({current_token});
                        current_token.clear();
                    }
                    if (c == ',')
                        token_vec.push_back({","});
                }
                else if (c == '(' || c == ')')
                {
                    if (!current_token.empty())
                    {
                        token_vec.push_back({current_token});
                        current_token.clear();
                    }
                    token_vec.push_back({std::string(1, c)});
                }
                else
                {
                    current_token.push_back(c);
                }
            }
            if (is_string)
            {
                err::addLogMsg(err::LogMsg("Parse") << "Unterminated string.");
                return false;
            }
            for (TokenRef pt : token_vec)
            {
                resolveTokenType(pt);
                if (pt.token_type == INVALID)
                {
                    err::addLogMsg(err::LogMsg("Parse") << "Invalid token '" << pt.text << "'.");
                    return false;
                }
            }
            return true;
        }

        /**
         *
         *  check next token against current token, for grammar
         *
         *
         **/
        bool checkGrammar(TokenContainerRef token_vec)
        {
            const SizeType size = token_vec.size();

            if (size == 0)
            {
                err::addLogMsg(err::LogMsg("Parse") << "Empty expression.");
                return false;
            }

            else if (size == 1)
            {
                if (token_vec.front().token_type & TT_DATAC)
                    return true;
                err::addLogMsg(err::LogMsg("Parse") << "Expected literal values or column name but found '" << token_vec.front().text << "'.");
                return false;
            }

            else if (size > 1 && !(token_vec.front().token_type & FUNCTION))
            {
                err::addLogMsg(err::LogMsg("Parse") << "Expected function name but found '" << token_vec.front().text << "'.");
                return false;
            }

            int16_t p_level = 0;
            IndexType i = 0;
            token_vec.push_back(Token("Invalid"));
            uint16_t is_vld_tkn = true; // is_valid_token to check if next token type is valid
            bool fst_op_paren = false;  // first_opening_parentheses to validate statements like A()B() (invalid)

            std::stack<Token *> function_stack;

            for (;
                 i < size && is_vld_tkn && !((fst_op_paren && p_level == 0) || p_level < 0) // to check patterns like ()() or ())(()
                 ;
                 ++i)
            {
                switch (token_vec[i].token_type)
                {
                case INT32:
                case INT64:
                case FLOAT32:
                case FLOAT64:
                case STRING:
                case BOOLEAN:
                case COLUMN:
                    is_vld_tkn = token_vec[i + 1].token_type & (COMMA | P_CLOSE);
                    break;
                case TType::FUNCTION:
                    is_vld_tkn = token_vec[i + 1].token_type & (P_OPEN);
                    function_stack.push(&token_vec[i]);
                    break;
                case COMMA:
                    is_vld_tkn = token_vec[i + 1].token_type & (TT_DATAC | FUNCTION);
                    break;
                case P_OPEN:
                    is_vld_tkn = token_vec[i + 1].token_type & (TT_DATAC | FUNCTION | P_CLOSE);
                    ++p_level;
                    fst_op_paren = true;
                    break;
                case P_CLOSE:
                    is_vld_tkn = token_vec[i + 1].token_type & (COMMA | P_CLOSE | INVALID);
                    --p_level;
                    function_stack.top()->element.asFncInfo().end_token = i;
                    function_stack.pop();
                    break;
                default:
                    break;
                }
            }
            if (i != size || p_level != 0)
            {
                err::addLogMsg(err::LogMsg("Parse") << "Invalid syntax near `" << token_vec[i - 1].text << "` token.");
                return false;
            }

            token_vec.pop_back();
            return true;
        }

        //
        // shift circular left   A B C [D E F] G H I -> A B C [E F D] G H I
        //          Add(3,Mul(3,9))  becomes Add(3,(3,9)Mul)
        //                ^      ^
        //              f_pos  end_pos
        //
        void leftCircularShift(TokenContainerRef token_vec, IndexType f_pos, IndexType end_pos)
        {
            Token token = token_vec[f_pos];
            for (IndexType i = f_pos; i < end_pos; ++i)
            {
                token_vec[i] = token_vec[i + 1];
            }
            token_vec[end_pos] = token;
        }

        // assigns token.element (Variant) to the actual data by converting token.text
        // to appropriate data
        void toDataVariant(TokenRef token)
        {
            const std::string &value = token.text;
            switch (token.token_type)
            {
            case INT32:
                token.element = static_cast<KInt32>(std::stol(value));
                break;
            case INT64:
                token.element = static_cast<KInt64>(std::stoll(value));
                break;
            case FLOAT32:
                token.element = static_cast<KFloat32>(std::stof(value));
                break;
            case FLOAT64:
                token.element = static_cast<KFloat64>(std::stod(value));
                break;
            case STRING:
                token.element = KString(value.begin() + 1, value.end() - 1); // remove "" from "string"
                break;
            case BOOLEAN:
                token.element = static_cast<KBoolean>(value == "True");
                break;
            default:
                break;
            }
        }

        bool findColumn(const AbstractTable *table, TokenRef token)
        {
            const std::string text(token.text.begin() + 1, token.text.end()); // ommit the dollar sign
            auto found_column = table->findColumn(text);
            if (!found_column)
            {
                err::addLogMsg(err::LogMsg("Reference") << "No such column `" << text << "`.");
                return false;
            }
            auto [column_index, data_type] = found_column.value();
            token.element = column_info_t{/*.index = */ column_index, /* .type = */ data_type};
            return true;
        }

        /**
         * This function assumes s is ended with '_' followed by zero or more characters
         * from "icrtbd"
         **/
        std::string functionToString(const std::string &function_text)
        {
            SizeType pos = function_text.length() - 1;
            while (function_text[pos] != '_')
                --pos;
            std::string function_prototype = function_text.substr(0, pos);
            function_prototype += "(  ";
            const char type_str[] = "iIfFsbdDu";
            IndexType i, j;
            const char *type_str_l[] = {"int32", "int64", "float32", "float64", "string", "boolean", "date", "date_time", "undefined"};
            for (i = pos + 1; i < function_text.length(); ++i)
            {
                for (j = 0; j < sizeof(type_str) - 1; ++j)
                {
                    if (type_str[j] == function_text[i])
                        break;
                }
                function_prototype += type_str_l[j];
                function_prototype += ", ";
            }
            function_prototype.pop_back(); // last space
            function_prototype.pop_back(); // last ,
            function_prototype += ")";
            return function_prototype;
            ;
        }

        inline char datatypeToChar(DataType data_type)
        {
            return "iIfFsbdDu"[indexForDataType(data_type, 8)];
        }

        /*////////////////////////////////////////////
        //f_pos         -> function                 //
        //f_pos + 1     -> (                        //
        //f_pos + 2     -> arg...                   //
        //...                                       //
        //f_end_pos     -> )                        //
        ////////////////////////////////////////////*/

        bool resolveFunction(const AbstractTable *table, DataType &return_type, TokenContainerRef token_vec, IndexType f_pos, IndexType f_end_pos, const bool c_shift = true)
        {
            token_vec[f_pos].text += '_';
            for (IndexType i = f_pos + 2; i < f_end_pos; ++i)
            {
                if (TT_DATA & token_vec[i].token_type) // if a data then
                {
                    toDataVariant(token_vec[i]); // convert its token text to appropriate value.
                    // add type of the data to the function name so it can resolve the correct overload of the function.
                    token_vec[f_pos].text += datatypeToChar(static_cast<DataType>(token_vec[i].token_type));
                }
                else if (COLUMN & token_vec[i].token_type) // if a column name then
                {
                    if (!findColumn(table, token_vec[i])) // find the column (resolve column), if not found return false.
                        return false;
                    token_vec[f_pos].text += datatypeToChar(token_vec[i].element.asColInfo().type);
                }
                else if (FUNCTION & token_vec[i].token_type) // if found a function as argument of current function then
                {
                    DataType type_l;
                    IndexType end_pos = token_vec[i].element.asFncInfo().end_token; // get the ending index of the function aka ')'.
                    // recursively resolve the function.
                    if (!resolveFunction(table, type_l, token_vec, i, token_vec[i].element.asFncInfo().end_token, c_shift))
                    {
                        return false;
                    }
                    token_vec[f_pos].text += datatypeToChar(type_l); // add this function's return type to current function name.
                    i = end_pos;                                     // advance the index i.
                }
            }

            using fm = FunctionStore;
            // find the function
            // if any_function_token.text is Add_ii it denotes Add(int32,int32)
            // i for int32, I for int64, f for float32, F for float64
            // b for boolean, s for string, d for date, D for date_time
            auto it = fm::store().find(token_vec[f_pos].text);
            if (it == fm::store().invalid()) // if function not found then write the logs and return false.
            {
                err::addLogMsg(err::LogMsg("Reference") << "No matching function to call `"
                                                        << functionToString(token_vec[f_pos].text) << "`.");
                return false;
            }
            const auto &[it_function, it_return_type, it_argc] = it->second;
            token_vec[f_pos].element.asFncInfo().function = it_function; // set the resolved function.
            token_vec[f_pos].element.asFncInfo().argc = it_argc;         // set the argument count.
            return_type = it_return_type;                                // set return type

            if (c_shift) // if circular shift required then
            {
                leftCircularShift(token_vec, f_pos, token_vec[f_pos].element.asFncInfo().end_token); // shift circularly.
            }
            return true;
        }

        bool checkReference(TokenContainer &token_vec, const AbstractTable *table, DataType required_type, const bool c_shift = true)
        {
            DataType f_return_type; // return type of the expression
            if (token_vec.size() == 1)
            {
                TokenRef token = token_vec.front();
                TType token_type = static_cast<TType>(token.token_type);
                if (token_type == COLUMN)
                {
                    if (!findColumn(table, token))
                        return false;
                    if (token.element.asColInfo().type != required_type)
                    {
                        err::addLogMsg(err::LogMsg("DataType") << "Type mismatch, requested type is `"
                                                               << required_type << "` but the column `" << token.text
                                                               << "` has type `" << token.element.asColInfo().type << "`.");
                        return false;
                    }
                }
                else // data
                {
                    f_return_type = static_cast<DataType>(static_cast<uint16_t>(token.token_type));
                    if (required_type != f_return_type)
                    {
                        err::addLogMsg(err::LogMsg("DataType") << "Type mismatch, requested type is `"
                                                               << required_type << "` but the formula has type `" << f_return_type
                                                               << "`.");
                        return false;
                    }
                    toDataVariant(token);
                }
                return true;
            }
            if (!resolveFunction(table, f_return_type, token_vec, 0, token_vec.size() - 1, c_shift))
            {
                return false;
            }
            if (f_return_type != required_type)
            {
                err::addLogMsg(err::LogMsg("DataType") << "Type mismatch, requested type is `"
                                                       << required_type << "` but the formula has type `" << f_return_type
                                                       << "`.");
                return false;
            }
            return true;
        }

        // remove comma, parentheses and invalid (doesn't actually exists) tokens
        void removeSeparator(TokenContainerRef token_vec)
        {
            TokenContainer res_tokens;
            for (TokenRef token : token_vec)
            {
                if (!(token.token_type & (P_OPEN | P_CLOSE | COMMA | INVALID)))
                    res_tokens.push_back(token);
            }
            res_tokens.shrink_to_fit();
            token_vec = res_tokens;
        }

        void optimize(TokenContainerRef token_vec)
        {
            const SizeType token_size = token_vec.size();
            TokenContainer container(token_size); // using it as stack.
            std::vector<Variant> arguments;
            IndexType top = -1;
            bool is_literal;
            for (IndexType token_index = 0; token_index < token_size; ++token_index)
            {
                TokenRef token = token_vec[token_index];
                if (token.token_type & FUNCTION)
                {
                    arguments.clear();
                    function_info_t &finfo = token.element.asFncInfo();
                    is_literal = true;
                    for (IndexType arg_index = finfo.argc; arg_index > 0; --arg_index)
                    {
                        TokenRef argToken = container[top + 1 - arg_index];
                        if (!(argToken.token_type & TT_DATA))
                        {
                            is_literal = false;
                            break;
                        }
                        arguments.push_back(argToken.element.asData());
                    }
                    if (is_literal)
                    {
                        top -= finfo.argc;
                        Variant result = finfo.function(arguments.data());
                        Token token_res(token.text);
                        token_res.element = result;
                        token_res.token_type = INT32; // doesn't really matter
                        container[++top] = token_res;
                    }
                    else
                    {
                        container[++top] = token;
                    }
                } // end of if type == function
                else
                {
                    container[++top] = token;
                }
            }
            container.resize(top + 1);
            token_vec = container;
        }

        bool getCheckedToken(const std::string &formula, TokenContainerRef token_vec, const AbstractTable *table, DataType data_type)
        {
            token_vec.clear();
            if (!parseToTokens(formula, token_vec))
                return false;
            if (!checkGrammar(token_vec))
                return false;
            if (!checkReference(token_vec, table, data_type))
                return false;
            // remove comma, p_open, p_close tokens
            removeSeparator(token_vec);
            // now we can evaluate formula

            optimize(token_vec);
            return true;
        }

        // to speedup the evaluation by allocating space required for max arguments.
        // note: it will help us to remove unnecessory resizing argv vector in evaluation functions.
        SizeType maxArgc(ConstTokenContainerRef token_vec)
        {
            SizeType max = 0; // this is the max size
            SizeType argc;
            for (ConstTokenRef token : token_vec)
                if (token.token_type == FUNCTION && (argc = token.element.asFncInfo().argc) > max)
                    max = argc;
            return max;
        }

        void evaluateFormula(ConstTokenContainerRef token_vec, AbstractTable *table, IndexType target_column, IndexType start_r, IndexType end_r)
        {
            std::vector<Variant> data_stack; // std::vector is better than std::stack
            SizeType argc;
            std::vector<Variant> arguments;
            arguments.resize(maxArgc(token_vec)); // now this arguments vector won't be resized.
            ++end_r;                              // increase it by 1
            for (IndexType row_index = start_r; row_index < end_r; ++row_index)
            {
                for (ConstTokenRef token : token_vec)
                {
                    if (token.token_type & FUNCTION)
                    {
                        argc = token.element.asFncInfo().argc;
                        std::copy(data_stack.end() - argc, data_stack.end(), arguments.begin());
                        data_stack.erase(data_stack.end() - argc, data_stack.end());
                        data_stack.push_back(token.element.asFncInfo().function(arguments.data()));
                    }
                    else if (token.token_type & COLUMN)
                    {
                        data_stack.push_back(table->getDataWC(row_index, token.element.asColInfo().index));
                    }
                    else if (token.token_type & TT_DATA)
                    {
                        data_stack.push_back(token.element.asData());
                    }
                }
                table->setDataWC(row_index, target_column, data_stack.back());
                data_stack.pop_back();
            }
        }

        km::Variant evaluateFormula(ConstTokenContainerRef token_vec, const AbstractTable *table, km::IndexType row_index)
        {
            std::vector<Variant> data_stack; // std::vector is better than std::stack
            std::vector<Variant> arguments;
            arguments.resize(maxArgc(token_vec)); // now this arguments vector won't be resized.
            for (ConstTokenRef token : token_vec)
            {
                if (token.token_type & FUNCTION)
                {
                    SizeType argc;
                    argc = token.element.asFncInfo().argc;
                    std::copy(data_stack.end() - argc, data_stack.end(), arguments.begin());
                    data_stack.erase(data_stack.end() - argc, data_stack.end());
                    data_stack.push_back(token.element.asFncInfo().function(arguments.data()));
                }
                else if (token.token_type & COLUMN)
                {
                    data_stack.push_back(table->getDataWC(row_index, token.element.asColInfo().index));
                }
                else if (token.token_type & TT_DATA)
                {
                    data_stack.push_back(token.element.asData());
                }
            }
            return data_stack.back();
        }

        bool filter(const std::string &formula, std::vector<IndexType> &index_vec, const AbstractTable *table)
        {
            TokenContainer token_vec;

            if (!parseToTokens(formula, token_vec))
                return false;
            if (!checkGrammar(token_vec))
                return false;
            if (!checkReference(token_vec, table, DataType::BOOLEAN))
                return false;
            // remove comma, p_open, p_close tokens
            removeSeparator(token_vec);
            filter(token_vec, index_vec, table);
            return true;
        }

        void filter(ConstTokenContainerRef token_vec, std::vector<IndexType> &index_vec, const AbstractTable *table)
        {
            // now we can evaluate formula
            std::vector<Variant> data_stack;
            SizeType argc;
            std::vector<Variant> arguments;
            arguments.resize(maxArgc(token_vec));

            const SizeType row_count = table->rowCount();
            index_vec.reserve(row_count);
            for (SizeType row_index = 0; row_index < row_count; ++row_index)
            {
                for (ConstTokenRef token : token_vec)
                {
                    if (token.token_type & FUNCTION)
                    {
                        argc = token.element.asFncInfo().argc;
                        std::copy(data_stack.end() - argc, data_stack.end(), arguments.begin());
                        data_stack.erase(data_stack.end() - argc, data_stack.end());
                        data_stack.push_back(token.element.asFncInfo().function(arguments.data()));
                    }
                    else if (token.token_type & COLUMN)
                    {
                        data_stack.push_back(table->getDataWC(row_index, token.element.asColInfo().index));
                    }
                    else if (token.token_type & TT_DATA)
                    {
                        data_stack.push_back(token.element.asData());
                    }
                }
                if (data_stack.back().asBoolean())
                    index_vec.push_back(row_index);
                data_stack.pop_back();
            }
            index_vec.shrink_to_fit();
        }

        bool filter(ConstTokenContainerRef token_vec, const AbstractTable *table, IndexType row_index)
        {
            std::vector<Variant> data_stack; // std::vector is better than std::stack
            std::vector<Variant> arguments;
            arguments.resize(maxArgc(token_vec)); // now this arguments vector won't be resized.

            for (ConstTokenRef token : token_vec)
            {
                if (token.token_type & FUNCTION)
                {
                    SizeType argc = token.element.asFncInfo().argc;
                    std::copy(data_stack.end() - argc, data_stack.end(), arguments.begin());
                    data_stack.erase(data_stack.end() - argc, data_stack.end());
                    data_stack.push_back(token.element.asFncInfo().function(arguments.data()));
                }
                else if (token.token_type & COLUMN)
                {
                    data_stack.push_back(table->getDataWC(row_index, token.element.asColInfo().index));
                }
                else if (token.token_type & TT_DATA)
                {
                    data_stack.push_back(token.element.asData());
                }
            }
            return data_stack.back().asBoolean();
        }
    } // namespace parse
} // namespace km
