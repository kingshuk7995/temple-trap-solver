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

// Concepts
template <typename F, typename StateType>
concept SuccessorFunc = requires(F f, const StateType& s) {
  { f(s) } -> std::same_as<std::vector<StateType>>;
};

template <typename F, typename StateType>
concept GoalTestFunc = requires(F f, const StateType& s) {
  { f(s) } -> std::same_as<bool>;
};

template <typename F, typename StateType>
concept HeuristicFunc = requires(F f, const StateType& s) {
  { f(s) } -> std::convertible_to<int>;
};

template <typename F, typename StateType>
concept CostFunc = requires(F f, const StateType& a, const StateType& b) {
  { f(a, b) } -> std::convertible_to<int>;
};

// Require copyable states
template <typename StateType>
concept AStarState = std::copyable<StateType>;

// A* implementation
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

  struct PQNode {
    int f;
    std::size_t counter;
    StateType state;
  };
  struct Cmp {
    bool operator()(const PQNode& a, const PQNode& b) const {
      if (a.f != b.f) return a.f > b.f;
      return a.counter > b.counter;
    }
  };
  std::priority_queue<PQNode, std::vector<PQNode>, Cmp> open_pq;

  g_score[start] = 0;
  f_score[start] = heuristic(start);
  std::size_t push_counter = 0;
  open_pq.push(PQNode{f_score[start], push_counter++, start});

  while (!open_pq.empty()) {
    PQNode top = open_pq.top();
    open_pq.pop();

    StateType current = std::move(top.state);

    auto it_f = f_score.find(current);
    if (it_f != f_score.end() && top.f > it_f->second) continue;

    if (is_goal(current)) {
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
      int g_current = g_score.contains(current) ? g_score[current] : INF;
      int tentative_g = g_current + cost_between(current, nb);
      int g_nb = g_score.contains(nb) ? g_score[nb] : INF;

      if (tentative_g < g_nb) {
        came_from[nb] = current;
        g_score[nb] = tentative_g;
        int nb_f = tentative_g + heuristic(nb);
        f_score[nb] = nb_f;
        open_pq.push(PQNode{nb_f, push_counter++, nb});
      }
    }
  }

  return std::nullopt;
}
