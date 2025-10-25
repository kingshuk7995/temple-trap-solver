#include <hashtable.h>

#include <bit>
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

namespace std {
template <>
struct hash<State> {
  std::size_t operator()(const State& s) const noexcept {
    std::size_t h = 0;

    auto hash_combine = [&h](std::size_t v) {
      h ^= v + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
    };

    hash_combine(std::hash<decltype(s.pawn_pos)>{}(s.pawn_pos));
    hash_combine(std::hash<decltype(s.water_pos)>{}(s.water_pos));

    std::size_t arr_hash = 0;
    for (auto v : s.tiles) {
      arr_hash ^= std::hash<std::uint8_t>{}(static_cast<std::uint8_t>(v)) +
                  0x9e3779b97f4a7c15ULL + (arr_hash << 6) + (arr_hash >> 2);
    }
    hash_combine(arr_hash);

    return h;
  }
};

template <>
struct equal_to<State> {
  bool operator()(State const& a, State const& b) const noexcept {
    return a.pawn_pos == b.pawn_pos && a.tiles == b.tiles &&
           a.water_pos == b.water_pos;
  }
};
}  // namespace std

int main() {
// #define DEBUG_INPUT
#ifdef DEBUG_INPUT
  int input_pawn_pos = 7;
  input_tile_data_t input_tile_infos = {{
      {0, 0},  // Goal at position 0, orientation irrelevant
      {4, 1},  // A at position 4, orientation 1
      {7, 3},  // B at position 7, orientation 3
      {1, 3},  // C at position 1, orientation 3
      {2, 3},  // D at position 2, orientation 3
      {8, 3},  // E at position 8, orientation 3
      {3, 2},  // F at position 3, orientation 2
      {6, 4},  // G at position 6, orientation 4
      {9, 2},  // H at position 9, orientation 2
      {5, 0},  // Water at position 5, orientation irrelevant
  }};
#else
  input_tile_data_t input_tile_infos_dbg = {{
      {0, 0},  // Goal at position 0, orientation irrelevant
      {4, 1},  // A at position 4, orientation 1
      {7, 3},  // B at position 7, orientation 3
      {1, 3},  // C at position 1, orientation 3
      {2, 3},  // D at position 2, orientation 3
      {8, 3},  // E at position 8, orientation 3
      {3, 2},  // F at position 3, orientation 2
      {6, 4},  // G at position 6, orientation 4
      {9, 2},  // H at position 9, orientation 2
      {5, 0},  // Water at position 5, orientation irrelevant
  }};
  auto input_data = handle_input();

  int8_t input_pawn_pos = input_data.first;
  input_tile_data_t input_tile_infos = input_data.second;

  if (input_tile_infos_dbg == input_tile_infos && input_pawn_pos == 9) {
    std::cout << "valid input\n";
  } else {
    std::cout << "pawn pos: " << input_pawn_pos << '\n';
    print_tile_data(input_tile_infos);
    print_tile_data(input_tile_infos_dbg);
  }
#endif

  Board board(input_tile_infos);
  State initial_state = State::from_input(input_pawn_pos, input_tile_infos);
  std::cout << "parse successful\n";
  auto successors = [&board](const State& s) { return s.successors(board); };
  auto goal_test = [](const State& s) -> bool { return s.is_goal(); };
  auto heuristics = [](const State& s) -> int {
    return static_cast<int>(s.heuristic());
  };
  auto cost_between = [](const State& a, const State& b) -> int {
    (void)a;
    (void)b;
    return 1;
  };

  auto first_successor_deb = successors(initial_state);
  auto result =
      astar(initial_state, successors, goal_test, heuristics, cost_between);
  if (result) {
    std::cout << "Found path of cost: " << result->size() - 1 << '\n';
    for (auto& it : *result) {
      std::cout << std::format("pawn pos: {} water pos: {}\n", it.pawn_pos,
                               it.water_pos);
    }
  } else {
    std::cout << "No path found.\n";
  }

  return 0;
}
