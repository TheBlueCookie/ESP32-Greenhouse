#pragma once

#include <Arduino.h>
#include <ThingSpeak.h>

#include <pin_def.h>
#include <restart_manager.h>

bool light_status;
bool manual_light_status;
bool next_status;
String light_status_str;

float cycle_on;
float cycle_off;
float cycle_total;

unsigned long lamp_timestamp;
unsigned int cycle_on_m;
unsigned int cycle_off_m;

void manualToggle()
{
    if (light_status == 0)
    {
        digitalWrite(RELAY_LAMP, HIGH);
        light_status = 1;
        manual_light_status = 1;
        light_status_str = "On";
        ThingSpeak.setField(API_LIGHT_FIELD, 1);
        Serial.println("Light On");
    }

    else if (light_status == 1)
    {
        digitalWrite(RELAY_LAMP, LOW);
        light_status = 0;
        manual_light_status = 0;
        light_status_str = "Off";
        ThingSpeak.setField(API_LIGHT_FIELD, 0);
        Serial.println("Light Off");
    }
}

void lightOn()
{
    digitalWrite(RELAY_LAMP, HIGH);
    lamp_timestamp = millis();
    next_status = 0;
    light_status = 1;
    light_status_str = "On";
    ThingSpeak.setField(API_LIGHT_FIELD, 1);
    Serial.println("Light On");
}

void lightOff()
{
    digitalWrite(RELAY_LAMP, LOW);
    lamp_timestamp = millis();
    next_status = 1;
    light_status = 0;
    light_status_str = "Off";
    ThingSpeak.setField(API_LIGHT_FIELD, 0);
    Serial.println("Light Off");
}

void updateCycle(float on, float off)
{
    cycle_on = on;
    cycle_off = off;
    cycle_total = cycle_on + cycle_off;

    cycle_on_m = cycle_on * 3600 * 1000;
    cycle_off_m = cycle_total * 3600 * 1000 - cycle_on_m;
    lightOn();
    saveFloatSetting(cycle_on, SETTINGS_DAY);
    saveFloatSetting(cycle_off, SETTINGS_NIGHT);
}

void setupLight()
{
    pinMode(RELAY_LAMP, OUTPUT);
    updateCycle(18, 6);
    lightOn();
    manual_light_status = 1;
}