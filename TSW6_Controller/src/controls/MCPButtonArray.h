/**
 * @file MCPButtonArray.h
 * @brief Multi-button input handler for MCP23S17 port expanders (Rob Tillaart library).
 *
 * - Supports NUM_MCP_EXPANDERS MCP23S17 chips
 * - 16 buttons per chip
 * - Software debouncing per button
 * - Registers itself + proxies in ControlRegistry
 *
 * Electrical assumption:
 * - Buttons connect MCP pin -> GND
 * - Pull-ups enabled (internal via MCP + optional external 10k)
 * - Therefore: HIGH = released, LOW = pressed
 */

#pragma once

#include <Arduino.h>
#include <SPI.h>

#include "Control.h"
#include "MCP23S17.h"
#include "../config.h"

#ifndef NUM_MCP_EXPANDERS
#define NUM_MCP_EXPANDERS 2
#endif

#define BUTTONS_PER_EXPANDER 16
#define TOTAL_BUTTONS (NUM_MCP_EXPANDERS * BUTTONS_PER_EXPANDER)

class MCPButtonArray : public Control
{
public:
    explicit MCPButtonArray(const String &idPrefix = "BTN", unsigned int debounceMs = 50);

    void begin() override;
    bool update() override;
    float getValue() const override;

    bool getButtonState(uint8_t index) const; 
    uint32_t getLastReading() const { return lastReading; }
    uint32_t getChangedPins() const { return changedPins; }

private:
    static constexpr uint8_t kMaxExpanders = NUM_MCP_EXPANDERS; // adjust if you want >2 later

    // SPI pin mapping: {SCK, MISO, MOSI, CS}
    const uint8_t spiPins[4] = { PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, PIN_SPI_CS };
      uint32_t lastReading = 0; 
      uint32_t changedPins = 0; 

    // Hardware address bits (A2..A0) per chip
    static constexpr uint8_t MCP_HW_ADDRESS_1 = 0; // 0b000
    static constexpr uint8_t MCP_HW_ADDRESS_2 = 1; // 0b001

    // MCP23S17 instances (common CS, different HW addresses)
    MCP23S17 mcp1;
    MCP23S17 mcp2;

    // Pointers to active expanders (size = NUM_MCP_EXPANDERS)
    MCP23S17* expanders[NUM_MCP_EXPANDERS];
    uint8_t expanderCount = NUM_MCP_EXPANDERS;

    // Debounce + state tracking
    bool states[TOTAL_BUTTONS];               // debounced states (LOW = pressed)
    bool readings[TOTAL_BUTTONS];             // last raw readings
    unsigned long debounceTimes[TOTAL_BUTTONS];

    unsigned int debounceDelayMs;
    int lastEventIndex;
    unsigned long lastPollTimeMs;

private:
    void initExpanderOrHalt(MCP23S17 &mcp, uint8_t chipIndex);
    void haltWithMcpError(uint8_t chipIndex, const char *msg);
};
