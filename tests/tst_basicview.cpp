#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <kmt/Table.hpp>
#include <kmt/BasicView.hpp>
#include <kmt/CsvWriter.hpp>
#include <kmt/Printer.hpp>

#include "test_helper.hpp"

using namespace testing;

using namespace km::tp; // KInt32, KInt64, ...
using dt = km::DataType;

TEST(BasicView, Constructor)
{
    km::Table empty_table("empty_table", {});

    km::Table filled_table("filled_table", {{"num", dt::INT32}, {"num2", dt::INT64}});

    filled_table.pauseSorting();  // notice, filled_table's sorting is paused, accessing it may give unpredictable results
    for (int i = 0; i < 100; ++i) // fill the table
        filled_table.insertRow({KInt32(i), KInt64(100 - i)});

    EXPECT_ANY_THROW(km::BasicView("view", &filled_table, {})); // filled_table's sorting is paused.
    filled_table.resumeSorting();                      // now filled_table is sorted and ready for the BasicView.
    EXPECT_NO_THROW(km::BasicView("valid_view_name", &filled_table,{}));

    EXPECT_ANY_THROW(km::BasicView("@invalid \\view/ name", &filled_table,{}));
    EXPECT_ANY_THROW(km::BasicView("valid view name", &empty_table, {}));

    EXPECT_ANY_THROW(km::BasicView("view", &filled_table, {}, "NOT(isEqual($xyz,10))")); // xyz is not a column in filled_table.

    EXPECT_ANY_THROW(km::BasicView("valid_view name", &filled_table, {"num", "num3"})); // num3 is not a column in filled_table.
    {
        km::BasicView view1("view_1", &filled_table, {}, "isEqual(mod($num2,2l),0l)");     // rows with even numbers in column `num2`
        ASSERT_EQ(view1.rowCount(), 50);                                                    // 50 odd and 50 even numbers are present in filled_table's column `num2`
        ASSERT_EQ(view1.columnCount(), 2);                                                  // filled_table has 2 columns.

        km::BasicView view2("view_2", &filled_table, {"num2"}, "isEqual(mod($num2,2l),0l)"); // rows with even numbers in column `num2`
        ASSERT_EQ(view2.rowCount(), 50);                                                     // 50 odd and 50 even numbers are present in filled_table's column `num2`
        ASSERT_EQ(view2.columnCount(), 1);                                                   // only num2 was selected.

        km::BasicView view3("view_3", &filled_table, {"num2"});
        ASSERT_EQ(view3.rowCount(), 100);  // 100 rows in filled_table
        ASSERT_EQ(view3.columnCount(), 1); // only num2 was selected.

        km::BasicView view4("view_4", &filled_table, {});
        ASSERT_EQ(view4.rowCount(), 100);  // selects all rows that filled_table has.
        ASSERT_EQ(view4.columnCount(), 2); // selects all columns that filled_table has.
    }
}


TEST(BasicView, EmptySort)
{
    km::Table *source_table = test_local::getStaticStudentTable();
    km::BasicView view_name("view_name",source_table,{"name"});
    ASSERT_EQ(source_table->rowCount(),view_name.rowCount());
    ASSERT_TRUE(test_local::isSorted(&view_name,0,km::SortingOrder::ASCENDING));

    km::BasicView view_id("view_id",source_table,{"id"});
    ASSERT_EQ(source_table->rowCount(),view_id.rowCount());
    ASSERT_TRUE(test_local::isSorted(&view_id,0,km::SortingOrder::ASCENDING));
}
TEST(BasicView, DataGetters)
{
    km::Table &source_table = *test_local::getStaticStudentTable();

    // change order of columns
    //  table ( 0 : name   ,    1 : id)
    //  view  ( 0 : id   ,    1 : name)
    km::BasicView view("my_view", &source_table, {"id", "name"},"","name");   // "" => selects all

    ASSERT_EQ(view.rowCount(), 10);
    ASSERT_EQ(view.columnCount(), 2);

    auto table_name_index = source_table.findColumn("name").value().first;
    auto table_id_index = source_table.findColumn("id").value().first;
    auto view_name_index = view.findColumn("name").value().first;
    auto view_id_index = view.findColumn("id").value().first;

    EXPECT_TRUE(table_name_index == 0 && view_name_index == 1);
    EXPECT_TRUE(table_id_index == 1 && view_id_index == 0);

    EXPECT_NO_THROW(source_table.getDataWC(5, 0).asString());                       // column[0] holds string values in source table.
    EXPECT_THROW(view.getDataWC(5, 0).asString(), std::bad_variant_access);         // column[0] holds 32 bit integer values in views.
    EXPECT_THROW(source_table.getDataWC(5, 0).asInt32(), std::bad_variant_access);  // column[0] holds string values in source table.
    EXPECT_NO_THROW(view.getDataWC(5, 0).asInt32());                                // column[0] holds 32 bit integer values in views.
    EXPECT_THROW(source_table.getDataWC(5, 1).asString(), std::bad_variant_access); // column[1] holds 32 bit integer values in source table.
    EXPECT_NO_THROW(view.getDataWC(5, 1).asString());                               // column[1] holds string values in views.
    EXPECT_NO_THROW(source_table.getDataWC(5, 1).asInt32());                        // column[1] holds 32 bit integer values in source table.
    EXPECT_THROW(view.getDataWC(5, 1).asInt32(), std::bad_variant_access);          // column[1] holds string values in views.

    EXPECT_EQ(view.getData(7, 1).value().asString(), "Keshav");
    EXPECT_EQ(view.getData(7, 0).value().asInt32(), 1);

    for (IndexType row = 0, row_count = source_table.rowCount(); row < row_count; ++row)
    {
        ASSERT_EQ(source_table.getDataWC(row, 0).asString(), view.getDataWC(row, 1).asString());
        ASSERT_EQ(source_table.getDataWC(row, 1).asInt32(), view.getDataWC(row, 0).asInt32());
    }
}

