/**
 * @file RotaryKnob.cpp
 * @brief Implementation of the RotaryKnob control.
 *
 * @details
 * Implements the Control interface.  Converts rotary encoder signals into
 * normalized values (−1.0 … +1.0).  Direction detection uses falling edge
 * on channel A, channel B defines rotation sign.
 *
 * @author
 *   Felix Lindemann
 * @date
 *   2025-10-28
 * @version
 *   2.0
 */

#include "RotaryKnob.h"

// --- Constructor ---
RotaryKnob::RotaryKnob(const String& id, uint8_t a, uint8_t b,
                       unsigned long debounceMs, float minV, float maxV)
    : Control(id, a),          // 'pin' field in base used for A-channel
      pinA(a),
      pinB(b),
      lastA(HIGH),
      lastB(HIGH),
      position(0),
      lastEventTime(0),
      debounce(debounceMs),
      lastDelta(0),
      minValue(minV),
      maxValue(maxV) {}

// --- Initialization ---
void RotaryKnob::begin() {
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  lastA = digitalRead(pinA);
  lastB = digitalRead(pinB);
  position = 0;
  lastDelta = 0;
  lastEventTime = millis();
}

// --- Update ---
bool RotaryKnob::update() {
  int nowA = digitalRead(pinA);
  int nowB = digitalRead(pinB);
  lastDelta = 0;

  // detect falling edge on A
  if (lastA == HIGH && nowA == LOW) {
    unsigned long now = millis();
    if (now - lastEventTime > debounce) {
      if (nowB == HIGH) {
        position++;
        lastDelta = +1;
      } else {
        position--;
        lastDelta = -1;
      }
      lastEventTime = now;
      lastA = nowA;
      lastB = nowB;
      return true;   // state changed
    }
  }

  lastA = nowA;
  lastB = nowB;
  return false;      // no change
}

// --- Normalized value ---
float RotaryKnob::getValue() const {
  // Map current position to normalized range (−1 … +1)
  float range = maxValue - minValue;
  if (range == 0) return 0.0f;
  float normalized = (position % 100) / 50.0f - 1.0f;  // wrap every 100 steps
  return constrain(normalized, minValue, maxValue);
}
