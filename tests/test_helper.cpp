#include "test_helper.hpp"
#include <kmt/Core.hpp>

namespace test_local
{
    km::Table *getStudentTable()
    {
        km::Table *source_table = new km::Table("table", {{"name", "your name", km::DataType::STRING}, {"id", "your id", km::DataType::INT32}});
        //           [sorting order]             name wise (table)   id wise (view)
        source_table->insertRow({"Keshav", 1});     // 7                 0
        source_table->insertRow({"Hemant", 2});     // 5                 1
        source_table->insertRow({"Hema", 3});       // 3                 2
        source_table->insertRow({"Hema", 4});       // 4                 3
        source_table->insertRow({"Aarati", 6});     // 0                 5
        source_table->insertRow({"Chhatrapal", 5}); // 2                 4
        source_table->insertRow({"Ketan", 8});      // 8                 7
        source_table->insertRow({"Bhupendra", 7});  // 1                 6
        source_table->insertRow({"Teman", 9});      // 9                 8
        source_table->insertRow({"Janaki", 10});    // 6                 9
        return source_table;
    }

    km::Table *getStaticStudentTable()
    {
        static std::shared_ptr<km::Table> table(getStudentTable());
        return table.get();
    }

    //register with isOdd_i and call as isOdd(int32)
    km::Variant is_odd(const km::Variant *args)
    {
        return static_cast<km::KBoolean> (args[0].asInt32() % 2);
    }

    bool isSorted(km::AbstractTable *table, km::IndexType column_index, km::SortingOrder s_order)
    {
        km::SizeType column_count = table->columnCount();
        if(column_count <= 1)
            return true;
        km::IndexType index = 0;
        auto column_type = table->columnAt(column_index).value().second;
        auto comp = s_order == km::SortingOrder::ASCENDING ? km::isGreaterComparatorFor(column_type) : km::isLessComparatorFor(column_type);
        while(index < (column_count - 1) && !comp(table->getDataWC(index,column_index),table->getDataWC(index+1,column_index))) {++index;};
        return (index == column_count - 1);
    }
}
