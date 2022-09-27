#include <catch2/catch.hpp>
#include <string_view>

#include <xml_constexpr_string.hpp>

#include "../src/xml_parsing.hpp"
#include <lexy/action/parse.hpp>
#include <lexy/callback.hpp>
#include <lexy/dsl.hpp>
#include <lexy_ext/compiler_explorer.hpp>
#include <lexy_ext/report_error.hpp>

using namespace std::string_view_literals;

TEST_CASE("File content is read at compile time.", "[constexpr_xml]")
{
  STATIC_REQUIRE(cexml::get_xml_input().starts_with("123"sv));
}

TEST_CASE("Parsed at compile time!")
{
  auto sv = cexml::get_xml_input();
  auto input = lexy::range_input(sv.begin(), sv.end());

  auto result = lexy::parse<production::ipv4_address>(input, lexy_ext::report_error);
  if (result) {
    STATIC_REQUIRE(false);
  } else {
    STATIC_REQUIRE(true);
  }
}
