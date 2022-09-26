#include <catch2/catch.hpp>
#include <xml_constexpr_string.hpp>

constexpr unsigned int Factorial(unsigned int number)// NOLINT(misc-no-recursion)
{
  return number <= 1 ? number : Factorial(number - 1) * number;
}

TEST_CASE("Factorials are computed with constexpr", "[factorial]")
{
  STATIC_REQUIRE(Factorial(1) == 1);
  STATIC_REQUIRE(Factorial(2) == 2);
  STATIC_REQUIRE(Factorial(3) == 6);
  STATIC_REQUIRE(Factorial(10) == 3628800);
}

TEST_CASE("File content is read at compile time.", "[constexpr_xml]")
{
  STATIC_REQUIRE(cexml::get_xml_input().starts_with('<'));
}
