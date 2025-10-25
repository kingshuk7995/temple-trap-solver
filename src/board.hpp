#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <format>
#include <utility>
#include <vector>

#include "types.hpp"

enum class Directions : std::int8_t {
  Up = 0,
  Down = 1,
  Left = 2,
  Right = 3,
};

inline Directions opposite_dir(Directions dir) {
  switch (dir) {
    case Directions::Up:
      return Directions::Down;
    case Directions::Left:
      return Directions::Right;
    case Directions::Right:
      return Directions::Left;
    case Directions::Down:
      return Directions::Up;
    default:
      assert(0 && "unexpected path");
      return {};
  }
}

enum class Floor : std::int8_t {
  Top,
  Floor,
  Water,
};

enum class TileTypes : int8_t {
  Water,
  Goal,
  Lane,
  L_Shape_Top,
  L_Shape_Bottom,
  Stairs,
};

struct GridElement {
  std::array<std::pair<Directions, Floor>, 2> openings;
  Floor floor;
};

class Board {
 public:
  Board(const input_tile_data_t& input_data) {
    for (int8_t i = 0; i < static_cast<int8_t>(TileNames::End); i++) {
      auto tile = static_cast<TileNames>(i);
      auto type = get_tiletype(tile);
      switch (type) {
        case TileTypes::Stairs:
          this->grid_info[i].floor = Floor::Floor;
          this->grid_info[i].openings =
              generate_opens_stairs(input_data[i].second);
          break;
        case TileTypes::L_Shape_Bottom:
          this->grid_info[i].floor = Floor::Floor;
          this->grid_info[i].openings =
              generate_from_L(input_data[i].second, Floor::Floor);
          break;
        case TileTypes::L_Shape_Top:
          this->grid_info[i].floor = Floor::Top;
          this->grid_info[i].openings =
              generate_from_L(input_data[i].second, Floor::Top);
          break;
        case TileTypes::Lane:
          this->grid_info[i].floor = Floor::Top;
          this->grid_info[i].openings =
              generate_opens_stairs(input_data[i].second);
          this->grid_info[i].openings[0].second = Floor::Top;
          this->grid_info[i].openings[1].second = Floor::Top;
        default:
          break;
      }
    }
    this->grid_info[static_cast<std::size_t>(TileNames::Water)].floor =
        Floor::Water;
    this->grid_info[static_cast<std::size_t>(TileNames::Water)]
        .openings[0]
        .second = Floor::Water;
    this->grid_info[static_cast<std::size_t>(TileNames::Water)]
        .openings[1]
        .second = Floor::Water;
    this->grid_info[(static_cast<std::size_t>(TileNames::Goal))].floor =
        Floor::Top;
    this->grid_info[(static_cast<std::size_t>(TileNames::Goal))].openings = {
        {{Directions::Right, Floor::Top}, {Directions::Right, Floor::Top}}};
  }
  Floor get_floor(const TileNames name) const {
    return grid_info[static_cast<std::size_t>(name)].floor;
  }

  std::array<std::pair<Directions, Floor>, 2> get_openings(
      const TileNames name) const {
    return grid_info[static_cast<std::size_t>(name)].openings;
  }

 private:
  std::array<GridElement, static_cast<std::size_t>(TileNames::End)> grid_info;
  TileTypes get_tiletype(TileNames name) {
    switch (name) {
      case TileNames::A:
      case TileNames::B:
        return TileTypes::L_Shape_Top;
      case TileNames::C:
        return TileTypes::Lane;
      case TileNames::D:
      case TileNames::E:
        return TileTypes::Stairs;
      case TileNames::F:
      case TileNames::G:
      case TileNames::H:
        return TileTypes::L_Shape_Bottom;
      case TileNames::Goal:
        return TileTypes::Goal;
      case TileNames::Water:
        return TileTypes::Water;
      default:
        assert(
            false &&
            std::format("wrong tile type: {}", static_cast<int>(name)).c_str());
        return TileTypes::Water;
    }
  }
  std::array<std::pair<Directions, Floor>, 2> generate_opens_stairs(
      int8_t orientation) {
    switch (orientation) {
      case 1:
        return {
            {{Directions::Up, Floor::Top}, {Directions::Down, Floor::Floor}}};
      case 2:
        return {{{Directions::Right, Floor::Top},
                 {Directions::Left, Floor::Floor}}};
      case 3:
        return {{{Directions::Left, Floor::Top},
                 {Directions::Right, Floor::Floor}}};
      case 4:
        return {
            {{Directions::Up, Floor::Floor}, {Directions::Down, Floor::Top}}};
      default:
        assert(0 &&
               std::format("invalid orientation: {}", orientation).c_str());
        return {};
    }
  }
  std::array<std::pair<Directions, Floor>, 2> generate_from_L(
      int8_t orientation, Floor tilefloor) {
    switch (orientation) {
      case 1:
        return {
            {{Directions::Right, tilefloor}, {Directions::Down, tilefloor}}};
      case 2:
        return {{{Directions::Left, tilefloor}, {Directions::Down, tilefloor}}};
      case 3:
        return {{{Directions::Right, tilefloor}, {Directions::Up, tilefloor}}};
      case 4:
        return {{{Directions::Left, tilefloor}, {Directions::Up, tilefloor}}};
      default:
        assert(0 &&
               std::format("invalid orientation: {}", orientation).c_str());
        return {};
    }
  }
};

