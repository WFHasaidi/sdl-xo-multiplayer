#include "core/Game.hpp"

namespace ttt {

bool Game::playMove(int row, int col) {
  if (state_ != State::Ongoing) {
    return false;
  }
  if (!board_.place(row, col, current_)) {
    return false;
  }

  evaluateBoard();
  if (state_ == State::Ongoing) {
    advanceTurn();
  }
  return true;
}

void Game::reset() {
  board_.reset();
  current_ = Player::X;
  state_ = State::Ongoing;
}

void Game::advanceTurn() {
  current_ = (current_ == Player::X) ? Player::O : Player::X;
}

void Game::evaluateBoard() {
  const auto winnerOpt = board_.winner();
  if (winnerOpt) {
    state_ = (*winnerOpt == Player::X) ? State::XWin : State::OWin;
    return;
  }
  if (board_.full()) {
    state_ = State::Draw;
  }
}

}  // namespace ttt
