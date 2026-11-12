// Copyright 2026 Andreas Burnickl
// SPDX-License-Identifier: GPL-3.0-or-later

/*!
 * \file
 * \brief A pin of the board as an output, its number known when compiling.
 */

#ifndef LEDCUBE_SRC_LEDCUBE_OUTPUTPIN_H_
#define LEDCUBE_SRC_LEDCUBE_OUTPUTPIN_H_

#include <Arduino.h>
#include <stdint.h>

#ifdef __AVR__
#include "ledcube/digitalWriteFast.h"
#endif

namespace ledcube {
namespace internal {

/*!
 * \brief A pin of the board as an output, its number known when compiling.
 *
 * On the AVR it writes the pin's port register directly, through
 * digitalWriteFast, which needs the number of the pin known when compiling;
 * the core's `digitalWrite()` looks the port up in its tables on every call.
 * Elsewhere, as in the tests on the host, it calls the core.
 *
 * \tparam Pin The number of the pin, as the core numbers it.
 */
template <uint8_t Pin>
class OutputPin {
 public:
  /*! \brief Makes the pin an output. */
  // NOLINTNEXTLINE(readability-function-cognitive-complexity): as Write()
  static void Begin() {
#ifdef __AVR__
    // NOLINTNEXTLINE(bugprone-signed-bitwise): as in Write()
    pinModeFast(Pin, OUTPUT);
#else
    pinMode(Pin, OUTPUT);
#endif
  }

  /*!
   * \brief Sets the pin to `level`.
   *
   * \param[in] level `HIGH` or `LOW`.
   */
  // NOLINTNEXTLINE(readability-function-cognitive-complexity): the macro's
  static void Write(uint8_t level) {
#ifdef __AVR__
    /*
     * The complexity and the signed shifts that clang-tidy counts are those of
     * digitalWriteFast's macros, a chain of ?: over the pin number with int
     * operands. They stay, as the pin is a constant and the compiler folds the
     * chain to the one port and bit: the Demo's flash is the same as with the
     * macro called in LedCube directly.
     */
    // NOLINTNEXTLINE(bugprone-signed-bitwise): the macro's, as above
    digitalWriteFast(Pin, level);
#else
    digitalWrite(Pin, level);
#endif
  }
};

}  // namespace internal
}  // namespace ledcube

#endif  // LEDCUBE_SRC_LEDCUBE_OUTPUTPIN_H_
