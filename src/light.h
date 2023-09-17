#pragma once

#include <Arduino.h>
#include <ThingSpeak.h>

#include <pin_def.h>

int light_status;
String light_status_str;

float cycle_on;
float cycle_off;
float cycle_total;

unsigned long on_timestamp;
unsigned long off_timestamp;
unsigned long cycle_on_millis;
unsigned long cycle_off_millis;

void updateCycle(float on, float off)
{
    cycle_on = on;
    cycle_off = off;
    cycle_total = cycle_on + cycle_off;
    cycle_on_millis = cycle_on * 3600 * 1000;
    cycle_off_millis = cycle_off * 3600 * 1000;
}

void lightOn()
{
    if (light_status == 0)
    {
        digitalWrite(RELAY_LAMP, HIGH);
        on_timestamp = millis();
        light_status = 1;
        light_status_str = "On";
        ThingSpeak.setField(API_LIGHT_FIELD, 1);
        Serial.println("Light On");
    }
}

void lightOff()
{
    if (light_status == 1)
    {
        digitalWrite(RELAY_LAMP, LOW);
        off_timestamp = millis();
        light_status = 0;
        light_status_str = "Off";
        ThingSpeak.setField(API_LIGHT_FIELD, 0);
        Serial.println("Light Off");
    }
}

void setupLight()
{
    pinMode(RELAY_LAMP, OUTPUT);
    updateCycle(18, 6);
    lightOn();
}