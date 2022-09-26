#include <functional>
#include <iostream>
#include <optional>

#include <CLI/CLI.hpp>
#include <lexy/callback.hpp>
#include <lexy/dsl.hpp>

#include <spdlog/spdlog.h>
// This file will be generated automatically when you run the CMake configuration step.
// It creates a namespace called `myproject`.
// You can modify the source template at `configured_files/config.hpp.in`.
#include "xml_parsing.hpp"
#include <internal_use_only/config.hpp>
namespace dsl = lexy::dsl;

struct ipv4_address
{
  // What is being matched.
  static constexpr auto rule = [] {
    // Match a sequence of (decimal) digits and convert it into a std::uint8_t.
    auto octet = dsl::integer<std::uint8_t>;

    // Match four of them separated by periods.
    return dsl::times<4>(octet, dsl::sep(dsl::period)) + dsl::eof;
  }();

  // How the matched output is being stored.
  static constexpr auto value =
    lexy::callback<std::uint32_t>([](std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d) {
      return (a << 24) | (b << 16) | (c << 8) | d;
    });
};


// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, const char **argv)
{
  try {
    CLI::App app{ fmt::format("{} version {}", myproject::cmake::project_name, myproject::cmake::project_version) };

    std::optional<std::string> message;
    app.add_option("-m,--message", message, "A message to print back out");
    bool show_version = false;
    app.add_flag("--version", show_version, "Show version information");

    CLI11_PARSE(app, argc, argv);

    if (show_version) {
      fmt::print("{}\n", myproject::cmake::project_version);
      return EXIT_SUCCESS;
    }

    // Use the default logger (stdout, multi-threaded, colored)
    spdlog::info("Hello, {}!", "World");

    if (message) {
      fmt::print("Message: '{}'\n", *message);
    } else {
      fmt::print("No Message Provided :(\n");
    }
  } catch (const std::exception &e) {
    spdlog::error("Unhandled exception in main: {}", e.what());
  }
}
