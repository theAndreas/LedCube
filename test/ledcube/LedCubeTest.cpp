// Copyright 2026 Andreas Burnickl
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ledcube/LedCube.h"

#include <gtest/gtest.h>
#include <stddef.h>
#include <stdint.h>

#include <map>
#include <vector>

#include "Arduino.h"
#include "ledcube/StandardTypes.h"

namespace ledcube {

// Every member, not only those a test calls: gcov counts a member of a
// template only where it is instantiated, so one no test calls would be
// missing from the coverage, not shown as a gap (ROADMAP T-12).
template class LedCube<13, 11, 12>;

namespace {

constexpr uint8_t kDataPin{13};
constexpr uint8_t kClockPin{11};
constexpr uint8_t kStoragePin{12};

using Cube = LedCube<kDataPin, kClockPin, kStoragePin>;
using Bytes = std::vector<uint8_t>;

// A chain of 74HC595 on the pins above, fed the writes the mock recorded: on
// each rising edge of the clock it shifts in the level of the data pin, and on
// each rising edge of the storage pin it takes over the bytes shifted in since
// the edge before. Each byte is assembled from its most significant bit, in the
// order the bytes were shifted in.
class ShiftRegisterChain {
 public:
  explicit ShiftRegisterChain(std::vector<PinWrite> const& writes) {
    for (PinWrite const& write : writes) {
      Apply(write);
    }
  }

  [[nodiscard]] std::vector<Bytes> const& latched() const { return latched_; }

 private:
  void Apply(PinWrite write) {
    bool const rising{write.level == HIGH && levels_[write.pin] == LOW};
    levels_[write.pin] = write.level;
    if (rising && write.pin == kClockPin) {
      ShiftIn(levels_[kDataPin]);
    }
    if (rising && write.pin == kStoragePin) {
      latched_.push_back(shifted_);
      shifted_.clear();
      bit_count_ = 0;
    }
  }

  void ShiftIn(uint8_t level) {
    if (bit_count_ % 8 == 0) {
      shifted_.push_back(0);
    }
    unsigned const bit{level == HIGH ? 1U : 0U};
    unsigned const before{shifted_.back()};
    shifted_.back() = static_cast<uint8_t>((before << 1U) | bit);
    ++bit_count_;
  }

  std::map<uint8_t, uint8_t> levels_;
  Bytes shifted_;
  size_t bit_count_{0};
  std::vector<Bytes> latched_;
};

class LedCubeTest : public ::testing::Test {
 protected:
  void TearDown() override { mock_pins = MockPins{}; }

  // What the shift registers took over since the last ForgetWrites().
  static std::vector<Bytes> Latched() {
    return ShiftRegisterChain{mock_pins.writes}.latched();
  }

  static void ForgetWrites() { mock_pins.writes.clear(); }

  static void RunTasks(Cube& cube, uint8_t count) {
    for (uint8_t i{0}; i < count; ++i) {
      cube.task();
    }
  }
};

TEST_F(LedCubeTest, SetsItsPinsAsOutputsOnInit) {
  Cube cube;

  cube.init();

  EXPECT_EQ(mock_pins.modes[kDataPin], OUTPUT);
  EXPECT_EQ(mock_pins.modes[kClockPin], OUTPUT);
  EXPECT_EQ(mock_pins.modes[kStoragePin], OUTPUT);
}

TEST_F(LedCubeTest, StartsAfterInit) {
  Cube cube;
  cube.init();

  EXPECT_EQ(cube.start(), E_OK);
}

TEST_F(LedCubeTest, RefusesToStartBeforeInit) {
  Cube cube;

  EXPECT_EQ(cube.start(), E_NOT_OK);
}

TEST_F(LedCubeTest, RefusesToStartWhileRunning) {
  Cube cube;
  cube.init();
  cube.start();

  EXPECT_EQ(cube.start(), E_NOT_OK);
}

TEST_F(LedCubeTest, StartsAgainAfterStop) {
  Cube cube;
  cube.init();
  cube.start();
  cube.stop();

  EXPECT_EQ(cube.start(), E_OK);
}

TEST_F(LedCubeTest, ShiftsNothingBeforeStart) {
  Cube cube;
  cube.init();
  ForgetWrites();

  cube.task();

  EXPECT_TRUE(mock_pins.writes.empty());
}

TEST_F(LedCubeTest, ShowsTheFirstLayerOnTheFirstTask) {
  Cube cube;
  cube.init();
  cube.start();
  ForgetWrites();

  cube.task();

  EXPECT_EQ(Latched(), (std::vector<Bytes>{{0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00}}));
}

TEST_F(LedCubeTest, ShowsTheNextLayerOnEachTask) {
  Cube cube;
  cube.init();
  cube.start();
  cube.task();
  ForgetWrites();

  cube.task();

  EXPECT_EQ(Latched(), (std::vector<Bytes>{{0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00}}));
}

TEST_F(LedCubeTest, ShowsTheNextFrameAfterTheLastLayer) {
  Cube cube;
  cube.init();
  cube.start();
  cube.setVoxel(3, 1, 0);
  cube.setNextFrameReady();
  cube.setNextFrameTimeElapsed();
  RunTasks(cube, 8);
  ForgetWrites();

  cube.task();

  EXPECT_EQ(Latched(), (std::vector<Bytes>{{0x01, 0x00, 0x08, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00}}));
}

TEST_F(LedCubeTest, KeepsTheFrameWhileTheNextIsNotReady) {
  Cube cube;
  cube.init();
  cube.start();
  cube.setVoxel(3, 1, 0);
  cube.setNextFrameTimeElapsed();
  RunTasks(cube, 8);
  ForgetWrites();

  cube.task();

  EXPECT_EQ(Latched(), (std::vector<Bytes>{{0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00}}));
}

TEST_F(LedCubeTest, KeepsTheFrameUntilItsTimeHasElapsed) {
  Cube cube;
  cube.init();
  cube.start();
  cube.setVoxel(3, 1, 0);
  cube.setNextFrameReady();
  RunTasks(cube, 8);
  ForgetWrites();

  cube.task();

  EXPECT_EQ(Latched(), (std::vector<Bytes>{{0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00}}));
}

}  // namespace
}  // namespace ledcube
