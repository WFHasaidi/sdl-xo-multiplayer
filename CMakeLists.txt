cmake_minimum_required(VERSION 3.22)
# Linux (Debian/Ubuntu):
#   sudo apt-get update && sudo apt-get install -y cmake g++ python3-pip
#   pip install conan
#   conan profile detect --force
#   conan install . -of build -s build_type=Debug --build=missing
#   cmake -S . -B build \
#     -DCMAKE_BUILD_TYPE=Debug \
#     -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake \
#     -DCMAKE_PREFIX_PATH="$(pwd)/build"
#   cmake --build build -j
#   ctest --test-dir build --output-on-failure
#   ./build/tictactoe
# Windows (MSVC + vcpkg or preinstalled SDL2):
#   pip install conan
#   conan profile detect --force
#   conan install . -of build -s build_type=Debug --build=missing
#   cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Debug ^
#     -DCMAKE_TOOLCHAIN_FILE=build\conan_toolchain.cmake ^
#     -DCMAKE_PREFIX_PATH="%cd%\build"
#   cmake --build build -j
#   ctest --test-dir build --output-on-failure
#   build\tictactoe.exe

project(sdl_xo_multiplayer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(TTT_BUILD_TESTS "Build unit tests" ON)
option(TTT_ENABLE_CLANG_TIDY "Enable clang-tidy static analysis targets" OFF)
option(TTT_ENABLE_CPPCHECK "Enable cppcheck static analysis targets" OFF)

if (TTT_ENABLE_CLANG_TIDY OR TTT_ENABLE_CPPCHECK)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

find_package(SDL2 REQUIRED CONFIG)

add_library(ttt_core
  src/core/Board.cpp
  src/core/Game.cpp
)
target_include_directories(ttt_core PUBLIC include)
target_compile_features(ttt_core PUBLIC cxx_std_20)

function(ttt_enable_warnings target)
  if (MSVC)
    target_compile_options(${target} PRIVATE /W4 /permissive- /EHsc "$<$<CONFIG:Debug>:/WX>")
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -pedantic "$<$<CONFIG:Debug>:-Werror>")
  endif()
endfunction()

ttt_enable_warnings(ttt_core)

add_executable(tictactoe src/app/main.cpp)
target_link_libraries(tictactoe PRIVATE ttt_core SDL2::SDL2)
if (TARGET SDL2::SDL2main)
  target_link_libraries(tictactoe PRIVATE SDL2::SDL2main)
endif()
target_compile_definitions(tictactoe PRIVATE SDL_MAIN_HANDLED)
ttt_enable_warnings(tictactoe)

function(ttt_collect_sources out_var)
  set(accumulated "")
  foreach(target IN LISTS ARGN)
    get_target_property(target_sources ${target} SOURCES)
    if (NOT target_sources)
      continue()
    endif()
    foreach(source_file IN LISTS target_sources)
      if (NOT IS_ABSOLUTE ${source_file})
        set(source_file "${CMAKE_CURRENT_SOURCE_DIR}/${source_file}")
      endif()
      list(APPEND accumulated "${source_file}")
    endforeach()
  endforeach()
  list(REMOVE_DUPLICATES accumulated)
  set(${out_var} "${accumulated}" PARENT_SCOPE)
endfunction()

if (TTT_BUILD_TESTS)
  enable_testing()

  add_executable(test_board tests/test_board.cpp)
  target_link_libraries(test_board PRIVATE ttt_core)
  ttt_enable_warnings(test_board)
  add_test(NAME test_board COMMAND test_board)

  add_executable(test_game tests/test_game.cpp)
  target_link_libraries(test_game PRIVATE ttt_core)
  ttt_enable_warnings(test_game)
  add_test(NAME test_game COMMAND test_game)
endif()

set(TTT_STATIC_ANALYSIS_TARGETS)
set(TTT_TARGETS_FOR_ANALYSIS ttt_core tictactoe)
if (TTT_BUILD_TESTS)
  list(APPEND TTT_TARGETS_FOR_ANALYSIS test_board test_game)
endif()

if (TTT_ENABLE_CLANG_TIDY)
  find_program(TTT_CLANG_TIDY_EXECUTABLE NAMES clang-tidy)
  if (NOT TTT_CLANG_TIDY_EXECUTABLE)
    message(FATAL_ERROR "TTT_ENABLE_CLANG_TIDY is ON but clang-tidy was not found")
  endif()
  ttt_collect_sources(TTT_CLANG_TIDY_SOURCES ${TTT_TARGETS_FOR_ANALYSIS})
  add_custom_target(ttt_clang_tidy
    COMMAND ${TTT_CLANG_TIDY_EXECUTABLE} --config-file=${CMAKE_SOURCE_DIR}/.clang-tidy -p
            ${CMAKE_BINARY_DIR} ${TTT_CLANG_TIDY_SOURCES}
    DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-tidy on project sources"
    VERBATIM)
  list(APPEND TTT_STATIC_ANALYSIS_TARGETS ttt_clang_tidy)
endif()

if (TTT_ENABLE_CPPCHECK)
  find_program(TTT_CPPCHECK_EXECUTABLE NAMES cppcheck)
  if (NOT TTT_CPPCHECK_EXECUTABLE)
    message(FATAL_ERROR "TTT_ENABLE_CPPCHECK is ON but cppcheck was not found")
  endif()
  add_custom_target(ttt_cppcheck
    COMMAND ${TTT_CPPCHECK_EXECUTABLE}
            --project=${CMAKE_BINARY_DIR}/compile_commands.json
            --enable=warning,style,performance,portability
            --std=c++20
            --inline-suppr
            --error-exitcode=1
    DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running cppcheck on project sources"
    VERBATIM)
  list(APPEND TTT_STATIC_ANALYSIS_TARGETS ttt_cppcheck)
endif()

if (TTT_STATIC_ANALYSIS_TARGETS)
  add_custom_target(static-analysis
    COMMENT "Aggregate static analysis targets"
    DEPENDS ${TTT_STATIC_ANALYSIS_TARGETS})
endif()
