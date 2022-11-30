#include "ast.hpp"
#include <fmt/format.h>

namespace ast {
template<> struct fmt::formatter<ast::xml_element>
{
};
}// namespace ast