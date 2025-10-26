/**
 * @file Button.cpp
 * @brief Implementation of the debounced Button control.
 *
 * @details
 * Implements the Control interface.  Uses internal pull-up
 * and returns 1.0 when pressed, 0.0 when released.
 *
 * @author
 *   Felix Lindemann
 * @date
 *   2025-10-28
 * @version
 *   2.0
 */

#include "Button.h"

// --- Constructor ---
Button::Button(const String& id, uint8_t gpio, unsigned int debounce)
    : Control(id, gpio),
      debounceDelay(debounce),
      lastStableState(HIGH),
      lastReading(HIGH),
      lastDebounceTime(0),
      lastEvent(0) {}

// --- Initialization ---
void Button::begin() {
  pinMode(pin, INPUT_PULLUP);
  lastReading = digitalRead(pin);
  lastStableState = lastReading;
  lastDebounceTime = millis();
}

// --- Update ---
bool Button::update() {
  bool reading = digitalRead(pin);
  lastEvent = 0;

  if (reading != lastReading) {
    lastDebounceTime = millis();
    lastReading = reading;
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (lastStableState != lastReading) {
      lastStableState = lastReading;
      lastEvent = (lastStableState == LOW) ? +1 : -1;
      return true;               // state changed
    }
  }
  return false;                  // no change
}

// --- Value getter ---
float Button::getValue() const {
  return (lastStableState == LOW) ? 1.0f : 0.0f;
}
