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
#include "ledcube/Error.h"
#include "ledcube/OutputPin.h"
#include "ledcube/attributes.h"

namespace ledcube {

/*! \brief The LEDs along each edge of the cube. */
constexpr uint8_t kLedsPerSide{8};
/*! \brief The layers of the cube, one above the other. */
constexpr uint8_t kLayers{kLedsPerSide};
/*! \brief The 74HC595 in the chain: one for the layers, one per row. */
constexpr uint8_t kShiftRegisters{9};
/*! \brief The outputs of a 74HC595. */
constexpr uint8_t kShiftRegisterWidth{8};

/*! \brief The state of a LedCube, which Begin(), Start() and Stop() change. */
enum class CubeState : uint8_t {
  kNone,    /*!< Made, Begin() not called yet. */
  kInit,    /*!< In Begin(). */
  kReady,   /*!< After Begin(): ready to Start(). */
  kRunning, /*!< After Start(): Update() shows the frame. */
  kStopped  /*!< After Stop(): Update() shows nothing. */
};

/*! \brief An axis of the cube. */
enum class Axis : uint8_t {
  kX, /*!< Within a row, the bits of a byte of the frame. */
  kY, /*!< Across the rows of a layer. */
  kZ  /*!< Across the layers. */
};

/* Type which defines the buffer with all led states */
using BufferType = uint8_t[kLedsPerSide][kLedsPerSide];
/* Type which points to the buffer for the next or current frame */
using FramePointer = uint8_t (*)[kLedsPerSide];

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

  uint8_t current_layer_;
  CubeState state_;
  FramePointer current_frame_;
  FramePointer next_frame_;
  BufferType buffer1_;
  BufferType buffer2_;
  bool next_frame_ready_;
  bool next_frame_time_elapsed_;

  // functions
  void ClearCube();
  void ShowLayer(uint8_t);
  void SendData(uint8_t data);
  void SwitchBufferPointer();
  void ShowNextFrame();

 public:
  LedCube();
  ~LedCube();

  // get methods
  uint8_t current_layer() { return current_layer_; }
  CubeState state() { return state_; }
  bool next_frame_ready() { return next_frame_ready_; }
  bool next_frame_time_elapsed() { return next_frame_time_elapsed_; }

  // set methods
  LEDCUBE_NODISCARD Error SetNextFrameReady();
  LEDCUBE_NODISCARD Error SetNextFrameTimeElapsed();

  // methods
  void Begin();
  LEDCUBE_NODISCARD Error Start();
  LEDCUBE_NODISCARD Error Stop();
  void Update();

  LEDCUBE_NODISCARD Error SetVoxel(uint8_t, uint8_t, uint8_t);
  LEDCUBE_NODISCARD Error ClearVoxel(uint8_t, uint8_t, uint8_t);
  LEDCUBE_NODISCARD Error WriteVoxel(uint8_t, uint8_t, uint8_t, bool);
  LEDCUBE_NODISCARD Error GetVoxel(uint8_t, uint8_t, uint8_t, bool*);

  void SetVoxelFast(uint8_t x, uint8_t y, uint8_t z) {
    bitSet(next_frame_[y][z], x);
  }
  void ClearVoxelFast(uint8_t x, uint8_t y, uint8_t z) {
    bitClear(next_frame_[y][z], x);
  }
  void WriteVoxelFast(uint8_t x, uint8_t y, uint8_t z, bool value) {
    bitWrite(next_frame_[y][z], x, value);
  }
  uint8_t GetVoxelFast(uint8_t x, uint8_t y, uint8_t z) {
    return bitRead(current_frame_[y][z], x);
  }

  LEDCUBE_NODISCARD Error GetVoxelsX(uint8_t, uint8_t, uint8_t*);
  LEDCUBE_NODISCARD Error SetVoxelsX(uint8_t, uint8_t, uint8_t);
  LEDCUBE_NODISCARD Error GetVoxelsY(uint8_t, uint8_t, uint8_t*);
  LEDCUBE_NODISCARD Error SetVoxelsY(uint8_t, uint8_t, uint8_t);
  LEDCUBE_NODISCARD Error GetVoxelsZ(uint8_t, uint8_t, uint8_t*);
  LEDCUBE_NODISCARD Error SetVoxelsZ(uint8_t, uint8_t, uint8_t);

