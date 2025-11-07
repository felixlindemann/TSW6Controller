#include "../config.h"
#include "../controls/Control.h"
#include "../repo/controlsRepo.h"

#if USE_GAMEPAD

#include "TSWGamePadControl.h"


static constexpr uint8_t GAMEPAD_PINS[] = PIN_GAMEPAD; 

static TSWGamePadControl *pad1Ptr = nullptr;
inline void setup_GamePad(TSWSpider* spider)
{
        static TSWGamePadControl pad1("pad1", GAMEPAD_PINS[0], GAMEPAD_PINS[1], GAMEPAD_PINS[2]);
  ControlRegistry::registerControl(&pad1, "TSWGamePadControl");
  pad1.setXInverted(true);
  pad1.setYInverted(true);
  pad1Ptr = &pad1;
}

#define SETUP_GAMEPAD(spiderPtr) setup_GamePad(spiderPtr)

#else
#define SETUP_GAMEPAD(...)
#endif