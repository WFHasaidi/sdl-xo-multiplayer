#include "core/Board.hpp"

#include <algorithm>
#include <array>

namespace ttt {

bool Board::place(int row, int col, Player p) {
  if (!inBounds(row, col) || p == Player::None) {
    return false;
  }
  Player& cell = cells_[index(row, col)];
  if (cell != Player::None) {
    return false;
  }
  cell = p;
  return true;
}

std::optional<Player> Board::winner() const {
  static constexpr std::array<std::array<int, 3>, 8> lines{
      {{{0, 1, 2}}, {{3, 4, 5}}, {{6, 7, 8}}, {{0, 3, 6}},
       {{1, 4, 7}}, {{2, 5, 8}}, {{0, 4, 8}}, {{2, 4, 6}}}};

  for (const auto& line : lines) {
    const Player first = cells_[line[0]];
    if (first == Player::None) {
      continue;
    }
    if (cells_[line[1]] == first && cells_[line[2]] == first) {
      return first;
    }
  }
  return std::nullopt;
}

bool Board::full() const {
  return std::all_of(cells_.begin(), cells_.end(),
                     [](Player p) { return p != Player::None; });
}

Player Board::at(int row, int col) const {
  if (!inBounds(row, col)) {
    return Player::None;
  }
  return cells_[index(row, col)];
}

void Board::reset() {
  cells_.fill(Player::None);
}

bool Board::inBounds(int row, int col) const {
  return row >= 0 && row < kSize && col >= 0 && col < kSize;
}

int Board::index(int row, int col) const {
  return row * kSize + col;
}

}  // namespace ttt
