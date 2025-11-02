/**
 * @file TSWGamePadControl.cpp
 * @brief Implementation of the TSWGamePadControl.
 *
 * @details
 * Uses GamepadJoystick as physical input (X/Y + Button)
 * and sends mapped TSW values through TSWSpider.
 */

#include "TSWGamePadControl.h"
#include <cmath> // fabs

// --- Constructor ---
TSWGamePadControl::TSWGamePadControl(const String &id,
                                     uint8_t pinX, uint8_t pinY, uint8_t pinButton,
                                     const String &ctrlX, const String &ctrlY,
                                     const String &ctrlBtn,
                                     TSWSpider *s,
                                     unsigned long sendInt)
    : TSWControl(id, s),
      gamepad(id + "_HW", pinX, pinY, pinButton),
      hasButton(true),
      controllerX(ctrlX),
      controllerY(ctrlY),
      controllerButton(ctrlBtn),
      lastSentTime(0),
      sendInterval(sendInt)
{
  // Default NotchTable for button
  Notch released = {"Released", 0.0f, 0, 0};
  Notch pressed = {"Pressed", 1.0f, 1, 1};
  buttonNotches.loadFromArray({released, pressed});
}

// --- Begin ---
void TSWGamePadControl::begin()
{
  gamepad.begin();
}

// --- Load custom notch mappings ---
void TSWGamePadControl::loadNotchesX(const String &filePath) { notchX.loadFromFile(filePath); }
void TSWGamePadControl::loadNotchesY(const String &filePath) { notchY.loadFromFile(filePath); }
void TSWGamePadControl::loadButtonNotches(const String &filePath) { buttonNotches.loadFromFile(filePath); }

// --- Update and send ---
void TSWGamePadControl::updateAndSend()
{
  if (!spider)
    return;
  if (!gamepad.update())
    return;

  unsigned long now = millis();

  // X axis
  int xVal = gamepad.getXCentered(); // −100 … +100
  float tswX = notchX.hasPositions() ? notchX.mapToTSW(xVal) : xVal / 100.0f;
  spider->setControllerValue(controllerX, tswX);

  // Y axis
  int yVal = gamepad.getYCentered(); // −100 … +100
  float tswY = notchY.hasPositions() ? notchY.mapToTSW(yVal) : yVal / 100.0f;
  spider->setControllerValue(controllerY, tswY);

  // Button
  float btnVal = buttonNotches.mapToTSW(gamepad.isPressed() ? 100 : 0);
  spider->setControllerValue(controllerButton, btnVal);

  lastSentTime = now;
}
