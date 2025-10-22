# AGENTS.md

> Practical guide for **agent-driven development** on *SDL2 Tic‑Tac‑Toe*.
> The assistant ("Codex") is used as a pair programmer; every AI-authored change is **reviewed** before merge.

## Goals
- Short iteration loops: small increments + tests.
- Portability: Linux & Windows (macOS as a bonus).
- No global state; C++20, RAII, explicit errors via `std::optional` or `expected`-style types.

## Target layout (v0)
```
include/
  core/Board.hpp
  core/Game.hpp
src/
  core/Board.cpp
  core/Game.cpp
  app/main.cpp
  app/Renderer.cpp
  app/Renderer.hpp
  net/TcpPeer.cpp
  net/TcpPeer.hpp
cmake/
  FindSDL2.cmake    # if needed
tests/
  test_board.cpp
  test_game.cpp
assets/
CMakeLists.txt
conanfile.txt (optional)
```

## Agent roles (shortcuts)
Prompts are referenced in `config.toml:[codex.roles]`.
- **Architect**: decomposes features, public APIs, risks.
- **Implementer**: delivers compilable code + tests.
- **Reviewer**: strict diff review (UB, lifetimes, portability, error handling).
- **Builder**: CMake, SDL2 discovery, presets.
- **Tester**: gtest, test matrix, coverage.
- **Doc Writer**: README/HOWTO updates.
- **Refactorer**: improvements without behavior changes.

## Workflow
1. Open a clear issue.
2. **Architect** proposes plan + APIs.
3. **Implementer** delivers a patch < 150 lines + tests.
4. **Reviewer** comments, requests fixes if needed.
5. **Builder/Tester** validate on Linux/Windows.
6. **Doc Writer** updates docs.

## Git conventions
- Branches: `feat/*`, `fix/*`, `refactor/*`, `docs/*`, `build/*`, `test/*`, `ci/*`
- Commit prefixes: `[feat]`, `[fix]`, `[refactor]`, `[docs]`, `[build]`, `[test]`, `[ci]`
- AI-assisted commits include trailers:
  ```
  Co-authored-by: ChatGPT Codex
  Signed-off-by: SAIDI Amine <amine.saidi.ing@gmail.com>
  ```

## Definition of Done (per feature)
- Build passes in Debug & Release.
- Unit tests green for `core::Board` & `core::Game`; coverage ≥ 70%.
- Executable opens an SDL2 window (smoke test) and draws a 3×3 grid.
- Network mode: handshake `HELLO(version)` + echo `STATE` (stub initially).
- README updated.

## Local setup (recommended)
### Option A — System packages (SDL2 installed)
```bash
# Linux (Debian/Ubuntu)
sudo apt-get update && sudo apt-get install -y libsdl2-dev cmake make g++

cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
ctest --test-dir build --output-on-failure
```

### Option B — Conan (cross-platform)
Minimal `conanfile.txt`:
```ini
[requires]
sdl/2.28.5

[generators]
CMakeDeps
CMakeToolchain

[layout]
basic
```
Commands:
```bash
conan profile detect --force
conan install . -s build_type=Debug --build=missing
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake
cmake --build build -j
ctest --test-dir build --output-on-failure
```

## Minimal CMake (to add next)
```cmake
cmake_minimum_required(VERSION 3.22)
project(sdl2_tictactoe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(TTT_BUILD_TESTS "Build unit tests" ON)

find_package(SDL2 REQUIRED)

add_executable(tictactoe
  src/app/main.cpp
)

# Link against SDL2::SDL2 or SDL2::SDL2main depending on your FindSDL2
# target_link_libraries(tictactoe PRIVATE SDL2::SDL2 SDL2::SDL2main)

if(TTT_BUILD_TESTS)
  enable_testing()
  add_executable(test_board tests/test_board.cpp src/core/Board.cpp)
  # target_link_libraries(test_board PRIVATE gtest::gtest)
  add_test(NAME board COMMAND test_board)
endif()
```

## Using the assistant effectively
- Always provide the **current file contents** or a **diff**.
- Ask for **unified patch** outputs for quick application.
- Start with a **failing test**, then implement the fix.

## Next steps
1. Add the minimal `CMakeLists.txt` (above).
2. Implement `Board` + tests.
3. Render the 3×3 grid with SDL2 in `main.cpp`.
4. Networking sketch (TCP): host server + join client.


