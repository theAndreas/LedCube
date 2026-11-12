// Copyright 2026 Andreas Burnickl
// SPDX-License-Identifier: GPL-3.0-or-later

/*!
 * \file
 * \brief Error, what a call of the library returns.
 */

#ifndef LEDCUBE_SRC_LEDCUBE_ERROR_H_
#define LEDCUBE_SRC_LEDCUBE_ERROR_H_

#include <stdint.h>

namespace ledcube {

/*!
 * \brief What went wrong in a call, or kNone.
 *
 * A function that returns it is marked `LEDCUBE_NODISCARD`: a call that
 * drops it is a warning.
 */
enum class Error : uint8_t {
  kNone,       /*!< Nothing: the call did what it should. */
  kOutOfRange, /*!< A coordinate or a circle lies beyond the cube. */
  kWrongState  /*!< The state forbids the call, as Start() before Begin(). */
};

}  // namespace ledcube

#endif  // LEDCUBE_SRC_LEDCUBE_ERROR_H_