class State {
 public:
  std::array<TileNames, static_cast<int>(TileNames::End)> tiles;
  std::int8_t pawn_pos;
  std::int8_t water_pos;

  static State from_input(std::int8_t pawn_pos, input_tile_data_t& data) {
    State st;
    st.pawn_pos = pawn_pos;
    for (int i = 0; i < static_cast<int>(TileNames::End); i++) {
      if (TileNames::Water == static_cast<TileNames>(i)) {
        st.water_pos = data[i].first;
      }
      st.tiles[data[i].first] = static_cast<TileNames>(i);
    }
    return st;
  }

  inline bool is_goal() const { return (this->pawn_pos == 0); }

  std::vector<State> successors(const Board& board) const {
    std::vector<State> successors;

    auto cur_pawn_floor = board.get_floor(this->tiles[this->pawn_pos]);
    // pawn movements
    for (auto& [opendir, openfloor] :
         board.get_openings(this->tiles[this->pawn_pos])) {
      int8_t next_pawn_pos = to_dir(this->pawn_pos, opendir);
      if (next_pawn_pos == -1 || next_pawn_pos == this->water_pos) {
        continue;
      }
      Directions required_next_opening = opposite_dir(opendir);
      bool ok = false;
      for (auto& [next_opendir, next_openfloor] :
           board.get_openings(this->tiles[next_pawn_pos])) {
        if (next_opendir == required_next_opening &&
            next_openfloor == openfloor) {
          ok = true;
        }
      }
      if (ok) {
        State next = *this;
        next.pawn_pos = next_pawn_pos;
        successors.push_back(next);
      }
    }

    if (cur_pawn_floor == Floor::Top) {
      return successors;
    }
    // tile movements
    static const Directions dir_array[] = {Directions::Up, Directions::Down,
                                           Directions::Left, Directions::Right};
    for (auto& dir : dir_array) {
      int8_t next_water_pos = to_dir(this->water_pos, dir);
      if (next_water_pos == -1 || next_water_pos == this->pawn_pos) {
        continue;
      }
      State next = *this;
      std::swap(next.tiles[water_pos], next.tiles[next_water_pos]);
      next.water_pos = next_water_pos;
      successors.push_back(next);
    }
    return successors;
  }

  int heuristic() const {
    // manhattan distance
    if (this->pawn_pos == 0) return 0;
    return (this->pawn_pos / 3) + (this->pawn_pos % 3) + 1;
  }

 private:
  inline static int8_t to_dir(const int8_t before, const Directions dir) {
    static const std::array<std::array<int8_t, 4>, 10> adj = {
        //       top down left right
        {/* 0 */ {{-1, -1, -1, 1}},
         /* 1 */ {{-1, 4, 0, 2}},
         /* 2 */ {{-1, 5, 1, 3}},
         /* 3 */ {{-1, 6, 2, -1}},
         /* 4 */ {{1, 7, -1, 5}},
         /* 5 */ {{2, 8, 4, 6}},
         /* 6 */ {{3, 9, 5, -1}},
         /* 7 */ {{4, -1, -1, 8}},
         /* 8 */ {{5, -1, 7, 9}},
         /* 9 */ {{6, -1, 8, -1}}}};

    if (before < 0 || before > 9) return -1;
    return adj[before][static_cast<std::size_t>(dir)];
  }
};
