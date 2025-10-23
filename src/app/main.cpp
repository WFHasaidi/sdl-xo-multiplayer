#include <SDL.h>

#include <algorithm>
#include <cmath>
#include <memory>
#include <string>

#include "core/Game.hpp"

namespace {

struct SDLDeleter {
  void operator()(SDL_Window* w) const { SDL_DestroyWindow(w); }
  void operator()(SDL_Renderer* r) const { SDL_DestroyRenderer(r); }
};

struct RendererSize {
  int width{0};
  int height{0};
};

RendererSize rendererSize(SDL_Renderer* renderer) {
  RendererSize size{};
  SDL_GetRendererOutputSize(renderer, &size.width, &size.height);
  return size;
}

void drawGrid(SDL_Renderer* renderer, int width, int height) {
  const int cols = ttt::Board::size();
  const int rows = ttt::Board::size();
  const int cellW = width / cols;
  const int cellH = height / rows;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  for (int c = 1; c < cols; ++c) {
    const int x = c * cellW;
    SDL_RenderDrawLine(renderer, x, 0, x, height);
  }
  for (int r = 1; r < rows; ++r) {
    const int y = r * cellH;
    SDL_RenderDrawLine(renderer, 0, y, width, y);
  }
}

SDL_Point cellOrigin(int row, int col, int cellW, int cellH) {
  return SDL_Point{col * cellW, row * cellH};
}

SDL_Point cellCenter(const SDL_Point& origin, int cellW, int cellH) {
  return SDL_Point{origin.x + cellW / 2, origin.y + cellH / 2};
}

void drawX(SDL_Renderer* renderer, const SDL_Point& origin, int cellW, int cellH) {
  const int padding = std::min(cellW, cellH) / 6;
  const SDL_Point topLeft{origin.x + padding, origin.y + padding};
  const SDL_Point topRight{origin.x + cellW - padding, origin.y + padding};
  const SDL_Point bottomLeft{origin.x + padding, origin.y + cellH - padding};
  const SDL_Point bottomRight{origin.x + cellW - padding, origin.y + cellH - padding};
  SDL_RenderDrawLine(renderer, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
  SDL_RenderDrawLine(renderer, topRight.x, topRight.y, bottomLeft.x, bottomLeft.y);
}

void drawO(SDL_Renderer* renderer, const SDL_Point& origin, int cellW, int cellH) {
  const int shortest = std::min(cellW, cellH);
  const int padding = shortest / 6;
  const int radius = shortest / 2 - padding;
  const SDL_Point center = cellCenter(origin, cellW, cellH);
  constexpr double pi = 3.14159265358979323846;
  const int segments = 60;
  for (int i = 0; i < segments; ++i) {
    const double theta1 = (2.0 * pi * i) / segments;
    const double theta2 = (2.0 * pi * (i + 1)) / segments;
    const int x1 = center.x + static_cast<int>(radius * std::cos(theta1));
    const int y1 = center.y + static_cast<int>(radius * std::sin(theta1));
    const int x2 = center.x + static_cast<int>(radius * std::cos(theta2));
    const int y2 = center.y + static_cast<int>(radius * std::sin(theta2));
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
  }
}

void drawBoard(SDL_Renderer* renderer, const ttt::Board& board, int width, int height) {
  const int cols = ttt::Board::size();
  const int rows = ttt::Board::size();
  const int cellW = width / cols;
  const int cellH = height / rows;

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  drawGrid(renderer, width, height);

  SDL_SetRenderDrawColor(renderer, 20, 20, 20, SDL_ALPHA_OPAQUE);
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      const ttt::Player p = board.at(r, c);
      if (p == ttt::Player::None) {
        continue;
      }
      const SDL_Point origin = cellOrigin(r, c, cellW, cellH);
      if (p == ttt::Player::X) {
        drawX(renderer, origin, cellW, cellH);
      } else {
        drawO(renderer, origin, cellW, cellH);
      }
    }
  }
}

std::string stateToTitle(const ttt::Game& game) {
  switch (game.state()) {
    case ttt::Game::State::XWin:
      return "X wins - press R to restart";
    case ttt::Game::State::OWin:
      return "O wins - press R to restart";
    case ttt::Game::State::Draw:
      return "Draw - press R to restart";
    case ttt::Game::State::Ongoing:
    default:
      return std::string("Turn: ") + (game.current() == ttt::Player::X ? "X" : "O");
  }
}

}  // namespace

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    SDL_Log("SDL_Init failed: %s", SDL_GetError());
    return 1;
  }

  std::unique_ptr<SDL_Window, SDLDeleter> window(
      SDL_CreateWindow("Tic-Tac-Toe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600,
                       SDL_WINDOW_SHOWN),
      SDLDeleter{});
  if (!window) {
    SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  Uint32 rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  std::unique_ptr<SDL_Renderer, SDLDeleter> renderer(
      SDL_CreateRenderer(window.get(), -1, rendererFlags), SDLDeleter{});
  if (!renderer) {
    rendererFlags = SDL_RENDERER_ACCELERATED;
    renderer.reset(SDL_CreateRenderer(window.get(), -1, rendererFlags));
  }
  if (!renderer) {
    SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  ttt::Game game;
  bool running = true;

  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        if (game.state() == ttt::Game::State::Ongoing) {
          const RendererSize size = rendererSize(renderer.get());
          const int cellW = size.width / ttt::Board::size();
          const int cellH = size.height / ttt::Board::size();
          if (cellW > 0 && cellH > 0) {
            const int col = event.button.x / cellW;
            const int row = event.button.y / cellH;
            game.playMove(row, col);
          }
        }
      } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
        game.reset();
      }
    }

    const RendererSize size = rendererSize(renderer.get());
    drawBoard(renderer.get(), game.board(), size.width, size.height);
    SDL_RenderPresent(renderer.get());
    SDL_SetWindowTitle(window.get(), stateToTitle(game).c_str());
  }

  SDL_Quit();
  return 0;
}
