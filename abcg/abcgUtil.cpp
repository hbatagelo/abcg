/**
 * @file abcgUtil.cpp
 * @brief General utility functions.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2023 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#include "abcgUtil.hpp"

namespace {
auto const codeBoldRed{"\033[1;31m"};
auto const codeBoldYellow{"\033[1;33m"};
auto const codeBoldBlue{"\033[1;34m"};
auto const codeReset{"\033[0m"};
} // namespace

/**
 * @brief Creates a string prefixed with the ANSI color code that corresponds to
 * foreground bold red.
 *
 * The reset color code is appended to the end of the string.
 *
 * @param str View of the input string.
 * @return New string surrounded by the color and reset codes.
 */
std::string abcg::toRedString(std::string_view str) {
  return std::string{codeBoldRed} + str.data() + std::string{codeReset};
}

/**
 * @brief Creates a string prefixed with the ANSI color code that corresponds to
 * foreground bold yellow.
 *
 * The reset color code is appended to the end of the string.
 *
 * @param str View of the input string.
 * @return New string surrounded by the color and reset codes.
 */
std::string abcg::toYellowString(std::string_view str) {
  return std::string{codeBoldYellow} + str.data() + std::string{codeReset};
}

/**
 * @brief Creates a string prefixed with the ANSI color code that corresponds to
 * foreground bold blue.
 *
 * The reset color code is appended to the end of the string.
 *
 * @param str View of the input string.
 * @return New string surrounded by the color and reset codes.
 */
std::string abcg::toBlueString(std::string_view str) {
  return std::string{codeBoldBlue} + str.data() + std::string{codeReset};
}