/**
 * @file abcg_string.hpp
 * @brief Declaration of string manipulation functions.
 *
 * This project is released under the MIT License.
 */

#ifndef ABCG_STRING_HPP_
#define ABCG_STRING_HPP_

#include <string>

namespace abcg {
void leftTrim(std::string &s);
void rightTrim(std::string &s);
void trim(std::string &s);
[[nodiscard]] std::string leftTrimCopy(std::string s);
[[nodiscard]] std::string rightTrimCopy(std::string s);
[[nodiscard]] std::string trimCopy(std::string s);
}  // namespace abcg

#endif