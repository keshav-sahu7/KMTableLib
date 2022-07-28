
#ifndef KMTABLE_SRC_LOGFILEHELPER_H
#define KMTABLE_SRC_LOGFILEHELPER_H

#include <ostream>

class LogFile
{
public:
    LogFile();
    void setStream(std::ostream *ofs, bool should_delete);
    void write(const std::string &str);
    ~LogFile();

private:
    std::ostream *m_ofs;
    bool m_should_delete;
};

inline LogFile::LogFile() : m_ofs(&std::cerr), m_should_delete(false)
{
    //
}

inline void LogFile::setStream(std::ostream *ofs, bool should_delete)
{
    if (m_should_delete)
        delete m_ofs;
    m_ofs = ofs;
    m_should_delete = should_delete;
}

inline void LogFile::write(const std::string &str)
{
    *m_ofs << str << std::endl;
}

inline LogFile::~LogFile()
{
    if (m_should_delete)
        delete m_ofs;
}

#endif // KMTABLE_SRC_LOGFILEHELPER_H
