cmake_minimum_required(VERSION 3.28)
project(aoc_2024 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-fmodules-ts)

add_subdirectory(extern/googletest)

add_compile_options(-Wall -Wextra -pedantic -Werror -Wconversion
  -Wsign-conversion)

add_subdirectory(day_1)
add_subdirectory(day_5)
add_subdirectory(day_6)
add_subdirectory(day_7)
add_subdirectory(day_8)
add_subdirectory(day_9)
add_subdirectory(day_10)
