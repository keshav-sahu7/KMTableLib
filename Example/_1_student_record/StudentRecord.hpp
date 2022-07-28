
#ifndef KMTABLE_EXAMPLE_STUDENTRECORD_HPP
#define KMTABLE_EXAMPLE_STUDENTRECORD_HPP

#include <string>

#include <kmt/Table.hpp>

#include "Marks.hpp"

class StudentRecord
{
public:
    StudentRecord();
    void addStudent(const std::string &name, const Marks &marks);
    void removeStudentF(const std::string &name);
    void removeStudentL(const std::string &name);
    void showAllStudent();
    void showTopStudents(const std::string &field);
    ~StudentRecord();

private:
    km::Table *m_table;
};

#endif // KMTABLE_EXAMPLE_STUDENTRECORD_HPP
