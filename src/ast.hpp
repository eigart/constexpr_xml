// Copyright (C) 2020-2022 Jonathan Müller and lexy contributors
// SPDX-License-Identifier: BSL-1.0

// Note:
// This file is a derivative work of lexy/example/xml.cpp

// TODO: Constexpr everything!
#ifndef AST_HPP
#define AST_HPP

#include <cstdio>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

namespace ast {

class xml_text
{
public:
  explicit xml_text(std::string text) : _text(std::move(text)) {}
  constexpr std::string_view to_sv() const { return _text; }

private:
  std::string _text;
};

class xml_reference
{
public:
  explicit xml_reference(char c) : _c(c) {}
  constexpr std::string_view to_sv() const { return "ref"sv; }

private:
  char _c;
};

// TODO: Implement attributes in a separate header just in case (licensing, updates etc.)
// class xml_attribute
// {
// public:
//   explicit xml_attribute(std::string attribute_name, std::string value)
//     : _attribute_name(std::move(attribute_name)), _value(std::move(value))
//   {}

// private:
//   std::string _attribute_name;
//   std::string _value;
// };

class xml_cdata
{
public:
  explicit xml_cdata(std::string text) : _text(std::move(text)) {}
  constexpr std::string_view to_sv() const { return _text; }

private:
  std::string _text;
};

class xml_element;
using xml_variant = std::variant<xml_element, xml_cdata, xml_reference, xml_text>;

class xml_element
{
public:
  explicit xml_element(std::string tag, std::vector<xml_variant> &&children = {})
    : _tag(std::move(tag)), _children(std::move(children))
  {}

  constexpr std::string_view to_sv() const { return _tag; }
  std::vector<xml_variant>::const_iterator begin() const { return _children.begin(); }
  std::vector<xml_variant>::const_iterator end() const { return _children.end(); }

private:
  std::string _tag;
  std::vector<xml_variant> _children;
};

}// namespace ast

#endif// AST_HPP