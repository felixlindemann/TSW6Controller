#include "config.h"
#include "ConfigStore.h"

// Global config instance
Config cfg;

const char* DEVICE_NAME = "TSW-Controller";
const char* DEVICE_SSID = "your-ssid";
const char* DEVICE_PASS = "your-pass";
const char* MDNS_NAME   = "tsw-controller";

const int I2C_SDA = 17;
const int I2C_SCL = 16;


/*

inline constexpr const char* DEVICE_NAME = "TSW-Controller";
inline constexpr const char* DEVICE_SSID = "your-ssid";
inline constexpr const char* DEVICE_PASS = "your-pass";
inline constexpr const char* MDNS_NAME   = "tsw-controller";

*/