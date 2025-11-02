/**
 * @file RotaryKnob.cpp
 * @brief Interrupt-based rotary encoder implementation for ESP32.
 *
 * @details
 * Uses full quadrature decoding (Gray code state table)
 * to determine direction without ambiguity.
 */

#include "RotaryKnob.h"

// --- Constructor ---
RotaryKnob::RotaryKnob(const String &id, uint8_t a, uint8_t b)
    : Control(id, 0), pinA(a), pinB(b),
      encoderDelta(0), lastState(0), lastChange(0) {}

      RotaryKnob::RotaryKnob(const String &id, uint8_t a, uint8_t b, int steps, float minVal, float maxVal)
    : RotaryKnob(id, a, b)
{
  // Parameters are ignored in this minimal implementation,
  // but this overload ensures backward compatibility with older code.
  (void)steps;
  (void)minVal;
  (void)maxVal;
}

// --- ISR Handlers ---
void IRAM_ATTR RotaryKnob::handleInterruptA(void *arg)
{
  RotaryKnob *self = static_cast<RotaryKnob *>(arg);
  uint8_t a = digitalRead(self->pinA);
  uint8_t b = digitalRead(self->pinB);
  uint8_t state = (a << 1) | b;

  static const int8_t table[4][4] = {
      {0, -1, +1, 0},
      {+1, 0, 0, -1},
      {-1, 0, 0, +1},
      {0, +1, -1, 0}};

  self->encoderDelta += table[self->lastState][state];
  self->lastState = state;
}

void IRAM_ATTR RotaryKnob::handleInterruptB(void *arg)
{
  handleInterruptA(arg); // identical processing
}

// --- Begin ---
void RotaryKnob::begin()
{
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);

  lastState = (digitalRead(pinA) << 1) | digitalRead(pinB);
  encoderDelta = 0;

  attachInterruptArg(digitalPinToInterrupt(pinA), handleInterruptA, this, CHANGE);
  attachInterruptArg(digitalPinToInterrupt(pinB), handleInterruptB, this, CHANGE);

  lastChange = millis();
}

// --- Update ---
bool RotaryKnob::update()
{
  lastChangeReason = "none";
  int8_t delta = encoderDelta;
  if (delta == 0)
    return false;
  encoderDelta = 0;

  if (delta > 0)
  {
    lastChangeReason = "turnRight";
    value = +1.0f;
  }
  else
  {
    lastChangeReason = "turnLeft";
    value = -1.0f;
  }

  lastChange = millis();
  return true;
}

// --- Get Value ---
float RotaryKnob::getValue() const
{
  return value;
}

// --- Reset ---
void RotaryKnob::reset()
{
  encoderDelta = 0;
  value = 0.0f;
  lastChangeReason = "none";
}
