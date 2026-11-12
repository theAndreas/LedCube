// Copyright 2017 Andreas Burnickl
// SPDX-License-Identifier: GPL-3.0-or-later

/*!
 * \file
 * \brief Transformation, which shifts and turns the voxels of a LedCube.
 */

#ifndef _TRANSFORMATION_H_
#define _TRANSFORMATION_H_

#include <stdint.h>

#include "Arduino.h"
#include "LedCube.h"
#include "ledcube/Error.h"
#include "ledcube/attributes.h"

namespace ledcube {

/*! \brief An angle to turn the cube by. */
enum class RotationAngle : uint8_t {
  kDegrees0,   /*!< Not at all. */
  kDegrees90,  /*!< A quarter turn. */
  kDegrees180, /*!< A half turn. */
  kDegrees270  /*!< Three quarters of a turn. */
};

/*! \brief The direction to turn in, looking along the axis. */
enum class RotationDirection : uint8_t {
  kClockwise,       /*!< Clockwise. */
  kCounterclockwise /*!< Counterclockwise. */
};

/*!
 * \brief Shifts and turns the voxels of a cube.
 *
 * \code{.cpp}
 * ledcube::LedCube<13, 11, 12> cube;
 * ledcube::Transformation<decltype(cube)> transformation(&cube);
 * \endcode
 *
 * \tparam Cube The type of the cube, a `LedCube`.
 */
template <typename Cube>
class Transformation {
 private:
  // stdReturnType ShiftCircle(LedCube*, Axis,
  // RotationDirection, uint8_t);
  Cube* cube_;

 public:
  Transformation(Cube* cube);
  ~Transformation();

  LEDCUBE_NODISCARD Error ShiftCircle(Axis, RotationDirection, uint8_t);
  void Rotate(Axis axis, RotationAngle degrees);
  void Mirror(Axis axis);
  LEDCUBE_NODISCARD Error RotateWithShift(Axis axis,
                                          RotationDirection direction,
                                          uint8_t clock);
  void ShiftForward(bool = false);
  void ShiftBackward(bool = false);
  void ShiftUpward(bool = false);
  void ShiftDownward(bool = false);
  void ShiftLeft(bool = false);
  void ShiftRight(bool = false);

