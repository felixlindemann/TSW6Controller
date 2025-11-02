/**
 * @file TSWRotaryKnob.cpp
 * @brief Implementation of the TSWRotaryKnob class for TSW incremental controls.
 *
 * @details
 * Uses RotaryKnob hardware input, applies NotchTable mapping,
 * and sends mapped values via TSWSpider.
 */
#include "TSWRotaryKnob.h"

// --- Constructor ---
TSWRotaryKnob::TSWRotaryKnob(const String& id, uint8_t a, uint8_t b,
                             TSWSpider* spider, float minVal, float maxVal)
    : RotaryKnob(id, a, b), TSWControl(id, spider),
      minValue(minVal), maxValue(maxVal), currentTSWValue(0.0f) {}

// --- Legacy overload (keeps compatibility) ---
TSWRotaryKnob::TSWRotaryKnob(const String& id, uint8_t a, uint8_t b,
                             int steps, float minVal, float maxVal)
    : RotaryKnob(id, a, b), TSWControl(id, nullptr),
      minValue(minVal), maxValue(maxVal), currentTSWValue(0.0f) {
  (void)steps;
}

// --- Begin ---
void TSWRotaryKnob::begin() {
  RotaryKnob::begin();  // use hardware init
}

// --- Update ---
bool TSWRotaryKnob::update() {
  bool changed = RotaryKnob::update();
  if (!changed) return false;

  // Map hardware value (-1.0/+1.0) to TSW range (min/max)
  float hw = getValue();   // from RotaryKnob
  if (hw > 0)
    currentTSWValue = maxValue;
  else if (hw < 0)
    currentTSWValue = minValue;
  else
    return false;

  // optional: trace
  // Serial.printf("[TSWRotaryKnob] %s => %.2f (%s)\n",
  //               getId().c_str(), currentTSWValue, getChangeReason());

  return true;
}

// --- Update + Send ---
void TSWRotaryKnob::updateAndSend() {
  if (update()) {
    sendValueToTSW(currentTSWValue);
  }
}
