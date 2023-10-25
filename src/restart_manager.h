#pragma once

#include <Preferences.h>

#include <fan_control.h>
#include <humidifier.h>
#include <light.h>
#include <credentials.h>

Preferences preferences;
unsigned int restart_cycle = 3600 * 24 * 30 * 1000; // restart ESP every 30 days, to prevent millis() overload (max 49 days)

void restoreSettings()
{
    preferences.begin(SETTINGS_NMSPC, true);
    target_pwm_intake = preferences.getFloat(SETTINGS_INTAKE);
    target_pwm_exhaust = preferences.getFloat(SETTINGS_EXHAUST);
    target_pwm_circ = preferences.getFloat(SETTINGS_CIRC);
    target_humidity = preferences.getFloat(SETTINGS_HUM_T);
    hum_pulse_duration = int(preferences.getFloat(SETTINGS_HUM_C));
    hum_pulse_duration_millis = hum_pulse_duration * 1000;
    cycle_on = preferences.getFloat(SETTINGS_DAY);
    cycle_off = preferences.getFloat(SETTINGS_NIGHT);
    cycle_total = cycle_on + cycle_off;
    cycle_on_m = cycle_on * 3600 * 1000;
    cycle_off_m = cycle_total * 3600 * 1000 - cycle_on_m;
    lightOn();
    preferences.end();
}

void saveFloatSetting(float val, const char *name)
{
    preferences.begin(SETTINGS_NMSPC, false);
    preferences.putFloat(name, val);
    preferences.end();
}