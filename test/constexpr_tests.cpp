#include <catch2/catch.hpp>
#include <string_view>

#include "../src/parser_production.hpp"
#include <lexy/action/parse.hpp>
#include <lexy/callback.hpp>
#include <lexy/callback/container.hpp>
#include <lexy/dsl.hpp>
#include <lexy/input/string_input.hpp>
#include <lexy_ext/compiler_explorer.hpp>
#include <lexy_ext/report_error.hpp>
#include <xml_constexpr_string.hpp>


using namespace std::string_view_literals;

TEST_CASE("Digit string view value.", "[constexpr_xml]")
{
  STATIC_REQUIRE(cxml::get_digit_input().starts_with("123"sv));
}

TEST_CASE("XML string view starts with.", "[constexpr_xml]")
{
  STATIC_REQUIRE(cxml::get_xml_input().starts_with("<Tag>"sv));
}

TEST_CASE("Digit parsed at compile time!", "[constexpr_digit]")
{
  constexpr auto input = lexy::string_input<lexy::ascii_encoding>(cxml::get_digit_input());
  constexpr auto result =
    lexy::parse<productions::number>(input, lexy::collect(lexy::callback<void>([](auto, auto) {})));

  STATIC_REQUIRE(result.has_value());
  STATIC_REQUIRE(result.value() == 123);
}