/**
 * @file TSWButton.cpp
 * @brief Implementation of the TSWButton class for binary TSW controls.
 *
 * @details
 * Initializes a default two-notch table (Released=0, Pressed=1)
 * and sends mapped values to TSWSpider when the button state changes.
 */

#include "TSWButton.h"
#include <cmath>  // for fabs

// --- Constructor ---
TSWButton::TSWButton(uint8_t pin, const String& ctrl, TSWSpider* s)
    : Button(ctrl + "_HW", pin),   // new Control-compatible ctor
      TSWControl(ctrl, s),
      lastSentValue(-1.0f) {

  // --- Default notch table for binary buttons ---
  Notch released = { "Released", 0.0f, 0, 0 };
  Notch pressed  = { "Pressed",  1.0f, 1, 1 };
  notches.loadFromArray({ released, pressed });
}

// --- Optional: load custom NotchTable from SD ---
void TSWButton::loadNotches(const String& filePath) {
  notches.loadFromFile(filePath);
}

// --- Update & send mapped value ---
void TSWButton::updateAndSend() {
  int event = update();  // +1 pressed / -1 released
  if (event == 0) return;

  float value = isPressed() ? 1.0f : 0.0f;
  if (notches.hasPositions())
    value = notches.mapToTSW(isPressed() ? 100 : 0);

  if (fabs(value - lastSentValue) > 0.001f) {
    spider->setControllerValue(controllerName, value);
    lastSentValue = value;
  }
}
