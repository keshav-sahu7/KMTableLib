#ifndef KMTABLE_TESTS_TABLE_HELPER_HPP
#define KMTABLE_TESTS_TABLE_HELPER_HPP
#include <memory>
#include <kmt/Table.hpp>

namespace test_local
{
    km::Table *getStudentTable();
    km::Table *getStaticStudentTable();
    km::Variant is_odd(const km::Variant *args);

    bool isSorted(km::AbstractTable *table, km::IndexType column_index, km::SortingOrder s_order = km::SortingOrder::ASCENDING);
}

#endif // KMTABLE_TESTS_TABLE_HELPER_HPP
