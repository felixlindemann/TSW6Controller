#include "../config.h"
#include "../controls/Control.h"
#include "../repo/controlsRepo.h"

#if USE_Rotary

#include "TSWRotaryKnob.h"

static constexpr uint8_t ROTARY_PINS[] = PIN_Rotary;

inline void setup_RotaryButton(TSWSpider *spider)
{
  static RotaryKnob rotary01("rot01", GPIO_NUM_16, GPIO_NUM_17);
  ControlRegistry::registerControl(&rotary01, "RotaryKnob");
  rotary01.begin();
}

#define SETUP_ROTARYBUTTON(spiderPtr) setup_RotaryButton(spiderPtr)

#else
#define SETUP_ROTARYBUTTON(...)
#endif