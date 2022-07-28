
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <kmt/Table.hpp>
#include <kmt/BasicView.hpp>

#include "test_helper.hpp"

using namespace testing;
using namespace km::tp;
using dt = km::DataType;

TEST(Table, Constructor)
{
    EXPECT_THROW(km::Table("@invalid/name", {}), std::invalid_argument);
    EXPECT_THROW(km::Table("valid_table_name", {{"valid_column1", dt::INT32}, {"invalid@bolumn$name", dt::INT32}, {"valid_column2", dt::INT32}}), std::invalid_argument);
    EXPECT_THROW(km::Table("valid table_name", {{"duplicate_column", dt::INT32}, {"unique_coulmn", dt::INT32}, {"duplicate_column", dt::INT32}}), std::invalid_argument);
    EXPECT_THROW(km::Table("valid table name", {{"invalid_datatype", dt::INT32 | dt::INT64}}), std::invalid_argument);
    EXPECT_NO_THROW(km::Table("student records", {{"name", dt::STRING}, {"roll_no", dt::INT32}, {"age", dt::INT32}, {"percentage", dt::FLOAT32}}, km::SortingOrder::ASCENDING));
}

TEST(Table, InsertRow)
{
    km::Table table("exam_result", {{"name", dt::STRING}, {"OS", dt::INT32}, {"networking", dt::INT32}, {"linux", dt::INT32}});
    EXPECT_NO_THROW(table.insertRow({"Keshav", 93, 90, 96}));
    EXPECT_NO_THROW(table.insertRow({"Aarati", 89, 83, 85}));

    EXPECT_EQ(table.insertRow({"Janaki", 87, 81, 88}), 1);    // Janki must be inserted between Aarati and Keshav e.g. index 1
    EXPECT_EQ(table.insertRow({"Bhupendra", 91, 90, 92}), 1); // Bhupendra must be inserted between  Aarati and Janaki e.g. index 1
    EXPECT_EQ(table.insertRow({"Hemant", 89, 90, 90}), 2);    // Hemant must be inserted after Bhupendra e.g. index 2
    table.pauseSorting();
    EXPECT_EQ(table.insertRow({"Chhattrapal", 83, 81, 85}), 5); // Chhatrapal should be inserted before Hemant, but sorting is paused.
    EXPECT_EQ(table.insertRow({"Hema", 82, 82, 84}), 6);        // Hema should be inserted before Hemant, but sorting is paused.
    table.resumeSorting();                                      // now Chhatrapal and Hema are inserted in correct position.
    EXPECT_EQ(table.searchInKeyColumn("Chhattrapal").front(), 2);
    EXPECT_EQ(table.rowCount(), 7);
    EXPECT_EQ(table.insertRow({"Yamini", 70.3, 71, 73}), km::INVALID_INDEX); // 70.3 is not a int32 value.
    EXPECT_EQ(table.insertRow({"Pratap", 82, 83}), km::INVALID_INDEX);       // only 3 elements are passed to insert to row.
    EXPECT_EQ(table.rowCount(), 7);
}

TEST(Table, DropRow)
{
    std::unique_ptr<km::Table> table(test_local::getStudentTable());
    EXPECT_TRUE(table->dropRow(3));         //drops first hema
    EXPECT_TRUE(table->dropRow(8));         //drops teman as teman is now at 8th index.
    EXPECT_FALSE(table->dropRow(table->rowCount()));
    EXPECT_FALSE(table->dropRow(table->rowCount() + 100));
}


TEST(Table, AddColumnEF)
{
    km::Table table("person", {{"name", dt::STRING}, {"age", dt::INT32}});
    table.insertRow({"Keshav Sahu", 24});
    table.insertRow({"Belton Petra", 15});
    table.insertRow({"Yash Sanap", 23});
    table.insertRow({"Laxmikant Yadav", 17});

    table.addColumnF({"can_vote", dt::BOOLEAN}, [&table](IndexType index)
                     { return static_cast<KBoolean>(table.getDataWC(index, 1).asInt32() >= 18); });
    table.addColumnE({"can_vote2", dt::BOOLEAN}, "isGreaterOrEqual($age,18)");
    //0 => name, 1 => can_vote, 2 => can_vote2
    EXPECT_EQ(table.getDataWC(0, 2).data(), table.getDataWC(0, 3).data());
    EXPECT_EQ(table.getDataWC(1, 2).data(), table.getDataWC(1, 3).data());
    EXPECT_EQ(table.getDataWC(2, 2).data(), table.getDataWC(2, 3).data());
    EXPECT_EQ(table.getDataWC(3, 2).data(), table.getDataWC(3, 3).data());
}

