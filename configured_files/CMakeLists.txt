
# A very simple example of a configured file that might need to be
# converted to one that is publicly installed in the case that
# you are developing a library
configure_file("config.hpp.in" "${CMAKE_BINARY_DIR}/configured_files/include/internal_use_only/config.hpp" ESCAPE_QUOTES)

# Read a file into variable XML_INPUT
file(READ example_digit_input.txt DIGIT_INPUT UTF-8)
file(READ example_xml_input.xml XML_INPUT UTF-8)

# Configured file containing a string view "R(XML_INPUT)"
configure_file("xml_constexpr_string.hpp.in" "${CMAKE_BINARY_DIR}/configured_files/include/xml_constexpr_string.hpp" ESCAPE_QUOTES)