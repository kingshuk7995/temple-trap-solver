#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <optional>
#include <queue>
#include <type_traits>
#include <unordered_map>
#include <vector>

/**
 * Concepts for Successor Function type for A*
 */
template <typename F, typename StateType>
concept SuccessorFunc = requires(F f, const StateType& s) {
  { f(s) } -> std::same_as<std::vector<StateType>>;
};

/**
 * Concepts for GoalTest Function type for A*
 */
template <typename F, typename StateType>
concept GoalTestFunc = requires(F f, const StateType& s) {
  { f(s) } -> std::same_as<bool>;
};

/**
 * Concepts for Heauristic Function type for A*
 */
template <typename F, typename StateType>
concept HeuristicFunc = requires(F f, const StateType& s) {
  { f(s) } -> std::convertible_to<int>;
};
/**
 * Concepts for Cost Function type for A*
 */
template <typename F, typename StateType>
concept CostFunc = requires(F f, const StateType& a, const StateType& b) {
  { f(a, b) } -> std::convertible_to<int>;
};
/**
 * Concepts for copyability of state needed for A*
 */
template <typename StateType>
concept AStarState = std::copyable<StateType>;

/**
 * @brief Generic A* (A-star) pathfinding algorithm.
 *
 * This implementation of A* works with any user-defined state type,
 * as long as it satisfies the @ref AStarState concept and provides
 * suitable hash and equality functions for use in unordered maps.
 *
 * The algorithm uses a priority queue (min-heap) for open set management
 * and unordered maps to maintain g-scores and f-scores.
 * It returns the path from the start state to the goal as a vector of states,
 * or `std::nullopt` if no path is found.
 *
 * @tparam StateType The type representing each search state.
 *                   Must satisfy the @ref AStarState concept.
 * @tparam Succ      Function type satisfying @ref SuccessorFunc<StateType>.
 *                   Should return a `std::vector<StateType>` of successor
 * states.
 * @tparam Goal      Function type satisfying @ref GoalTestFunc<StateType>.
 *                   Should return `true` if the given state is a goal.
 * @tparam Heur      Function type satisfying @ref HeuristicFunc<StateType>.
 *                   Should return an integer heuristic estimate of
 * cost-to-goal.
 * @tparam Cost      Function type satisfying @ref CostFunc<StateType>.
 *                   Should return an integer transition cost between two
 * states.
 * @tparam Hash      Hash functor for `StateType`. Defaults to
 * `std::hash<StateType>`.
 * @tparam Eq        Equality functor for `StateType`. Defaults to
 * `std::equal_to<StateType>`.
 *
 * @param start          The initial state.
 * @param get_successors  Function that returns successors of a given state.
 * @param is_goal         Function that checks whether a state is the goal.
 * @param heuristic       Function that computes heuristic cost for a state.
 * @param cost_between    Function that computes the actual cost between two
 * states.
 * @param hash            (Optional) Hash function object for unordered_map.
 * @param eq              (Optional) Equality comparator for unordered_map.
 *
 * @return `std::optional<std::vector<StateType>>` containing the sequence
 *         of states from start to goal if a path exists; `std::nullopt`
 * otherwise.
 *
 * @note The algorithm assumes that the heuristic is *admissible* (never
 * overestimates).
 * @warning This version uses dynamic memory (`std::unordered_map`,
 * `std::priority_queue`). For deterministic or real-time systems, consider
 * replacing them with `std::pmr::unordered_map` or fixed-size arena-based
 * containers.
 * @see AStarState, SuccessorFunc, GoalTestFunc, HeuristicFunc, CostFunc
 */
template <AStarState StateType, SuccessorFunc<StateType> Succ,
          GoalTestFunc<StateType> Goal, HeuristicFunc<StateType> Heur,
          CostFunc<StateType> Cost, typename Hash = std::hash<StateType>,
          typename Eq = std::equal_to<StateType>>
std::optional<std::vector<StateType>> astar(const StateType& start,
                                            Succ&& get_successors,
                                            Goal&& is_goal, Heur&& heuristic,
                                            Cost&& cost_between,
                                            Hash hash = Hash{}, Eq eq = Eq{}) {
  if (is_goal(start)) return std::vector<StateType>{start};

  using ScoreMap = std::unordered_map<StateType, int, Hash, Eq>;
  using CameFrom = std::unordered_map<StateType, StateType, Hash, Eq>;

  const int INF = std::numeric_limits<int>::max();

  ScoreMap g_score(0, hash, eq);
  ScoreMap f_score(0, hash, eq);
  CameFrom came_from(0, hash, eq);

  /// Priority queue node
  struct PQNode {
    int f;
    std::size_t counter;  /// tie-breaker
    StateType state;
  };
  struct Compare {
    bool operator()(const PQNode& a, const PQNode& b) const {
      if (a.f != b.f) return a.f > b.f;
      return a.counter > b.counter;
    }
  };
  std::priority_queue<PQNode, std::vector<PQNode>, Compare> open_pq;

  /// Initialize
  g_score[start] = 0;
  f_score[start] = heuristic(start);
  std::size_t push_counter = 0;
  open_pq.emplace(PQNode{f_score[start], push_counter++, start});

  while (!open_pq.empty()) {
    PQNode top = open_pq.top();
    open_pq.pop();
    StateType current = std::move(top.state);

    // Skip if this node has a worse f than the latest
    auto it_f = f_score.find(current);
    if (it_f != f_score.end() && top.f > it_f->second) continue;

    if (is_goal(current)) {
      // path
      std::vector<StateType> path;
      path.push_back(current);
      while (!eq(current, start)) {
        current = came_from.at(current);
        path.push_back(current);
      }
      std::reverse(path.begin(), path.end());
      return path;
    }

    for (const StateType& nb : get_successors(current)) {
      int g_current = g_score[current];
      int tentative_g = g_current + cost_between(current, nb);

      auto it_g_nb = g_score.find(nb);
      int g_nb = (it_g_nb != g_score.end()) ? it_g_nb->second : INF;

      if (tentative_g < g_nb) {
        came_from.insert_or_assign(nb, current);
        g_score.insert_or_assign(nb, tentative_g);
        int nb_f = tentative_g + heuristic(nb);
        f_score.insert_or_assign(nb, nb_f);
        open_pq.emplace(PQNode{nb_f, push_counter++, nb});
      }
    }
  }

  return std::nullopt;  /// No path
}
