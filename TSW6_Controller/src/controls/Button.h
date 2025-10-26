/**
 * @file Button.h
 * @brief Debounced button class with edge event detection.
 *
 * @details
 * The Button class provides software debouncing and event detection
 * for simple digital push buttons. It detects transitions and returns:
 *   +1 → pressed  (rising edge: HIGH → LOW)
 *   -1 → released (falling edge: LOW → HIGH)
 *    0 → no change
 *
 * The method isPressed() returns the current stable logical state.
 *
 * Notes:
 * - Uses internal pull-up (INPUT_PULLUP)
 * - Returns inverted logic (LOW = pressed)
 * - Designed for stable polling in loop() without interrupts
 *
 * Typical usage:
 * @code
 *   Button fireButton(4);
 *   fireButton.begin();
 *   int event = fireButton.update();
 *   if (event == +1) Serial.println("Pressed");
 *   if (event == -1) Serial.println("Released");
 * @endcode
 *
 * @author Felix Lindemann
 * @date 2025-10-26
 * @version 1.0
 *
 * @copyright
 * This code is part of the TSW Controller Project.
 * Licensed under the MIT License.
 */
#pragma once
#include <Arduino.h>

class Button {
private:
  uint8_t pin;                  // GPIO pin
  bool lastStableState;         // last debounced state
  bool lastReading;             // last raw reading
  unsigned long lastDebounceTime;
  unsigned int debounceDelay;   // debounce time in ms

public:
  explicit Button(uint8_t gpio, unsigned int debounce = 50);

  void begin();
  int update();

  bool isPressed() const;
  uint8_t getPin() const;
};
