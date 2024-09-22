/**
 * @file ErrorHandler.hpp
 * @author Keshav Sahu
 * @date May 1st 2022
 * @brief This file contains error handling classes and functions.
 */

#ifndef KMTABLELIB_KMT_ERRORHANDLER_HPP
#define KMTABLELIB_KMT_ERRORHANDLER_HPP

#include <functional>
#include "LogMsg.hpp"

namespace km::err
{

    using ErrorHandler = std::function<void(const std::string &)>;

    void setErrorHandler(ErrorHandler handler);
    ErrorHandler getErrorHandler();

    /**
     * @brief Notifies the error handler function.
     * 
     * If @ref isLogHandlerPaused() returns true it  will prepend all the unhandled
     * error messages (if any) and calls the error handler function set by
     * @ref setErrorHandler(). Otherwise it will push the message to queue.
     * 
     * @code
     * bool search(const char *str){
     *      addLogMsg("search : str not found!");
     *      return false;
     * }
     * int main(){
     *      pauseLogHandler();
     *          search(str);
     *      resumeLogHandler();
     *      addLogMsg("replace : could not replace!");
     * }
     * 
     * //replace : could not replace!
     * //Due to
     * //       search : str not found!
     * @endcode
     */
    void addLogMsg(std::string_view log_msg);

    /**
     * @brief overloaded function.
     * 
     * It is used to create more fancy error messages through @ref LogMsg class. @a log_msg is
     * the LogMsg object that holds the error message.
     */
    inline void addLogMsg(const LogMsg &log_msg)
    {
        addLogMsg(log_msg.getString());
    }

    /**
     * @brief recentLog returns most recent unhandled log message.
     * 
     * It can be used to retrive logs if some messages were added but logs handlers were paused.
     */
    std::string recentLog();

    /**
     * @brief Opens @a log_file to write error messages.
     * 
     * @a log_file is set as error/log stream. Errors are written in this file. If @a log_file is not empty
     * and is opened successfully it will be used for writing logs. If it is not empty but couldn't be opened
     * then error messages are written to previous logs and no changes happens.
     * 
     * If @a log_file is empty then std::cerr (default) is set. It returns false only if @a log_file is not empty
     * and couldn't be opened, true in other cases.
     */
    bool setLogFileName(std::string_view log_file);

    /**
     * @brief Sets already opened stream as log/error handler.
     * 
     * Sets @a out_stream as error stream. If @a out_stream contains errors then writes error in the current
     * error handler and returns false. Else sets @a out_stream as error stream and returns true. @a should_delete
     * ensures wheather this stream should be deleted at the end of the program or when error handler is changed.
     * For example, if you are setting std::cout then @a should_delete should be false.
     */
    bool setLogStream(std::ostream *out_stream, bool should_delete = false);

    /**
     * @brief Pause log handling.
     * 
     * It pauses log handler that means error messages added through addLogMsg will be pushed to a queue instead
     * of calling the error handler function.
     */
    void pauseLogHandler();

    /**
     * @brief Resumes log handler.
     *
     * @note This won't call error handler function with unhandled error messages. You need to  call @ref addLogMsg
     * or @ref handleUnhandledErrMsgs.
     */
    void resumeLogHandler();

    /**
     * @brief Returns whether log handler function is paused or not.
     */
    bool isLogHandlerPaused();

    /**
     * @brief Calls log handler with unhandled messages.
     */
    void handleUnhandledErrMsgs();

    
    /**
     * @brief It is a helper function to automatically lock the error handler and unlock automatically
     * at the end. It also keeps track wheather it was previously locked and resets at that state
     * at the end.
     */
    class LockLogFileHandler
    {
    public:
        /**
         * @brief Constructor
         */
        LockLogFileHandler();

        /**
         * @brief Returns wheather error handler were paused before creation of this object.
         */
        bool wasLockedAlready() const;

        /**
         * @brief Resumes log handler if it was not paused before creation of the object.
         */
        void resume();

        /**
         * @brief Destructor destructs and set it in the previous state (paused/resume).
         */
        ~LockLogFileHandler();

    private:
        bool m_already_paused; // if it was already set to pause
    };

    inline LockLogFileHandler::LockLogFileHandler()
        : m_already_paused(isLogHandlerPaused())
    {
        if (!m_already_paused) // if already paused, don't do anything
            pauseLogHandler();
    }

    inline bool LockLogFileHandler::wasLockedAlready() const
    {
        return m_already_paused;
    }

    inline void LockLogFileHandler::resume()
    {
        if (!wasLockedAlready())
            resumeLogHandler();
    }

    inline LockLogFileHandler::~LockLogFileHandler()
    {
        if (!wasLockedAlready()) // do not resume if it was already paused before calling this class constructor.
            resumeLogHandler();
        else
            pauseLogHandler(); // in case if resumeLogWriting() was called manually
    }

} // namespace km::err

#endif // KMTABLELIB_KMT_ERRORHANDLER_HPP
