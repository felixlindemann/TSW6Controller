/**
 * @file TSWControlRegistry.h
 * @brief Central registry for all TSWControl instances.
 *
 * @details
 * Provides:
 * - Registration of TSWControl instances
 * - JSON serialization for REST API (Angular integration)
 * - Filtering by control type
 * - Config update from JSON
 *
 * Usage:
 * @code
 *   TSWControlRegistry::registerControl(myLever);
 *   String json = TSWControlRegistry::toJson();
 *   TSWControlRegistry::updateFromJson(id, jsonDoc);
 * @endcode
 *
 * @author Felix Lindemann
 * @date 2025-12-25
 * @version 1.0
 */

#pragma once

#include <Arduino.h>
#include <vector>
#include <ArduinoJson.h>
#include "../TSW_Controls/TSWControl.h"
#include "../config.h"

class TSWControlRegistry {
private:
    static std::vector<TSWControl*> controls;

public:
    /**
     * Register a TSWControl instance.
     * @param control  Pointer to the TSWControl (not owned, must stay valid)
     * @return true if registered successfully
     */
    static bool registerControl(TSWControl* control);

    /**
     * Find a control by its controllerName.
     * @param controllerName  The TSW controller name
     * @return Pointer to the control, or nullptr if not found
     */
    static TSWControl* find(const String& controllerName);

    /**
     * Get all registered controls.
     */
    static const std::vector<TSWControl*>& getAll() { return controls; }

    /**
     * Get controls filtered by type.
     * @param type  Control type (e.g., "TSWLever", "TSWButton", "TSWMCPButton")
     */
    static std::vector<TSWControl*> getByType(const char* type);

    /**
     * Get count of registered controls.
     */
    static size_t count() { return controls.size(); }

    /**
     * Clear all registered controls.
     */
    static void clear() { controls.clear(); }

    // ==================== JSON Serialization ====================

    /**
     * Serialize all controls to JSON array.
     * @param doc  JsonDocument to populate
     */
    static void toJsonArray(JsonDocument& doc);

    /**
     * Serialize all controls to JSON string.
     * @return JSON string with array of all controls
     */
    static String toJsonString();

    /**
     * Serialize a single control to JSON.
     * @param controllerName  The TSW controller name
     * @param doc  JsonDocument to populate
     * @return true if control found and serialized
     */
    static bool toJson(const String& controllerName, JsonDocument& doc);

    /**
     * Get summary of all controls (minimal info for list view).
     * @param doc  JsonDocument to populate
     */
    static void toSummaryJson(JsonDocument& doc);

    // ==================== Config Update from Angular ====================

    /**
     * Update a control's configuration from JSON.
     * Supported fields:
     * - controllerName: Change the TSW controller name
     * - inverted: For lever/slider controls
     * - notches: Update the NotchTable
     *
     * @param controllerName  Current controller name to identify the control
     * @param json  JsonObject with the new configuration
     * @return true if update was successful
     */
    static bool updateFromJson(const String& controllerName, const JsonObject& json);

    /**
     * List all controls to Serial (debug).
     */
    static void listAll();
};
