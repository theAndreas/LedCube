// Copyright 2017 Andreas Burnickl
// SPDX-License-Identifier: GPL-3.0-or-later

/*!
 * \file
 * \brief LedCube, which drives an 8x8x8 LED cube through 74HC595 shift
 *        registers.
 */

#ifndef _LEDCUBE_H_
#define _LEDCUBE_H_

#include <stdint.h>
#include <string.h>

#include "Arduino.h"
#include "StandardTypes.h"
#include "ledcube/OutputPin.h"

/* LedCube Configuration Parameter */
#define LEDCUBE_NUMBER_OF_LEDS_PER_SIDE 8
#define LEDCUBE_NUMBER_OF_LAYERS LEDCUBE_NUMBER_OF_LEDS_PER_SIDE
#define LEDCUBE_NUMBER_OF_SHIFT_REGISTERS 9
#define LEDCUBE_SHIFT_REGISTER_BITWIDTH 8

namespace ledcube {

/*! \brief The state of a LedCube, which init(), start() and stop() change. */
enum class CubeState : uint8_t {
  kNone,    /*!< Made, init() not called yet. */
  kInit,    /*!< In init(). */
  kReady,   /*!< After init(): ready to start(). */
  kRunning, /*!< After start(): task() shows the frame. */
  kStopped  /*!< After stop(): task() shows nothing. */
};

/*! \brief An axis of the cube. */
enum class Axis : uint8_t {
  kX, /*!< Within a row, the bits of a byte of the frame. */
  kY, /*!< Across the rows of a layer. */
  kZ  /*!< Across the layers. */
};

/* Type which defines the buffer with all led states */
using BufferType =
    uint8_t[LEDCUBE_NUMBER_OF_LEDS_PER_SIDE][LEDCUBE_NUMBER_OF_LEDS_PER_SIDE];
/* Type which points to the buffer for the next or current frame */
using pFrameType = uint8_t (*)[LEDCUBE_NUMBER_OF_LEDS_PER_SIDE];

/*!
 * \brief Drives an 8x8x8 LED cube through a chain of 74HC595 shift registers.
 *
 * The pins are known when compiling, so that on the AVR each write to them is
 * a write to the port register, not a call of the core's `digitalWrite()`.
 *
 * \code{.cpp}
 * ledcube::LedCube<13, 11, 12> cube;  // data, clock, storage
 * \endcode
 *
 * \tparam DataPin The pin at the serial input (SER) of the first 74HC595.
 * \tparam ClockPin The pin at the shift clock (SRCLK) of every 74HC595.
 * \tparam StoragePin The pin at the storage clock (RCLK) of every 74HC595.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
class LedCube {
 private:
  using DataOutput = ledcube::internal::OutputPin<DataPin>;
  using ClockOutput = ledcube::internal::OutputPin<ClockPin>;
  using StorageOutput = ledcube::internal::OutputPin<StoragePin>;

  uint8_t CurrentLayer;
  CubeState State;
  pFrameType CurrentFrame;
  pFrameType NextFrame;
  BufferType Buffer1;
  BufferType Buffer2;
  bool NextFrameReady;
  bool NextFrameTimeElapsed;

  // functions
  void clearCube();
  stdReturnType showLayer(uint8_t);
  void sendData(uint8_t Data);
  void switchBufferPointer();
  stdReturnType setLayer();
  stdReturnType showNextFrame();

 public:
  LedCube();
  ~LedCube();

  // get methods
  uint8_t getCurrentLayer() { return CurrentLayer; }
  CubeState getState() { return State; }
  bool getNextFrameReady() { return NextFrameReady; }
  bool getNextFrameTimeElapsed() { return NextFrameTimeElapsed; }

  // set methods
  stdReturnType setNextFrameReady();
  stdReturnType setNextFrameTimeElapsed();

  // methods
  void init();
  stdReturnType start();
  stdReturnType stop();
  void task();

  stdReturnType setVoxel(uint8_t, uint8_t, uint8_t);
  stdReturnType clearVoxel(uint8_t, uint8_t, uint8_t);
  stdReturnType writeVoxel(uint8_t, uint8_t, uint8_t, bool);
  stdReturnType getVoxel(uint8_t, uint8_t, uint8_t, bool*);

  void setVoxelFast(uint8_t X, uint8_t Y, uint8_t Z) {
    bitSet(NextFrame[Y][Z], X);
  }
  void clearVoxelFast(uint8_t X, uint8_t Y, uint8_t Z) {
    bitClear(NextFrame[Y][Z], X);
  }
  void writeVoxelFast(uint8_t X, uint8_t Y, uint8_t Z, bool Value) {
    bitWrite(NextFrame[Y][Z], X, Value);
  }
  uint8_t getVoxelFast(uint8_t X, uint8_t Y, uint8_t Z) {
    return bitRead(CurrentFrame[Y][Z], X);
  }

  stdReturnType getVoxelsX(uint8_t, uint8_t, uint8_t*);
  stdReturnType setVoxelsX(uint8_t, uint8_t, uint8_t);
  stdReturnType getVoxelsY(uint8_t, uint8_t, uint8_t*);
  stdReturnType setVoxelsY(uint8_t, uint8_t, uint8_t);
  stdReturnType getVoxelsZ(uint8_t, uint8_t, uint8_t*);
  stdReturnType setVoxelsZ(uint8_t, uint8_t, uint8_t);

  uint8_t getVoxelsXFast(uint8_t, uint8_t);
  void setVoxelsXFast(uint8_t, uint8_t, uint8_t);
  uint8_t getVoxelsYFast(uint8_t, uint8_t);
  void setVoxelsYFast(uint8_t, uint8_t, uint8_t);
  uint8_t getVoxelsZFast(uint8_t, uint8_t);
  void setVoxelsZFast(uint8_t, uint8_t, uint8_t);

  void clearVoxels() { memset(NextFrame, 0, sizeof(BufferType)); }
  void setVoxels() { memset(NextFrame, 255, sizeof(BufferType)); }

  void copyCurrentFrameToNextFrame() {
    memcpy(NextFrame, CurrentFrame, sizeof(BufferType));
  }
};

/*!
 * \brief LedCube Constructor.
 *
 * Instantiation of the LedCube library.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
LedCube<DataPin, ClockPin, StoragePin>::LedCube() {
  CurrentLayer = 0;
  State = CubeState::kNone;
  NextFrameReady = false, NextFrameTimeElapsed = false;
  CurrentFrame = Buffer1;
  NextFrame = Buffer2;
  memset(Buffer1, 0, sizeof(Buffer1));
  memset(Buffer2, 0, sizeof(Buffer2));
}

template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
LedCube<DataPin, ClockPin, StoragePin>::~LedCube() {}

/*!
 * \brief Initialization of the Led Cube hardware.
 *
 * This function initializes the IO hardware and the shift registers 74HC595.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::init() {
  State = CubeState::kInit;
  clearCube();
  DataOutput::Begin();
  ClockOutput::Begin();
  StorageOutput::Begin();
  State = CubeState::kReady;
}

/*!
 * \brief Starts the led cube showing a frame.
 *
 * This function starts with showing the current buffer on the led cube
 * therefore the task function has to be called periodically.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::start() {
  bool const startable{State == CubeState::kReady ||
                       State == CubeState::kStopped};
  if (startable) {
    State = CubeState::kRunning;
    return E_OK;
  }
  return E_NOT_OK;
}

/*!
 * \brief Stops the led cube showing a frame.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::stop() {
  if (State == CubeState::kRunning) {
    State = CubeState::kStopped;
    return E_OK;
  }
  return E_NOT_OK;
}

/*!
 * \brief Show layer which is the next in line.
 *
 * This function shows one layer of the current frame on the cube has to be
 * called periodically.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::task() {
  if (State == CubeState::kRunning) {
    if (CurrentLayer >= LEDCUBE_NUMBER_OF_LAYERS) {
      CurrentLayer = 0;
      /* is next frame ready to show? */
      if (NextFrameReady && NextFrameTimeElapsed) {
        showNextFrame();
        NextFrameReady = false;
        NextFrameTimeElapsed = false;
      }
    }
    showLayer(CurrentLayer);
    CurrentLayer++;
  }
}

