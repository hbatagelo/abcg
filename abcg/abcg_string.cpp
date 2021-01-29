/**
 * @file abcg_string.cpp
 * @brief Definition of string manipulation functions.
 *
 * Adapted from
 * https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring.
 *
 * This project is released under the MIT License.
 */

#include "abcg_string.hpp"

#include <cctype>

// Trim from start (in place)
void abcg::leftTrim(std::string &s) {
  auto lambda{[](auto ch) { return std::isspace(ch) == 0; }};
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), lambda));
}

// Trim from end (in place)
void abcg::rightTrim(std::string &s) {
  auto lambda{[](auto ch) { return std::isspace(ch) == 0; }};
  s.erase(std::find_if(s.rbegin(), s.rend(), lambda).base(), s.end());
}

// Trim from both ends (in place)
void abcg::trim(std::string &s) {
  leftTrim(s);
  rightTrim(s);
}

// Trim from start (copying)
std::string abcg::leftTrimCopy(std::string s) {
  leftTrim(s);
  return s;
}

// Trim from end (copying)
std::string abcg::rightTrimCopy(std::string s) {
  rightTrim(s);
  return s;
}

// Trim from both ends (copying)
std::string abcg::trimCopy(std::string s) {
  trim(s);
  return s;
}