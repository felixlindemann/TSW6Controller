/**
 * @file AnalogSlider.cpp
 * @brief Implementation of the AnalogSlider control.
 *
 * @author
 *   Felix Lindemann
 * @date
 *   2025-10-28
 * @version
 *   2.0
 */

#include "AnalogSlider.h"

// --- Constructor ---
AnalogSlider::AnalogSlider(const String &id, uint8_t gpio)
    : Control(id, gpio)
{
  minValue = 0;
  maxValue = MAX_ANALOG;
  zero = 0;
  rawThreshold = 10; // ADC steps required for change
  interval = 50;     // ms
  inverted = false;
  lastRead = 0;
  lastRaw = 0;
  lastValue = 0;
}

// --- Initialization ---
void AnalogSlider::begin()
{
  pinMode(pin, INPUT);

  #if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_ARCH_SAMD)
  analogSetPinAttenuation(pin, ADC_11db);   
  #endif
  delay(5); // small pause after USB init
  lastRaw = analogRead(pin) - zero;
  lastValue = getPercent(lastRaw);
  lastChangeReason = "init";
}

// --- Update ---
bool AnalogSlider::update()
{
  lastChangeReason = "none";
  unsigned long now = millis();
  if (now - lastRead < interval)
    return false;
  lastRead = now;

  int newRaw = analogRead(pin) - zero;
  newRaw = constrain(newRaw, 0, MAX_ANALOG);

  // --- Hardware inversion ---
  if (inverted)
    newRaw = MAX_ANALOG - newRaw;

  if (abs(newRaw - lastRaw) < rawThreshold)
    return false;
  lastRaw = newRaw;

  int newPercent = getPercent(newRaw);
  newPercent = constrain(newPercent, 0, 100);

  if (newPercent != lastValue)
  {
    lastValue = newPercent;
    lastChangeReason = "moved";
    return true;
  }
  return false;
}

// --- Value Getters ---
float AnalogSlider::getValue() const
{
  float val = static_cast<float>(lastValue) / 100.0f;
  return constrain(val, 0.0f, 1.0f);
}

int AnalogSlider::getRawValue() const { return constrain(lastRaw, 0, MAX_ANALOG); }
int AnalogSlider::getPercentValue() const { return constrain(lastValue, 0, 100); }

// --- Config setters ---
void AnalogSlider::setZero(int z) { zero = z; }
void AnalogSlider::setMinValue(int z) { minValue = z; }
void AnalogSlider::setMaxValue(int z) { maxValue = z; }
void AnalogSlider::setInterval(unsigned long i) { interval = i; }
void AnalogSlider::setInverted(bool inv) { inverted = inv; }
void AnalogSlider::setRawThreshold(int t) { rawThreshold = t; }

// --- Helper ---
int AnalogSlider::getPercent(int raw) const
{
  raw = constrain(raw, 0, MAX_ANALOG);
  return map(raw, 0, MAX_ANALOG, 0, 100);
}
