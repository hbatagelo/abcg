/**
 * @file abcgTimer.hpp
 * @brief Header file of abcg::Timer.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_TIMER_HPP_
#define ABCG_TIMER_HPP_

#include <chrono>

namespace abcg {
class Timer;
} // namespace abcg

/**
 * @brief Represents a timer based on the monotonic clock
 * `std::chrono::steady_clock`.
 */
class abcg::Timer {
public:
  [[nodiscard]] double elapsed() const;
  double restart();

private:
  using clock = std::chrono::steady_clock;

  clock::time_point start{clock::now()};
};

#endif