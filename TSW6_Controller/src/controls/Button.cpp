/**
 * @file Button.cpp
 * @brief Implementation of the Button class.
 *
 * @details
 * Provides debouncing and edge detection logic for
 * digital input buttons with internal pull-up.
 *
 * @author Felix Lindemann
 * @date 2025-10-26
 * @version 1.0
 *
 * @copyright
 * This code is part of the TSW Controller Project.
 * Licensed under the MIT License.
 */

#include "Button.h"

// --- Constructor ---
Button::Button(uint8_t gpio, unsigned int debounce)
    : pin(gpio),
      debounceDelay(debounce),
      lastStableState(HIGH),
      lastReading(HIGH),
      lastDebounceTime(0) {}

// --- Initialization ---
void Button::begin() {
  pinMode(pin, INPUT_PULLUP);
  lastReading = digitalRead(pin);
  lastStableState = lastReading;
  lastDebounceTime = millis();
}

// --- Update method ---
int Button::update() {
  bool reading = digitalRead(pin);
  int event = 0;

  if (reading != lastReading) {
    lastDebounceTime = millis();
    lastReading = reading;
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (lastStableState != lastReading) {
      lastStableState = lastReading;
      event = (lastStableState == LOW) ? +1 : -1;
    }
  }

  return event;
}

// --- Accessors ---
bool Button::isPressed() const { return lastStableState == LOW; }
uint8_t Button::getPin() const { return pin; }
