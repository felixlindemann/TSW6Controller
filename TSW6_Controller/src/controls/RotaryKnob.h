/**
 * @file RotaryKnob.h
 * @brief Robust quadrature rotary encoder with direction detection.
 *
 * @details
 * Reads two digital inputs (A/B) using interrupts.
 * Reports +1.0 for right (CW) and -1.0 for left (CCW) turns.
 *
 * Hardware setup:
 *  - A/B pins each pulled up via 10kΩ to 3.3V
 *  - Optional 100nF to GND for hardware debouncing
 *  - Common pin to GND
 *
 * @author
 *   Felix Lindemann
 * @date
 *   2025-11-02
 * @version
 *   2.1
 */
#pragma once
#include <Arduino.h>
#include "Control.h"

class RotaryKnob : public Control {
private:
  uint8_t pinA;
  uint8_t pinB;

  volatile int8_t encoderDelta;
  volatile uint8_t lastState;
  unsigned long lastChange;

  float value;  // <-- added: holds the last reported value (-1.0 / +1.0)

  static void IRAM_ATTR handleInterruptA(void* arg);
  static void IRAM_ATTR handleInterruptB(void* arg);

public:
  RotaryKnob(const String& id, uint8_t a, uint8_t b);

  // --- Extended variant (with extra args for compatibility) ---
  RotaryKnob(const String& id, uint8_t a, uint8_t b, int steps, float minVal, float maxVal);

  void begin() override;
  bool update() override;
  float getValue() const override;

  void reset();
};

