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
  // stdReturnType shiftCircle(LedCube*, Axis,
  // RotationDirection, uint8_t);
  Cube* pCube;

 public:
  Transformation(Cube* pLedCube);
  ~Transformation();

  LEDCUBE_NODISCARD Error shiftCircle(Axis, RotationDirection, uint8_t);
  void rotate(Axis ShiftAxis, RotationAngle Degrees);
  void mirror(Axis ShiftAxis);
  LEDCUBE_NODISCARD Error rotateWithShift(Axis ShiftAxis,
                                          RotationDirection Direction,
                                          uint8_t Clock);
  void shiftForward(bool = false);
  void shiftBackward(bool = false);
  void shiftUpward(bool = false);
  void shiftDownward(bool = false);
  void shiftLeft(bool = false);
  void shiftRight(bool = false);

  void shiftForwardFast(bool = false);
  void shiftBackwardFast(bool = false);
};

template <typename Cube>
Transformation<Cube>::Transformation(Cube* pLedCube) {
  pCube = pLedCube;
}

template <typename Cube>
Transformation<Cube>::~Transformation() {}

template <typename Cube>
void Transformation<Cube>::shiftForward(bool Rotate) {
  for (uint8_t Z = 0; Z < kLedsPerSide; Z++) {
    for (uint8_t X = 0; X < kLedsPerSide - 1; X++) {
      pCube->setVoxelsYFast(X, Z, pCube->getVoxelsYFast(X + 1, Z));
    }
    if (Rotate)
      pCube->setVoxelsYFast(kLedsPerSide - 1, Z, pCube->getVoxelsYFast(0, Z));
    else
      pCube->setVoxelsYFast(kLedsPerSide - 1, Z, 0);
  }
}

template <typename Cube>
void Transformation<Cube>::shiftBackward(bool Rotate) {
  for (uint8_t Z = 0; Z < kLedsPerSide; Z++) {
    for (uint8_t X = 1; X < kLedsPerSide; X++) {
      pCube->setVoxelsYFast(X, Z, pCube->getVoxelsYFast(X - 1, Z));
    }
    if (Rotate)
      pCube->setVoxelsYFast(0, Z, pCube->getVoxelsYFast(kLedsPerSide - 1, Z));
    else
      pCube->setVoxelsYFast(0, Z, 0);
  }
}

template <typename Cube>
void Transformation<Cube>::shiftUpward(bool Rotate) {
  for (uint8_t Y = 0; Y < kLedsPerSide; Y++) {
    for (uint8_t Z = 1; Z < kLedsPerSide; Z++) {
      pCube->setVoxelsXFast(Y, Z, pCube->getVoxelsXFast(Y, Z - 1));
    }
    if (Rotate)
      pCube->setVoxelsXFast(Y, 0, pCube->getVoxelsXFast(Y, kLedsPerSide - 1));
    else
      pCube->setVoxelsXFast(Y, 0, 0);
  }
}

template <typename Cube>
void Transformation<Cube>::shiftDownward(bool Rotate) {
  for (uint8_t Y = 0; Y < kLedsPerSide - 1; Y++) {
    for (uint8_t Z = 0; Z < kLedsPerSide; Z++) {
      pCube->setVoxelsXFast(Y, Z, pCube->getVoxelsXFast(Y, Z + 1));
    }
    if (Rotate)
      pCube->setVoxelsXFast(Y, kLedsPerSide - 1, pCube->getVoxelsXFast(Y, 0));
    else
      pCube->setVoxelsXFast(Y, kLedsPerSide - 1, 0);
  }
}

template <typename Cube>
void Transformation<Cube>::shiftLeft(bool Rotate) {
  for (uint8_t Z = 0; Z < kLedsPerSide; Z++) {
    for (uint8_t Y = 0; Y < kLedsPerSide - 1; Y++) {
      pCube->setVoxelsXFast(Y, Z, pCube->getVoxelsXFast(Y + 1, Z));
    }
    if (Rotate)
      pCube->setVoxelsXFast(kLedsPerSide - 1, Z, pCube->getVoxelsXFast(0, Z));
    else
      pCube->setVoxelsXFast(kLedsPerSide - 1, Z, 0);
  }
}

