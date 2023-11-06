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

bool light_one_active;
bool light_two_active;

void lightOn();
void lightOff();

void manualToggle()
{
    if (light_status == 0)
    {
        if (light_one_active)
        {
            digitalWrite(RELAY_LAMP, HIGH);
        }

        if (light_two_active)
        {
            digitalWrite(RELAY_LAMP_2, HIGH);
        }

        light_status = 1;
        manual_light_status = 1;
        digitalWrite(FAN_LIGHTS_ONOFF, HIGH);
        light_status_str = "On";
        ThingSpeak.setField(API_LIGHT_FIELD, 1);
        Serial.println("Light On");
    }

    else if (light_status == 1)
    {
        digitalWrite(RELAY_LAMP, LOW);
        digitalWrite(RELAY_LAMP_2, LOW);
        digitalWrite(FAN_LIGHTS_ONOFF, LOW);
        light_status = 0;
        manual_light_status = 0;
        light_status_str = "Off";
        ThingSpeak.setField(API_LIGHT_FIELD, 0);
        Serial.println("Light Off");
    }
}

void setLightOneActive(bool new_status)
{
    light_one_active = new_status;
    saveBoolSetting(light_one_active, SETTINGS_LIGHT_ONE);

    if (!new_status)
    {
        digitalWrite(RELAY_LAMP, LOW);
    }
}

void setLightTwoActive(bool new_status)
{
    light_two_active = new_status;
    saveBoolSetting(light_two_active, SETTINGS_LIGHT_TWO);

    if (!new_status)
    {
        digitalWrite(RELAY_LAMP_2, LOW);
    }
}

void toggleLightActive(int light)
{
    if (light == 1)
    {
        setLightOneActive(!light_one_active);
    }

    else if (light == 2)
    {
        setLightTwoActive(!light_two_active);
    }

    if (light_status || manual_light_status)
    {
        if (light_one_active)
        {
            digitalWrite(RELAY_LAMP, HIGH);
        }

        if (light_two_active)
        {
            digitalWrite(RELAY_LAMP_2, HIGH);
        }
    }
}

void lightOn()
{
    if (light_one_active)
    {
        digitalWrite(RELAY_LAMP, HIGH);
    }

    if (light_two_active)
    {
        digitalWrite(RELAY_LAMP_2, HIGH);
    }
    lamp_timestamp = millis();
    digitalWrite(FAN_LIGHTS_ONOFF, HIGH);
    next_status = 0;
    light_status = 1;
    light_status_str = "On";
    ThingSpeak.setField(API_LIGHT_FIELD, 1);
    Serial.println("Light On");
}

void lightOff()
{
    digitalWrite(RELAY_LAMP, LOW);
    digitalWrite(RELAY_LAMP_2, LOW);
    digitalWrite(FAN_LIGHTS_ONOFF, LOW);
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
    saveFloatSetting(cycle_on, SETTINGS_DAY);
    saveFloatSetting(cycle_off, SETTINGS_NIGHT);
}

void setupLight()
{
    pinMode(RELAY_LAMP, OUTPUT);
    pinMode(RELAY_LAMP_2, OUTPUT);
    pinMode(FAN_LIGHTS_ONOFF, OUTPUT);
    lightOn();
    manual_light_status = 0;
}