  uint8_t GetVoxelsXFast(uint8_t, uint8_t);
  void SetVoxelsXFast(uint8_t, uint8_t, uint8_t);
  uint8_t GetVoxelsYFast(uint8_t, uint8_t);
  void SetVoxelsYFast(uint8_t, uint8_t, uint8_t);
  uint8_t GetVoxelsZFast(uint8_t, uint8_t);
  void SetVoxelsZFast(uint8_t, uint8_t, uint8_t);

  void ClearVoxels() { memset(next_frame_, 0, sizeof(BufferType)); }
  void SetVoxels() { memset(next_frame_, 255, sizeof(BufferType)); }

  void CopyCurrentFrameToNextFrame() {
    memcpy(next_frame_, current_frame_, sizeof(BufferType));
  }
};

/*!
 * \brief LedCube Constructor.
 *
 * Instantiation of the LedCube library.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
LedCube<DataPin, ClockPin, StoragePin>::LedCube() {
  current_layer_ = 0;
  state_ = CubeState::kNone;
  next_frame_ready_ = false, next_frame_time_elapsed_ = false;
  current_frame_ = buffer1_;
  next_frame_ = buffer2_;
  memset(buffer1_, 0, sizeof(buffer1_));
  memset(buffer2_, 0, sizeof(buffer2_));
}

template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
LedCube<DataPin, ClockPin, StoragePin>::~LedCube() {}

/*!
 * \brief Initialization of the Led Cube hardware.
 *
 * This function initializes the IO hardware and the shift registers 74HC595.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::Begin() {
  state_ = CubeState::kInit;
  ClearCube();
  DataOutput::Begin();
  ClockOutput::Begin();
  StorageOutput::Begin();
  state_ = CubeState::kReady;
}

/*!
 * \brief Starts the led cube showing a frame.
 *
 * This function starts with showing the current buffer on the led cube
 * therefore Update() has to be called periodically.
 *
 * \return Error::kNone, or Error::kWrongState if the state forbids the call.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
Error LedCube<DataPin, ClockPin, StoragePin>::Start() {
  bool const startable{state_ == CubeState::kReady ||
                       state_ == CubeState::kStopped};
  if (startable) {
    state_ = CubeState::kRunning;
    return Error::kNone;
  }
  return Error::kWrongState;
}

/*!
 * \brief Stops the led cube showing a frame.
 *
 * \return Error::kNone, or Error::kWrongState if the state forbids the call.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
Error LedCube<DataPin, ClockPin, StoragePin>::Stop() {
  if (state_ == CubeState::kRunning) {
    state_ = CubeState::kStopped;
    return Error::kNone;
  }
  return Error::kWrongState;
}

/*!
 * \brief Show layer which is the next in line.
 *
 * This function shows one layer of the current frame on the cube has to be
 * called periodically.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::Update() {
  if (state_ == CubeState::kRunning) {
    if (current_layer_ >= kLayers) {
      current_layer_ = 0;
      /* is next frame ready to show? */
      if (next_frame_ready_ && next_frame_time_elapsed_) {
        ShowNextFrame();
        next_frame_ready_ = false;
        next_frame_time_elapsed_ = false;
      }
    }
    ShowLayer(current_layer_);
    current_layer_++;
  }
}

