// Copyright 2026 Andreas Burnickl
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ledcube/Transformation.h"

#include "ledcube/LedCube.h"

namespace ledcube {

// Every member, though no test calls one yet: gcov counts a member of a
// template only where it is instantiated, so without this Transformation
// would be missing from the coverage, not shown as a gap (ROADMAP T-12).
template class Transformation<LedCube<13, 11, 12>>;

}  // namespace ledcube
