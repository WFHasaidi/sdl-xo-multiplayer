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
ttt_enable_warnings(tictactoe)

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
