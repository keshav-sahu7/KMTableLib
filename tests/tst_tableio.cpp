#include <filesystem>

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <kmt/TableIO.hpp>
#include <kmt/Table.hpp>

#include "test_helper.hpp"

#define SAME_OF(tgt1, tgt2, context)   tgt1->context , tgt2->context

TEST(TableIO, TableIO)
{
    std::unique_ptr<km::Table> table(test_local::getStudentTable());
    std::filesystem::create_directory("table_dir");
    ASSERT_TRUE(km::writeTableTo(table.get(),"table_dir"));

    std::unique_ptr<km::Table> tmp_table(km::readTableFrom("table", "table_dir"));

    ASSERT_TRUE(static_cast<bool>(tmp_table));

    ASSERT_EQ(table->getName(), tmp_table->getName());
    ASSERT_EQ(table->rowCount(), tmp_table->rowCount());
    ASSERT_EQ(table->columnCount(), tmp_table->columnCount());
    ASSERT_EQ(table->getSortingOrder(), tmp_table->getSortingOrder());

    for(km::IndexType c = 0, c_count = table->columnCount(); c < c_count; ++c)
    {
        const auto &col_1 = table->getColumnMetaData(c);
        const auto &col_2 = tmp_table->getColumnMetaData(c);
        EXPECT_EQ(col_1.column_name,col_2.column_name);
        EXPECT_EQ(col_1.display_name,col_2.display_name);
        EXPECT_EQ(col_1.data_type,col_2.data_type);
        auto comp = km::isEqualComparatorFor(col_1.data_type);
        for(km::IndexType r = 0, r_count = table->rowCount(); r < r_count; ++r)
        {
            EXPECT_TRUE(comp(table->getDataWC(r,c),tmp_table->getDataWC(r,c)));
        }
    }
}
