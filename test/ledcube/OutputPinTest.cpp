// Copyright 2026 Andreas Burnickl
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ledcube/OutputPin.h"

#include <gtest/gtest.h>
#include <stddef.h>

#include "Arduino.h"

namespace ledcube::internal {
namespace {

class OutputPinTest : public ::testing::Test {
 protected:
  void TearDown() override { mock_pins = MockPins{}; }
};

TEST_F(OutputPinTest, MakesThePinAnOutput) {
  OutputPin<7>::Begin();

  EXPECT_EQ(mock_pins.modes[7], OUTPUT);
}

TEST_F(OutputPinTest, WritesHigh) {
  OutputPin<7>::Write(HIGH);

  ASSERT_EQ(mock_pins.writes.size(), size_t{1});
  EXPECT_EQ(mock_pins.writes[0].pin, 7);
  EXPECT_EQ(mock_pins.writes[0].level, HIGH);
}

TEST_F(OutputPinTest, WritesLow) {
  OutputPin<7>::Write(LOW);

  ASSERT_EQ(mock_pins.writes.size(), size_t{1});
  EXPECT_EQ(mock_pins.writes[0].pin, 7);
  EXPECT_EQ(mock_pins.writes[0].level, LOW);
}

}  // namespace
}  // namespace ledcube::internal
