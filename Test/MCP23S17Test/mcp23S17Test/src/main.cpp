#include <Arduino.h>
#include <SPI.h>
#include <MCP23S17.h>

// --- ESP32 / Lolin32 Lite pins ---
constexpr uint8_t PIN_LED = 22; // On-board LED or external LED
constexpr uint8_t PIN_SPI_SCK = 18;
constexpr uint8_t PIN_SPI_MISO = 19;
constexpr uint8_t PIN_SPI_MOSI = 23;

// Shared CS for both MCP23S17
constexpr uint8_t PIN_CS_MCP = 5;

// Hardware address bits (A2..A0) per chip
// MCP #1: A2=0, A1=0, A0=0 -> address 0
// MCP #2: A2=0, A1=0, A0=1 -> address 1
constexpr uint8_t MCP_HW_ADDRESS_1 = 0; // 0b000
constexpr uint8_t MCP_HW_ADDRESS_2 = 1; // 0b001

// MCP23S17 instances (hardware SPI, shared CS, different addresses)
MCP23S17 mcp1(PIN_CS_MCP, MCP_HW_ADDRESS_1);
MCP23S17 mcp2(PIN_CS_MCP, MCP_HW_ADDRESS_2);

// Last known state of all 16 pins (bit 0..15)
// With INPUT_PULLUP + button to GND: 1 = released, 0 = pressed
uint16_t lastState1 = 0xFFFF;
uint16_t lastState2 = 0xFFFF;

// LED state
bool ledState = false;

// Toggle ESP32 LED
void toggleLed()
{
    ledState = !ledState;
    digitalWrite(PIN_LED, ledState ? HIGH : LOW);

    Serial.print("LED toggled: ");
    Serial.println(ledState ? "ON" : "OFF");
}

// Initial setup per MCP: hardware addressing + all pins as input with pull-up
void setupMCP(MCP23S17 &mcp, const char *name)
{
    // Enable hardware addressing (important for shared CS + A0..A2)
    if (!mcp.enableHardwareAddress())
    {
        Serial.print("WARN: enableHardwareAddress() failed for ");
        Serial.println(name);
    }

    // Configure all 16 pins as input
    if (!mcp.pinMode16(0xFFFF))
    {
        Serial.print("WARN: pinMode16 failed for ");
        Serial.println(name);
    }

    // Enable pull-up on all pins (buttons to GND)
    if (!mcp.setPullup16(0xFFFF))
    {
        Serial.print("WARN: setPullup16 failed for ");
        Serial.println(name);
    }
}

// Helper: log button event with global index 0..31
void logButtonEvent(uint8_t buttonIndex, bool pressed)
{
    // Print with leading zero: 00..31
    Serial.print("Button ");
    if (buttonIndex < 10)
    {
        Serial.print('0');
    }
    Serial.print(buttonIndex);
    Serial.print(pressed ? " gedrückt" : " losgelassen");
    Serial.println();
}

// Scan a complete MCP for changes on all 16 pins
void handleChip(MCP23S17 &mcp, uint16_t &lastState, uint8_t baseIndex)
{
    // Read all 16 pins (bit 0..15)
    uint16_t current = mcp.read16();

    // Which bits have changed?
    uint16_t changed = lastState ^ current;
    if (changed == 0)
    {
        // No changes, nothing to do
        return;
    }

    // Iterate over all pins
    for (uint8_t pin = 0; pin < 16; ++pin)
    {
        uint16_t mask = (1u << pin);
        if ((changed & mask) == 0)
        {
            continue; // this pin did not change
        }

        bool wasHigh = (lastState & mask) != 0; // 1 = released (pull-up)
        bool isHigh = (current & mask) != 0;

        uint8_t buttonIndex = baseIndex + pin; // 0..31 across both MCPs

        // PRESSED: HIGH -> LOW (1 -> 0)
        if (wasHigh && !isHigh)
        {
            logButtonEvent(buttonIndex, true);
            toggleLed(); // example action on press
        }

        // RELEASED: LOW -> HIGH (0 -> 1)
        if (!wasHigh && isHigh)
        {
            logButtonEvent(buttonIndex, false);
        }
    }

    // Remember new state
    lastState = current;
}

void setup()
{
    Serial.begin(115200);
    delay(200);

    Serial.println();
    Serial.println("Hello World: Lolin32 + 2x MCP23S17");
    Serial.println("Scanning ALL 32 pins for changes.");
    Serial.println("Wiring: MCP pin -> Button -> GND, pull-ups enabled (internal + 10k).");

    // LED pin
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW);
    ledState = false;

    // Start SPI before mcp.begin()
    SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI);
    Serial.print("read16 MCP1 = 0x");
    Serial.println(mcp1.read16(), HEX);

    Serial.print("read16 MCP2 = 0x");
    Serial.println(mcp2.read16(), HEX);

    // Init MCP #1
    if (!mcp1.begin(false)) // false = do not auto-configure all pins
    {
        Serial.println("ERROR: mcp1.begin() failed!");
    }
    else
    {
        Serial.println("MCP1 initialized (HW address 0).");
        setupMCP(mcp1, "MCP1");
        lastState1 = mcp1.read16(); // read initial state
    }

    // Init MCP #2
    if (!mcp2.begin(false))
    {
        Serial.println("ERROR: mcp2.begin() failed!");
    }
    else
    {
        Serial.println("MCP2 initialized (HW address 1).");
        setupMCP(mcp2, "MCP2");
        lastState2 = mcp2.read16(); // read initial state
    }
}

void loop()
{
    // MCP1: buttons 0..15
    handleChip(mcp1, lastState1, 0);

    // MCP2: buttons 16..31
    handleChip(mcp2, lastState2, 16);

    // Small polling interval + light debouncing
    delay(10);
}
