/**
 * @file TSWMCPButtonArray.setup.h
 * @brief Setup for MCP23S17 button array with individual TSWMCPButton instances.
 *
 * @details
 * Architecture:
 * - One shared MCPButtonArray (hardware layer) handles all MCP23S17 expanders
 * - Individual TSWMCPButton instances reference the shared array
 * - Each TSWMCPButton is a TSWControl, enabling runtime discovery and web config
 */

#pragma once

#include "../config.h"
#include "../controls/Control.h"
#include "../controls/MCPButtonArray.h"
#include "../repo/controlsRepo.h"

#if USE_MCP_BUTTON_ARRAY

#include "TSWMCPButton.h"
#include <vector>

// Shared hardware layer - single instance for all buttons
static MCPButtonArray* sharedButtonArray = nullptr;

// Collection of individual button controls
static std::vector<TSWMCPButton*> mcpButtons;

/**
 * Initialize the shared MCPButtonArray hardware layer.
 * Call this once before creating any TSWMCPButton instances.
 */
inline void initMCPButtonArrayHardware(unsigned int debounceMs = 50)
{
    static MCPButtonArray buttonArrayInstance("MCP_HW", debounceMs);
    buttonArrayInstance.begin();
    sharedButtonArray = &buttonArrayInstance;
    LOG_HW_INFO("MCPButtonArray hardware initialized\n");
}

/**
 * Create a TSWMCPButton for a specific button index.
 * The button is registered and can be discovered at runtime.
 *
 * @param id        Controller name (e.g., "Horn", "Sander")
 * @param spider    TSWSpider instance
 * @param index     Button index (0..TOTAL_BUTTONS-1)
 * @param pressed   Value when pressed (default: 1.0)
 * @param released  Value when released (default: 0.0)
 * @return Pointer to the created button (owned by static vector)
 */
inline TSWMCPButton* createMCPButton(const String& id,
                                      TSWSpider* spider,
                                      uint8_t index,
                                      float pressed = 1.0f,
                                      float released = 0.0f)
{
    if (!sharedButtonArray) {
        LOG_SW_ERROR("MCPButtonArray hardware not initialized!\n");
        return nullptr;
    }
    if (index >= TOTAL_BUTTONS) {
        LOG_SW_ERROR("Button index %d out of range (max %d)\n", index, TOTAL_BUTTONS - 1);
        return nullptr;
    }

    TSWMCPButton* btn = new TSWMCPButton(id, spider, sharedButtonArray, index);
    btn->setMapping(released, pressed);
    mcpButtons.push_back(btn);
    
    LOG_SW_INFO("Created TSWMCPButton[%d]: %s\n", index, id.c_str());
    return btn;
}

/**
 * Setup example: create buttons with specific mappings.
 * Customize this function for your hardware configuration.
 */
inline void setup_MCPButtonArray(TSWSpider* spider)
{
    // Initialize hardware layer first
    initMCPButtonArrayHardware(50);

    // Example button mappings - customize for your setup:
    // createMCPButton("Horn", spider, 0);
    // createMCPButton("Sander", spider, 1);
    // createMCPButton("Bell", spider, 2);
    // createMCPButton("Wiper", spider, 3, 1.0f, 0.0f);
    
    // Or create all buttons with a prefix pattern:
    // for (uint8_t i = 0; i < TOTAL_BUTTONS; i++) {
    //     char name[16];
    //     snprintf(name, sizeof(name), "BTN_%02d", i);
    //     createMCPButton(name, spider, i);
    // }

    LOG_SW_INFO("MCP Button setup complete: %d buttons configured\n", mcpButtons.size());
}

/**
 * Update all MCP buttons. Call this in the main loop.
 * First updates the shared hardware, then each button checks for changes.
 */
inline void update_MCPButtonArray()
{
    if (!sharedButtonArray) return;

    // Update hardware layer once (reads all expanders, handles debounce)
    sharedButtonArray->update();

    // Each button checks if it changed and sends update
    for (auto* btn : mcpButtons) {
        if (btn) btn->updateAndSend();
    }
}

/**
 * Get all registered MCP buttons (for web service enumeration)
 */
inline const std::vector<TSWMCPButton*>& getMCPButtons() {
    return mcpButtons;
}

/**
 * Get the shared MCPButtonArray hardware instance
 */
inline MCPButtonArray* getSharedButtonArray() {
    return sharedButtonArray;
}

#define SETUP_MCP_BUTTON_ARRAY(spiderPtr) setup_MCPButtonArray(spiderPtr)
#define UPDATE_MCP_BUTTON_ARRAY() update_MCPButtonArray()

#else
// Stubs when MCP buttons are disabled
#define SETUP_MCP_BUTTON_ARRAY(...)
#define UPDATE_MCP_BUTTON_ARRAY()

inline const std::vector<void*> getMCPButtons() { return {}; }
#endif
