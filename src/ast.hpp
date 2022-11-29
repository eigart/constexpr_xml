// Copyright (C) 2020-2022 Jonathan Müller and lexy contributors
// SPDX-License-Identifier: BSL-1.0

// Note:
// This file is a derivative work of lexy/example/xml.cpp

// TODO: Constexpr everything!
#ifndef AST_HPP
#define AST_HPP

#include <cstdio>
#include <memory>
#include <string>
#include <vector>

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
  friend std::ostream &operator<<(std::ostream &os, const xml_text &node) { return os << node._text; }

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

  friend std::ostream &operator<<(std::ostream &os, const xml_reference &node)
  {
    switch (node._c) {
    case '"':
      return os << "&quot;";
      break;
    case '&':
      return os << "&amp;";
      break;
    case '\'':
      return os << "&apos;";
      break;
    case '<':
      return os << "&lt;";
      break;
    case '>':
      return os << "&gt;";
      break;
    }
    return os;
  }

private:
  char _c;
};

// TODO: Implement attributes in a separate header just in case (licensing, updates etc.)
// class xml_attribute final : public xml_node
// {
// public:
//   explicit xml_attribute(std::string attribute_name, std::string value)
//     : _attribute_name(std::move(attribute_name)), _value(std::move(value))
//   {}

//   void print() const override
//   {
//     std::printf(" %s=", _attribute_name.c_str());
//     std::printf("\"%s\"s>", _value.c_str());
//   }

// private:
//   std::string _attribute_name;
//   std::string _value;
// };

// using xml_attribute_ptr = std::unique_ptr<xml_attribute>;

class xml_cdata final : public xml_node
{
public:
  explicit xml_cdata(std::string text) : _text(std::move(text)) {}

  void print() const override { std::printf("<![CDATA[%s]]>", _text.c_str()); }
  friend std::ostream &operator<<(std::ostream &os, const xml_cdata &node)
  {
    return os << "<![CDATA[" << node._text << "]]>";
  }

private:
  std::string _text;
};

class xml_element final : public xml_node
{
public:
  explicit xml_element(std::string tag, std::vector<xml_node_ptr> &&children = {})
    : _tag(std::move(tag)), _children(std::move(children))
  {}

  void print() const override
  {
    std::printf("<%s>", _tag.c_str());
    for (auto &child : _children) child->print();
    std::printf("</%s>", _tag.c_str());
  }

  friend std::ostream &operator<<(std::ostream &os, const xml_element &node)
  {
    for (auto &child : node._children) os << child;
    return os;
  }

private:
  std::string _tag;
  std::vector<xml_node_ptr> _children;
};
}// namespace ast

#endif// AST_HPP