#include "ErrorHandler.hpp"

#include <iostream>
#include <fstream>
#include <stack>
#include <functional>
#include "LogFileHelper.h"

namespace km::err
{

    namespace
    {
        // must have at least one string so recentLog won't cause any UB.
        std::stack<std::string> log_msg_;
        bool pause_log_handling_ = false;
        LogFile log_file_;

        void defaultHandler(const std::string &error_msg)
        {
            log_file_.write(error_msg);
        }

        ErrorHandler error_handler_ = defaultHandler;

        void replaceNL(std::string &str, const std::string &rpls)
        {
            const auto rpls_size = rpls.size();
            str.reserve(str.size() + std::count(str.begin(), str.end(), '\n') * rpls_size);
            // result_str.reserve(str.size() + std::count(str.begin(),str.end(),'\n') * rpls_size);
            std::size_t pos = str.find('\n');
            while (pos != std::string::npos)
            {
                str.replace(pos, 1, rpls);
                pos = str.find('\n', pos + rpls_size);
            }
        }
    } // namespace

    void setErrorHandler(ErrorHandler handler)
    {
        if (handler)
            error_handler_ = handler;
        else
            error_handler_ = defaultHandler;
    }

    ErrorHandler getErrorHandler()
    {
        return error_handler_;
    }

    void pauseLogHandler()
    {
        pause_log_handling_ = true;
    }

    void resumeLogHandler()
    {
        pause_log_handling_ = false;
    }

    bool isLogHandlerPaused()
    {
        return pause_log_handling_;
    }

    void handleUnhandledErrMsgs()
    {
        if (!isLogHandlerPaused() && !log_msg_.empty())
        {
            std::string error_msg = log_msg_.top();
            log_msg_.pop();
            error_msg.append("\n");
            int level = 1;
            while (!log_msg_.empty())
            {
                error_msg.append(level - 1, '\t').append("Due to\n").append(level, '\t');
                std::string str = log_msg_.top();
                replaceNL(str, std::string("\n").append(level, '\t'));
                error_msg.append(str).append("\n");
                ++level;
                log_msg_.pop();
            }
            error_handler_(error_msg);
        }
    }

    bool setLogFileName(std::string_view log_file)
    {
        if (!log_file.empty()) // neither null nor empty
        {
            std::ofstream *temp = new std::ofstream;
            temp->open(log_file.data());
            if (temp->is_open())
            {
                log_file_.setStream(temp, true);
                setErrorHandler(defaultHandler);
                return true;
            }
            addLogMsg(err::LogMsg("IO") << "Error can't open the file `" << log_file.data() << "` for writing logs.");
            delete temp;
            return false;
        }
        setErrorHandler(defaultHandler);
        log_file_.setStream(&std::cerr, false);
        return true;
    }

    bool setLogStream(std::ostream *out_stream, bool should_delete)
    {
        if (out_stream->fail())
        {
            addLogMsg(LogMsg("IO") << "given stream to handle the logs has errors! Not switching logs to the stream.");
            return false;
        }
        else
        {
            log_file_.setStream(out_stream, should_delete);
            setErrorHandler(defaultHandler);
            return true;
        }
    }

    void addLogMsg(std::string_view log_msg)
    {
        if (!isLogHandlerPaused())
        {
            std::string error_msg(log_msg);
            error_msg.append("\n");
            int level = 1;
            while (!log_msg_.empty())
            {
                error_msg.append(level - 1, '\t').append("Due to\n").append(level, '\t');
                std::string str = log_msg_.top();
                replaceNL(str, std::string("\n").append(level, '\t'));
                error_msg.append(str).append("\n");
                ++level;
                log_msg_.pop();
            }
            error_handler_(error_msg);
        }
        else
        {
            log_msg_.push(std::string(log_msg));
        }
    }

    std::string recentLog()
    {
        return log_msg_.empty() ? std::string() : log_msg_.top();
    }

} // namespace km::err
