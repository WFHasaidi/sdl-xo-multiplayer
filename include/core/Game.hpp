#pragma once

#include "core/Board.hpp"

namespace ttt {

class Game {
public:
  enum class State { Ongoing, XWin, OWin, Draw };

  bool playMove(int row, int col);
  void reset();

  State state() const { return state_; }
  Player current() const { return current_; }
  const Board& board() const { return board_; }

private:
  Board board_;
  Player current_{Player::X};
  State state_{State::Ongoing};

  void advanceTurn();
  void evaluateBoard();
};

}  // namespace ttt
