# Adapted from https://github.com/sivachandran/cmake-bin2h by Harlen Batagelo
#
# Original license: sivachandran/cmake-bin2h is licensed under the MIT License
#
# Copyright 2020 Sivachandran Paramasivam
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

include(CMakeParseArguments)

# Function to wrap a given string into multiple lines at the given column
# position.
#
# Parameters:
#
# VARIABLE: The name of the CMake variable holding the string.
#
# AT_COLUMN: The column position at which string will be wrapped.
function(WRAP_STRING)
  set(oneValueArgs VARIABLE AT_COLUMN LINE_PREFIX)
  cmake_parse_arguments(WRAP_STRING "${options}" "${oneValueArgs}" "" ${ARGN})

  string(LENGTH ${${WRAP_STRING_VARIABLE}} stringLength)
  math(EXPR offset "0")

  while(stringLength GREATER 0)

    if(stringLength GREATER ${WRAP_STRING_AT_COLUMN})
      math(EXPR length "${WRAP_STRING_AT_COLUMN}")
    else()
      math(EXPR length "${stringLength}")
    endif()

    string(SUBSTRING ${${WRAP_STRING_VARIABLE}} ${offset} ${length} line)
    set(lines "${lines}\n${WRAP_STRING_LINE_PREFIX}${line}")

    math(EXPR stringLength "${stringLength} - ${length}")
    math(EXPR offset "${offset} + ${length}")

  endwhile()

  set(${WRAP_STRING_VARIABLE}
      "${lines}"
      PARENT_SCOPE)
endfunction()

# Function to embed contents of a file as byte array in C/C++ header file(.h).
# The header file will contain a byte array and integer variable holding the
# size of the array.
#
# Parameters:
#
# SOURCE_FILE: The path of source file whose contents will be embedded in the
# header file.
#
# VARIABLE_NAME:The name of the variable for the byte array. The string "_SIZE"
# will be append to this name and will be used a variable name for size
# variable.
#
# HEADER_FILE: The path of header file.
#
# NULL_TERMINATE: If specified a null byte(zero) will be append to the byte
# array. This will be useful if the source file is a text file and we want to
# use the file contents as string. But the size variable holds size of the byte
# array without this null byte.
#
# Usage:
#
# bin2h(SOURCE_FILE "Logo.png" HEADER_FILE "Logo.h" VARIABLE_NAME "LOGO_PNG")
function(BIN2H)
  set(options NULL_TERMINATE)
  set(oneValueArgs SOURCE_FILE VARIABLE_NAME HEADER_FILE)
  cmake_parse_arguments(BIN2H "${options}" "${oneValueArgs}" "" ${ARGN})

  # reads source file contents as hex string
  file(READ ${BIN2H_SOURCE_FILE} hexString HEX)
  string(LENGTH ${hexString} hexStringLength)

  # appends null byte if asked
  if(BIN2H_NULL_TERMINATE)
    set(hexString "${hexString}00")
  endif()

  # wraps the hex string into multiple lines at column 24(i.e. 12 bytes per
  # line)
  wrap_string(VARIABLE hexString AT_COLUMN 24 LINE_PREFIX "    ")
  math(EXPR arraySize "${hexStringLength} / 2")

  # adds '0x' prefix and comma suffix before and after every byte respectively
  string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " arrayValues ${hexString})
  # removes trailing comma
  string(REGEX REPLACE ", $" "" arrayValues ${arrayValues})

  # converts the variable name into proper C identifier
  string(MAKE_C_IDENTIFIER "${BIN2H_VARIABLE_NAME}" BIN2H_VARIABLE_NAME)
  string(TOUPPER "${BIN2H_VARIABLE_NAME}" BIN2H_VARIABLE_NAME)

  # declares byte array and the length variables
  set(arrayDefinition
      "static std::array<unsigned char, ${arraySize}> const ${BIN2H_VARIABLE_NAME}{ ${arrayValues} };"
  )

  set(declarations "${arrayDefinition}\n")
  file(APPEND ${BIN2H_HEADER_FILE} "${declarations}")
endfunction()
