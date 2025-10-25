#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>

enum class TileNames : std::size_t {
  Goal = 0,
  A = 1,
  B = 2,
  C = 3,
  D = 4,
  E = 5,
  F = 6,
  G = 7,
  H = 8,
  Water = 9,
  End = 10,
};

using input_tile_data_t = std::array<std::pair<std::int8_t, std::int8_t>,
                                     static_cast<std::size_t>(TileNames::End)>;
