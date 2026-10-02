cmake_minimum_required(VERSION 3.20)
project(my-lexer LANGUAGES CXX)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(my-lexer src/main.cpp src/data_structure/keyword_table.cpp
src/data_structure/translate_table.cpp src/state_machine/analysis.cpp
src/utils/tools.cpp)
target_include_directories(my-lexer PRIVATE src)