/**
 * @file TSWRotaryKnob.cpp
 * @brief Implementation of the TSWRotaryKnob class for TSW incremental controls.
 *
 * @details
 * Uses RotaryKnob hardware input, applies NotchTable mapping,
 * and sends mapped values via TSWSpider.
 */

#include "TSWRotaryKnob.h"
#include <cmath>

// --- Constructor ---
TSWRotaryKnob::TSWRotaryKnob(uint8_t pinA, uint8_t pinB, const String& ctrl,
                             TSWSpider* s, float minV, float maxV,
                             float step, unsigned long sendInt)
    : RotaryKnob(ctrl + "_HW", pinA, pinB, 2, minV, maxV),
      TSWControl(ctrl, s),
      lastValue(0.0f),
      minValue(minV),
      maxValue(maxV),
      stepSize(step),
      lastSentTime(0),
      sendInterval(sendInt) {}

// --- Load Notch configuration ---
void TSWRotaryKnob::loadNotches(const String& filePath) {
  notches.loadFromFile(filePath);
}

// --- Update and send value ---
void TSWRotaryKnob::updateAndSend() {
  int delta = update();   // +1 / -1 / 0
  if (delta == 0) return;

  lastValue += delta * stepSize;
  lastValue = constrain(lastValue, minValue, maxValue);

  float tswValue = notches.hasPositions()
                       ? notches.mapToTSW(int(lastValue * 100))
                       : lastValue;

  unsigned long now = millis();
  if (now - lastSentTime >= sendInterval) {
    sendValueToTSW(tswValue);
    lastSentTime = now;
  }
}