template <typename Cube>
void Transformation<Cube>::shiftRight(bool Rotate) {
  for (uint8_t Z = 0; Z < kLedsPerSide; Z++) {
    for (uint8_t Y = 1; Y < kLedsPerSide; Y++) {
      pCube->setVoxelsXFast(Y, Z, pCube->getVoxelsXFast(Y - 1, Z));
    }
    if (Rotate)
      pCube->setVoxelsXFast(0, Z, pCube->getVoxelsXFast(kLedsPerSide - 1, Z));
    else
      pCube->setVoxelsXFast(0, Z, 0);
  }
}

template <typename Cube>
void Transformation<Cube>::shiftForwardFast(bool Rotate) {
  for (uint8_t Z = 0; Z < kLedsPerSide; Z++) {
    for (uint8_t Y = 0; Y < kLedsPerSide; Y++) {
      pCube->setVoxelsXFast(Y, Z, pCube->getVoxelsXFast(Y, Z) >> 1);
      if (Rotate)
        pCube->writeVoxelFast(kLedsPerSide - 1, Y, Z,
                              pCube->getVoxelFast(0, Y, Z));
    }
  }
}

template <typename Cube>
void Transformation<Cube>::shiftBackwardFast(bool Rotate) {
  for (uint8_t Z = 0; Z < kLedsPerSide; Z++) {
    for (uint8_t Y = 0; Y < kLedsPerSide; Y++) {
      pCube->setVoxelsXFast(Y, Z, pCube->getVoxelsXFast(Y, Z) << 1);
      if (Rotate)
        pCube->writeVoxelFast(0, Y, Z,
                              pCube->getVoxelFast(kLedsPerSide - 1, Y, Z));
    }
  }
}

template <typename Cube>
Error Transformation<Cube>::shiftCircle(Axis ShiftAxis,
                                        RotationDirection Direction,
                                        uint8_t Circle) {
  uint8_t const LEDs = kLedsPerSide;

  if (Circle > 0 && Circle <= LEDs / 2) {
    Circle--;
    if (RotationDirection::kClockwise == Direction) {
      if (Axis::kZ == ShiftAxis) {
        for (uint8_t Z = 0; Z < LEDs; Z++) {
          /* shift front, right, back, left */
          for (int8_t Y = Circle; Y < LEDs - 1 - Circle; Y++) {
            pCube->writeVoxelFast(Circle, Y, Z,
                                  pCube->getVoxelFast(Circle, Y + 1, Z));
          }
          for (int8_t X = Circle; X < LEDs - 1 - Circle; X++) {
            pCube->writeVoxelFast(
                X, LEDs - 1 - Circle, Z,
                pCube->getVoxelFast(X + 1, LEDs - 1 - Circle, Z));
          }
          for (int8_t Y = LEDs - 1 - Circle; Y > Circle; Y--) {
            pCube->writeVoxelFast(
                LEDs - 1 - Circle, Y, Z,
                pCube->getVoxelFast(LEDs - 1 - Circle, Y - 1, Z));
          }
          for (int8_t X = LEDs - 1 - Circle; X > Circle; X--) {
            pCube->writeVoxelFast(X, Circle, Z,
                                  pCube->getVoxelFast(X - 1, Circle, Z));
          }
        }
      }
    }
    if (RotationDirection::kCounterclockwise == Direction) {
      if (Axis::kZ == ShiftAxis) {
        for (uint8_t Z = 0; Z < LEDs; Z++) {
          /* shift left, back, right, front */
          for (int8_t X = Circle; X < LEDs - 1 - Circle; X++) {
            pCube->writeVoxelFast(X, Circle, Z,
                                  pCube->getVoxelFast(X + 1, Circle, Z));
          }
          for (int8_t Y = Circle; Y < LEDs - 1 - Circle; Y++) {
            pCube->writeVoxelFast(
                LEDs - 1 - Circle, Y, Z,
                pCube->getVoxelFast(LEDs - 1 - Circle, Y + 1, Z));
          }
          for (int8_t X = LEDs - 1 - Circle; X > Circle; X--) {
            pCube->writeVoxelFast(
                X, LEDs - 1 - Circle, Z,
                pCube->getVoxelFast(X - 1, LEDs - 1 - Circle, Z));
          }
          for (int8_t Y = LEDs - 1 - Circle; Y > Circle; Y--) {
            pCube->writeVoxelFast(Circle, Y, Z,
                                  pCube->getVoxelFast(Circle, Y - 1, Z));
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
