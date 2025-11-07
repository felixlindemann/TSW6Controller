#include "../config.h"
#include "../controls/Control.h"
#include "../repo/controlsRepo.h"

#if USE_BUTTON

#include "TSWButton.h"

static constexpr uint8_t BUTTON_PINS[] = PIN_BUTTONS;

inline void setup_Buttons(TSWSpider *spider)
{
  if (sizeof(BUTTON_PINS) / sizeof(uint8_t) == 0)
    return;
  int i = 0;
  static Button btn1("btnRed", BUTTON_PINS[i]);
  ControlRegistry::registerControl(&btn1, "Button");
  i++;
}

#define SETUP_BUTTONS(spiderPtr) setup_Buttons(spiderPtr)

#else
#define SETUP_BUTTONS(...)
#endif