#include "../config.h"
#include "../controls/Control.h"
#include "../repo/controlsRepo.h"

#if USE_ANALOG_SLIDER

#include "TSWLever.h"

static constexpr uint8_t ANALOG_PINS[] = PIN_ANALOG_SLIDER;
static constexpr bool ANALOG_INV[] = ANALOG_SLIDER_INVERTED;

TSWLever *sld1 = nullptr;
TSWLever *sld2 = nullptr;
TSWLever *sld3 = nullptr;
TSWLever *sld4 = nullptr;

inline void setup_analogSlider(TSWSpider *spider)
{

    sld1 = new TSWLever(ANALOG_PINS[0], "AFB", spider);
    sld1->setInverted(ANALOG_INV[0]);
    ControlRegistry::registerControl(sld1, "TSWLever");
    sld1->loadNotches("/config/BR406/afb.json");

    sld2 = new TSWLever(ANALOG_PINS[1], "Leistungsregler", spider);
    sld2->setInverted(ANALOG_INV[1]);
    ControlRegistry::registerControl(sld2, "TSWLever");
    sld2->loadNotches("/config/BR406/leistung.json");

    sld3 = new TSWLever(ANALOG_PINS[2], "Rollo", spider);
    sld3->setInverted(ANALOG_INV[2]);
    ControlRegistry::registerControl(sld3, "TSWLever");
    sld3->loadNotches("/config/BR406/rollo.json");

    sld4 = new TSWLever(ANALOG_PINS[3], "Bremse", spider);
    sld4->setInverted(ANALOG_INV[3]);
    ControlRegistry::registerControl(sld4, "TSWLever");
    sld4->loadNotches("/config/BR406/bremse.json");
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