// Copyright (C) 2020-2022 Jonathan Müller and lexy contributors
// SPDX-License-Identifier: BSL-1.0

// Derivative work of lexy/example/xml.cpp

#ifndef PARSER_PRODUCTION_HPP
#define PARSER_PRODUCTION_HPP

#include "ast.hpp"

#include <lexy/callback.hpp>
#include <lexy/dsl.hpp>


namespace dsl = lexy::dsl;

namespace productions {
// The grammar of (a subset of) XML.
// It does not support attributes, the XML prolog, doctype, or processing instructions.
// It also only supports the pre-defined entity references.

namespace dsl = lexy::dsl;

// An invalid character error.
// Used with try_ to turn generic failures into more specific ones.
struct invalid_character
{
  static constexpr auto name() { return "invalid character"; }
};

// Whitespace that is manually inserted in a couple of places.
constexpr auto ws = dsl::whitespace(dsl::ascii::space / dsl::ascii::newline);

// Comment.
struct comment
{
  static constexpr auto rule = LEXY_LIT("<!--") >> dsl::until(LEXY_LIT("-->"));
  static constexpr auto value = lexy::forward<void>;
};

// Non-markup content.
struct text
{
  static constexpr auto rule = [] {
    auto char_ = (dsl::code_point - dsl::lit_c<'<'> - dsl::lit_c<'&'>).error<invalid_character>;
    return dsl::identifier(char_);
  }();
  static constexpr auto value = lexy::as_string<std::string> | lexy::construct<ast::xml_text>;
};

// The name of a tag or entity.
constexpr auto name = [] {
  // We only support ASCII here, as I'm too lazy to type all the code point ranges out.
  auto head_char = dsl::ascii::alpha / dsl::lit_c<':'> / dsl::lit_c<'_'>;
  auto trailing_char = head_char / dsl::lit_c<'-'> / dsl::lit_c<'.'> / dsl::ascii::digit;

  return dsl::identifier(head_char.error<invalid_character>, trailing_char);
}();

// A pre-defined entity reference.
struct reference
{
  struct unknown_entity
  {
    static LEXY_CONSTEVAL auto name() { return "unknown entity"; }
  };

  // The predefined XML entities and their replacement values.
  static constexpr auto entities = lexy::symbol_table<char>
                                         .map<LEXY_SYMBOL("quot")>('"')
                                         .map<LEXY_SYMBOL("amp")>('&')
                                         .map<LEXY_SYMBOL("apos")>('\'')
                                         .map<LEXY_SYMBOL("lt")>('<')
                                         .map<LEXY_SYMBOL("gt")>('>');

  static constexpr auto rule = [] {
    // The actual reference parses a name and performs a lookup to produce the replacement
    // values.
    auto reference = dsl::symbol<entities>(name).error<unknown_entity>;
    return dsl::lit_c<'&'> >> reference + dsl::lit_c<';'>;
  }();
  static constexpr auto value = lexy::construct<ast::xml_reference>;
};

// A CDATA section.
struct cdata
{
  static constexpr auto rule = [] {
    // We define a string with custom delimiters.
    auto delim = dsl::delimited(LEXY_LIT("<![CDATA["), LEXY_LIT("]]>"));
    return delim(dsl::code_point);
  }();

  // We build a string; then we construct a node from it.
  static constexpr auto value = lexy::as_string<std::string> >> lexy::construct<ast::xml_cdata>;
};

// A tagged XML element.
struct element
{
  struct tag_mismatch
  {
    static LEXY_CONSTEVAL auto name() { return "closing tag doesn't match"; }
  };

  static constexpr auto rule = [] {
    // The brackets for surrounding the opening and closing tag.
    auto open_tagged = dsl::brackets(LEXY_LIT("<"), LEXY_LIT(">"));
    auto close_tagged = dsl::brackets(LEXY_LIT("</"), LEXY_LIT(">"));

    // To check that the name is equal for the opening and closing tag,
    // we use a variable that can store one name.
    // Note: this only declares the variable, we still need to create it below.
    auto name_var = dsl::context_identifier<struct name_var_tag>(name);

    // The open tag parses a name and captures it in the variable.
    // It also checks for an empty tag (<name/>), in which case we're done and immediately
    // return.
    auto empty = dsl::if_(LEXY_LIT("/") >> LEXY_LIT(">") + dsl::return_);
    auto open_tag = open_tagged(name_var.capture() + ws + empty);

    // The closing tag matches the name again and requires that it matches the one we've stored
    // earlier.
    auto close_tag = close_tagged(name_var.rematch().error<tag_mismatch> + ws);

    // The content of the element.
    auto content = dsl::p<comment> | dsl::p<cdata>//
                   | dsl::peek(LEXY_LIT("<")) >> dsl::recurse<element>//
                   | dsl::p<reference> | dsl::else_ >> dsl::p<text>;

    // We match a (possibly empty) list of content surrounded itself by the open and close tag.
    // But first we create the variable that holds the name.
    return name_var.create() + dsl::brackets(open_tag, close_tag).opt_list(content);
  }();

  // We collect the children as vector; then we construct a node from it.
  static constexpr auto value = lexy::as_list<std::vector<ast::xml_variant>> >> lexy::callback<ast::xml_element>(
                                  [](auto name, lexy::nullopt = {}) {
                                    return ast::xml_element(lexy::as_string<std::string>(name));
                                  },
                                  [](auto name, auto &&children) {
                                    return ast::xml_element(lexy::as_string<std::string>(name), LEXY_MOV(children));
                                  });
};

// An XML document.
struct document
{
  static constexpr auto rule = [] {
    // We allow surrounding the document with comments and whitespace.
    // (Whitespace does not allow productions, so we need to inline it).
    auto ws_comment = ws | dsl::inline_<comment>;
    return ws_comment + dsl::p<element> + ws_comment + dsl::eof;
  }();
  static constexpr auto value = lexy::forward<ast::xml_element>;
};
}// namespace productions

namespace productions {
struct number
{
  static constexpr auto rule = dsl::integer<int>(dsl::n_digits<3>);
  static constexpr auto value = lexy::forward<int>;
};

using num_var = std::variant<int, float>;

struct number_variant
{
  static constexpr auto rule = dsl::integer<int>(dsl::n_digits<3>);
  static constexpr auto value = lexy::forward<num_var>;
};

}// namespace productions
// #ifndef LEXY_TEST
// int main(int argc, char **argv)
// {
//   if (argc < 2) {
//     std::fprintf(stderr, "usage: %s <filename>", argv[0]);
//     return 1;
//   }

//   // We assume UTF-8 encoded input.
//   auto file = lexy::read_file<lexy::utf8_encoding>(argv[1]);
//   if (!file) {
//     std::fprintf(stderr, "file '%s' not found", argv[1]);
//     return 1;
//   }

//   auto document = lexy::parse<grammar::document>(file.buffer(), lexy_ext::report_error.path(argv[1]));
//   if (!document) return 2;

//   document.value()->print();
// }
// }

// namespace production {
// struct attribute
// {
//   static constexpr auto rule = [] {return 0;}();

//   static constexpr auto value = [] {return 0;};
// };

// }// namespace production
#endif// PARSER_PRODUCTION_HPP