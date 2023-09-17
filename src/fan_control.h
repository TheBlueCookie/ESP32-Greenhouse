#pragma once

#include <Arduino.h>
#include <ThingSpeak.h>

#include <pin_def.h>

const int freq = 25000;
const int resolution = 8;

const int ledChannel_exhaust = 0;
const int ledChannel_circ = 1;

float current_pwm_exhaust;
float current_pwm_circ;

float target_pwm_exhaust;
float target_pwm_circ;

void setPWMExhaust(float pwm)
{
    if (0 <= pwm && pwm <= 1)
    {
        int cycle = pwm * 255;
        ledcWrite(ledChannel_exhaust, cycle);
        current_pwm_exhaust = pwm;
        ThingSpeak.setField(API_EXHAUST_FIELD, pwm);
        Serial.println("Changed exhuast PWM.");
    }
}

void setPWMCirc(float pwm)
{
    if (0 <= pwm && pwm <= 1)
    {
        int cycle = pwm * 255;
        ledcWrite(ledChannel_circ, cycle);
        current_pwm_circ = pwm;
        ThingSpeak.setField(API_CIRC_FIELD, pwm);
        Serial.println("Changed circulation PWM.");
    }
}

void setupFans()
{
    ledcSetup(ledChannel_exhaust, freq, resolution);
    ledcAttachPin(FAN_EXHAUST_PWM, ledChannel_exhaust);

    ledcSetup(ledChannel_circ, freq, resolution);
    ledcAttachPin(FAN_CIRC_PWM, ledChannel_circ);

    current_pwm_circ = 0.25;
    current_pwm_exhaust = 0.25;

    target_pwm_circ = 0.25;
    target_pwm_exhaust = 0.25;

    setPWMExhaust(current_pwm_exhaust);
    setPWMCirc(current_pwm_circ);
}