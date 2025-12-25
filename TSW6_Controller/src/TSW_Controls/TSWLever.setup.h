#include "../config.h"
#include "../controls/Control.h"
#include "../repo/controlsRepo.h"
#include "../repo/TSWControlRegistry.h"

#if USE_ANALOG_SLIDER

#include "TSWLever.h"

static constexpr uint8_t ANALOG_PINS[] = PIN_ANALOG_SLIDER;

TSWLever *sld1 = nullptr;
TSWLever *sld2 = nullptr;
TSWLever *sld3 = nullptr;
TSWLever *sld4 = nullptr;

inline void setup_analogSlider(TSWSpider *spider)
{
    // Note: inverted setting is now loaded from JSON config files

    sld1 = new TSWLever(ANALOG_PINS[0], "AFB", spider);
    ControlRegistry::registerControl(sld1, "TSWLever");
    TSWControlRegistry::registerControl(sld1);
    sld1->loadNotches("/config/CurrentLoco/afb.json");

    sld2 = new TSWLever(ANALOG_PINS[1], "Leistungsregler", spider);
    ControlRegistry::registerControl(sld2, "TSWLever");
    TSWControlRegistry::registerControl(sld2);
    sld2->loadNotches("/config/CurrentLoco/leistung.json");

    sld3 = new TSWLever(ANALOG_PINS[2], "Rollo", spider);
    ControlRegistry::registerControl(sld3, "TSWLever");
    TSWControlRegistry::registerControl(sld3);
    sld3->loadNotches("/config/CurrentLoco/rollo.json");

    sld4 = new TSWLever(ANALOG_PINS[3], "Bremse", spider);
    ControlRegistry::registerControl(sld4, "TSWLever");
    TSWControlRegistry::registerControl(sld4);
    sld4->loadNotches("/config/CurrentLoco/bremse.json");
}

inline void update_analogSlider()
{
    sld1->updateAndSend();
    sld2->updateAndSend();
    sld3->updateAndSend();
    sld4->updateAndSend();
}

#define SETUP_ANALOG_SLIDER(a) setup_analogSlider(a)
#define UPDATE_ANALOG_SLIDER() update_analogSlider()

#else
#define SETUP_ANALOG_SLIDER(...)
#define UPDATE_ANALOG_SLIDER()
#endif