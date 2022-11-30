#include <functional>
#include <iostream>
#include <optional>
#include <string_view>

#include <spdlog/spdlog.h>

#include <CLI/CLI.hpp>

// cmake configured files
#include <internal_use_only/config.hpp>
#include <xml_constexpr_string.hpp>

#include <lexy/action/parse.hpp>
#include <lexy/callback.hpp>
#include <lexy/dsl.hpp>
#include <lexy/input/string_input.hpp>
#include <lexy_ext/report_error.hpp>

#include "productions.hpp"


void print_xml_variant(const ast::xml_variant &var)
{
  std::visit(
    [](auto &&arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, ast::xml_element>) {
        fmt::print("<{0}>", arg.to_sv());
        for (auto &&var : arg) { print_xml_variant(var); }
        fmt::print("<{0}>", arg.to_sv());
      } else if constexpr (std::is_same_v<T, ast::xml_text>) {
        fmt::print("<{0}>", arg.to_sv());
        // return arg.to_sv();
      } else if constexpr (std::is_same_v<T, ast::xml_cdata>) {
        puts("cdata");
      } else if constexpr (std::is_same_v<T, ast::xml_reference>) {
        fmt::print("<{0}>", arg.to_sv());
      } else {
        puts("nothing");
      }
    },
    var);
}


// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, const char ** /*argv*/)
{
  auto input = lexy::string_input<lexy::ascii_encoding>(cxml::get_xml_input());
  auto result = lexy::parse<productions::document>(input, lexy_ext::report_error);

  // auto input = lexy::string_input<lexy::ascii_encoding>(cxml::get_digit_input());
  // auto result = lexy::parse<productions::number_variant>(input, lexy::collect(lexy::callback<void>([](auto, auto)
  // {})));
  if (result.has_value()) {
    // fmt::print("\n{}\n", result.value());
    // fmt::print("\n{}\n", std::get<0>(result.value()));
    print_xml_variant(result.value());
    fmt::print("Success!\n");
    return 0;
  } else {
    fmt::print("!Success \n");
    return 1;
  }

  // try {
  //   CLI::App app{ fmt::format("{} version {}", myproject::cmake::project_name, myproject::cmake::project_version) };
  //   std::optional<std::string> message;
  //   app.add_option("-m,--message", message, "A message to print back out");
  //   bool show_version = false;
  //   app.add_flag("--version", show_version, "Show version information");

  //   CLI11_PARSE(app, argc, argv);

  //   if (show_version) {
  //     fmt::print("{}\n", myproject::cmake::project_version);
  //     return EXIT_SUCCESS;
  //   }

  //   // Use the default logger (stdout, multi-threaded, colored)
  //   spdlog::info("Hello, {}!", "World");

  //   if (message) {
  //     fmt::print("Message: '{}'\n", *message);
  //   } else {
  //     fmt::print("No Message Provided :(\n");
  //   }
  // } catch (const std::exception &e) {
  // spdlog::error("Unhandled exception in main: {}", e.what());
  // }
}
