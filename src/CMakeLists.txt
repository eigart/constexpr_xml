find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)

# Generic test that uses conan libs
add_executable(intro)
target_sources(intro PRIVATE main.cpp productions.hpp ast.hpp)

target_link_libraries(
  intro
  PUBLIC project_options project_warnings
  PRIVATE CLI11::CLI11 fmt::fmt spdlog::spdlog foonathan::lexy)

target_include_directories(intro 
PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include" foonathan::include_directories)
