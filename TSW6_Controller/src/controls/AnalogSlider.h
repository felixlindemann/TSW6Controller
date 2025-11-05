/**
 * @file AnalogSlider.h
 * @brief Hardware abstraction for an analog slider (potentiometer) input.
 *
 * @details
 * Implements the Control interface for analog inputs.
 * Reads a voltage (0–MAX_ANALOG) and converts it to a normalized range 0–100 %.
 * Supports inversion, threshold filtering and dead zones to stabilize noisy inputs.
 *
 * Notes:
 * - No analogRead() is called in the constructor to prevent boot-time freezes
 *   on boards such as the Arduino Leonardo (USB initialization issue).
 * - MAX_ANALOG is determined at compile time (1023 or 4095, depending on board).
 *
 * Example:
 * @code
 *   AnalogSlider throttle("SLIDER_AFB", A0);
 *   throttle.begin();
 *   if (throttle.update()) {
 *       float val = throttle.getValue(); // normalized 0.0–1.0
 *   }
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
#include "../config.h"

class AnalogSlider : public Control {
private:
  int minValue;            // lower calibration limit
  int maxValue;            // upper calibration limit
  int zero;                // offset or midpoint
  int lastValue;           // last stable percent value (0–100)
  int lastRaw;             // last stable ADC raw value
  int rawThreshold;        // minimal raw change required
  bool inverted;           // axis inverted?
  unsigned long lastRead;  // last read time
  unsigned long interval;  // read interval (ms)
  static constexpr int MAX_ANALOG =
#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_ARCH_SAMD)
      4095;
#else
      1023;
#endif

  int getPercent(int raw) const;

public:
  explicit AnalogSlider(const String& id, uint8_t gpio);

  // --- Lifecycle ---
  void begin() override;
  bool update() override;

  // --- Value retrieval ---
  float getValue() const override;   // normalized 0.0–1.0
  int getRawValue() const;
  int getPercentValue() const;

  // --- Configuration ---
  void setZero(int z);
  void setMinValue(int z);
  void setMaxValue(int z);
  void setInterval(unsigned long i);
  void setInverted(bool inv);
  void setRawThreshold(int t);
};
