// Copyright 2026 Andreas Burnickl
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ledcube/Transformation.h"

#include <gtest/gtest.h>

#include "ledcube/Error.h"
#include "ledcube/LedCube.h"

namespace ledcube {

// Every member, though no test calls one yet: gcov counts a member of a
// template only where it is instantiated, so without this Transformation
// would be missing from the coverage, not shown as a gap (ROADMAP T-12).
template class Transformation<LedCube<13, 11, 12>>;

namespace {

using Cube = LedCube<13, 11, 12>;

TEST(TransformationTest, RefusesACircleBeyondTheCube) {
  Cube cube;
  Transformation<Cube> transformation(&cube);

  EXPECT_EQ(
      transformation.shiftCircle(Axis::kZ, RotationDirection::kClockwise, 5),
      Error::kOutOfRange);
}

}  // namespace
}  // namespace ledcube
