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
#include <LittleFS.h>
#include "../config.h"

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
    String label;
    std::vector<Notch> positions;

public:
    bool loadFromFile(const String &path);
    bool loadFromArray(const std::vector<Notch> &list);
    bool hasPositions() const { return !positions.empty(); }
    size_t getPositionCount() const { return positions.size(); }

    float mapToTSW(int percent) const;
    
    const String &getControllerName() const { return controller; }
    void setControllerName(const String &name) { controller = name; }
    void setLabel(const String &lbl) { label = lbl; }
    const String &getLabel() const { return label; }
    void getPositions(std::vector<Notch> &out) const { out = positions; }
    void clear() { positions.clear(); }
    
    /**
     * Helper to create a simple binary button mapping (Released/Pressed).
     * @param controllerName  The TSW controller name
     * @param releasedValue   Value when button is released (default: 0.0)
     * @param pressedValue    Value when button is pressed (default: 1.0)
     */
    void setupBinaryButton(const String &controllerName, 
                           float releasedValue = 0.0f, 
                           float pressedValue = 1.0f);
};
