#pragma once
#include "DisplayLog.h"

#include <Arduino.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "../config.h"
void setupDisplay();
void loopDisplay();

extern DisplayLog gDisplayLog;


   