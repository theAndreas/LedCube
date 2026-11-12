// Copyright 2026 Andreas Burnickl
// SPDX-License-Identifier: GPL-3.0-or-later
//
// The part of the Arduino core that the library uses, for the tests on the
// host: its types and bit macros; pinMode() and digitalWrite(), which record
// into mock_pins what the tests read; __FlashStringHelper and F(); and Print
// and Serial, which print into a std::string the tests read. The host has no
// separate flash, so a text "in flash" is a text in RAM here; that the library
// reads flash with pgm_read is what SelfCheck in simavr checks, not these
// tests. Extend it as the library comes to use more of the core.

#ifndef LEDCUBE_TEST_MOCK_ARDUINO_H_
#define LEDCUBE_TEST_MOCK_ARDUINO_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>  // the core includes it, for memset() and memcpy()

#include <map>
#include <string>
#include <vector>

// The core's macros, by its names and as it defines them: the library is
// compiled against them as against the core.

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage,modernize-macro-to-enum)
#define HIGH 0x1
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage,modernize-macro-to-enum)
#define LOW 0x0
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage,modernize-macro-to-enum)
#define INPUT 0x0
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage,modernize-macro-to-enum)
#define OUTPUT 0x1
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage,readability-identifier-naming)
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage,readability-identifier-naming)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage,readability-identifier-naming)
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage,readability-identifier-naming)
#define bitWrite(value, bit, bitvalue) \
  ((bitvalue) ? bitSet(value, bit) : bitClear(value, bit))

// One call of digitalWrite().
struct PinWrite {
  uint8_t pin;
  uint8_t level;
};

// What the sketch would have done to the pins.
struct MockPins {
  std::map<uint8_t, uint8_t> modes;  // the mode pinMode() set last, by pin
  std::vector<PinWrite> writes;      // each digitalWrite(), in order
};

// A global, as test/.clang-tidy allows.
extern MockPins mock_pins;

// The core's names.
// NOLINTNEXTLINE(readability-identifier-naming)
void pinMode(uint8_t pin, uint8_t mode);
// NOLINTNEXTLINE(readability-identifier-naming)
void digitalWrite(uint8_t pin, uint8_t val);

// The core's name, reserved as it is: the library takes this very type.
class __FlashStringHelper;  // NOLINT(bugprone-reserved-identifier)

// The core's macros, as it defines them.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PSTR(text) (text)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define F(text) (reinterpret_cast<__FlashStringHelper const*>(PSTR(text)))

// As the core's Print: write() is what a derived class provides, print()
// what the library calls. Copied and moved as the core's is, by the compiler.
class Print {  // NOLINT(cppcoreguidelines-special-member-functions)
 public:
  virtual ~Print() = default;

  virtual size_t write(uint8_t c) = 0;

  size_t print(__FlashStringHelper const* text);
  size_t print(char const* text);
  size_t print(char c) { return write(static_cast<uint8_t>(c)); }
  size_t print(unsigned int number);
  size_t println();
};

// What the sketch would print on Serial, in `output`.
class MockSerial : public Print {
 public:
  size_t write(uint8_t c) override {
    output.push_back(static_cast<char>(c));
    return 1;
  }

  std::string output;
};

// The core's name for it; a global, as test/.clang-tidy allows.
extern MockSerial Serial;  // NOLINT(readability-identifier-naming)

#endif  // LEDCUBE_TEST_MOCK_ARDUINO_H_
