/**
 * @file abcg_elapsedtimer.cpp
 * @brief Definition of abcg::ElapsedTimer class members.
 *
 * This project is released under the MIT License.
 */

#include "abcg_elapsedtimer.hpp"

using namespace std::chrono;

double abcg::ElapsedTimer::elapsed() const {
  return duration_cast<duration<double>>(steady_clock::now() - start).count();
}

double abcg::ElapsedTimer::restart() {
  const auto now{steady_clock::now()};
  const auto elapsed{duration_cast<duration<double>>(now - start).count()};
  start = now;

  return elapsed;
}