/**
 * @file GamepadJoystick.cpp
 * @brief Implementation of the GamepadJoystick class.
 *
 * @details
 * Contains all method definitions for the 2-axis analog joystick
 * with optional button, including input reading, filtering,
 * smoothing, and value normalization for HID or UI usage.
 *
 * @author Felix Lindemann
 * @date 2025-10-26
 * @version 1.0
 *
 * @copyright
 * This code is part of the TSW Controller Project.
 * Licensed under the MIT License.
 */

 #include "GamepadJoystick.h"

// --- Constructors ---
GamepadJoystick::GamepadJoystick(uint8_t x, uint8_t y)
    : xPin(x), yPin(y), btnPin(255), hasButton(false) {
  defaultInit();
}

GamepadJoystick::GamepadJoystick(uint8_t x, uint8_t y, uint8_t button)
    : xPin(x), yPin(y), btnPin(button), hasButton(true) {
  defaultInit();
}

// --- Initialization ---
void GamepadJoystick::begin() {
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  if (hasButton) pinMode(btnPin, INPUT_PULLUP);
  detectAnalogResolution();
  calibrateCenter();
}

void GamepadJoystick::calibrateCenter() {
  delay(20);
  xZero = analogRead(xPin);
  yZero = analogRead(yPin);
  xRaw = 0;
  yRaw = 0;
}

// --- Configuration ---
void GamepadJoystick::setXDeadZone(int dz) { xDeadZone = dz; }
void GamepadJoystick::setYDeadZone(int dz) { yDeadZone = dz; }
void GamepadJoystick::setXThreshold(int t) { xThreshold = t; }
void GamepadJoystick::setYThreshold(int t) { yThreshold = t; }
void GamepadJoystick::setXInverted(bool inv) { xInverted = inv; }
void GamepadJoystick::setYInverted(bool inv) { yInverted = inv; }
void GamepadJoystick::setInterval(unsigned long i) { interval = i; }
void GamepadJoystick::setSmoothing(int s) { smoothFactor = constrain(s, 0, 10); }

// --- Update ---
bool GamepadJoystick::update() {
  unsigned long now = millis();
  if (now - lastRead < interval) return false;
  lastRead = now;

  bool changed = false;
  int rawX = analogRead(xPin) - xZero;
  int rawY = analogRead(yPin) - yZero;

  // Dead zone
  if (abs(rawX) < xDeadZone) rawX = 0;
  if (abs(rawY) < yDeadZone) rawY = 0;

  // Smoothing
  if (smoothFactor > 0) {
    rawX = (xRaw * smoothFactor + rawX) / (smoothFactor + 1);
    rawY = (yRaw * smoothFactor + rawY) / (smoothFactor + 1);
  }

  // Threshold / change detection
  if (abs(rawX - xRaw) >= xThreshold) { xRaw = rawX; changed = true; }
  if (abs(rawY - yRaw) >= yThreshold) { yRaw = rawY; changed = true; }

  // Button
  if (hasButton) {
    lastButtonPressed = buttonPressed;
    buttonPressed = (digitalRead(btnPin) == LOW);
    if (buttonPressed != lastButtonPressed) changed = true;
  }

  return changed;
}

// --- Getters ---
int GamepadJoystick::getXRaw() const { return xInverted ? -xRaw : xRaw; }
int GamepadJoystick::getYRaw() const { return yInverted ? -yRaw : yRaw; }

bool GamepadJoystick::isPressed() const {
  return hasButton ? buttonPressed : false;
}

int GamepadJoystick::getXPercent() const {
  int val = map(getXRaw() + maxAnalog / 2, 0, maxAnalog, 0, 100);
  return constrain(val, 0, 100);
}

int GamepadJoystick::getYPercent() const {
  int val = map(getYRaw() + maxAnalog / 2, 0, maxAnalog, 0, 100);
  return constrain(val, 0, 100);
}

int GamepadJoystick::getXCentered() const {
  int val = map(getXRaw(), -maxAnalog / 2, maxAnalog / 2, -100, 100);
  return constrain(val, -100, 100);
}

int GamepadJoystick::getYCentered() const {
  int val = map(getYRaw(), -maxAnalog / 2, maxAnalog / 2, -100, 100);
  return constrain(val, -100, 100);
}

int GamepadJoystick::getXHID() const {
  return map(getXCentered(), -100, 100, -32767, 32767);
}

int GamepadJoystick::getYHID() const {
  return map(getYCentered(), -100, 100, -32767, 32767);
}

// --- Internal helpers ---
void GamepadJoystick::defaultInit() {
  xZero = 0;
  yZero = 0;
  xDeadZone = 30;
  yDeadZone = 30;
  xThreshold = 3;
  yThreshold = 3;
  xInverted = false;
  yInverted = false;
  interval = 10;
  smoothFactor = 2;
  lastRead = 0;
  xRaw = 0;
  yRaw = 0;
  buttonPressed = false;
  lastButtonPressed = false;
  maxAnalog = 1023;  // default, overwritten if board supports detection
}

void GamepadJoystick::detectAnalogResolution() {
#if defined(ESP32) || defined(ESP8266)
  // ESP boards support 12-bit ADCs
  maxAnalog = 4095;
#elif defined(ARDUINO_ARCH_SAMD)
  // e.g. Arduino Zero
  maxAnalog = 4095;
#else
  // Default for classic 10-bit AVR boards
  maxAnalog = 1023;
#endif
}
