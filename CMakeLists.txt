cmake_minimum_required(VERSION 3.10)
project(100-elo LANGUAGES CXX)

if(WINDOWS STREQUAL "true")
    set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
    set(TARGET_OS "windows")
elseif(MACOS STREQUAL "true")
    set(CMAKE_CXX_COMPILER clang++)
    set(TARGET_OS "macos")
else()
    set(CMAKE_CXX_COMPILER g++)
    set(TARGET_OS "linux")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMMON_SOURCES
    src/main.cxx
    src/helpers.cxx
    src/pst.cxx
    src/settings.cxx
    src/tt.cxx
    src/uci.cxx
    src/search.cxx
    src/evaluate.cxx
    src/board.cxx)

if(TARGET_OS STREQUAL "macos")
    add_executable(${PROJECT_NAME}-${TARGET_OS}-apple-m1 ${COMMON_SOURCES})
    add_executable(${PROJECT_NAME}-${TARGET_OS}-apple-m2 ${COMMON_SOURCES})

    target_include_directories(${PROJECT_NAME}-${TARGET_OS}-apple-m1 PRIVATE src/${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/chess-library/include)
    target_include_directories(${PROJECT_NAME}-${TARGET_OS}-apple-m2 PRIVATE src/${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/chess-library/include)

    target_compile_options(${PROJECT_NAME}-${TARGET_OS}-apple-m1 PRIVATE -Wall -Wextra -O3 -mcpu=apple-m1 -g -flto -DNDEBUG)
    target_compile_options(${PROJECT_NAME}-${TARGET_OS}-apple-m2 PRIVATE -Wall -Wextra -O3 -mcpu=apple-m2 -g -flto -DNDEBUG)

    target_link_options(${PROJECT_NAME}-${TARGET_OS}-apple-m1 PRIVATE)
    target_link_options(${PROJECT_NAME}-${TARGET_OS}-apple-m2 PRIVATE)
else()
    add_executable(${PROJECT_NAME}-${TARGET_OS}-x86_64        ${COMMON_SOURCES})
    add_executable(${PROJECT_NAME}-${TARGET_OS}-x86_64-modern ${COMMON_SOURCES})
    add_executable(${PROJECT_NAME}-${TARGET_OS}-x86_64-v2     ${COMMON_SOURCES})
    add_executable(${PROJECT_NAME}-${TARGET_OS}-x86_64-v3     ${COMMON_SOURCES})
    add_executable(${PROJECT_NAME}-${TARGET_OS}-x86_64-v4     ${COMMON_SOURCES})

    target_include_directories(${PROJECT_NAME}-${TARGET_OS}-x86_64        PRIVATE src/${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/chess-library/include)
    target_include_directories(${PROJECT_NAME}-${TARGET_OS}-x86_64-modern PRIVATE src/${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/chess-library/include)
    target_include_directories(${PROJECT_NAME}-${TARGET_OS}-x86_64-v2     PRIVATE src/${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/chess-library/include)
    target_include_directories(${PROJECT_NAME}-${TARGET_OS}-x86_64-v3     PRIVATE src/${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/chess-library/include)
    target_include_directories(${PROJECT_NAME}-${TARGET_OS}-x86_64-v4     PRIVATE src/${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/chess-library/include)

    target_compile_options(${PROJECT_NAME}-${TARGET_OS}-x86_64        PRIVATE -Wall -Wextra -O3 -march=x86-64 -g -flto -DNDEBUG)
    target_compile_options(${PROJECT_NAME}-${TARGET_OS}-x86_64-modern PRIVATE -Wall -Wextra -O3 -march=native -g -flto -DNDEBUG)
    target_compile_options(${PROJECT_NAME}-${TARGET_OS}-x86_64-v2     PRIVATE -Wall -Wextra -O3 -march=x86-64-v2 -g -flto -DNDEBUG)
    target_compile_options(${PROJECT_NAME}-${TARGET_OS}-x86_64-v3     PRIVATE -Wall -Wextra -O3 -march=x86-64-v3 -g -flto -DNDEBUG)
    target_compile_options(${PROJECT_NAME}-${TARGET_OS}-x86_64-v4     PRIVATE -Wall -Wextra -O3 -march=x86-64-v4 -g -flto -DNDEBUG)

    target_link_options(${PROJECT_NAME}-${TARGET_OS}-x86_64        PRIVATE -static)
    target_link_options(${PROJECT_NAME}-${TARGET_OS}-x86_64-modern PRIVATE -static)
    target_link_options(${PROJECT_NAME}-${TARGET_OS}-x86_64-v2     PRIVATE -static)
    target_link_options(${PROJECT_NAME}-${TARGET_OS}-x86_64-v3     PRIVATE -static)
    target_link_options(${PROJECT_NAME}-${TARGET_OS}-x86_64-v4     PRIVATE -static)
endif()