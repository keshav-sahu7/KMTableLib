#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <kmt/Core.hpp>

using namespace km::tp;

using dt = km::DataType;

TEST(Core, Datatype)
{
    ASSERT_EQ(km::indexForDataType(dt::INT32), 0);
    ASSERT_EQ(km::indexForDataType(dt::INT64), 1);
    ASSERT_EQ(km::indexForDataType(dt::FLOAT32), 2);
    ASSERT_EQ(km::indexForDataType(dt::FLOAT64), 3);
    ASSERT_EQ(km::indexForDataType(dt::STRING), 4);
    ASSERT_EQ(km::indexForDataType(dt::BOOLEAN), 5);
    ASSERT_EQ(km::indexForDataType(dt::DATE), 6);
    ASSERT_EQ(km::indexForDataType(dt::DATE_TIME), 7);

    ASSERT_EQ(km::indexForDataType(dt::BOOLEAN | dt::INT32), km::INVALID_INDEX);
    ASSERT_EQ(km::indexForDataType(dt::INT32 | dt::INT64, 20), 20);

    ASSERT_EQ(km::dataTypeToString(dt::INT32), "int32");
    ASSERT_EQ(km::dataTypeToString(dt::INT64), "int64");
    ASSERT_EQ(km::dataTypeToString(dt::FLOAT32), "float32");
    ASSERT_EQ(km::dataTypeToString(dt::FLOAT64), "float64");
    ASSERT_EQ(km::dataTypeToString(dt::STRING), "string");
    ASSERT_EQ(km::dataTypeToString(dt::BOOLEAN), "boolean");
    ASSERT_EQ(km::dataTypeToString(dt::DATE), "date");
    ASSERT_EQ(km::dataTypeToString(dt::DATE_TIME), "date_time");

    ASSERT_EQ(km::dataTypeToString(dt::BOOLEAN | dt::INT32), "undefined");
    ASSERT_EQ(km::dataTypeToString(dt::INT32 & dt::INT64), "undefined");
}

TEST(Core, Variant)
{
    EXPECT_NO_THROW(km::Variant(30).asInt32());
    EXPECT_NO_THROW(km::Variant(KInt64(320)).asInt64());
    EXPECT_NO_THROW(km::Variant(KFloat32(3.1f)).asFloat32());
    EXPECT_NO_THROW(km::Variant(KFloat64(3.9)).asFloat64());
    EXPECT_NO_THROW(km::Variant("c style string").asString());
    EXPECT_NO_THROW(km::Variant(KString("std::string")).asString());
    EXPECT_NO_THROW(km::Variant(true).asBoolean());
    EXPECT_NO_THROW(km::Variant(KDate{2022, 3, 15}).asDate());
    EXPECT_NO_THROW(km::Variant(KDateTime{{2022, 3, 15}, {20, 33, 33}}).asDateTime());

    EXPECT_THROW(km::Variant(30).asInt64(), std::bad_variant_access);
    EXPECT_THROW(km::Variant(KInt64(320)).asInt32(), std::bad_variant_access);
    EXPECT_THROW(km::Variant(KFloat32(3.1f)).asFloat64(), std::bad_variant_access);
    EXPECT_THROW(km::Variant(KFloat64(3.9)).asFloat32(), std::bad_variant_access);
    EXPECT_THROW(km::Variant("c style string").asBoolean(), std::bad_variant_access); // msvc converts const char* to boolean in std::variant instead of std::string
    EXPECT_THROW(km::Variant(KString("std::string")).asDate(), std::bad_variant_access);
    EXPECT_THROW(km::Variant(true).asInt32(), std::bad_variant_access);
    EXPECT_THROW(km::Variant(KDate{2022, 3, 15}).asDateTime(), std::bad_variant_access);
    EXPECT_THROW(km::Variant(KDateTime{{2022, 3, 15}, {20, 33, 33}}).asDate(), std::bad_variant_access);

    EXPECT_EQ(km::Variant(30).index(), 0);
    EXPECT_EQ(km::Variant(KInt64(320)).index(), 1);
    EXPECT_EQ(km::Variant(KFloat32(3.1f)).index(), 2);
    EXPECT_EQ(km::Variant(KFloat64(3.9)).index(), 3);
    EXPECT_EQ(km::Variant("c style string").index(), 4);
    EXPECT_EQ(km::Variant(KString("std::string")).index(), 4);
    EXPECT_EQ(km::Variant(true).index(), 5);
    EXPECT_EQ(km::Variant(KDate{2022, 3, 15}).index(), 6);
    EXPECT_EQ(km::Variant(KDateTime{{2022, 3, 15}, {20, 33, 33}}).index(), 7);

    // these won't be compiled.
    // km::Variant(1u);
    // km::Variant('c');
    // km::Variant(3ull);
    // km::Variant(4.4l);
}

