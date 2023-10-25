#pragma once

#include <Arduino.h>
#include <connection.h>
#include <ThingSpeak.h>

#include <pin_def.h>
#include <credentials.h>
#include <sensors.h>
#include <fan_control.h>

bool humidifier_status;
bool humidifier_manual_block = false;
String humidifier_status_str = "Off";

float target_humidity;
unsigned int hum_pulse_duration;
unsigned int hum_pulse_duration_millis;
unsigned int hum_action_timestamp = 0;
unsigned int hum_check_cycle;
unsigned int hum_timestamp = 0;

unsigned int bme280_cycle_buffer;
float intake_pwm_buffer;

void turnOffHumidifier()
{
    if (humidifier_status)
    {
        digitalWrite(RELAY_HUMIDIFIER, 0);
        humidifier_status = false;
        humidifier_status_str = "Off";
        bme280_cycle = bme280_cycle_buffer;
        target_pwm_intake = intake_pwm_buffer;
        Serial.println("Humidfier turned off.");
    }

    if (humidifier_manual_block)
        {
            humidifier_status_str = "Off \\(Blocked\\)";
        }
    
}

void turnOnHumidifier()
{
    if (!humidifier_status && !humidifier_manual_block)
    {
        digitalWrite(RELAY_HUMIDIFIER, 1);
        humidifier_status = true;
        humidifier_status_str = "On";
        bme280_cycle_buffer = bme280_cycle;
        bme280_cycle = 2 * 1000;
        intake_pwm_buffer = current_pwm_intake;
        target_pwm_intake = 0.2;
        Serial.println("Humidfier turned on.");
    }
    
}

void changeHumPulseDuration(float val)
{
    hum_pulse_duration = int(val);
    hum_pulse_duration_millis = hum_pulse_duration * 1000;
    saveFloatSetting(val, SETTINGS_HUM_C);
}

void changeTargetHumidity(float val)
{
    target_humidity = val;
    saveFloatSetting(val, SETTINGS_HUM_T);
}

void evaluateHumidifierAction()
{
    if (target_humidity * 100 * 0.98 <= bme280_meas[1])
    {
        Serial.println("Humidity higher than minimum target range.");
        turnOffHumidifier();
    }

    else
    {
        Serial.println("Humidity lower than minimum target range.");
        turnOnHumidifier();
        hum_action_timestamp = millis();
    }
}

void setupHumidifier()
{
    pinMode(RELAY_HUMIDIFIER, OUTPUT);
    turnOffHumidifier();
    changeTargetHumidity(0.5);
    changeHumPulseDuration(10);
    hum_check_cycle = 5000;
}
