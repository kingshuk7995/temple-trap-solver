#pragma once

#include "board.hpp"
#include "solver.hpp"
#include <vector>
#include <optional>
#include <algorithm>

class GameController {
private:
    Board board;
    State current_state;
    std::optional<std::vector<State>> cached_path;

public:
    GameController(const input_tile_data_t& input_data, int8_t initial_pawn_pos)
        : board(input_data),
          current_state(State::from_input(initial_pawn_pos, const_cast<input_tile_data_t&>(input_data))) {}

    void set_state(const State& state) {
        current_state = state;
        /// @brief If state is not on the cached path, invalidate the cache.
        if (cached_path.has_value()) {
            auto it = std::find(cached_path->begin(), cached_path->end(), current_state);
            if (it != cached_path->end()) {
                cached_path->erase(cached_path->begin(), it);
            } else {
                cached_path = std::nullopt;
            }
        }
    }

    State get_current_state() const { return current_state; }

    bool is_goal() const { return current_state.is_goal(); }

    std::optional<State> get_hint() {
        if (is_goal()) return std::nullopt;

        if (cached_path.has_value() && cached_path->size() > 1) {
            if (cached_path->front() == current_state) {
                return cached_path->at(1);
            }
        }

        // Rerun A*
        auto successors = [this](const State& s) { return s.successors(board); };
        auto goal_test = [](const State& s) -> bool { return s.is_goal(); };
        auto heuristics = [this](const State& s) -> int {
            return static_cast<int>(s.heuristic(const_cast<Board&>(board)));
        };
        auto cost_between = [](const State&, const State&) -> int { return 1; };

        cached_path = astar(current_state, successors, goal_test, heuristics, cost_between);

        if (cached_path.has_value() && cached_path->size() > 1) {
            return cached_path->at(1);
        }
        
        return std::nullopt;
    }
    
    /// @brief Attempt a tile move to the water position
    bool move_tile(int8_t from_pos) {
        auto successors = current_state.successors(board);
        for (const auto& succ : successors) {
            if (succ.water_pos == from_pos && current_state.water_pos != from_pos) {
                set_state(succ);
                return true;
            }
        }
        return false;
    }

    /// @brief Attempt a pawn move
    bool move_pawn(int8_t to_pos) {
        auto successors = current_state.successors(board);
        for (const auto& succ : successors) {
            if (succ.pawn_pos == to_pos && current_state.pawn_pos != to_pos) {
                set_state(succ);
                return true;
            }
        }
        return false;
    }
};
