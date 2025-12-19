#pragma once

#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <format>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <types.hpp>
#include <utility>

using input_tile_data_t = std::array<std::pair<std::int8_t, std::int8_t>,
                                     static_cast<std::size_t>(TileNames::End)>;

std::pair<int8_t, input_tile_data_t> handle_input() {
  int pawn_pos;
  input_tile_data_t input_tile_info{};
  input_tile_info[static_cast<std::size_t>(TileNames::Goal)] = {
      static_cast<int8_t>(0), static_cast<int8_t>(0)};

  std::cout << "============ Board Positions ============" << std::endl;
  std::cout << "the position of the tile on the board\n";
  std::cout << "0 1 2 3\n"
            << "  4 5 6\n"
            << "  7 8 9\n";
  std::cout << "============== Orientation ==============" << std::endl;
  std::cout << "the position of the mark on the tile\n";
  std::cout << "1 2\n"
            << "3 4\n";

  std::array<std::pair<char, TileNames>, 8> tiles{{
      {'A', TileNames::A},
      {'B', TileNames::B},
      {'C', TileNames::C},
      {'D', TileNames::D},
      {'E', TileNames::E},
      {'F', TileNames::F},
      {'G', TileNames::G},
      {'H', TileNames::H},
  }};

  std::bitset<10> used_pos{};
  used_pos[0] = true;

  for (auto& [tilestr, tile] : tiles) {
    int pos = 0, orientation = 0;
    std::cout << std::format("\nInput for Tile: {}\n", tilestr);

  input_pos:
    std::cout << "Position (1-9): ";
    if (!(std::cin >> pos)) {
      std::cout << "Invalid input (not a number). Try again.\n";
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      goto input_pos;
    }
    if (pos <= 0 || pos > 9) {
      std::cout << "Position must be in range [1,9]. Try again.\n";
      goto input_pos;
    }
    if (used_pos[pos]) {
      std::cout << "Position already taken. Choose another.\n";
      goto input_pos;
    }

    used_pos[pos] = true;

  input_orientation:
    std::cout << "Orientation (1-4): ";
    if (!(std::cin >> orientation)) {
      std::cout << "Invalid input (not a number). Try again.\n";
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      goto input_orientation;
    }
    if (orientation < 1 || orientation > 4) {
      std::cout << "Orientation must be in range [1,4]. Try again.\n";
      goto input_orientation;
    }

    input_tile_info[static_cast<std::size_t>(tile)] = {
        static_cast<std::int8_t>(pos), static_cast<std::int8_t>(orientation)};
  }

  assert(used_pos.count() == 9 && "invalid position inputs");

  for (std::int8_t i = 1; i <= 9; i++) {
    if (!used_pos[i]) {
      input_tile_info[static_cast<std::size_t>(TileNames::Water)] = {
          i, static_cast<int8_t>(0)};
      break;
    }
  }
  std::cout << "================= Pawn =================" << std::endl;
pawn_input:
  std::cout << "Position (1-9): ";
  if (!(std::cin >> pawn_pos)) {
    std::cout << "Invalid input (not a number). Try again.\n";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    goto pawn_input;
  }
  if (pawn_pos ==
          input_tile_info[static_cast<std::size_t>(TileNames::Water)].first ||
      pawn_pos ==
          input_tile_info[static_cast<std::size_t>(TileNames::A)].first ||
      pawn_pos ==
          input_tile_info[static_cast<std::size_t>(TileNames::B)].first ||
      pawn_pos ==
          input_tile_info[static_cast<std::size_t>(TileNames::C)].first) {
    std::cout << "Invalid pawn position. Try again.\n";
    goto pawn_input;
  }
  return std::make_pair(static_cast<std::int8_t>(pawn_pos), input_tile_info);
}

// Utility function to print the contents of input_tile_data_t
void print_tile_data(const input_tile_data_t& data) {
  // Names corresponding to the TileNames enum order
  const char* tile_names[] = {"Goal", "A", "B", "C", "D",
                              "E",    "F", "G", "H", "Water"};

  std::cout << "--------------------------------\n";
  std::cout << "| " << std::setw(6) << std::left << "Tile"
            << " | " << std::setw(8) << std::left << "Position"
            << " | " << std::setw(11) << std::left << "Orientation"
            << " |\n";
  std::cout << "--------------------------------\n";

  for (int i = 0; i < static_cast<int>(TileNames::End); ++i) {
    std::cout << "| " << std::setw(6) << std::left << tile_names[i] << " | "
              << std::setw(8) << std::left
              << static_cast<int>(data[i].first)  // Cast to int for printing
              << " | " << std::setw(11) << std::left
              << static_cast<int>(data[i].second)  // Cast to int for printing
              << " |\n";
  }
  std::cout << "--------------------------------\n";
}