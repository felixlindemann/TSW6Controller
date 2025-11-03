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
 *   2025-11-02
 * @version
 *   2.2
 */

#include "GamepadJoystick.h"
#include <math.h>

// --- Constructor ---
GamepadJoystick::GamepadJoystick(const String &id, uint8_t x, uint8_t y, uint8_t button)
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
void GamepadJoystick::begin()
{
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(pin, INPUT_PULLUP);

#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_ARCH_SAMD)
  analogSetPinAttenuation(xPin, ADC_11db);
  analogSetPinAttenuation(yPin, ADC_11db);
  analogReadResolution(12);
#endif

  delay(10);
  calibrateCenter();
}

// --- Update ---
bool GamepadJoystick::update()
{
  lastChangeReason = "none"; // reset reason at start of each update

  unsigned long now = millis();
  if (now - lastRead < interval)
    return false;
  lastRead = now;

  int newX = analogRead(xPin);
  int newY = analogRead(yPin);
  bool changed = false;

  if (abs(newX - xRaw) > xThreshold)
  {
    xRaw = newX;
    changed = true;
    lastChangeReason = "axisX";
  }
  if (abs(newY - yRaw) > yThreshold)
  {
    yRaw = newY;
    changed = true;
    lastChangeReason = "axisY";
  }

  lastButtonPressed = buttonPressed;
  buttonPressed = (digitalRead(pin) == LOW);
  if (buttonPressed != lastButtonPressed)
  {
    changed = true; 
      Serial.printf(" [GAMEPAD BUTTON %s]\n",
                  buttonPressed ? "pressed" : "released");
  
    lastChangeReason = "button";
  }

  return changed;
}

// --- Value (vector magnitude 0.0–1.0) ---
float GamepadJoystick::getValue() const
{
  float x = getXCentered() / 100.0f;
  float y = getYCentered() / 100.0f;
  float mag = sqrtf(x * x + y * y);
  return constrain(mag, 0.0f, 1.0f);
}

// --- Center calibration with averaging ---
void GamepadJoystick::calibrateCenter()
{
  const int samples = 20;
  long sumX = 0, sumY = 0;

  for (int i = 0; i < samples; i++)
  {
    sumX += analogRead(xPin);
    sumY += analogRead(yPin);
    delay(5);
  }

  xZero = sumX / samples;
  yZero = sumY / samples;

  Serial.printf("[CALIB] Joystick centerX=%d, centerY=%d\n", xZero, yZero);
}

// --- Centered conversion −100 … +100 % (symmetrical around center) ---
int GamepadJoystick::toCentered(int raw, int zero, int deadZone) const
{
  int diff = raw - zero;

  // Apply dead zone
  if (abs(diff) < deadZone)
    return 0;

  // Normalize to a symmetrical ±100% range around center
  float centered = static_cast<float>(diff) / static_cast<float>(MAX_ANALOG / 2);
  centered = constrain(centered, -1.0f, 1.0f);
  return static_cast<int>(centered * 100.0f);
}

int GamepadJoystick::getXCentered() const
{
  int centered = toCentered(xRaw, xZero, xDeadZone);
  return xInverted ? -centered : centered;
}

int GamepadJoystick::getYCentered() const
{
  int centered = toCentered(yRaw, yZero, yDeadZone);
  return yInverted ? -centered : centered;
}
