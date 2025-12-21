#include "display.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// Create the SH1106 instance here (this is the actual definition)
static Adafruit_SH1106G gDisplay(128, 64, &Wire, -1);

static void redrawSh1106(const String* lines, uint8_t lineCount)
{
    gDisplay.clearDisplay();
    gDisplay.setTextSize(1);
    gDisplay.setTextColor(SH110X_WHITE);

    const int lineHeight = 10;
    for (uint8_t i = 0; i < lineCount; i++)
    {
        gDisplay.setCursor(0, i * lineHeight);
        gDisplay.print(lines[i]);
    }

    gDisplay.display();
}

// Do NOT name it "log" (conflicts with math log())
DisplayLog gDisplayLog(6, 22, redrawSh1106);

void setupDisplay()
{
    // Choose your I2C pins
    Wire.begin(I2C_SDA, I2C_SCL); // SDA, SCL

    // Start SH1106. (address is usually 0x3C, sometimes 0x3D)
    if (!gDisplay.begin(0x3C, true))
    {
        // If you want to hard-stop:
        // while(true) { delay(100); }
        return;
    }

    gDisplay.clearDisplay();
    gDisplay.display();

    gDisplayLog.begin();
}

void loopDisplay()
{
    // nothing for now
}
