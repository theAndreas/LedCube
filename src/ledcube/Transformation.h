// Copyright 2017 Andreas Burnickl
// SPDX-License-Identifier: GPL-3.0-or-later

/*!
 * \file
 * \brief Transformation, which shifts and turns the voxels of a LedCube.
 */

#ifndef _TRANSFORMATION_H_
#define _TRANSFORMATION_H_

#include "Arduino.h"
#include "LedCube.h"
#include "StandardTypes.h"

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
  // RotationDirection, byte);
  Cube* pCube;

 public:
  Transformation(Cube* pLedCube);
  ~Transformation();

  stdReturnType shiftCircle(Axis, RotationDirection, byte);
  void rotate(Axis ShiftAxis, RotationAngle Degrees);
  void mirror(Axis ShiftAxis);
  stdReturnType rotateWithShift(Axis ShiftAxis, RotationDirection Direction,
                                byte Clock);
  void shiftForward(boolean = false);
  void shiftBackward(boolean = false);
  void shiftUpward(boolean = false);
  void shiftDownward(boolean = false);
  void shiftLeft(boolean = false);
  void shiftRight(boolean = false);

  void shiftForwardFast(boolean = false);
  void shiftBackwardFast(boolean = false);
};

template <typename Cube>
Transformation<Cube>::Transformation(Cube* pLedCube) {
  pCube = pLedCube;
}

template <typename Cube>
Transformation<Cube>::~Transformation() {}

template <typename Cube>
void Transformation<Cube>::shiftForward(boolean Rotate) {
  for (byte Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
    for (byte X = 0; X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1; X++) {
      pCube->setVoxelsYFast(X, Z, pCube->getVoxelsYFast(X + 1, Z));
    }
    if (Rotate)
      pCube->setVoxelsYFast(LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1, Z,
                            pCube->getVoxelsYFast(0, Z));
    else
      pCube->setVoxelsYFast(LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1, Z, 0);
  }
}

template <typename Cube>
void Transformation<Cube>::shiftBackward(boolean Rotate) {
  for (byte Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
    for (byte X = 1; X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; X++) {
      pCube->setVoxelsYFast(X, Z, pCube->getVoxelsYFast(X - 1, Z));
    }
    if (Rotate)
      pCube->setVoxelsYFast(
          0, Z, pCube->getVoxelsYFast(LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1, Z));
    else
      pCube->setVoxelsYFast(0, Z, 0);
  }
}

template <typename Cube>
void Transformation<Cube>::shiftUpward(boolean Rotate) {
  for (byte Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Y++) {
    for (byte Z = 1; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
      pCube->setVoxelsXFast(Y, Z, pCube->getVoxelsXFast(Y, Z - 1));
    }
    if (Rotate)
      pCube->setVoxelsXFast(
          Y, 0, pCube->getVoxelsXFast(Y, LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1));
    else
      pCube->setVoxelsXFast(Y, 0, 0);
  }
}

template <typename Cube>
void Transformation<Cube>::shiftDownward(boolean Rotate) {
  for (byte Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1; Y++) {
    for (byte Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
      pCube->setVoxelsXFast(Y, Z, pCube->getVoxelsXFast(Y, Z + 1));
    }
    if (Rotate)
      pCube->setVoxelsXFast(Y, LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1,
                            pCube->getVoxelsXFast(Y, 0));
    else
      pCube->setVoxelsXFast(Y, LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1, 0);
  }
}

template <typename Cube>
void Transformation<Cube>::shiftLeft(boolean Rotate) {
  for (byte Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
    for (byte Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1; Y++) {
      pCube->setVoxelsXFast(Y, Z, pCube->getVoxelsXFast(Y + 1, Z));
    }
    if (Rotate)
      pCube->setVoxelsXFast(LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1, Z,
                            pCube->getVoxelsXFast(0, Z));
    else
      pCube->setVoxelsXFast(LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1, Z, 0);
  }
}

template <typename Cube>
void Transformation<Cube>::shiftRight(boolean Rotate) {
  for (byte Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
    for (byte Y = 1; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Y++) {
      pCube->setVoxelsXFast(Y, Z, pCube->getVoxelsXFast(Y - 1, Z));
    }
    if (Rotate)
      pCube->setVoxelsXFast(
          0, Z, pCube->getVoxelsXFast(LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1, Z));
    else
      pCube->setVoxelsXFast(0, Z, 0);
  }
}

template <typename Cube>
void Transformation<Cube>::shiftForwardFast(boolean Rotate) {
  for (byte Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
    for (byte Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Y++) {
      pCube->setVoxelsXFast(Y, Z, pCube->getVoxelsXFast(Y, Z) >> 1);
      if (Rotate)
        pCube->writeVoxelFast(LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1, Y, Z,
                              pCube->getVoxelFast(0, Y, Z));
    }
  }
}

template <typename Cube>
void Transformation<Cube>::shiftBackwardFast(boolean Rotate) {
  for (byte Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
    for (byte Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Y++) {
      pCube->setVoxelsXFast(Y, Z, pCube->getVoxelsXFast(Y, Z) << 1);
      if (Rotate)
        pCube->writeVoxelFast(
            0, Y, Z,
            pCube->getVoxelFast(LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1, Y, Z));
    }
  }
}

template <typename Cube>
stdReturnType Transformation<Cube>::shiftCircle(Axis ShiftAxis,
                                                RotationDirection Direction,
                                                byte Circle) {
  byte const LEDs = LEDCUBE_NUMBER_OF_LEDS_PER_SIDE;

  if (Circle > 0 && Circle <= LEDs / 2) {
    Circle--;
    if (RotationDirection::kClockwise == Direction) {
      if (Axis::kZ == ShiftAxis) {
        for (byte Z = 0; Z < LEDs; Z++) {
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
        for (byte Z = 0; Z < LEDs; Z++) {
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
    return E_OK;
  } else {
    return E_NOT_OK;
  }
}

}  // namespace ledcube

#endif
