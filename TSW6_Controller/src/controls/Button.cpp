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
Button::Button(const String &id, uint8_t gpio, unsigned int debounce)
    : Control(id, gpio),
      debounceDelay(debounce),
      lastStableState(HIGH),
      lastReading(HIGH),
      lastDebounceTime(0),
      lastEvent(0) {}

// --- Initialization ---
void Button::begin()
{
  pinMode(pin, INPUT_PULLUP);
  lastReading = digitalRead(pin);
  lastStableState = lastReading;
  lastDebounceTime = millis();
  LOG_HW_DEBUG("Button %s initialized on GPIO %d (state: %s)\n", 
               getId().c_str(), pin, lastStableState == LOW ? "pressed" : "released");
}

// --- Update ---
bool Button::update()
{
  lastChangeReason = "none"; // reset reason at start of each update
  bool reading = digitalRead(pin);
  lastEvent = 0;

  if (reading != lastReading)
  {
    lastDebounceTime = millis();
    lastReading = reading;
  }

  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    if (lastStableState != lastReading)
    {
      lastStableState = lastReading;
      lastEvent = (lastStableState == LOW) ? +1 : -1;
      lastChangeReason = reading ? "pressed" : "released";
      
      if (isPressed()) {
        LOG_HW_INFO("Button %s pressed\n", getId().c_str());
      } else {
        LOG_HW_DEBUG("Button %s released\n", getId().c_str());
      }

      return true; // state changed
    }
  }
  return false; // no change
}

// --- Value getter ---
float Button::getValue() const
{
  return (lastStableState == LOW) ? 1.0f : 0.0f;
}
