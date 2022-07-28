#include <iostream>
#include <memory>
#include <thread>
#include <filesystem>
#include <string>
#include <chrono>
#include <map>

#include <kmt/Table.hpp>
#include <kmt/BasicView.hpp>
#include <kmt/FunctionStore.hpp>
#include <kmt/Printer.hpp>
#include <kmt/TableIO.hpp>

using namespace km::tp;

#if defined(_WIN32) || defined(_WIN64)
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif


inline void waitForReturnKeyPress()
{
    std::cout << "Press Enter Key" << std::flush;
    std::string str;
    std::getline(std::cin, str);
}


static km::KString trimmed(const km::KString &str)
{
    auto s = std::find_if(str.begin(), str.end(), [](char c){ return !std::isspace(c);});
    auto e = std::find_if(str.rbegin(), str.rend(), [](char c) {return !std::isspace(c);});

    if(s == str.end())
        return {};

    std::size_t l = e - str.rbegin();
    return std::string(s, str.end() - l);
}


static KInt32 getInt(std::string_view prompt = std::string_view())
{
    KInt32 x;
    if(!prompt.empty())
        std::cout << prompt << " : " << std::flush;
    std::cin >> x;
    while (!std::cin.good())
    {
        std::cout << "Invalid Input enter again!\n\t\t: ";
        std::cout.flush();
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        std::cin >> x;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return x;
}

static KString getLine(std::string_view prompt = std::string_view())
{
    KString str;
    if(!prompt.empty())
        std::cout << prompt << " : " << std::flush;

    std::getline(std::cin, str);
    str = trimmed(str);
    std::replace_if(str.begin(), str.end(), [](char c){ return std::isspace(c);}, ' ');
    std::replace(str.begin(), str.end(), '"', '\'');
    return str;
}

void printData(const km::Variant &v, std::ostream &o)
{
    if(v.index() == 0)          // 0 => int, see km::indexForDataType()
        o << std::setw(10) << std::right << v.asInt32() << '|';
    else if(v.index() == 4)     // 4 => string
        o << std::setw(20) << std::left << v.asString() << '|';
}

void printHeaders(const std::vector<std::string> &, std::ostream &out, const char *)
{
    //ignoring headers passed to this function
    out << "+-------------------+--------------------+----------+--------------------+\n";
    out << "|       Title       |      Author        |   Price  |         Genre      |\n";
    out << "+-------------------+--------------------+----------+--------------------+";
    out.flush();
}


class BookLibrary
{
public:
    BookLibrary();
    ~BookLibrary();
    void addNewBook();
    void showAllBooks();
    void filterBookByAuthor();
    void filterBookByTitle();
    void filterBookByGenre();
    void filterBookByPriceRange();

private:
    static void print(const km::AbstractTable *table);
private:
    km::Table *m_table;
};

BookLibrary::BookLibrary()
{
    if (!(m_table = km::readTableFrom("BookLibrary.kmt", "BookLibraryData")))
    {
        m_table = new km::Table("BookLibrary", {{"title", km::DataType::STRING},
                                                {"author", km::DataType::STRING},
                                                {"price", km::DataType::INT32},
                                                {"genre", km::DataType::STRING}});
    }
}

BookLibrary::~BookLibrary()
{
    if (!std::filesystem::exists("BookLibraryData"))
        std::filesystem::create_directory("BookLibraryData");
    km::writeTableTo(m_table, "BookLibraryData");
    delete m_table;
}

void BookLibrary::addNewBook()
{
    KString title, author, genre;
    KInt32 price;
    std::cout << "+------------------------------------------------------------------------+\n";
    std::cout << "|                                 Add Book                               |\n";
    std::cout << "+------------------------------------------------------------------------+\n\n";
    title = getLine("Book Title");
    author = getLine("Author Name");
    price = getInt("Book Price");
    genre = getLine("Genre [Use space to separate genre]");
    m_table->insertRow({title, author, price, genre});
    std::cout << "Book added successfully!" << std::endl;
}

void BookLibrary::showAllBooks()
{
    std::cout << "+------------------------------------------------------------------------+\n";
    std::cout << "|                             All Books                                  |\n";
    std::cout << "+------------------------------------------------------------------------+\n\n\n";
    print(m_table);
}

void BookLibrary::filterBookByAuthor()
{
    std::cout << "+------------------------------------------------------------------------+\n";
    std::cout << "|                              Search Books                              |\n";
    std::cout << "+------------------------------------------------------------------------+\n\n";
    KString author = getLine("        Enter Author Name");
    km::BasicView view("Books Search Result", m_table, {},
                       "contains(toLower($author), toLower(\"" + author + "\"))", "author");
    std::cout << " Books of " << author << std::endl;
    print(&view);
}

void BookLibrary::filterBookByTitle()
{
    std::cout << "+------------------------------------------------------------------------+\n";
    std::cout << "|                          Search Books                                  |\n";
    std::cout << "+------------------------------------------------------------------------+\n\n";
    KString title = getLine("        Enter Book Title");
    km::BasicView view("Books Search Result", m_table, {},
                       "contains(toLower($title), toLower(\"" + title + "\"))",
                       "title");
    std::cout << " Books similar to `" << title << '`' << std::endl;
    print(&view);
}

void BookLibrary::filterBookByPriceRange()
{
    KInt32 range_s, range_e;

    std::cout << "+------------------------------------------------------------------------+\n";
    std::cout << "|                          Search Books                                  |\n";
    std::cout << "+------------------------------------------------------------------------+\n\n";
    range_s = getInt("        Enter Lowest Price");
    range_e = getInt("        Enter Highest Price");
    km::BasicView view("Books Search Result", m_table, {},
                       "isInRange($price, " + std::to_string(range_s) + ", " + std::to_string(range_e) + ")",
                       "price");
    std::cout << " Books in price range [" << range_s << " - " << range_e << "]" << std::endl;
    print(&view);
}

void BookLibrary::print(const km::AbstractTable *table)
{
    km::Printer printer(table);
    printer.printTableName(false);
    printer.setFunction({"title","author","price", "genre"}, printData);
    printer.setHeaderPrinter(printHeaders);
    printer.print(std::cout, "");
    std::cout << "+-------------------+--------------------+----------+--------------------+" << std::endl;
}

void BookLibrary::filterBookByGenre()
{
    KString genre;

    std::cout << "+------------------------------------------------------------------------+\n";
    std::cout << "|                             Search Books                               |\n";
    std::cout << "+------------------------------------------------------------------------+\n\n";

    genre = getLine("        Enter Genre");
    km::BasicView view("Books Search Result", m_table, {},
                       "contains( toLower($genre), toLower(\"" + genre + "\"))");

    std::cout << " Books of Genre " << genre << std::endl;
    print(&view);
}

char getMenuOption()
{
    std::system(CLEAR);
    std::cout << "+------------------------------------------------------------------------+\n"
              << "|                           SELECT OPTION                                |\n"
              << "+------------------------------------------------------------------------+\n"
              << "|                [1] Add Books                                           |\n"
              << "|                [2] List All Books                                      |\n"
              << "|                [3] Search Book By Name                                 |\n"
              << "|                [4] Search Book By Author                               |\n"
              << "|                [5] Search Book By Price Range                          |\n"
              << "|                [6] Search Book By Gener                                |\n"
              << "|                [7] Quit                                                |\n"
              << "+------------------------------------------------------------------------+\n"
              << "                 >>";
    std::cout << std::flush;
    char c;
    std::cin >> c;
    if(c != '\n')
    {
        std::string invalid_str;
        std::getline(std::cin, invalid_str);
    }
    std::system(CLEAR);
    return c;
}

int main()
{
    using namespace std::chrono_literals;
    std::system(CLEAR);
    std::cout << "\n\n\t\t\tWelcome to KM's Public Library!" << std::endl;
    std::this_thread::sleep_for(2s);

    BookLibrary library;

    // enable all functions to work with formula.
    km::initAllFnc();
    char c;

    std::map<char, void (BookLibrary::*)()> fmap =
    {
    {'1', &BookLibrary::addNewBook},
    {'2', &BookLibrary::showAllBooks},
    {'3', &BookLibrary::filterBookByTitle},
    {'4', &BookLibrary::filterBookByAuthor},
    {'5', &BookLibrary::filterBookByPriceRange},
    {'6', &BookLibrary::filterBookByGenre}
};

    while ((c = getMenuOption()) != '7')
    {
        auto it = fmap.find(c);
        try
        {
            if(it != fmap.end())
            {
                (library.*(it->second))();
                waitForReturnKeyPress();
            }
        }
        catch(std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }

    }
    return EXIT_SUCCESS;
}

