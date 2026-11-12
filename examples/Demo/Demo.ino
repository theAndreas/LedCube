// Copyright 2017 Andreas Burnickl
// SPDX-License-Identifier: GPL-3.0-or-later

#include <Arduino.h>
#include <stdint.h>

#include "LedCube.h"
#include "ledcube/attributes.h"
#include "ledcube/font/8x8_horizontal_LSB_1.h"

#define SER_74 13  // serial input
#define RCK_74 12  // storage input
#define SCK_74 11  // clock input

void setChar(uint8_t);

ledcube::LedCube<SER_74, SCK_74, RCK_74> Cube;
ledcube::Transformation<decltype(Cube)> CubeTrans(&Cube);
int Counter;
uint8_t CharCounter = 0;

void setup() {
  Cube.init();
  pinMode(7, OUTPUT);
  for (uint8_t x = 0; x < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; x++) {
    for (uint8_t y = 0; y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; y++) {
      for (uint8_t z = 0; z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; z++) {
        // Cube.setVoxel(x, y, z);
      }
    }
  }

  // None of these can fail: the cube is ready after init() and running after
  // start(), and the rows lie within it.
  ledcube::Discard(Cube.start());
  ledcube::Discard(Cube.setNextFrameReady());
  ledcube::Discard(Cube.setNextFrameTimeElapsed());
  ledcube::Discard(Cube.setVoxelsX(1, 1, B01111110));
  ledcube::Discard(Cube.setVoxelsX(2, 7, B00111100));
  ledcube::Discard(Cube.setVoxelsX(3, 5, B00011000));
  setChar(3);
}

void loop() {
  if (Counter == 200) {
    // CubeTrans.shiftBackward(&Cube, true);
    // setChar(CharCounter);
    // None of these can fail: the cube runs since setup(), and the circles 1
    // to 4 are those of an 8x8x8 cube.
    ledcube::Discard(Cube.setNextFrameReady());
    ledcube::Discard(Cube.setNextFrameTimeElapsed());
    ledcube::Discard(CubeTrans.shiftCircle(
        ledcube::Axis::kZ, ledcube::RotationDirection::kCounterclockwise, 1));
    ledcube::Discard(CubeTrans.shiftCircle(
        ledcube::Axis::kZ, ledcube::RotationDirection::kCounterclockwise, 2));
    ledcube::Discard(CubeTrans.shiftCircle(
        ledcube::Axis::kZ, ledcube::RotationDirection::kCounterclockwise, 3));
    ledcube::Discard(CubeTrans.shiftCircle(
        ledcube::Axis::kZ, ledcube::RotationDirection::kCounterclockwise, 4));
    Counter = 0;
    CharCounter++;
  }
  Counter++;
  delay(1);
  digitalWrite(7, HIGH);
  Cube.task();
  digitalWrite(7, LOW);
}

void setChar(uint8_t Number) {
  for (uint8_t i = 0; i < 8; i++) {
    Cube.setVoxelsYFast(0, 7 - i, pgm_read_byte_near(&font[Number][i]));
  }
}
