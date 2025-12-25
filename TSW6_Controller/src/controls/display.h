#pragma once
#include "DisplayLog.h"

#include <Arduino.h>

#if USEDISPLAY
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#endif

#include "../config.h"
void setupDisplay();
void loopDisplay();

extern DisplayLog gDisplayLog;


   