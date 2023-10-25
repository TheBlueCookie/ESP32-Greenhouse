#pragma once

#include <Preferences.h>
#include <credentials.h>

Preferences preferences;
unsigned long restart_cycle = uint32_t(3600 * 24)  * uint32_t(30 * 1000); // restart ESP every 30 days, to prevent millis() overflow (max 49 days)

void saveFloatSetting(float val, const char *name)
{
    preferences.begin(SETTINGS_NMSPC, false);
    preferences.putFloat(name, val);
    preferences.end();
    Serial.println("Saved parameter to drive. " + String(name) + ": " + String(val));
}
