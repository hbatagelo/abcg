/**
 * @file abcgTimer.cpp
 * @brief Definition of abcg::Timer members.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#include "abcgTimer.hpp"

using namespace std::chrono;

/**
 * @brief Returns how much time has elapsed since the timer has started.
 *
 * @return Time, in seconds, since the timer object was created, or since the
 * last call to abcg::Timer::restart.
 */
double abcg::Timer::elapsed() const {
  return duration_cast<duration<double>>(steady_clock::now() - start).count();
}

/**
 * @brief Restarts the timer and returns the last elapsed time.
 *
 * @return Time, in seconds, since the timer object was created, or since the
 * last call to abcg::Timer::restart.
 */
double abcg::Timer::restart() {
  auto const now{steady_clock::now()};
  auto const elapsed{duration_cast<duration<double>>(now - start).count()};
  start = now;

  return elapsed;
}