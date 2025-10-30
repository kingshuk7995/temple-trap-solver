#include <hashtable.h>

#include <bit>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <thread>
#include <type_traits>

#include "board.hpp"
#include "input.hpp"
#include "solver.hpp"

int main() {
// #define DEBUG_INPUT
#ifdef DEBUG_INPUT
  // test case 1
  // int input_pawn_pos = 7;
  // input_tile_data_t input_tile_infos = {{
  //     {0, 0},  // Goal at position 0, orientation irrelevant
  //     {4, 1},  // A at position 4, orientation 1
  //     {7, 3},  // B at position 7, orientation 3
  //     {1, 3},  // C at position 1, orientation 3
  //     {2, 3},  // D at position 2, orientation 3
  //     {8, 3},  // E at position 8, orientation 3
  //     {3, 2},  // F at position 3, orientation 2
  //     {6, 4},  // G at position 6, orientation 4
  //     {9, 2},  // H at position 9, orientation 2
  //     {5, 0},  // Water at position 5, orientation irrelevant
  // }};
  // test case 2
  int input_pawn_pos = 2;
  input_tile_data_t input_tile_infos = {{
      {0, 0},
      {1, 2},
      {4, 3},
      {8, 2},
      {2, 4},
      {6, 3},
      {3, 3},
      {7, 3},
      {9, 2},
      {5, 0},
  }};
#else
  auto input_data = handle_input();

  int8_t input_pawn_pos = input_data.first;
  input_tile_data_t input_tile_infos = input_data.second;
#endif

  Board board(input_tile_infos);
  State initial_state = State::from_input(input_pawn_pos, input_tile_infos);

  auto successors = [&board](const State& s) { return s.successors(board); };
  auto goal_test = [](const State& s) -> bool { return s.is_goal(); };
  auto heuristics = [&board](const State& s) -> int {
    return static_cast<int>(s.heuristic(board));
  };
  auto cost_between = [](const State& a, const State& b) -> int {
    (void)a;
    (void)b;
    return 1;
  };
  auto st = std::chrono::high_resolution_clock::now();
  auto first_successor_deb = successors(initial_state);
  auto result =
      astar(initial_state, successors, goal_test, heuristics, cost_between);
  auto end = std::chrono::high_resolution_clock::now();

  if (result) {
    std::cout << "Found path of cost: " << result->size() - 1 << '\n';
    if (result->size() == 0) {
      std::cout << "length 0 path\n";
      return -1;
    }
    if (result->size() == 1) {
      std::cout << "initial state is goal\n";
      return 0;
    }
    for (std::size_t i = 1ull; i < result->size(); i++) {
      std::cout << result->at(i - 1).get_action(result->at(i)) << std::endl;
    }
  } else {
    std::cout << "No path found.\n";
  }
  std::cout
      << "\ntime required: "
      << std::chrono::duration_cast<std::chrono::microseconds>(end - st).count()
      << " microseconds\n";
  return 0;
}
