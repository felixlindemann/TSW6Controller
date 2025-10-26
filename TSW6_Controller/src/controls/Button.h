/**
 * @file Button.h
 * @brief Digital push-button implementing the Control interface.
 *
 * @details
 * The Button class provides software debouncing and edge detection for
 * digital inputs.  It reports transition events (+1 pressed, –1 released)
 * and implements the common Control interface with a normalized
 * value output (1 = pressed, 0 = released).
 *
 * Example:
 * @code
 *   Button sifa("BTN_SIFA", 7);
 *   sifa.begin();
 *   if (sifa.update())
 *       Serial.println(sifa.getValue() > 0 ? "Pressed" : "Released");
 * @endcode
 *
 * @author
 *   Felix Lindemann
 * @date
 *   2025-10-28
 * @version
 *   2.0
 */

#pragma once
#include <Arduino.h>
#include "Control.h"

class Button : public Control {
private:
  bool lastStableState;          // last debounced state
  bool lastReading;              // last raw reading
  unsigned long lastDebounceTime;
  unsigned int debounceDelay;    // debounce time [ms]
  int lastEvent;                 // +1 pressed, –1 released, 0 none

public:
  explicit Button(const String& id, uint8_t gpio, unsigned int debounce = 50);

  void begin() override;
  bool update() override;
  float getValue() const override;   // 1.0 pressed / 0.0 released

  bool isPressed() const { return lastStableState == LOW; }
  int getEvent() const { return lastEvent; }
};