/*!
 * \brief Clear all LEDs on the cube.
 *
 * This function initializes the shift registers with zero.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::clearCube() {
  DataOutput::Write(LOW);

  for (uint8_t i = 0; i < (LEDCUBE_NUMBER_OF_SHIFT_REGISTERS *
                           LEDCUBE_SHIFT_REGISTER_BITWIDTH);
       i++) {
    ClockOutput::Write(HIGH);
    ClockOutput::Write(LOW);
  }
  StorageOutput::Write(HIGH);
  StorageOutput::Write(LOW);
}

/*!
 * \brief Set a given voxel in the frame buffer.
 *
 * \param[in] X X-axis coordinate.
 * \param[in] Y Y-axis coordinate.
 * \param[in] Z Z-axis coordinate.
 * \return E_OK or E_NOT_OK.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::setVoxel(uint8_t X,
                                                               uint8_t Y,
                                                               uint8_t Z) {
  if (X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE &&
      Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE &&
      Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE) {
    bitSet(NextFrame[Y][Z], X);
    return E_OK;
  } else {
    return E_NOT_OK;
  }
}

/*!
 * \brief Clear a given voxel in the frame buffer.
 *
 * \param[in] X X-axis coordinate.
 * \param[in] Y Y-axis coordinate.
 * \param[in] Z Z-axis coordinate.
 * \return E_OK or E_NOT_OK.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::clearVoxel(uint8_t X,
                                                                 uint8_t Y,
                                                                 uint8_t Z) {
  if (X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE &&
      Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE &&
      Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE) {
    bitClear(NextFrame[Y][Z], X);
    return E_OK;
  } else {
    return E_NOT_OK;
  }
}

/*!
 * \brief Set given voxel to a given value in the frame buffer.
 *
 * \param[in] X X-axis coordinate.
 * \param[in] Y Y-axis coordinate.
 * \param[in] Z Z-axis coordinate.
 * \param[in] Value Voxel of value on or off.
 * \return E_OK or E_NOT_OK.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::writeVoxel(uint8_t X,
                                                                 uint8_t Y,
                                                                 uint8_t Z,
                                                                 bool Value) {
  if (X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE &&
      Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE &&
      Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE) {
    bitWrite(NextFrame[Y][Z], X, Value);
    return E_OK;
  } else {
    return E_NOT_OK;
  }
}

/*!
 * \brief Get a given voxel of the frame buffer.
 *
 * \param[in] X X-axis coordinate.
 * \param[in] Y Y-axis coordinate.
 * \param[in] Z Z-axis coordinate.
 * \param[out] Value Voxel of value on or off.
 * \return E_OK or E_NOT_OK.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::getVoxel(uint8_t X,
                                                               uint8_t Y,
                                                               uint8_t Z,
                                                               bool* Value) {
  if (X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE &&
      Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE &&
      Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE) {
    *Value = bitRead(CurrentFrame[Y][Z], X);
    return E_OK;
  } else {
    return E_NOT_OK;
  }
}

/*!
 * \brief Get a all voxels from the x-axis of the frame buffer.
 *
 * \param[in] Y Y-axis coordinate.
 * \param[in] Z Z-axis coordinate.
 * \param[out] Value Voxels with value on or off.
 * \return E_OK or E_NOT_OK.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::getVoxelsX(
    uint8_t Y, uint8_t Z, uint8_t* Value) {
  if (Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE &&
      Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE) {
    *Value = CurrentFrame[Y][Z];
    return E_OK;
  } else {
    return E_NOT_OK;
  }
}

/*!
 * \brief Set all voxels from the x-axis in the frame buffer with a given value.
 *
 * \param[in] Y Y-axis coordinate.
 * \param[in] Z Z-axis coordinate.
 * \param[in] Value Voxels with value on or off.
 * \return E_OK or E_NOT_OK.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::setVoxelsX(
    uint8_t Y, uint8_t Z, uint8_t Value) {
  if (Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE &&
      Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE) {
    NextFrame[Y][Z] = Value;
    return E_OK;
  } else {
    return E_NOT_OK;
  }
}

/*!
 * \brief Get a all voxels from the y-axis of the frame buffer.
 *
 * \param[in] X X-axis coordinate.
 * \param[in] Z Z-axis coordinate.
 * \param[out] Value Voxels with value on or off.
 * \return E_OK or E_NOT_OK.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::getVoxelsY(
    uint8_t X, uint8_t Z, uint8_t* Value) {
  if (X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE &&
      Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE) {
    for (uint8_t Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Y++) {
      bitWrite(*Value, Y, bitRead(CurrentFrame[Y][Z], X));
    }
    return E_OK;
  } else {
    return E_NOT_OK;
  }
}

/*!
 * \brief Set all voxels from the y-axis in the frame buffer with a given value.
 *
 * \param[in] X X-axis coordinate.
 * \param[in] Z Z-axis coordinate.
 * \param[in] Value Voxels with value on or off.
 * \return E_OK or E_NOT_OK.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::setVoxelsY(
    uint8_t X, uint8_t Z, uint8_t Value) {
  if (X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE &&
      Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE) {
    for (uint8_t Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Y++) {
      bitWrite(NextFrame[Y][Z], X, bitRead(Value, Y));
    }
    return E_OK;
  } else {
    return E_NOT_OK;
  }
}

/*!
 * \brief Get a all voxels from the z-axis of the frame buffer.
 *
 * \param[in] X X-axis coordinate.
 * \param[in] Y Y-axis coordinate.
 * \param[out] Value Voxels with value on or off.
 * \return E_OK or E_NOT_OK.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::getVoxelsZ(
    uint8_t X, uint8_t Y, uint8_t* Value) {
  if (X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE &&
      Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE) {
    for (uint8_t Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
      bitWrite(*Value, Z, bitRead(CurrentFrame[Y][Z], X));
    }
    return E_OK;
  } else {
    return E_NOT_OK;
  }
}

/*!
 * \brief Set all voxels from the z-axis in the frame buffer with a given value.
 *
 * \param[in] X X-axis coordinate.
 * \param[in] Y Y-axis coordinate.
 * \param[in] Value Voxels with value on or off.
 * \return E_OK or E_NOT_OK.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::setVoxelsZ(
    uint8_t X, uint8_t Y, uint8_t Value) {
  if (X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE &&
      Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE) {
    for (uint8_t Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
      bitWrite(NextFrame[Y][Z], X, bitRead(Value, Z));
    }
    return E_OK;
  } else {
    return E_NOT_OK;
  }
}

/*!
 * \brief Get a all voxels from the x-axis of the frame buffer.
 *
 * No check for out of bounds.
 *
 * \param[in] Y Y-axis coordinate.
 * \param[in] Z Z-axis coordinate.
 * \param[out] Value Voxels with value on or off.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
uint8_t LedCube<DataPin, ClockPin, StoragePin>::getVoxelsXFast(uint8_t Y,
                                                               uint8_t Z) {
  return CurrentFrame[Y][Z];
}

/*!
 * \brief Set all voxels from the x-axis in the frame buffer with a given value.
 *
 * No check for out of bounds.
 *
 * \param[in] Y Y-axis coordinate.
 * \param[in] Z Z-axis coordinate.
 * \param[in] Value Voxels with value on or off.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::setVoxelsXFast(uint8_t Y,
                                                            uint8_t Z,
                                                            uint8_t Value) {
  NextFrame[Y][Z] = Value;
}

/*!
 * \brief Get a all voxels from the y-axis of the frame buffer.
 *
 * No check for out of bounds.
 *
 * \param[in] X X-axis coordinate.
 * \param[in] Z Z-axis coordinate.
 * \param[out] Value Voxels with value on or off.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
uint8_t LedCube<DataPin, ClockPin, StoragePin>::getVoxelsYFast(uint8_t X,
                                                               uint8_t Z) {
  uint8_t Value = 0;

  for (uint8_t Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Y++) {
    bitWrite(Value, Y, bitRead(CurrentFrame[Y][Z], X));
  }
  return Value;
}

/*!
 * \brief Set all voxels from the y-axis in the frame buffer with a given value.
 *
 * No check for out of bounds.
 *
 * \param[in] X X-axis coordinate.
 * \param[in] Z Z-axis coordinate.
 * \param[in] Value Voxels with value on or off.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::setVoxelsYFast(uint8_t X,
                                                            uint8_t Z,
                                                            uint8_t Value) {
  for (uint8_t Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Y++) {
    bitWrite(NextFrame[Y][Z], X, bitRead(Value, Y));
  }
}

/*!
 * \brief Get a all voxels from the z-axis of the frame buffer.
 *
 * No check for out of bounds.
 *
 * \param[in] X X-axis coordinate.
 * \param[in] Y Y-axis coordinate.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
uint8_t LedCube<DataPin, ClockPin, StoragePin>::getVoxelsZFast(uint8_t X,
                                                               uint8_t Y) {
  uint8_t Value = 0;

  for (uint8_t Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
    bitWrite(Value, Z, bitRead(CurrentFrame[Y][Z], X));
  }
  return Value;
}

/*!
 * \brief Set all voxels from the z-axis in the frame buffer with a given value.
 *
 * No check for out of bounds.
 *
 * \param[in] X X-axis coordinate.
 * \param[in] Y Y-axis coordinate.
 * \param[in] Value Voxels with value on or off.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::setVoxelsZFast(uint8_t X,
                                                            uint8_t Y,
                                                            uint8_t Value) {
  for (uint8_t Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
    bitWrite(NextFrame[Y][Z], X, bitRead(Value, Z));
  }
}

/*!
 * \brief Set next frame to ready state.
 *
 * \return E_OK or E_NOT_OK.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::setNextFrameReady() {
  if (State == CubeState::kRunning) {
    NextFrameReady = true;
    return E_OK;
  } else {
    return E_NOT_OK;
  }
}

/*!
 * \brief Set next frame time elapsed.
 *
 * \return E_OK or E_NOT_OK.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType
LedCube<DataPin, ClockPin, StoragePin>::setNextFrameTimeElapsed() {
  if (State == CubeState::kRunning) {
    NextFrameTimeElapsed = true;
    return E_OK;
  } else {
    return E_NOT_OK;
  }
}

/*!
 * \brief Send data to shift registers.
 *
 * This function shifts the given data to the shift registers 74HC595.
 *
 * \param[in] Data Data (Byte) to send.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::sendData(uint8_t Data) {
  for (uint8_t i = 0; i < 8; i++) {
    // LSB First
    // digitalWrite(DataInPin, !!(Data & (1 << i)));
    // MSB First
    DataOutput::Write(!!(Data & (1 << (7 - i))));
    // toggle clock pin
    ClockOutput::Write(HIGH);
    ClockOutput::Write(LOW);
  }
}

/*!
 * \brief Show a given layer on the cube.
 *
 * This function shows a given layer on the cube.
 *
 * \param[in] Layer Layer which should be shown on the cube.
 * \return E_OK or E_NOT_OK.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::showLayer(uint8_t Layer) {
  stdReturnType ReturnValue = E_NOT_OK;

  if (Layer < LEDCUBE_NUMBER_OF_LAYERS) {
    /* put on given layer */
    sendData(1U << Layer);
    /* send the layer data to the shift registers */
    for (uint8_t Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Y++) {
      sendData(CurrentFrame[Y][Layer]);
    }
    // toggle storage pin
    StorageOutput::Write(HIGH);
    StorageOutput::Write(LOW);

    ReturnValue = E_OK;
  }
  return ReturnValue;
}

/*!
 * \brief Show next frame on the cube.
 *
 * This function activates the next frame only if next frame is ready function
 * is called from timer interrupt normally.
 *
 * \return E_OK or E_NOT_OK.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::showNextFrame() {
  switchBufferPointer();
  return E_OK;
}

/*!
 * \brief Switch the two frame buffers.
 *
 * This function switches the two frame buffers CubeBuffer1 and CubeBuffer2.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::switchBufferPointer() {
  pFrameType pTmp;

  pTmp = CurrentFrame;
  CurrentFrame = NextFrame;
  NextFrame = pTmp;
}

}  // namespace ledcube

#endif