TEST(BasicView, SortByColumnName)
{
    km::Table &source_table = *test_local::getStaticStudentTable();

    km::BasicView view("my_view", &source_table, {});
    ASSERT_EQ(view.rowCount(), 10);
    ASSERT_EQ(view.columnCount(), 2);

    EXPECT_EQ(view.getKeyColumn(), 0);
    view.sortBy("non_existing_column");
    EXPECT_EQ(view.getKeyColumn(), 0);
    view.sortBy("id");
    EXPECT_EQ(view.getKeyColumn(), 1);
    EXPECT_EQ(view.getDataWC(0, 0).asString(), "Keshav");
    EXPECT_EQ(view.getDataWC(1, 0).asString(), "Hemant");
    EXPECT_EQ(view.getDataWC(2, 0).asString(), "Hema");
    EXPECT_EQ(view.getDataWC(3, 0).asString(), "Hema");
    EXPECT_EQ(view.getDataWC(4, 0).asString(), "Chhatrapal");
    EXPECT_EQ(view.getDataWC(5, 0).asString(), "Aarati");
    EXPECT_EQ(view.getDataWC(6, 0).asString(), "Bhupendra");
    EXPECT_EQ(view.getDataWC(7, 0).asString(), "Ketan");
    EXPECT_EQ(view.getDataWC(8, 0).asString(), "Teman");
    EXPECT_EQ(view.getDataWC(9, 0).asString(), "Janaki");

    view.sortBy("name");
    EXPECT_EQ(view.getKeyColumn(), 0);
}

TEST(BasicView, InsertionEvent)
{
    std::shared_ptr<km::Table> table(test_local::getStudentTable());

    km::BasicView view_name("view", table.get(), {}); // sorted by name but in descending
    view_name.sortBy(km::SortingOrder::DESCENDING);

    km::BasicView view_id("view", table.get(), {}, "isLess($id,20)"); // sorted by id but in ascending
    view_id.sortBy("id");

    EXPECT_EQ(view_name.rowCount(), 10);
    EXPECT_EQ(view_id.rowCount(), 10);
    // do insertions in table, and view will be automatically updated
    table->insertRow({"Ranga", 23}); // inserted in view_name | 23 < 20 => false, not inserted in view_id
    table->insertRow({"Billa", 19}); // inserted in view_name | 19 < 20 => true, inserted in view_id

    EXPECT_EQ(view_name.rowCount(), 12);
    EXPECT_EQ(view_id.rowCount(), 11);

    EXPECT_EQ(view_name.getDataWC(9, 0).asString(), "Billa");
    EXPECT_EQ(view_id.getDataWC(10, 0).asString(), "Billa");
    EXPECT_EQ(table->getDataWC(2, 0).asString(), "Billa");
}