TEST(Core, DateTime)
{
    EXPECT_TRUE(KDate({2022, 4, 28}) < KDate({2023, 3, 27}));
    EXPECT_TRUE(KDate({2022, 4, 28}) < KDate({2022, 5, 27}));
    EXPECT_TRUE(KDate({2022, 4, 28}) < KDate({2022, 4, 29}));

    EXPECT_FALSE(KDate({2022, 4, 28}) < KDate({2022, 3, 29}));
    EXPECT_TRUE(KDate({2023, 3, 27}) > KDate({2022, 4, 28}));
    EXPECT_TRUE(KDate({2022, 5, 27}) > KDate({2022, 4, 28}));
    EXPECT_TRUE(KDate({2022, 4, 29}) > KDate({2022, 4, 28}));

    EXPECT_TRUE(KDate({2023, 4, 21}) == KDate({2023, 4, 21}));

    KDate today = {2022, 3, 16};
    KDate next_day = {2022, 3, 17};
    EXPECT_TRUE(today != next_day);
    EXPECT_TRUE(KDateTime({today, {23, 13, 55}}) < KDateTime({today, {23, 13, 56}}));
    EXPECT_TRUE(KDateTime({today, {23, 13, 55}}) < KDateTime({today, {23, 14, 54}}));
    EXPECT_TRUE(KDateTime({today, {23, 13, 55}}) < KDateTime({today, {23, 14, 55}}));
    EXPECT_TRUE(KDateTime({today, {23, 13, 55}}) < KDateTime({next_day, {0, 13, 54}}));

    EXPECT_TRUE(KDateTime({today, {23, 13, 56}}) > KDateTime({today, {23, 13, 55}}));
    EXPECT_TRUE(KDateTime({today, {23, 14, 54}}) > KDateTime({today, {23, 13, 55}}));
    EXPECT_TRUE(KDateTime({today, {23, 14, 55}}) > KDateTime({today, {23, 13, 55}}));
    EXPECT_TRUE(KDateTime({next_day, {00, 13, 54}}) > KDateTime({today, {23, 13, 55}}));

    EXPECT_TRUE(KDateTime({{today}, {00, 00, 00}}) != KDateTime({{next_day}, {00, 00, 00}}));
    EXPECT_FALSE(KDateTime({{today}, {00, 00, 00}}) == KDateTime({{next_day}, {00, 00, 00}}));
    EXPECT_EQ(km::toString(today, '/'), "16/03/2022");
    EXPECT_EQ(km::toString(KDateTime{today, {23, 20, 2}}, '/'), "16/03/2022 23:20:02");
}

TEST(Core, TypeTraits)
{
    EXPECT_TRUE((km::k_is_same<KInt32, int32_t>::value));
    EXPECT_FALSE((km::k_is_same<KInt64, unsigned long long int>::value));
    EXPECT_TRUE((km::k_is_in_list<float, KInt32, KInt64, KFloat32, KFloat64>::value));
    EXPECT_FALSE((km::k_is_in_list<char, KInt32, KInt64, KFloat32, KFloat64>::value));
    EXPECT_TRUE(km::k_is_arithmetic<KInt64>::value);
    EXPECT_FALSE(km::k_is_arithmetic<KString>::value);
    EXPECT_TRUE(km::k_is_float<KFloat32>::value);
    EXPECT_FALSE(km::k_is_float<KInt64>::value);
    EXPECT_TRUE(km::k_is_integer<KInt32>::value);
    EXPECT_FALSE(km::k_is_integer<KBoolean>::value);
    EXPECT_TRUE(km::k_is_ktype<KInt32>::value);
    EXPECT_FALSE(km::k_is_ktype<unsigned short>::value);
}