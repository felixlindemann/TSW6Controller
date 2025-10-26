
#pragma once
#include <Arduino.h>

/**
 * @file AnalogSlider.h
 * @brief Class representing an analog slider (potentiometer) input.
 *
 * @details
 * The AnalogSlider class reads an analog signal (0–MAX_ANALOG) and maps it
 * to a normalized range of 0–100 %. It supports inversion, threshold filtering
 * and dead zones to stabilize noisy inputs.
 *
 * Notes:
 * - No analogRead() is called in the constructor to prevent boot-time freezes
 *   on boards such as the Arduino Leonardo (USB initialization issue).
 * - MAX_ANALOG is determined at compile time (1023 or 4095, depending on board).
 *
 * Typical usage:
 * @code
 *   AnalogSlider throttle(A0);
 *   throttle.begin();
 *   if (throttle.update()) {
 *       int percent = throttle.getPercentValue();
 *   }
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



class AnalogSlider {
private:
  uint8_t pin;             // GPIO pin
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
  explicit AnalogSlider(uint8_t gpio);

  void begin();

  // --- Setters ---
  void setZero(int z);
  void setMinValue(int z);
  void setMaxValue(int z);
  void setInterval(unsigned long i);
  void setInverted(bool inv);
  void setRawThreshold(int t);

  // --- Update ---
  bool update();

  // --- Getters ---
  int getRawValue() const;
  int getPercentValue() const;
};
