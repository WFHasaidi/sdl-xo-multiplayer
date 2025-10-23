#include <exception>
#include <iostream>

#include "core/Game.hpp"

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
  using ttt::Game;
  using ttt::Player;

  Game game;
  EXPECT_EQ(game.state(), Game::State::Ongoing);
  EXPECT_EQ(game.current(), Player::X);

  EXPECT_TRUE(game.playMove(0, 0));
  EXPECT_EQ(game.current(), Player::O);
  EXPECT_TRUE(!game.playMove(0, 0));
  EXPECT_EQ(game.state(), Game::State::Ongoing);

  EXPECT_TRUE(game.playMove(1, 1));  // O
  EXPECT_TRUE(game.playMove(0, 1));  // X
  EXPECT_TRUE(game.playMove(2, 2));  // O
  EXPECT_TRUE(game.playMove(0, 2));  // X wins
  EXPECT_EQ(game.state(), Game::State::XWin);
  EXPECT_TRUE(!game.playMove(2, 0));

  game.reset();
  EXPECT_EQ(game.state(), Game::State::Ongoing);
  EXPECT_EQ(game.current(), Player::X);
  EXPECT_TRUE(game.playMove(0, 0));  // X
  EXPECT_TRUE(game.playMove(0, 1));  // O
  EXPECT_TRUE(game.playMove(0, 2));  // X
  EXPECT_TRUE(game.playMove(1, 1));  // O
  EXPECT_TRUE(game.playMove(1, 0));  // X
  EXPECT_TRUE(game.playMove(1, 2));  // O
  EXPECT_TRUE(game.playMove(2, 1));  // X
  EXPECT_TRUE(game.playMove(2, 0));  // O
  EXPECT_TRUE(game.playMove(2, 2));  // X -> Draw
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
