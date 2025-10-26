/**
 * @file NotchTable.h
 * @brief Class representing a mapping between slider percentage and TSW values.
 *
 * @details
 * The NotchTable holds a list of named notches that define how an analog input
 * (0–100 %) maps to a specific TSW controller value (0.0–1.0). Each notch entry
 * defines a label, target value and percent range.
 *
 * Typical usage:
 * @code
 *   NotchTable table;
 *   table.loadFromFile("/sd/notches/throttle.json");
 *   float value = table.mapToTSW(42);
 * @endcode
 *
 * @author Felix Lindemann
 * @date 2025-10-26
 * @version 1.0
 *
 * @copyright
 * This code is part of the TSW Controller Project.
 * Licensed under the MIT License.
 */

#pragma once
#include <Arduino.h>
#include <vector>
#include <ArduinoJson.h>
#include <FS.h>
#include <SD.h>

struct Notch
{
    String label;
    float tswValue;
    int rangeMin;
    int rangeMax;
};

class NotchTable
{
private:
    String controller;
    std::vector<Notch> positions;

public:
    bool loadFromFile(const String &path);
    bool loadFromArray(const std::vector<Notch> &list);
    bool hasPositions() const { return !positions.empty(); }

    float mapToTSW(int percent) const;
    const String &getControllerName() const { return controller; }
};
