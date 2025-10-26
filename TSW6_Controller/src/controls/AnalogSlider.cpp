/**
 * @file AnalogSlider.cpp
 * @brief Implementation of the AnalogSlider class.
 *
 * @author Felix Lindemann
 * @date 2025-10-26
 * @version 1.0
 */

#include "AnalogSlider.h"

// --- Constructor ---
AnalogSlider::AnalogSlider(uint8_t gpio)
    : pin(gpio) {
  pinMode(pin, INPUT);

  // Default configuration
  minValue = 0;
  maxValue = MAX_ANALOG;
  zero = 0;
  rawThreshold = 10;   // ADC steps required for change
  interval = 50;       // ms
  inverted = false;
  lastRead = 0;
  lastRaw = 0;
  lastValue = 0;
}

// --- Initialization ---
void AnalogSlider::begin() {
  delay(5);  // small pause after USB init
  lastRaw = analogRead(pin) - zero;
  lastValue = getPercent(lastRaw);
}

// --- Setters ---
void AnalogSlider::setZero(int z) { zero = z; }
void AnalogSlider::setMinValue(int z) { minValue = z; }
void AnalogSlider::setMaxValue(int z) { maxValue = z; }
void AnalogSlider::setInterval(unsigned long i) { interval = i; }
void AnalogSlider::setInverted(bool inv) { inverted = inv; }
void AnalogSlider::setRawThreshold(int t) { rawThreshold = t; }

// --- Update ---
bool AnalogSlider::update() {
  unsigned long now = millis();
  if (now - lastRead < interval) return false;
  lastRead = now;

  int newRaw = analogRead(pin) - zero;

  if (newRaw <= minValue) {
    newRaw = inverted ? MAX_ANALOG : 0;
  } else if (newRaw >= maxValue) {
    newRaw = inverted ? 0 : MAX_ANALOG;
  } else {
    if (inverted) newRaw = MAX_ANALOG - newRaw;
  }

  newRaw = constrain(newRaw, 0, MAX_ANALOG);

  // Only update if change is significant
  if (abs(newRaw - lastRaw) < rawThreshold) return false;
  lastRaw = newRaw;

  int newPercent = getPercent(newRaw);
  if (newPercent != lastValue) {
    lastValue = newPercent;
    return true;
  }
  return false;
}

// --- Getters ---
int AnalogSlider::getRawValue() const { return lastRaw; }
int AnalogSlider::getPercentValue() const { return lastValue; }

// --- Internal helper ---
int AnalogSlider::getPercent(int raw) const {
  return map(raw, 0, MAX_ANALOG, 0, 100);
}
