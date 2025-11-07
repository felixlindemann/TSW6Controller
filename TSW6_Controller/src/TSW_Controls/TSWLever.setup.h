#include "../config.h"
#include "../controls/Control.h"
#include "../repo/controlsRepo.h"

#if USE_ANALOG_SLIDER

#include "TSWLever.h"


static constexpr uint8_t ANALOG_PINS[] = PIN_ANALOG_SLIDER;
static constexpr bool ANALOG_INV[] = ANALOG_SLIDER_INVERTED;

inline void setup_analogSlider(TSWSpider* spider)
{
    for (int i = 0; i < 3; ++i) {
        String id = "sld" + String(i + 1);
        static TSWLever lever(ANALOG_PINS[i], id, spider);
        lever.setInverted(ANALOG_INV[i]);
        ControlRegistry::registerControl(&lever, "TSWLever");
    }
}

#define SETUP_ANALOG_SLIDER(  a) setup_analogSlider(a)

#else
#define SETUP_ANALOG_SLIDER(...)
#endif