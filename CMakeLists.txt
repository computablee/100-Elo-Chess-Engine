cmake_minimum_required(VERSION 3.10)
project(100Elo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    main.cxx
    helpers.cxx
    pst.cxx
    settings.cxx
    tt.cxx
    uci.cxx
    search.cxx
    evaluate.cxx)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -O3 -march=native)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/chess-library/include)