TEST(Table, BasicView)
{
    km::Table table("person", {{"name", dt::STRING}, {"age", dt::INT32}});
    table.insertRow({"Keshav Sahu", 24});
    table.insertRow({"Belton Petra", 17});

    // compile time function
    table.addColumnF({"can_vote", dt::BOOLEAN}, [&table](IndexType index)
                     { return static_cast<KBoolean>(table.getDataWC(index, 1).asInt32() >= 18); });

    km::BasicView view1("view_1", &table, {}, "isGreaterOrEqual($age,18)"); // using dynamic function IsGreaterOrEqual
    km::BasicView view2("view_2", &table, {}, "$can_vote");                 // using existing can_vote column
    ASSERT_EQ(view1.columnCount(), view2.columnCount());
    ASSERT_EQ(view1.rowCount(), view2.rowCount());

    const SizeType r_count = view1.rowCount();
    const SizeType c_count = view1.columnCount();
    for (IndexType c = 0; c < c_count; ++c)
        for (IndexType r = 0; r < r_count; ++r)
            EXPECT_EQ(view1.getDataWC(r, c).data(), view2.getDataWC(r, c).data());
}

TEST(Table, TransformColumn)
{
    km::Table table("person", {{"name", dt::STRING}}, km::SortingOrder::ASCENDING);
    table.insertRow({"Keshav Sahu"});   // index 0
    table.insertRow({"Yash Sanap"});    // index 1
    
    EXPECT_TRUE(table.transformColumn("name", "lowerCase($name)"));
    EXPECT_EQ(table.getDataWC(0, 0).asString(), "keshav sahu");
    EXPECT_EQ(table.getDataWC(1, 0).asString(), "yash sanap");

    EXPECT_TRUE(table.transformColumn("name", "upperCase($name)"));
    EXPECT_EQ(table.getDataWC(0, 0).asString(), "KESHAV SAHU");
    EXPECT_EQ(table.getDataWC(1, 0).asString(), "YASH SANAP");

    EXPECT_FALSE(table.transformColumn("non_existing_column", "sentenseCase($name)"));
    EXPECT_FALSE(table.transformColumn("name","nonExistingFunction($name)"));
    EXPECT_FALSE(table.transformColumn("name","add(5,3)"));     //add(5,3) returns int32 not string.
}

TEST(Table, FloatingPointEquality)
{
    constexpr KFloat64 search_for = 0.0001;
    constexpr KFloat64 epsilon = 0.00000000000000001;
    km::Table table("floating_point_equality_test", {{"values", dt::FLOAT64}});
    table.setEpsilon("values", KFloat64(epsilon)); // floating point accuracy used for searching the data
    table.insertRow({KFloat64(0.0001000000000002645)});
    table.insertRow({KFloat64(0.0001000000000000032)});
    table.insertRow({KFloat64(0.0001000000000001242)});

    std::vector<IndexType> indices = table.search("values", search_for);
    EXPECT_EQ(indices.size(), 1);
    if (!indices.empty())
    {
        auto verify = [search_for, epsilon](KFloat64 value)
        {
            return std::abs(search_for - value) < epsilon;
        }(table.getDataWC(indices[0], 0).asFloat64());
        EXPECT_TRUE(verify);
    }
}

