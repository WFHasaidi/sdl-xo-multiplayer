#pragma once

#include <array>
#include <optional>

namespace ttt {

enum class Player { None = 0, X, O };

class Board {
public:
  bool place(int row, int col, Player p);
  std::optional<Player> winner() const;
  bool full() const;
  Player at(int row, int col) const;
  void reset();

  static constexpr int size() { return kSize; }

private:
  static constexpr int kSize = 3;
  std::array<Player, kSize * kSize> cells_{};

  bool inBounds(int row, int col) const;
  int index(int row, int col) const;
};

}  // namespace ttt
