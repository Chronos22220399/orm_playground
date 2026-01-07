#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <ess/orm/core_orm.hpp>

using namespace ess::orm::attribute;
using namespace ess::orm::meta;
TEST_CASE("DefaultExpr check") {
  CHECK_FALSE(is_valid_default_expr<"'helo'"_fs>);

  CHECK_FALSE(is_valid_default_expr<"helo'"_fs>);

  CHECK_FALSE(is_valid_default_expr<"'helo"_fs>);

  CHECK_FALSE(is_valid_default_expr<"helo'"_fs>);

  CHECK(is_valid_default_expr<"now()"_fs>);
}

enum class Color { red = 1, green = 2 };

TEST_CASE("DefaultValue check") {
  // 整型
  CHECK_EQ(DefaultValue<10>::value, 10);

  CHECK_NE(DefaultValue<10>::value, 11);

  CHECK(std::is_same_v<DefaultValue<10>::semantic_type, sql_integer>);

  // 浮点型
  CHECK_EQ(DefaultValue<1.3>::value, 1.3);

  CHECK_NE(DefaultValue<1.3>::value, 1.0);

  CHECK(std::is_same_v<DefaultValue<1.3>::semantic_type, sql_floating>);

  // 枚举型
  CHECK_EQ(DefaultValue<Color::red>::value, Color::red);

  CHECK_NE(DefaultValue<Color::red>::value, Color::green);

  CHECK(std::is_same_v<DefaultValue<Color::red>::semantic_type, sql_integer>);

  // 布尔型
  CHECK_EQ(DefaultValue<true>::value, true);

  CHECK_NE(DefaultValue<false>::value, true);

  CHECK(std::is_same_v<DefaultValue<false>::semantic_type, sql_boolean>);

  // FixedString
  CHECK(fs_equal(DefaultValue<"hello"_fs>::value, "hello"_fs));

  CHECK(!fs_equal(DefaultValue<"hello"_fs>::value, "helo"_fs));

  CHECK(std::is_same_v<DefaultValue<"hello"_fs>::semantic_type, sql_text>);

  // SqlNull
  CHECK(std::is_same_v<DefaultValue<SqlNull{}>::semantic_type, sql_null>);
}

TEST_CASE("DefaultValue semantic_type check") {

  // integer literals
  CHECK(std::is_same_v<DefaultValue<0>::semantic_type, sql_integer>);

  CHECK(std::is_same_v<DefaultValue<42>::semantic_type, sql_integer>);

  CHECK(std::is_same_v<DefaultValue<42l>::semantic_type, sql_integer>);

  CHECK(std::is_same_v<DefaultValue<42ll>::semantic_type, sql_integer>);

  // floating literals
  CHECK(std::is_same_v<DefaultValue<1.0>::semantic_type, sql_floating>);

  CHECK(std::is_same_v<DefaultValue<3.14>::semantic_type, sql_floating>);

  // boolean
  CHECK(std::is_same_v<DefaultValue<true>::semantic_type, sql_boolean>);

  CHECK(std::is_same_v<DefaultValue<false>::semantic_type, sql_boolean>);

  // fixed string
  CHECK(std::is_same_v<DefaultValue<"hello"_fs>::semantic_type, sql_text>);
}

TEST_CASE("cpp_type_to_sql_semantic check") {

  // integer family
  CHECK(std::is_same_v<cpp_type_to_sql_semantic<int>::type, sql_integer>);

  CHECK(std::is_same_v<cpp_type_to_sql_semantic<long>::type, sql_integer>);

  CHECK(std::is_same_v<cpp_type_to_sql_semantic<long long>::type, sql_integer>);

  // floating family
  CHECK(std::is_same_v<cpp_type_to_sql_semantic<float>::type, sql_floating>);

  CHECK(std::is_same_v<cpp_type_to_sql_semantic<double>::type, sql_floating>);

  // boolean
  CHECK(std::is_same_v<cpp_type_to_sql_semantic<bool>::type, sql_boolean>);

  // enum → integer
  CHECK(std::is_same_v<cpp_type_to_sql_semantic<Color>::type, sql_integer>);

  // fixed_string
  CHECK(std::is_same_v<cpp_type_to_sql_semantic<decltype("hell"_fs)>::type,
                       sql_text>);
}

TEST_CASE("sql_compatible check") {

  // int column
  CHECK(sql_compatible_v<cpp_type_to_sql_semantic_t<int>,
                         DefaultValue<120>::semantic_type>);

  CHECK(sql_compatible_v<cpp_type_to_sql_semantic<long>::type,
                         DefaultValue<120>::semantic_type>);

  CHECK(sql_compatible_v<cpp_type_to_sql_semantic<long long>::type,
                         DefaultValue<120ll>::semantic_type>);

  // bool column
  CHECK(sql_compatible_v<cpp_type_to_sql_semantic<bool>::type,
                         DefaultValue<false>::semantic_type>);

  // enum column
  CHECK(sql_compatible_v<cpp_type_to_sql_semantic<Color>::type,
                         DefaultValue<10>::semantic_type>);

  // floating column
  CHECK(sql_compatible_v<cpp_type_to_sql_semantic<float>::type,
                         DefaultValue<1.2>::semantic_type>);

  CHECK(sql_compatible_v<cpp_type_to_sql_semantic<double>::type,
                         DefaultValue<1.2>::semantic_type>);

  // fixed_string
  constexpr auto str = "ello"_fs;
  CHECK(sql_compatible_v<cpp_type_to_sql_semantic_t<decltype(str)>,
                         DefaultValue<str>::semantic_type>);
}

TEST_CASE("has_duplicated_attributes check") {
  using namespace ess::orm::attribute;
}

TEST_CASE("fixed_string check") {
  // fs_equal
  CHECK(fs_equal("hello"_fs, "hello"_fs));
  CHECK_FALSE(fs_equal("hello"_fs, "hell"_fs));
  CHECK(fs_equal(""_fs, ""_fs));
  CHECK_FALSE(fs_equal("1"_fs, ""_fs));
  CHECK_FALSE(fs_equal("\0\0\0"_fs, ""_fs));

  // fs_concat
  constexpr auto str_1 = "hello"_fs;
  constexpr auto str_2 = "123"_fs;
  CHECK_FALSE(fs_equal(fs_concat(str_1, str_2), str_2));
  CHECK(fs_equal(fs_concat(str_1, str_2), "hello123"_fs));
  CHECK(fs_equal(fs_concat(str_1, "\0\0\0"_fs), "hello\0\0\0"_fs));

  // fs_index
  constexpr auto res_1 = fs_find(str_1, 'h');
  CHECK(res_1.success == true);
  CHECK(res_1.index == 0);
  constexpr auto res_2 = fs_find(str_1, 'z');
  CHECK(res_2.success == false);

  // fs_substr
  constexpr auto str_3 = "helllo"_fs;
  CHECK(fs_substr<1, 3>(str_3).has_value());
  CHECK(fs_equal(*fs_substr<1, 3>(str_3), "ell"_fs));
  CHECK_FALSE(fs_equal(*fs_substr<1, 3>(str_3), "ello"_fs));
  CHECK(fs_substr<1, 3>(str_3).value().size() == 4);
  constexpr auto res_3 = fs_substr<1, 7>(str_3);
  constexpr auto res_4 = fs_substr<0, str_3.size() - 1>(str_3);
  CHECK_FALSE(res_3.has_value());
  CHECK(res_4.has_value());
  CHECK(fs_equal(res_4.value(), str_3));
}
