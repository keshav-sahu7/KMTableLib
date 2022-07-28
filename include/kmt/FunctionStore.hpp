#ifndef KMTABLELIB_KMT_FUNCTIONMANAGER_HPP
#define KMTABLELIB_KMT_FUNCTIONMANAGER_HPP

#include <map>
#include <string>
#include <vector>

#include "Core.hpp"

namespace km
{

    void initAllFnc();

    /**
     * @brief Class for holding function information.
     * The FunctionInfo struct is used to store basic function information such as a pointer to the function,
     * return type of the function and number of arguments it takes. Function must not throw any exception.
     */
    struct FunctionInfo
    {
        Variant (*function)(const Variant *); ///< function pointer to custom function.
        DataType return_type;                 ///< return type of the function.
        SizeType argc;                        ///< number of arguments the function takes.
    };

    /**
     * @brief A global container class for functions which will be used by formula.
     * The FunctionStore class. It is a container class, to register your own function
     * to be called through formulae you must add function to only
     * instance of FunctionStore.
     *
     * Here's a function to add a KFloat32 number with KInt64 number and return value
     * of type KFloat64.
     * @code {.cpp}
     *      Variant myCustomAddFunction(const Variant *args)
     *      {
     *          KFloat32 num_1 = args[0].asFloat32();
     *          KInt64 num_2 = args[1].asInt64();
     *          return KFloat32(num_1 + num_2);
     *      }
     * @endcode
     * To be able to use this function with Formula for auto filling columns
     * and filtering tables. We have to register this function to the function store
     * instance.
     * @code {.cpp}
     *      FunctionStore::store().addEntry(
     *          "myCustomAddFunction_fI",       //notice _fI, see below for more information
     *          {               //function info
     *              myCustomAddFunction,        //function ptr
     *              DataType::FLOAT32,  //return type
     *              2           //argc argument count
     *          }
     *      );
     * @endcode
     *
     * Now to call this function, we will do something like this
     *
     * @code {.cpp}
     * Table table("my_table", {"x",DataType::INT64});
     *
     * //after some insertions
     * table.addColumnE({"new_column", DataType::FLOAT32}, "myCustomAddFunction($x,3.f)")
     * @endcode
     *
     * **Rules for function names :**
     *      Notice the @b "_fI" after the "add". After a function name of your choice it must
     *      be followed by an underscore `_` and zero or more characters from [iIfFsbdD]
     *      character set, where
     *
     *      i denotes KInt32
     *      I denotes KInt64
     *      f denotes KFloat32
     *      F denotes KFloat64
     *      s denotes KString
     *      b denotes KBoolean
     *      d denotes KDate
     *      D denotes KDateTime
     *
     *      For function that doesn't take any argument, function name must end with an underscore `_`.
     *
     * @note You explicitly need to provide the return type and number of arguments it takes.
     */

    class FunctionStore final
    {
    public:
        FunctionStore(const FunctionStore &) = delete;
        FunctionStore &operator=(const FunctionStore &) = delete;

        /**
         * @brief Registers the function with @a function_name .
         *
         * See the function name requirements, as it won't check whether the function name
         * is valid or not. Returns true if inserted, false otherwise.
         */
        bool addEntry(const std::string &function_name, const FunctionInfo &function_info);

        /**
         * @brief Registers the function.
         *
         * Adds the function provided by pair @a function of function name and function info.
         * Returns true if inserted, false otherwise.
         */
        bool addEntry(const std::pair<std::string, FunctionInfo> &function);

        /**
         * @brief Registers multiple functions to the function store.
         *
         * If @a function_vec has a function that couldn't be inserted then that function will be ignored.
         */
        void addEntries(const std::vector<std::pair<std::string, FunctionInfo>> &function_vec);

        /**
         * @brief Returns ending iterator of the underlying map.
         *
         * It can be used with find() function to check whether the function name exists or not by
         * comparing returned iterator with the end iterator returned by this function.
         */
        auto invalid() const
        {
            return m_map.end();
        }

        /**
         * @brief Finds the function entry by @a function_name in the store.
         *
         * Returns iterator to the function name and function info pair. If it does not exist then
         * end iterator of the underlying map is returned which can be compared with invalid().
         */
        auto find(const std::string &function_name) const
        {
            return m_map.find(function_name);
        }

        /**
         * @brief Returns the number of functions the store holds.
         */
        SizeType count() const;

        /**
         * @brief Returns the object of this singleton class.
         *
         * The only instance of this class can be accessed with this function. Register functions with
         * addEntry() function.
         */
        static FunctionStore &store();

        /**
         * @brief destructor.
         */
        ~FunctionStore() = default;

    private:
        /**
         * @brief constructor.
         */
        FunctionStore() = default;

        std::map<std::string, FunctionInfo> m_map;
    };

    inline bool FunctionStore::addEntry(const std::string &function_name, const FunctionInfo &function_info)
    {
        return m_map.insert({function_name, function_info}).second;
    }

    inline bool FunctionStore::addEntry(const std::pair<std::string, FunctionInfo> &function)
    {
        return m_map.insert(function).second;
    }

    inline void FunctionStore::addEntries(const std::vector<std::pair<std::string, FunctionInfo>> &function_vec)
    {
        for (const auto &v : function_vec)
        {
            m_map.insert(v);
        }
    }

    inline SizeType FunctionStore::count() const
    {
        return m_map.size();
    }

} // namespace km

#endif // KMTABLELIB_KMT_FUNCTIONMANAGER_HPP
