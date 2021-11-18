#include <gtest/gtest.h>

#include <rapidjson/document.h>

using namespace rapidjson;

TEST(tests_rapidjson, basic_test)
{
    constexpr const char* test_json = "{"
                                      "   \"hello\": \"world\","
                                      "   \"t\": true,"
                                      "   \"f\": false,"
                                      "   \"n\": null,"
                                      "   \"i\": 123,"
                                      "   \"pi\": 3.1416,"
                                      "   \"a\": [1, 2, 3, 4]"
                                      "}";

    Document document;
    document.Parse(test_json);

    ASSERT_EQ(document.HasMember("hello"), true);
    ASSERT_EQ(document.HasMember("hellO"), false);

    ASSERT_EQ(document["hello"].IsString(), true);
    ASSERT_EQ(document["hello"].IsInt(), false);

    ASSERT_EQ(document["t"].IsBool(), true);

    ASSERT_EQ(document["i"].IsNumber(), true);

    ASSERT_EQ(document["i"].IsInt(), true);

    ASSERT_EQ(document["pi"].IsNumber(), true);
    ASSERT_EQ(document["pi"].IsDouble(), true);
    ASSERT_EQ(document["pi"].IsInt(), false);
}