/*!
 * \brief Clear all LEDs on the cube.
 *
 * This function initializes the shift registers with zero.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::ClearCube() {
  DataOutput::Write(LOW);

  for (uint8_t i = 0; i < (kShiftRegisters * kShiftRegisterWidth); i++) {
    ClockOutput::Write(HIGH);
    ClockOutput::Write(LOW);
  }
  StorageOutput::Write(HIGH);
  StorageOutput::Write(LOW);
}

/*!
 * \brief Set a given voxel in the frame buffer.
 *
 * \param[in] x X-axis coordinate.
 * \param[in] y Y-axis coordinate.
 * \param[in] z Z-axis coordinate.
 * \return Error::kNone, or Error::kOutOfRange if a coordinate lies beyond the
 *         cube.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
Error LedCube<DataPin, ClockPin, StoragePin>::SetVoxel(uint8_t x, uint8_t y,
                                                       uint8_t z) {
  if (x < kLedsPerSide && y < kLedsPerSide && z < kLedsPerSide) {
    bitSet(next_frame_[y][z], x);
    return Error::kNone;
  } else {
    return Error::kOutOfRange;
  }
}

/*!
 * \brief Clear a given voxel in the frame buffer.
 *
 * \param[in] x X-axis coordinate.
 * \param[in] y Y-axis coordinate.
 * \param[in] z Z-axis coordinate.
 * \return Error::kNone, or Error::kOutOfRange if a coordinate lies beyond the
 *         cube.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
Error LedCube<DataPin, ClockPin, StoragePin>::ClearVoxel(uint8_t x, uint8_t y,
                                                         uint8_t z) {
  if (x < kLedsPerSide && y < kLedsPerSide && z < kLedsPerSide) {
    bitClear(next_frame_[y][z], x);
    return Error::kNone;
  } else {
    return Error::kOutOfRange;
  }
}

/*!
 * \brief Set given voxel to a given value in the frame buffer.
 *
 * \param[in] x X-axis coordinate.
 * \param[in] y Y-axis coordinate.
 * \param[in] z Z-axis coordinate.
 * \param[in] value Voxel of value on or off.
 * \return Error::kNone, or Error::kOutOfRange if a coordinate lies beyond the
 *         cube.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
Error LedCube<DataPin, ClockPin, StoragePin>::WriteVoxel(uint8_t x, uint8_t y,
                                                         uint8_t z,
                                                         bool value) {
  if (x < kLedsPerSide && y < kLedsPerSide && z < kLedsPerSide) {
    bitWrite(next_frame_[y][z], x, value);
    return Error::kNone;
  } else {
    return Error::kOutOfRange;
  }
}

/*!
 * \brief Get a given voxel of the frame buffer.
 *
 * \param[in] x X-axis coordinate.
 * \param[in] y Y-axis coordinate.
 * \param[in] z Z-axis coordinate.
 * \param[out] value Voxel of value on or off.
 * \return Error::kNone, or Error::kOutOfRange if a coordinate lies beyond the
 *         cube.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
Error LedCube<DataPin, ClockPin, StoragePin>::GetVoxel(uint8_t x, uint8_t y,
                                                       uint8_t z, bool* value) {
  if (x < kLedsPerSide && y < kLedsPerSide && z < kLedsPerSide) {
    *value = bitRead(current_frame_[y][z], x);
    return Error::kNone;
  } else {
    return Error::kOutOfRange;
  }
}

/*!
 * \brief Get a all voxels from the x-axis of the frame buffer.
 *
 * \param[in] y Y-axis coordinate.
 * \param[in] z Z-axis coordinate.
 * \param[out] value Voxels with value on or off.
 * \return Error::kNone, or Error::kOutOfRange if a coordinate lies beyond the
 *         cube.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
Error LedCube<DataPin, ClockPin, StoragePin>::GetVoxelsX(uint8_t y, uint8_t z,
                                                         uint8_t* value) {
  if (y < kLedsPerSide && z < kLedsPerSide) {
    *value = current_frame_[y][z];
    return Error::kNone;
  } else {
    return Error::kOutOfRange;
  }
}

/*!
 * \brief Set all voxels from the x-axis in the frame buffer with a given value.
 *
 * \param[in] y Y-axis coordinate.
 * \param[in] z Z-axis coordinate.
 * \param[in] value Voxels with value on or off.
 * \return Error::kNone, or Error::kOutOfRange if a coordinate lies beyond the
 *         cube.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
Error LedCube<DataPin, ClockPin, StoragePin>::SetVoxelsX(uint8_t y, uint8_t z,
                                                         uint8_t value) {
  if (y < kLedsPerSide && z < kLedsPerSide) {
    next_frame_[y][z] = value;
    return Error::kNone;
  } else {
    return Error::kOutOfRange;
  }
}

/*!
 * \brief Get a all voxels from the y-axis of the frame buffer.
 *
 * \param[in] x X-axis coordinate.
 * \param[in] z Z-axis coordinate.
 * \param[out] value Voxels with value on or off.
 * \return Error::kNone, or Error::kOutOfRange if a coordinate lies beyond the
 *         cube.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
Error LedCube<DataPin, ClockPin, StoragePin>::GetVoxelsY(uint8_t x, uint8_t z,
                                                         uint8_t* value) {
  if (x < kLedsPerSide && z < kLedsPerSide) {
    for (uint8_t y = 0; y < kLedsPerSide; y++) {
      bitWrite(*value, y, bitRead(current_frame_[y][z], x));
    }
    return Error::kNone;
  } else {
    return Error::kOutOfRange;
  }
}

/*!
 * \brief Set all voxels from the y-axis in the frame buffer with a given value.
 *
 * \param[in] x X-axis coordinate.
 * \param[in] z Z-axis coordinate.
 * \param[in] value Voxels with value on or off.
 * \return Error::kNone, or Error::kOutOfRange if a coordinate lies beyond the
 *         cube.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
Error LedCube<DataPin, ClockPin, StoragePin>::SetVoxelsY(uint8_t x, uint8_t z,
                                                         uint8_t value) {
  if (x < kLedsPerSide && z < kLedsPerSide) {
    for (uint8_t y = 0; y < kLedsPerSide; y++) {
      bitWrite(next_frame_[y][z], x, bitRead(value, y));
    }
    return Error::kNone;
  } else {
    return Error::kOutOfRange;
  }
}

/*!
 * \brief Get a all voxels from the z-axis of the frame buffer.
 *
 * \param[in] x X-axis coordinate.
 * \param[in] y Y-axis coordinate.
 * \param[out] value Voxels with value on or off.
 * \return Error::kNone, or Error::kOutOfRange if a coordinate lies beyond the
 *         cube.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
Error LedCube<DataPin, ClockPin, StoragePin>::GetVoxelsZ(uint8_t x, uint8_t y,
                                                         uint8_t* value) {
  if (x < kLedsPerSide && y < kLedsPerSide) {
    for (uint8_t z = 0; z < kLedsPerSide; z++) {
      bitWrite(*value, z, bitRead(current_frame_[y][z], x));
    }
    return Error::kNone;
  } else {
    return Error::kOutOfRange;
  }
}

/*!
 * \brief Set all voxels from the z-axis in the frame buffer with a given value.
 *
 * \param[in] x X-axis coordinate.
 * \param[in] y Y-axis coordinate.
 * \param[in] value Voxels with value on or off.
 * \return Error::kNone, or Error::kOutOfRange if a coordinate lies beyond the
 *         cube.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
Error LedCube<DataPin, ClockPin, StoragePin>::SetVoxelsZ(uint8_t x, uint8_t y,
                                                         uint8_t value) {
  if (x < kLedsPerSide && y < kLedsPerSide) {
    for (uint8_t z = 0; z < kLedsPerSide; z++) {
      bitWrite(next_frame_[y][z], x, bitRead(value, z));
    }
    return Error::kNone;
  } else {
    return Error::kOutOfRange;
  }
}

/*!
 * \brief Get a all voxels from the x-axis of the frame buffer.
 *
 * No check for out of bounds.
 *
 * \param[in] y Y-axis coordinate.
 * \param[in] z Z-axis coordinate.
 * \param[out] value Voxels with value on or off.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
uint8_t LedCube<DataPin, ClockPin, StoragePin>::GetVoxelsXFast(uint8_t y,
                                                               uint8_t z) {
  return current_frame_[y][z];
}

/*!
 * \brief Set all voxels from the x-axis in the frame buffer with a given value.
 *
 * No check for out of bounds.
 *
 * \param[in] y Y-axis coordinate.
 * \param[in] z Z-axis coordinate.
 * \param[in] value Voxels with value on or off.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::SetVoxelsXFast(uint8_t y,
                                                            uint8_t z,
                                                            uint8_t value) {
  next_frame_[y][z] = value;
}

/*!
 * \brief Get a all voxels from the y-axis of the frame buffer.
 *
 * No check for out of bounds.
 *
 * \param[in] x X-axis coordinate.
 * \param[in] z Z-axis coordinate.
 * \param[out] value Voxels with value on or off.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
uint8_t LedCube<DataPin, ClockPin, StoragePin>::GetVoxelsYFast(uint8_t x,
                                                               uint8_t z) {
  uint8_t value = 0;

  for (uint8_t y = 0; y < kLedsPerSide; y++) {
    bitWrite(value, y, bitRead(current_frame_[y][z], x));
  }
  return value;
}

/*!
 * \brief Set all voxels from the y-axis in the frame buffer with a given value.
 *
 * No check for out of bounds.
 *
 * \param[in] x X-axis coordinate.
 * \param[in] z Z-axis coordinate.
 * \param[in] value Voxels with value on or off.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::SetVoxelsYFast(uint8_t x,
                                                            uint8_t z,
                                                            uint8_t value) {
  for (uint8_t y = 0; y < kLedsPerSide; y++) {
    bitWrite(next_frame_[y][z], x, bitRead(value, y));
  }
}

/*!
 * \brief Get a all voxels from the z-axis of the frame buffer.
 *
 * No check for out of bounds.
 *
 * \param[in] x X-axis coordinate.
 * \param[in] y Y-axis coordinate.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
uint8_t LedCube<DataPin, ClockPin, StoragePin>::GetVoxelsZFast(uint8_t x,
                                                               uint8_t y) {
  uint8_t value = 0;

  for (uint8_t z = 0; z < kLedsPerSide; z++) {
    bitWrite(value, z, bitRead(current_frame_[y][z], x));
  }
  return value;
}

/*!
 * \brief Set all voxels from the z-axis in the frame buffer with a given value.
 *
 * No check for out of bounds.
 *
 * \param[in] x X-axis coordinate.
 * \param[in] y Y-axis coordinate.
 * \param[in] value Voxels with value on or off.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::SetVoxelsZFast(uint8_t x,
                                                            uint8_t y,
                                                            uint8_t value) {
  for (uint8_t z = 0; z < kLedsPerSide; z++) {
    bitWrite(next_frame_[y][z], x, bitRead(value, z));
  }
}

/*!
 * \brief Set next frame to ready state.
 *
 * \return Error::kNone, or Error::kWrongState if the state forbids the call.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
Error LedCube<DataPin, ClockPin, StoragePin>::SetNextFrameReady() {
  if (state_ == CubeState::kRunning) {
    next_frame_ready_ = true;
    return Error::kNone;
  } else {
    return Error::kWrongState;
  }
}

/*!
 * \brief Set next frame time elapsed.
 *
 * \return Error::kNone, or Error::kWrongState if the state forbids the call.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
Error LedCube<DataPin, ClockPin, StoragePin>::SetNextFrameTimeElapsed() {
  if (state_ == CubeState::kRunning) {
    next_frame_time_elapsed_ = true;
    return Error::kNone;
  } else {
    return Error::kWrongState;
  }
}

/*!
 * \brief Send data to shift registers.
 *
 * This function shifts the given data to the shift registers 74HC595.
 *
 * \param[in] data Data (Byte) to send.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::SendData(uint8_t data) {
  for (uint8_t i = 0; i < 8; i++) {
    // LSB First
    // digitalWrite(DataInPin, !!(Data & (1 << i)));
    // MSB First
    DataOutput::Write(!!(data & (1 << (7 - i))));
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
 * \param[in] layer Layer which should be shown on the cube.
 * \pre `layer` < kLayers, as Update() ensures.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::ShowLayer(uint8_t layer) {
  /* put on given layer */
  SendData(1U << layer);
  /* send the layer data to the shift registers */
  for (uint8_t y = 0; y < kLedsPerSide; y++) {
    SendData(current_frame_[y][layer]);
  }
  // toggle storage pin
  StorageOutput::Write(HIGH);
  StorageOutput::Write(LOW);
}

/*!
 * \brief Show next frame on the cube.
 *
 * This function activates the next frame only if next frame is ready function
 * is called from timer interrupt normally.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::ShowNextFrame() {
  SwitchBufferPointer();
}

/*!
 * \brief Switch the two frame buffers.
 *
 * This function switches the two frame buffers CubeBuffer1 and CubeBuffer2.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::SwitchBufferPointer() {
  FramePointer tmp;

  tmp = current_frame_;
  current_frame_ = next_frame_;
  next_frame_ = tmp;
}

}  // namespace ledcube

#endif