  void ShiftForwardFast(bool = false);
  void ShiftBackwardFast(bool = false);
};

template <typename Cube>
Transformation<Cube>::Transformation(Cube* cube) {
  cube_ = cube;
}

template <typename Cube>
Transformation<Cube>::~Transformation() {}

template <typename Cube>
void Transformation<Cube>::ShiftForward(bool rotate) {
  for (uint8_t z = 0; z < kLedsPerSide; z++) {
    for (uint8_t x = 0; x < kLedsPerSide - 1; x++) {
      cube_->SetVoxelsYFast(x, z, cube_->GetVoxelsYFast(x + 1, z));
    }
    if (rotate)
      cube_->SetVoxelsYFast(kLedsPerSide - 1, z, cube_->GetVoxelsYFast(0, z));
    else
      cube_->SetVoxelsYFast(kLedsPerSide - 1, z, 0);
  }
}

template <typename Cube>
void Transformation<Cube>::ShiftBackward(bool rotate) {
  for (uint8_t z = 0; z < kLedsPerSide; z++) {
    for (uint8_t x = 1; x < kLedsPerSide; x++) {
      cube_->SetVoxelsYFast(x, z, cube_->GetVoxelsYFast(x - 1, z));
    }
    if (rotate)
      cube_->SetVoxelsYFast(0, z, cube_->GetVoxelsYFast(kLedsPerSide - 1, z));
    else
      cube_->SetVoxelsYFast(0, z, 0);
  }
}

template <typename Cube>
void Transformation<Cube>::ShiftUpward(bool rotate) {
  for (uint8_t y = 0; y < kLedsPerSide; y++) {
    for (uint8_t z = 1; z < kLedsPerSide; z++) {
      cube_->SetVoxelsXFast(y, z, cube_->GetVoxelsXFast(y, z - 1));
    }
    if (rotate)
      cube_->SetVoxelsXFast(y, 0, cube_->GetVoxelsXFast(y, kLedsPerSide - 1));
    else
      cube_->SetVoxelsXFast(y, 0, 0);
  }
}

template <typename Cube>
void Transformation<Cube>::ShiftDownward(bool rotate) {
  for (uint8_t y = 0; y < kLedsPerSide - 1; y++) {
    for (uint8_t z = 0; z < kLedsPerSide; z++) {
      cube_->SetVoxelsXFast(y, z, cube_->GetVoxelsXFast(y, z + 1));
    }
    if (rotate)
      cube_->SetVoxelsXFast(y, kLedsPerSide - 1, cube_->GetVoxelsXFast(y, 0));
    else
      cube_->SetVoxelsXFast(y, kLedsPerSide - 1, 0);
  }
}

template <typename Cube>
void Transformation<Cube>::ShiftLeft(bool rotate) {
  for (uint8_t z = 0; z < kLedsPerSide; z++) {
    for (uint8_t y = 0; y < kLedsPerSide - 1; y++) {
      cube_->SetVoxelsXFast(y, z, cube_->GetVoxelsXFast(y + 1, z));
    }
    if (rotate)
      cube_->SetVoxelsXFast(kLedsPerSide - 1, z, cube_->GetVoxelsXFast(0, z));
    else
      cube_->SetVoxelsXFast(kLedsPerSide - 1, z, 0);
  }
}

template <typename Cube>
void Transformation<Cube>::ShiftRight(bool rotate) {
  for (uint8_t z = 0; z < kLedsPerSide; z++) {
    for (uint8_t y = 1; y < kLedsPerSide; y++) {
      cube_->SetVoxelsXFast(y, z, cube_->GetVoxelsXFast(y - 1, z));
    }
    if (rotate)
      cube_->SetVoxelsXFast(0, z, cube_->GetVoxelsXFast(kLedsPerSide - 1, z));
    else
      cube_->SetVoxelsXFast(0, z, 0);
  }
}

template <typename Cube>
void Transformation<Cube>::ShiftForwardFast(bool rotate) {
  for (uint8_t z = 0; z < kLedsPerSide; z++) {
    for (uint8_t y = 0; y < kLedsPerSide; y++) {
      cube_->SetVoxelsXFast(y, z, cube_->GetVoxelsXFast(y, z) >> 1);
      if (rotate)
        cube_->WriteVoxelFast(kLedsPerSide - 1, y, z,
                              cube_->GetVoxelFast(0, y, z));
    }
  }
}

template <typename Cube>
void Transformation<Cube>::ShiftBackwardFast(bool rotate) {
  for (uint8_t z = 0; z < kLedsPerSide; z++) {
    for (uint8_t y = 0; y < kLedsPerSide; y++) {
      cube_->SetVoxelsXFast(y, z, cube_->GetVoxelsXFast(y, z) << 1);
      if (rotate)
        cube_->WriteVoxelFast(0, y, z,
                              cube_->GetVoxelFast(kLedsPerSide - 1, y, z));
    }
  }
}

template <typename Cube>
Error Transformation<Cube>::ShiftCircle(Axis axis, RotationDirection direction,
                                        uint8_t circle) {
  uint8_t const leds = kLedsPerSide;

  if (circle > 0 && circle <= leds / 2) {
    circle--;
    if (RotationDirection::kClockwise == direction) {
      if (Axis::kZ == axis) {
        for (uint8_t z = 0; z < leds; z++) {
          /* shift front, right, back, left */
          for (int8_t y = circle; y < leds - 1 - circle; y++) {
            cube_->WriteVoxelFast(circle, y, z,
                                  cube_->GetVoxelFast(circle, y + 1, z));
          }
          for (int8_t x = circle; x < leds - 1 - circle; x++) {
            cube_->WriteVoxelFast(
                x, leds - 1 - circle, z,
                cube_->GetVoxelFast(x + 1, leds - 1 - circle, z));
          }
          for (int8_t y = leds - 1 - circle; y > circle; y--) {
            cube_->WriteVoxelFast(
                leds - 1 - circle, y, z,
                cube_->GetVoxelFast(leds - 1 - circle, y - 1, z));
          }
          for (int8_t x = leds - 1 - circle; x > circle; x--) {
            cube_->WriteVoxelFast(x, circle, z,
                                  cube_->GetVoxelFast(x - 1, circle, z));
          }
        }
      }
    }
    if (RotationDirection::kCounterclockwise == direction) {
      if (Axis::kZ == axis) {
        for (uint8_t z = 0; z < leds; z++) {
          /* shift left, back, right, front */
          for (int8_t x = circle; x < leds - 1 - circle; x++) {
            cube_->WriteVoxelFast(x, circle, z,
                                  cube_->GetVoxelFast(x + 1, circle, z));
          }
          for (int8_t y = circle; y < leds - 1 - circle; y++) {
            cube_->WriteVoxelFast(
                leds - 1 - circle, y, z,
                cube_->GetVoxelFast(leds - 1 - circle, y + 1, z));
          }
          for (int8_t x = leds - 1 - circle; x > circle; x--) {
            cube_->WriteVoxelFast(
                x, leds - 1 - circle, z,
                cube_->GetVoxelFast(x - 1, leds - 1 - circle, z));
          }
          for (int8_t y = leds - 1 - circle; y > circle; y--) {
            cube_->WriteVoxelFast(circle, y, z,
                                  cube_->GetVoxelFast(circle, y - 1, z));
          }
        }
      }
    }
    return Error::kNone;
  } else {
    return Error::kOutOfRange;
  }
}

}  // namespace ledcube

#endif