TEST(BasicView, DropEvent)
{
    std::shared_ptr<km::Table> table(test_local::getStudentTable());

    km::BasicView view_name("by_name", table.get(), {}); // sorted by name but in ascending order

    km::BasicView view_id("by_id", table.get(), {}, "isOdd($id)"); // sorted by id but in descending order, only odd id (e.g. id % 2 == 1) will be stored.
    view_id.sortBy("id", km::SortingOrder::DESCENDING);

    EXPECT_EQ(view_name.rowCount(), 10);
    EXPECT_EQ(view_id.rowCount(), 5);

    table->dropRow(3); //(Hema id:3)           included in view_id and view_name
    table->dropRow(5); // after removal of (Hema id:3), (Janaki id:10 is placed at 5), included in view_name only

    EXPECT_EQ(view_name.rowCount(), 8);
    EXPECT_EQ(view_id.rowCount(), 4);

    EXPECT_EQ(view_name.getDataWC(5, 0).asString(), "Keshav");
    EXPECT_EQ(view_id.getDataWC(3, 0).asString(), "Keshav");
    EXPECT_EQ(table->getDataWC(5, 0).asString(), "Keshav");
}

TEST(BasicView, DataUpdateEvent1)
{
    km::Table table("simple_table", {{"x", dt::INT32}, {"y", dt::INT32}, {"z", dt::INT32}});
    for (KInt32 i = 0; i < 10; ++i)
    {
        table.insertRow({i, i, i}); // all rows will have same value
    }
    km::BasicView view1("view1", &table, {}); // selects all columns
    km::BasicView view2("view2", &view1, {}); // selects all columns

    view1.sortBy("y", km::SortingOrder::DESCENDING);
    view2.sortBy("z", km::SortingOrder::ASCENDING);

    ASSERT_EQ(view1.rowCount(), table.rowCount());
    ASSERT_EQ(view2.rowCount(), view1.rowCount());
    EXPECT_EQ(view1.getDataWC(0, 0).asInt32(), 9); //{9,9,9} is the first row in view1
    EXPECT_EQ(view2.getDataWC(0, 0).asInt32(), 0); //{0,0,0} is the first row in view2

    table.setData(0, 1, KInt32(100)); // 100 will take the {0,0,0} => {0,100,0} to the top in view1 and won't affect view2
    ASSERT_EQ(view1.rowCount(), table.rowCount());
    ASSERT_EQ(view2.rowCount(), view1.rowCount());
    EXPECT_EQ(view1.getDataWC(0, 0).asInt32(), 0);
    EXPECT_EQ(view2.getDataWC(0, 0).asInt32(), 0);

    table.setData(5, 1, KInt32(0)); // it will take down {5,5,5} => {5,0,5} to the bottom in view1 and won't affect view2
    EXPECT_EQ(view1.getDataWC(9, 0).asInt32(), 5);
    EXPECT_EQ(view2.getDataWC(9, 0).asInt32(), 9);

    table.setData(5, 2, KInt32(20));               // should not change the order of {5,0,5} => {5,0,20} in view1 but should change view2
    EXPECT_EQ(view1.getDataWC(9, 0).asInt32(), 5); // last because of 0
    EXPECT_EQ(view2.getDataWC(9, 0).asInt32(), 5); // last because of 20

    EXPECT_EQ(view1.getDataWC(5, 0).asInt32(), 4);
    EXPECT_EQ(view2.getDataWC(4, 0).asInt32(), 4);
    table.setData(4, 2, KInt32(10)); // should not change the order of {4,4,4} => {4,4,10} in view1 but should change view2
    EXPECT_EQ(view1.getDataWC(5, 0).asInt32(), 4);
    EXPECT_EQ(view2.getDataWC(8, 0).asInt32(), 4);
}





