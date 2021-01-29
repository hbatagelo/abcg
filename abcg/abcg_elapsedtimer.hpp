/**
 * @file abcg_elapsedtimer.hpp
 * @brief abcg::ElapsedTimer header file.
 *
 * Declaration of abcg::ElapsedTimer class.
 *
 * This project is released under the MIT License.
 */

#ifndef ABCG_ELAPSEDTIMER_HPP_
#define ABCG_ELAPSEDTIMER_HPP_

#include <chrono>

namespace abcg {
class ElapsedTimer;
}  // namespace abcg

/**
 * @brief abcg::ElapsedTimer class.
 *
 */
class abcg::ElapsedTimer {
 public:
  [[nodiscard]] double elapsed() const;
  double restart();

 private:
  using clock = std::chrono::steady_clock;

  clock::time_point start{clock::now()};
};

#endif