cmake_minimum_required(VERSION 3.10)
project(100Elo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    src/main.cxx
    src/helpers.cxx
    src/pst.cxx
    src/settings.cxx
    src/tt.cxx
    src/uci.cxx
    src/search.cxx
    src/evaluate.cxx)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -O3 -march=native -g)

target_include_directories(${PROJECT_NAME} PRIVATE src/${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/chess-library/include)