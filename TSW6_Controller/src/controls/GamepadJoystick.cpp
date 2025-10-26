/**
 * @file GamepadJoystick.cpp
 * @brief Implementation of the GamepadJoystick Control.
 *
 * @details
 * Implements Control interface for a two-axis joystick with button.
 * X/Y axes return centered percentage values (−100 … +100 %).
 * The button is bound to the Control::pin.
 *
 * @author
 *   Felix Lindemann
 * @date
 *   2025-10-28
 * @version
 *   2.0
 */

#include "GamepadJoystick.h"
#include <math.h>

// --- Constructor ---
GamepadJoystick::GamepadJoystick(const String& id, uint8_t x, uint8_t y, uint8_t button)
    : Control(id, button),
      xPin(x),
      yPin(y),
      xZero(0),
      yZero(0),
      xDeadZone(50),
      yDeadZone(50),
      xThreshold(20),
      yThreshold(20),
      xInverted(false),
      yInverted(false),
      interval(50),
      buttonPressed(false),
      lastButtonPressed(false),
      lastRead(0),
      xRaw(0),
      yRaw(0) {}

// --- Initialization ---
void GamepadJoystick::begin() {
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(pin, INPUT_PULLUP);
  delay(10);
  calibrateCenter();
}

// --- Update ---
bool GamepadJoystick::update() {
  unsigned long now = millis();
  if (now - lastRead < interval) return false;
  lastRead = now;

  int newX = analogRead(xPin);
  int newY = analogRead(yPin);
  bool changed = false;

  if (abs(newX - xRaw) > xThreshold) {
    xRaw = newX;
    changed = true;
  }
  if (abs(newY - yRaw) > yThreshold) {
    yRaw = newY;
    changed = true;
  }

  lastButtonPressed = buttonPressed;
  buttonPressed = (digitalRead(pin) == LOW);
  if (buttonPressed != lastButtonPressed) changed = true;

  return changed;
}

// --- Value (vector magnitude 0.0–1.0) ---
float GamepadJoystick::getValue() const {
  float x = getXCentered() / 100.0f;
  float y = getYCentered() / 100.0f;
  float mag = sqrtf(x * x + y * y);
  return constrain(mag, 0.0f, 1.0f);
}

// --- Center calibration ---
void GamepadJoystick::calibrateCenter() {
  xZero = analogRead(xPin);
  yZero = analogRead(yPin);
}

// --- Centered values −100 … +100 % ---
int GamepadJoystick::toCentered(int raw, int zero) const {
  int diff = raw - zero;
  if (abs(diff) < xDeadZone) return 0;
  int range = MAX_ANALOG / 2;
  diff = constrain(diff, -range, range);
  return map(diff, -range, range, -100, 100);
}

int GamepadJoystick::getXCentered() const {
  int centered = toCentered(xRaw, xZero);
  return xInverted ? -centered : centered;
}

int GamepadJoystick::getYCentered() const {
  int centered = toCentered(yRaw, yZero);
  return yInverted ? -centered : centered;
}
