// Copyright (C) 2020-2022 Jonathan Müller and lexy contributors
// SPDX-License-Identifier: BSL-1.0

// Derivative work of lexy/example/xml.cpp

#ifndef AST_HPP
#define AST_HPP

#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include <lexy/action/parse.hpp>// lexy::parse
#include <lexy/callback.hpp>// value callbacks
#include <lexy/dsl.hpp>// lexy::dsl::*
#include <lexy/input/file.hpp>// lexy::read_file

#include <lexy_ext/report_error.hpp>// lexy_ext::report_error

namespace ast {
class xml_node
{
public:
  xml_node(const xml_node &) = delete;
  xml_node &operator=(const xml_node &) = delete;
  virtual ~xml_node() = default;

  virtual void print() const = 0;

protected:
  xml_node() = default;
};

using xml_node_ptr = std::unique_ptr<xml_node>;

class xml_text final : public xml_node
{
public:
  explicit xml_text(std::string text) : _text(std::move(text)) {}

  void print() const override { std::fputs(_text.c_str(), stdout); }

private:
  std::string _text;
};

class xml_reference final : public xml_node
{
public:
  explicit xml_reference(char c) : _c(c) {}

  void print() const override
  {
    switch (_c) {
    case '"':
      std::fputs("&quot;", stdout);
      break;
    case '&':
      std::fputs("&amp;", stdout);
      break;
    case '\'':
      std::fputs("&apos;", stdout);
      break;
    case '<':
      std::fputs("&lt;", stdout);
      break;
    case '>':
      std::fputs("&gt;", stdout);
      break;
    }
  }

private:
  char _c;
};

class xml_attribute final : public xml_node
{
public:
  explicit xml_attribute(std::string attribute_name, std::string value)
    : _attribute_name(std::move(attribute_name)), _value(std::move(value))
  {}

  void print() const override
  {
    std::printf(" %s=", _attribute_name.c_str());
    std::printf("\"%\"s>", _value.c_str());
  }

private:
  std::string _attribute_name;
  std::string _value;
};

class xml_cdata final : public xml_node
{
public:
  explicit xml_cdata(std::string text) : _text(std::move(text)) {}

  void print() const override { std::printf("<![CDATA[%s]]>", _text.c_str()); }

private:
  std::string _text;
};

class xml_element final : public xml_node
{
public:
  explicit xml_element(std::string tag, std::vector<xml_node_ptr> children = {})
    : _tag(std::move(tag)), _children(std::move(children))
  {}

  void print() const override
  {
    std::printf("<%s>", _tag.c_str());
    for (auto &child : _children) child->print();
    std::printf("</%s>", _tag.c_str());
  }

private:
  std::string _tag;
  std::vector<xml_node_ptr> _children;
  std::vector<xml_attribute_ptr> _attributes;
};
}// namespace ast

#endif// AST_HPP