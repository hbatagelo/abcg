/**
 * @file abcgUtil.hpp
 * @brief Declaration of general utility functions.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#ifndef ABCG_UTIL_HPP_
#define ABCG_UTIL_HPP_

#include <functional>
#include <string>

namespace abcg {

/**
 * @brief Creates a hash value from several values, combining them with a seed
 * value.
 *
 * Based on boost::hash_combine. It can be used as follows:
 * @code
 * std::size_t hash{};
 * abcg::hashCombineSeed(hash, Foo{}, 42, "Some text");
 * abcg::hashCombineSeed(hash, 3.14);
 * // Use hash...
 * @endcode
 *
 * @tparam TFirst Typename of the first value.
 * @tparam TRest Typename of the variadic arguments.
 *
 * @param seed Seed value.
 * @param value First value.
 * @param rest Remaining values.
 *
 * @remark The values must have an injected std::hash specialization.
 */
template <typename TFirst, typename... TRest>
void hashCombineSeed(std::size_t &seed, const TFirst &value,
                     const TRest &...rest) {
  seed ^= std::hash<TFirst>{}(value) + 0x9e3779b97f4a7c15 + (seed << 6) +
          (seed >> 2);
  (hashCombineSeed(seed, rest), ...);
}

/**
 * @brief Creates a hash value from several values.
 *
 * This is similar to abcg::hashCombineSeed, but implicitly uses a zero seed. It
 * can be used as follows:
 * @code
 * std::size_t hash{abcg::hashCombine(Foo{}, 42, "Some text", 3.14)};
 * @endcode
 *
 * @tparam TFirst Typename of the first value.
 * @tparam TRest Typename of the variadic arguments.
 *
 * @param value First value.
 * @param rest Remaining values.
 *
 * @remark The values must have an injected std::hash specialization.
 */
template <typename TFirst, typename... TRest>
std::size_t hashCombine(const TFirst &value, const TRest &...rest) {
  std::size_t seed{};
  hashCombineSeed(seed, value, rest...);
  return seed;
}

std::string toRedString(std::string_view str);
std::string toYellowString(std::string_view str);
std::string toBlueString(std::string_view str);

} // namespace abcg

#endif