TEST(BasicView, DataUpdateEvent2)
{
    km::Table table("simple_table", {{"x", dt::INT32}, {"y", dt::INT32}, {"z", dt::INT32}});
    for (KInt32 i = 0; i < 30; ++i)
    {
        table.insertRow({i, i, i});
    }
    //1 3 5 7 9 11 13 14 15 17 19 20 21 23 25 27 29
    km::BasicView view1("view1", &table, {}, "isOdd($z)"); // selects all columns from table where x is odd number

    //3 9 14 21 27
    km::BasicView view2("view2", &view1, {}, "isEqual(mod($y,3),0)"); // selects all columns from view1 where y is divisible by 3

    ASSERT_EQ(view1.rowCount(), 15);
    ASSERT_EQ(view2.rowCount(), 5);

    table.setData(10,1,KInt32(100));            //{10,10,10} => {10,100,10} is part of neither view1 nor view2.
    ASSERT_EQ(view1.rowCount(), 15);
    ASSERT_EQ(view2.rowCount(), 5);

    table.setData(11,1,KInt32(100));            //{11,11,11} => {11,100,11} is part of view1 but not part of view2.
    ASSERT_EQ(view1.rowCount(), 15);
    ASSERT_EQ(view2.rowCount(), 5);

    table.setData(13,2,KInt32(30));             //{13,13,13} => {13,13,30} should be removed from view1 and no effect on view2.
    ASSERT_EQ(view1.rowCount(), 14);
    ASSERT_EQ(view2.rowCount(), 5);
    EXPECT_EQ(view2.getDataWC(4,0).asInt32(), 27);


    table.setData(14,1,KInt32(39));             //{14,14,14} => {14,39,14} should not affect both view1 and view2.
    ASSERT_EQ(view1.rowCount(), 14);
    ASSERT_EQ(view2.rowCount(), 5);

    table.setData(14,2,KInt32(37));             //{14,39,14} => {14,39,37} should be added in both view1 and view2.
    ASSERT_EQ(view1.rowCount(), 15);
    ASSERT_EQ(view2.rowCount(), 6);
    EXPECT_EQ(view1.getDataWC(6,2).asInt32(), 37);
    EXPECT_EQ(view2.getDataWC(2,1).asInt32(), 39);

    table.setData(15,2,61);                     //{15,15,15} => {15,15,61} should not affect view1 and view2
    ASSERT_EQ(view1.rowCount(), 15);
    ASSERT_EQ(view2.rowCount(), 6);
    EXPECT_EQ(view1.getDataWC(7,2).asInt32(), 61);
    EXPECT_EQ(view2.getDataWC(3,0).asInt32(), 15);
    
    table.setData(15,1,61);                     //{15,15,61} => {15,61,61} should not affect view1 but should be removed from view2
    ASSERT_EQ(view1.rowCount(), 15);
    ASSERT_EQ(view2.rowCount(), 5);
    EXPECT_EQ(view1.getDataWC(7,2).asInt32(), 61);
    EXPECT_EQ(view2.getDataWC(3,0).asInt32(), 21);

    table.setData(20,2,91);                 //{20,20,20} => {20,20,91} should be added in view1 but not in view2
    ASSERT_EQ(view1.rowCount(), 16);
    ASSERT_EQ(view2.rowCount(), 5);
    EXPECT_EQ(view1.getDataWC(10,2).asInt32(), 91);

    km::writeAsCsv(&view2,"view2.1");
    view1.sortBy("y", km::SortingOrder::DESCENDING);        //both view1 and view2 will be sorted by column "y" but in opposite order.
    

    
    table.setData(27,1,15);             //should change the order in view1 and view2.
    ASSERT_EQ(view1.rowCount(), 16);
    ASSERT_EQ(view2.rowCount(), 5);
    EXPECT_EQ(view1.getDataWC(10,0).asInt32(),27);
    EXPECT_EQ(view2.getDataWC(4,0).asInt32(),27);

    km::writeAsCsv(&view2,"view2.2");
    view2.sortBy("y",km::SortingOrder::ASCENDING);
    EXPECT_EQ(view2.getDataWC(2,0).asInt32(),27);
    km::writeAsCsv(&view1,"view1");
    km::writeAsCsv(&view2,"view2");
}


TEST(BasicView,SourceSortedEvent)
{
    km::Table table("table",{
                        {
                            "x", dt::INT32
                        },
                        {
                            "y", dt::INT32
                        }
                    });
    table.pauseSorting();
    for(KInt32 i = 0; i < 1000; ++i)
    {
        table.insertRow({i,1000 - i * (i%2)});
    }
    table.resumeSorting();

    km::BasicView view1("view1",&table, {}, "isEqual(mod($x,5),0)", "x", km::SortingOrder::DESCENDING);
    km::BasicView view2("view2",&view1, {}, "NOT(isOdd($y))", "y");

    EXPECT_TRUE(test_local::isSorted(&view1, 0, km::SortingOrder::DESCENDING));
    EXPECT_TRUE(test_local::isSorted(&view2, 1, km::SortingOrder::ASCENDING));

    view2.sortBy("x",km::SortingOrder::DESCENDING);
    EXPECT_TRUE(test_local::isSorted(&view1, 0, km::SortingOrder::DESCENDING));
    EXPECT_TRUE(test_local::isSorted(&view2, 0, km::SortingOrder::DESCENDING));
    view1.sortBy("y", km::SortingOrder::ASCENDING);
    EXPECT_TRUE(test_local::isSorted(&view1, 1, km::SortingOrder::ASCENDING));
    EXPECT_TRUE(test_local::isSorted(&view2, 0, km::SortingOrder::DESCENDING));
}

TEST(BasicView,DanglingView)
{
    km::Table *table = test_local::getStudentTable();
    km::BasicView view("view",table,{});
    EXPECT_EQ(table->rowCount(),view.rowCount());
    EXPECT_EQ(table->columnCount(),view.columnCount());
    delete table;
    view.sortBy("id");
    EXPECT_EQ(view.rowCount(),0);
    EXPECT_EQ(view.columnCount(),0);
}
