cmake_minimum_required(VERSION 3.20)

project(
    BuriedPoint
    VERSION 1.1.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_BURIED_SHARED_LIBS "Build BuriedPoint shared library" ON)
option(BUILD_BURIED_STATIC_LIBS "Build BuriedPoint static library" ON)
option(BUILD_BURIED_EXAMPLES "Build examples" OFF)
option(BUILD_BURIED_TESTS "Build tests" OFF)

if(MSVC)
    add_compile_options(/W4 /MP)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(src)

if(BUILD_BURIED_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_BURIED_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()