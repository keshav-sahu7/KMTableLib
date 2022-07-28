
#ifndef KMTABLELIB_KMT_LOGMSG_HPP
#define KMTABLELIB_KMT_LOGMSG_HPP

#include "Core.hpp"

namespace km::err
{

    /**
     * @brief The LogMsg class helps in creating formatted error message.
     * 
     * It can be used to generate a good formatted error messages.
     */
    class LogMsg final
    {
    
    public:
    
        /**
        * @brief Constructor
        * 
        * Constructs object with @a err_type . If @a err_type = "Arithmetic" then it will
        * construct error message saying "Arithmetic Issue : ".
        */
        explicit LogMsg(const std::string &err_type);

        /**
         * @brief adds @a msg/data to the error string and returns reference to itself.
         */
        ///@{
        LogMsg &operator << (const char *msg);
        LogMsg &operator << (const KString &msg);
        LogMsg &operator << (KInt32 data);
        LogMsg &operator << (KInt64 data);
        LogMsg &operator << (KFloat32 data);
        LogMsg &operator << (KFloat64 data);
        LogMsg &operator << (KBoolean data);
        LogMsg &operator << (const KDate &data);
        LogMsg &operator << (const KDateTime &data);
        LogMsg &operator << (DataType data_type);
        LogMsg &operator << (const Variant &variant);
        ///@}

        /**
         * @brief Get the generated error string.
         */
        const std::string &getString() const;

        /**
         * @brief Copy Constructor
         */
        LogMsg(const LogMsg&) = default;

        /**
         * @brief Assignment operator 
         */
        LogMsg& operator = (const LogMsg&) = default;
        
        /**
         * @brief Destructor
         */
        ~LogMsg() = default;

    private:
        std::string m_msg;

    };

    inline LogMsg::LogMsg(const std::string &err_type) : m_msg(err_type + " Issue : ")
    {
        //
    }

    inline const std::string &LogMsg::getString() const
    {
        return m_msg;
    }

    inline LogMsg &LogMsg::operator << (const KString &msg)
    {
        m_msg += msg;
        return *this;
    }

    inline LogMsg &LogMsg::operator << (const char *msg)
    {
        m_msg += msg;
        return *this;
    }

    inline LogMsg &LogMsg::operator << (KInt32 data)
    {
        m_msg += std::to_string(data);
        return *this;
    }

    inline LogMsg &LogMsg::operator << (KInt64 data)
    {
        m_msg += std::to_string(data);
        return *this;
    }

    inline LogMsg &LogMsg::operator << (KFloat32 data)
    {
        m_msg += std::to_string(data);
        return *this;
    }

    inline LogMsg &LogMsg::operator << (KFloat64 data)
    {
        m_msg += std::to_string(data);
        return *this;
    }

    inline LogMsg &LogMsg::operator << (KBoolean data)
    {
        m_msg += (data) ? ("True") : ("False");
        return *this;
    }

    inline LogMsg &LogMsg::operator << (DataType data_type)
    {
        m_msg += dataTypeToString(data_type);
        return *this;
    }

} // namespace km

#endif // KMTABLELIB_KMT_LOGMSG_HPP
