cmake_minimum_required(VERSION 3.20)
project(my-lexer LANGUAGES CXX)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(my-lexer src/main.cpp src/data_structure/keyword_table.cpp
src/data_structure/translate_table.cpp src/state_machine/analysis.cpp
src/utils/tools.cpp)
target_include_directories(my-lexer PRIVATE src)
if(MSVC)
    target_compile_options(my-lexer PRIVATE /utf-8)
endif()

include(CTest)
if(BUILD_TESTING)
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(Python3_Interpreter_FOUND)
        add_test(NAME lexer_regression
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_lexer.py"
                "$<TARGET_FILE:my-lexer>")
    endif()
endif()