TEST(Table, SearchFeature)
{
    km::Table table("search_example", {{"Name", dt::STRING}, {"Age", dt::INT32}});
    std::vector<std::pair<KString, KInt32>> pairs = {
        {"Keshav", 25},     // 25 %> 1       index[5]
        {"Yash", 23},       // 23 %> 1       index[7]
        {"Hemant", 25},     // 25 %> 2       index[4]
        {"Hema", 23},       // 23 %> 2       index[3]
        {"Aarati", 23},     // 23 %> 3       index[0]
        {"Subbarao", 24},   // 24 %> 1       index[6]
        {"Ali", 26},        // 26 %> 1       index[1]
        {"Christopher", 27} // 27 %> 1       index[2]
    };

    for (auto &[name, age] : pairs)
    {
        table.insertRow({name, age});
    }
    auto age_23 = table.search("Age", KInt32(23));
    auto age_25 = table.search("Age", KInt32(25));
    auto name_subbarao = table.search("Name", "Ali");

    ASSERT_EQ(age_23.size(), 3);
    EXPECT_EQ(table.getDataWC(age_23[0], 0).asString(), "Aarati");
    EXPECT_EQ(table.getDataWC(age_23[1], 0).asString(), "Hema");
    EXPECT_EQ(table.getDataWC(age_23[2], 0).asString(), "Yash");

    ASSERT_EQ(age_25.size(), 2);
    EXPECT_EQ(table.getDataWC(age_25[0], 0).asString(), "Hemant");
    EXPECT_EQ(table.getDataWC(age_25[1], 0).asString(), "Keshav");

    ASSERT_EQ(name_subbarao.size(), 1);
    EXPECT_EQ(name_subbarao[0], 1);
    EXPECT_EQ(table.getDataWC(name_subbarao[0], 1).asInt32(), 26);
}

TEST(Table, DataGetters)
{
    km::Table &table = *test_local::getStaticStudentTable(); // see table_helper.cpp

    ASSERT_EQ(table.rowCount(), 10);
    ASSERT_EQ(table.columnCount(), 2);

    std::optional<km::Variant> valid_data = table.getData(3, 0);
    EXPECT_TRUE(valid_data.has_value());
    EXPECT_TRUE(valid_data.value().asString() == "Hema");
    std::optional<km::Variant> invalid_data = table.getData(0, 3); // 3 is invalid as table has only 2 columns.
    EXPECT_FALSE(invalid_data.has_value());

    EXPECT_EQ(table.getDataWC(0, 0).asString(), "Aarati");
    EXPECT_EQ(table.getDataWC(1, 0).asString(), "Bhupendra");
    EXPECT_EQ(table.getDataWC(2, 0).asString(), "Chhatrapal");

    EXPECT_TRUE(table.getDataWC(3, 0).asString() == "Hema" && table.getDataWC(3, 1).asInt32() == 3); // retain the insertion order
    EXPECT_TRUE(table.getDataWC(4, 0).asString() == "Hema" && table.getDataWC(4, 1).asInt32() == 4);

    EXPECT_EQ(table.getDataWC(5, 0).asString(), "Hemant");
    EXPECT_EQ(table.getDataWC(6, 0).asString(), "Janaki");
    EXPECT_EQ(table.getDataWC(7, 0).asString(), "Keshav");
    EXPECT_EQ(table.getDataWC(8, 0).asString(), "Ketan");
    EXPECT_EQ(table.getDataWC(9, 0).asString(), "Teman");

    for (IndexType row = 0, row_count = table.rowCount(); row < row_count; ++row)
    {
        // getData and getDataWC does the same thing (except getData does bound checkings)
        ASSERT_EQ(table.getData(row, 0).value().asString(), table.getDataWC(row, 0).asString());
        ASSERT_EQ(table.getData(row, 1).value().asInt32(), table.getDataWC(row, 1).asInt32());
    }

    EXPECT_EQ(table.getKeyColumn(), 0);
}

TEST(Table, AddColumn)
{
    km::Table table("table", {});
    ASSERT_EQ(table.columnCount(), 0);
    ASSERT_EQ(table.rowCount(), 0);

    struct column_info
    {
        std::string column_name;
        dt data_type;
        km::Variant fill_with;
        bool expected_result;
    } columns[6] = {
        {"valid_name", dt::INT32, 10, true},
        {"@=#invalid_name", dt::INT64, KInt64(30ll), false},        //had issues in linux with g++.
        {"duplicate_name", dt::STRING, "hello", true},
        {"duplicate_name", dt::INT32, "5", false},
        {"matching_type", dt::BOOLEAN, true, true},
        {"non_matching_type", dt::INT32, "string data instead of int32", false}};
    SizeType column_count = 0;
    for (int i = 0; i < 6; ++i)
    {
        const auto &[column_name, data_type, fill_with, expected_result] = columns[i];
        EXPECT_EQ(table.addColumn({column_name, data_type}, fill_with), expected_result);
        expected_result && ++column_count;
        EXPECT_EQ(table.columnCount(), column_count);
    }
    
    EXPECT_EQ(table.columnCount(), 3);
}

