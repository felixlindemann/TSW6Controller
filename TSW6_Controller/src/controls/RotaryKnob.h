/**
 * @file RotaryKnob.h
 * @brief Incremental rotary encoder implementing the Control interface.
 *
 * @details
 * Reads a two-channel rotary encoder (A/B pins) and detects direction and steps.
 * Each update() call evaluates encoder edges and updates an internal position
 * counter.  The current value is exposed as a normalized float (−1.0 … +1.0)
 * relative to configured min/max limits.
 *
 * Example:
 * @code
 *   RotaryKnob brake("ROT_BRAKE", 4, 5);
 *   brake.begin();
 *   if (brake.update()) {
 *       float delta = brake.getValue();
 *       Serial.println(delta);
 *   }
 * @endcode
 *
 * @note
 *  - Default debounce = 2 ms between valid steps.
 *  - Direction detection: Channel A falling edge, Channel B defines sign.
 *
 * @auth or Felix Lindemann
 * @date 2025-10-28
 * @version 2.0
 */

#pragma once
#include <Arduino.h>
#include "Control.h"

class RotaryKnob : public Control {
private:
  uint8_t pinA;
  uint8_t pinB;
  int lastA;
  int lastB;
  int position;
  unsigned long lastEventTime;
  unsigned long debounce;     // minimal time between events [ms]
  int lastDelta;              // +1 / –1 / 0
  float minValue;
  float maxValue;

public:
  RotaryKnob(const String& id, uint8_t a, uint8_t b,
             unsigned long debounceMs = 2,
             float minV = -1.0f, float maxV = 1.0f);

  void begin() override;
  bool update() override;
  float getValue() const override;   // normalized −1.0 … +1.0

  int getPosition() const { return position; }
  void reset() { position = 0; }
  int getLastDelta() const { return lastDelta; }
};
