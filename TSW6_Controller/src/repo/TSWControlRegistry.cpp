/**
 * @file TSWControlRegistry.cpp
 * @brief Implementation of TSWControlRegistry.
 */

#include "TSWControlRegistry.h"

// Static member initialization
std::vector<TSWControl*> TSWControlRegistry::controls;

bool TSWControlRegistry::registerControl(TSWControl* control) {
    if (!control) return false;

    // Check if already registered (by controllerName)
    for (auto* c : controls) {
        if (c->getControllerName() == control->getControllerName()) {
            LOG_SW_WARN("TSWControl '%s' already registered\n", 
                        control->getControllerName().c_str());
            return false;
        }
    }

    controls.push_back(control);
    LOG_SW_INFO("Registered TSWControl: %s [%s]\n", 
                control->getControllerName().c_str(),
                control->getControlType());
    return true;
}

TSWControl* TSWControlRegistry::find(const String& controllerName) {
    for (auto* c : controls) {
        if (c->getControllerName() == controllerName) {
            return c;
        }
    }
    return nullptr;
}

std::vector<TSWControl*> TSWControlRegistry::getByType(const char* type) {
    std::vector<TSWControl*> result;
    for (auto* c : controls) {
        if (strcmp(c->getControlType(), type) == 0) {
            result.push_back(c);
        }
    }
    return result;
}

void TSWControlRegistry::toJsonArray(JsonDocument& doc) {
    JsonArray arr = doc.to<JsonArray>();
    for (auto* c : controls) {
        JsonObject obj = arr.createNestedObject();
        c->toJson(obj);
    }
}

String TSWControlRegistry::toJsonString() {
    DynamicJsonDocument doc(8192);
    toJsonArray(doc);
    String result;
    serializeJson(doc, result);
    return result;
}

bool TSWControlRegistry::toJson(const String& controllerName, JsonDocument& doc) {
    TSWControl* c = find(controllerName);
    if (!c) return false;
    
    JsonObject obj = doc.to<JsonObject>();
    c->toJson(obj);
    return true;
}

void TSWControlRegistry::toSummaryJson(JsonDocument& doc) {
    JsonArray arr = doc.to<JsonArray>();
    for (auto* c : controls) {
        JsonObject obj = arr.createNestedObject();
        obj["controllerName"] = c->getControllerName();
        obj["type"] = c->getControlType();
        obj["hardwareType"] = c->getHardwareType();
    }
}

bool TSWControlRegistry::updateFromJson(const String& controllerName, const JsonObject& json) {
    TSWControl* c = find(controllerName);
    if (!c) {
        LOG_SW_ERROR("Control not found: %s\n", controllerName.c_str());
        return false;
    }

    // Update controllerName if provided
    if (json.containsKey("controllerName")) {
        String newName = json["controllerName"].as<String>();
        if (newName.length() > 0 && newName != controllerName) {
            c->setControllerName(newName);
            LOG_SW_INFO("Updated controllerName: %s -> %s\n", 
                        controllerName.c_str(), newName.c_str());
        }
    }

    // Update notches if provided
    if (json.containsKey("notches")) {
        JsonObject notchesJson = json["notches"];
        // TODO: Implement NotchTable::fromJson() for full notch updates
        LOG_SW_INFO("Notches update requested for: %s (not yet implemented)\n", 
                    controllerName.c_str());
    }

    return true;
}

void TSWControlRegistry::listAll() {
    LOG_SW_INFO("=== Registered TSWControls (%d) ===\n", controls.size());
    for (auto* c : controls) {
        LOG_SW_INFO("  %s [%s] hw:%s\n", 
                    c->getControllerName().c_str(),
                    c->getControlType(),
                    c->getHardwareType());
    }
}
