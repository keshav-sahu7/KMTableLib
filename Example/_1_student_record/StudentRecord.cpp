#include "StudentRecord.hpp"

#include <iostream>
#include <iomanip>

#include <kmt/Printer.hpp>
#include <kmt/BasicView.hpp>
#include <kmt/ErrorHandler.hpp>

void printMarks(const km::Variant &data, std::ostream &out)
{
    out << std::left << std::setw(16) << data.asInt32();
}

void printName(const km::Variant &data, std::ostream &out)
{
    out << std::left << std::setw(16) << data.asString();
}

void headerPrinter(const std::vector<std::string> &headers, std::ostream &out, const char * /*del*/)
{
    for (const auto &header : headers)
        out << std::left << std::setw(16) << header;
    out << '\n' << std::string(headers.size() * 16, '-');
}

StudentRecord::StudentRecord()
{
    try
    {
        m_table = new km::Table("Student Record",
                                {{"name", km::DataType::STRING},
                                 {"math", km::DataType::INT32},
                                 {"physics", km::DataType::INT32},
                                 {"chemistry", km::DataType::INT32}},
                                km::SortingOrder::ASCENDING);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << "aborting program!" << std::endl;
        std::abort();
    }
}

void StudentRecord::addStudent(const std::string &name, const Marks &marks)
{
    m_table->insertRow({name,marks.math(), marks.physics(), marks.chemistry()});
}

void StudentRecord::removeStudentF(const std::string &name)
{
    std::vector<km::IndexType> searches = m_table->search("name", name);
    if (searches.empty())
    {
        std::cerr << "No student named `" << name << "` found!" << std::endl;
        return;
    }
    m_table->dropRow(searches.front());
    std::cerr << "Removed student `" << name << "`." << std::endl;
}

void StudentRecord::removeStudentL(const std::string &name)
{
    std::vector<km::IndexType> searches = m_table->search("name", name);
    if (searches.empty())
    {
        std::cerr << "No student named `" << name << "` found!" << std::endl;
        return;
    }
    m_table->dropRow(searches.back());
    std::cerr << "Removed student `" << name << "`." << std::endl;
}

void StudentRecord::showAllStudent()
{
    km::Printer printer(m_table);
    printer.setFunction({"math", "physics", "chemistry"}, printMarks);
    printer.setFunction("name",printName);
    printer.setHeaderPrinter(headerPrinter);
    printer.printTableName(false);
    printer.print(std::cout, "");
}

void StudentRecord::showTopStudents(const std::string &field)
{
    try
    {
        km::BasicView view("Topper Students", m_table, {"name", field}, "True"); // True selects all
        view.sortBy(field, km::SortingOrder::DESCENDING);
        km::Printer printer(&view);
        printer.setFunction("name",printName);
        printer.setFunction(field, printMarks);
        printer.setHeaderPrinter(headerPrinter);
        printer.printTableName(false);
        printer.print(std::cout, "");
    }
    catch (...)
    {
        std::cerr << "No such field!" << std::endl;
    }
}

StudentRecord::~StudentRecord()
{
    delete m_table;
}
