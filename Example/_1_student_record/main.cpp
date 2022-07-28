#include <iostream>
#include <thread>
#include "StudentRecord.hpp"

#if defined(_WIN32) || defined(_WIN64)
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

template <typename T>
static T get()
{
    T x;
    std::cin >> x;
    while (!std::cin.good())
    {
        std::cout << "Invalid Input enter again!\n>>>";
        std::cout.flush();
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        std::cin >> x;
    }
    return x;
}

int main()
{
    StudentRecord student_record;

    std::string student_name; // commonly needed variables
    char choice;
    do
    {
        std::system(CLEAR);
        std::cout << "-----[Student Record]------------------------------------------------\n";
        std::cout << "     1. Add Student\n";
        std::cout << "     2. Remove Student (First Occurence)\n";
        std::cout << "     3. Remove Student (Last Occurence)\n";
        std::cout << "     4. Show All Students\n";
        std::cout << "     5. Show Top Students\n";
        std::cout << "     Else key to Exit\n";
        std::cout << "+-------------------------------------------------------------------+\n";
        std::cout << "     Enter your choice\n>>>";
        std::cout.flush();
        std::cin >> choice;
        std::system(CLEAR);
        switch (choice)
        {
        case '1':
        {
            std::cout << "+-----[Add Student]--------------------------------------------------+\n";
            int math, phys, chem;
            std::cout << "Student Name\n>>>";
            std::cin >> student_name;
            std::cout << "Enter three marks [0 <= math,physics,chemistry <= 50]\n"
                         "     (Note : If any mark is invalid it will be treated as 0)\n>>>";
            math = get<int>();
            phys = get<int>();
            chem = get<int>();
            student_record.addStudent(student_name, Marks(math, phys, chem));
        }
        break;
        case '2':
        {
            std::cout << "+-----[Remove Student]-----------------------------------------------+\n";
            std::cout << "Student Name\n>>>";
            std::cin >> student_name;
            student_record.removeStudentF(student_name);
        }
        break;
        case '3':
        {
            std::cout << "+-----[Remove Student]-----------------------------------------------+" << std::endl;
            std::cout << "Student Name\n>>>" << std::flush;
            std::cin >> student_name;
            student_record.removeStudentL(student_name);
        }
        break;
        case '4':
        {
            std::cout << "+-----[All Students]-------------------------------------------------+" << std::endl;
            student_record.showAllStudent();
        }
        break;
        case '5':
        {
            std::cout << "+-----[Top Students]-------------------------------------------------+" << std::endl;
            std::cout << "Subject <math|physics|chemistry>\n>>>" << std::flush;
            std::string subject;
            std::cin >> subject;
            student_record.showTopStudents(subject);
        }
        break;
        default:
        {
            std::cout << "Thanks for using this application!" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::exit(0);
        }
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        std::cin.get();
        std::system(CLEAR);
    } while (true);
}
