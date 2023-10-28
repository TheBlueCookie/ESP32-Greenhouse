#pragma once

#include <Arduino.h>
#include <ThingSpeak.h>

#include <pin_def.h>
#include <restart_manager.h>

const int freq = 25000;
const int resolution = 8;

const int ledChannel_exhaust = 0;
const int ledChannel_circ = 1;
const int ledChannel_intake = 2;

float current_pwm_exhaust;
float current_pwm_circ;
float current_pwm_intake;

float target_pwm_exhaust;
float target_pwm_circ;
float target_pwm_intake;

float idle_pwm_exhaust;
float idle_pwm_intake;
float idle_pwm_circ;

void setPWMExhaust(float pwm)
{
    if (0 <= pwm && pwm <= 1)
    {
        int cycle = pwm * 255;
        ledcWrite(ledChannel_exhaust, cycle);
        current_pwm_exhaust = pwm;
        ThingSpeak.setField(API_EXHAUST_FIELD, pwm * 100);
        Serial.println("Changed exhaust PWM: " + String(pwm));
    }
}

void setPWMCirc(float pwm)
{
    if (0 <= pwm && pwm <= 1)
    {
        int cycle = pwm * 255;
        ledcWrite(ledChannel_circ, cycle);
        current_pwm_circ = pwm;
        ThingSpeak.setField(API_CIRC_FIELD, pwm * 100);
        Serial.println("Changed circulation PWM: " + String(pwm));
    }
}

void setPWMIntake(float pwm)
{
    if (0 <= pwm && pwm <= 1)
    {
        int cycle = pwm * 255;
        ledcWrite(ledChannel_intake, cycle);
        current_pwm_intake = pwm;
        ThingSpeak.setField(API_INTAKE_FIELD, pwm * 100);
        Serial.println("Changed intake PWM: " + String(pwm));
    }
}

void setIdlePWMExhaust(float pwm)
{
    if (0 <= pwm && pwm <= 1)
    {
        int cycle = pwm * 255;
        idle_pwm_exhaust = pwm;
        saveFloatSetting(pwm, SETTINGS_EXHAUST);
        Serial.println("Changed idle exhaust PWM: " + String(pwm));
    }
}

void setIdlePWMCirc(float pwm)
{
    if (0 <= pwm && pwm <= 1)
    {
        int cycle = pwm * 255;
        idle_pwm_circ = pwm;
        saveFloatSetting(pwm, SETTINGS_CIRC);
        Serial.println("Changed idle circulation PWM: " + String(pwm));
    }
}

void setIdlePWMIntake(float pwm)
{
    if (0 <= pwm && pwm <= 1)
    {
        int cycle = pwm * 255;
        idle_pwm_intake = pwm;
        saveFloatSetting(pwm, SETTINGS_INTAKE);
        Serial.println("Changed idle intake PWM: " + String(pwm));
    }
}

void setupFans()
{
    ledcSetup(ledChannel_exhaust, freq, resolution);
    ledcAttachPin(FAN_EXHAUST_PWM, ledChannel_exhaust);

    ledcSetup(ledChannel_circ, freq, resolution);
    ledcAttachPin(FAN_CIRC_PWM, ledChannel_circ);

    ledcSetup(ledChannel_intake, freq, resolution);
    ledcAttachPin(FAN_INTAKE_PWM, ledChannel_intake);

    current_pwm_circ = 0;
    current_pwm_exhaust = 0;
    current_pwm_intake = 0;

    target_pwm_exhaust = idle_pwm_exhaust;
    target_pwm_intake = idle_pwm_intake;
    target_pwm_circ = idle_pwm_circ;
}