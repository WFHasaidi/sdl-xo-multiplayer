#include <exception>
#include <iostream>

#include "core/Board.hpp"

namespace {

int failures = 0;

#define EXPECT_TRUE(expr)                                                                     \
  do {                                                                                        \
    if (!(expr)) {                                                                            \
      std::cerr << __FILE__ << ":" << __LINE__ << " EXPECT_TRUE(" #expr ") failed\n";         \
      ++failures;                                                                             \
    }                                                                                         \
  } while (false)

#define EXPECT_EQ(lhs, rhs)                                                                   \
  do {                                                                                        \
    auto _l = (lhs);                                                                          \
    auto _r = (rhs);                                                                          \
    if (!((_l) == (_r))) {                                                                    \
      std::cerr << __FILE__ << ":" << __LINE__ << " EXPECT_EQ(" #lhs ", " #rhs ") failed\n";  \
      ++failures;                                                                             \
    }                                                                                         \
  } while (false)

}  // namespace

int run_tests() {
  using ttt::Board;
  using ttt::Player;

  Board board;
  EXPECT_TRUE(board.place(0, 0, Player::X));
  EXPECT_TRUE(!board.place(0, 0, Player::O));
  EXPECT_TRUE(!board.place(3, 0, Player::X));

  board.reset();
  board.place(0, 0, Player::O);
  board.place(0, 1, Player::O);
  board.place(0, 2, Player::O);
  const auto winner = board.winner();
  EXPECT_TRUE(winner.has_value());
  if (winner.has_value()) {
    EXPECT_EQ(*winner, Player::O);
  }
  EXPECT_TRUE(!board.full());
  return failures == 0 ? 0 : 1;
}

int main() {
  try {
    return run_tests();
  } catch (const std::exception& ex) {
    std::cerr << "Unexpected exception: " << ex.what() << "\n";
  } catch (...) {
    std::cerr << "Unknown exception caught\n";
  }
  return 1;
}
