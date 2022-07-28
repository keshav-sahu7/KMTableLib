#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <kmt/Table.hpp>
#include <kmt/CSVWriter.hpp>

#include "test_helper.hpp"

TEST(CsvWriter,CsvWriter)
{
    const km::Table &table = *test_local::getStaticStudentTable();
    EXPECT_FALSE(km::writeAsCsv(nullptr,"file_name"));        //null table
    EXPECT_FALSE(std::filesystem::exists("file_name.csv"));
    
    EXPECT_TRUE(km::writeAsCsv(&table,"my_data"));              //note : it doesn't have file extension.
    EXPECT_TRUE(std::filesystem::exists("my_data.csv"));
    EXPECT_TRUE(km::writeAsCsv(&table,"my_data2.csv"));         //it has .csv file extension.
    EXPECT_TRUE(std::filesystem::exists("my_data2.csv"));
    EXPECT_TRUE(km::writeAsCsv(&table,".bin"));
    EXPECT_TRUE(std::filesystem::exists(".bin.csv"));
    EXPECT_TRUE(km::writeAsCsv(&table,".csv"));         //note : .csv is taken as a file name
    EXPECT_TRUE(std::filesystem::exists(".csv.csv"));
}

