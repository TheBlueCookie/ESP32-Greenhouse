#pragma once

#include <Arduino.h>
#include <connection.h>
#include <ThingSpeak.h>

#include <pin_def.h>
#include <credentials.h>
#include <sensors.h>
#include <fan_control.h>
#include <restart_manager.h>

bool humidifier_status;
bool humidifier_manual_block = false;
String humidifier_status_str = "Off";

float target_humidity;
unsigned int hum_pulse_duration;
unsigned int hum_pulse_duration_millis;
unsigned int hum_action_timestamp = 0;
unsigned int hum_check_cycle;
unsigned int hum_timestamp = 0;
const int hum_target_range = 3;             // all humidity values that are within +- of the target are accepted when not controlling humidity
const float hum_fan_threshold = 0.75;       // maximum PWM value that the fans should run to reduce humidity
const float hum_exhaust_intake_diff = 0.05; // difference between exhaust and intake fan

const int hum_hist_len = 15;
float hum_hist[hum_hist_len];
unsigned int hum_hist_interval = 60 * 1000;
unsigned int hum_hist_timestamp = 0;
int hum_hist_index = 0;
bool try_hum_reduction = true;
bool hum_control_active = false;
float max_threshold_hum_fan = 10; // humidity difference between actual and target for which the maximum fan speed is applied
float min_offset_hum_fan = 0.15; // the minimum offset of fan speed to decrease humidity;
float max_offsett_hum_fan = 0.6; // the maximum offset of fan speed to decrase humidity;
float rate_hum_fan = (max_offsett_hum_fan - min_offset_hum_fan) / max_threshold_hum_fan; // rate of linear relation between humidity difference and fan speed offset;

void turnOffHumidifier()
{
    if (humidifier_status)
    {
        digitalWrite(RELAY_HUMIDIFIER, 0);
        humidifier_status = false;
        humidifier_status_str = "Off";
        target_pwm_intake = idle_pwm_intake;
        target_pwm_exhaust = idle_pwm_exhaust;
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
        target_pwm_intake = 0.15;
        target_pwm_exhaust = 0.15;
        hum_control_active = true;
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

void writeHumHist()
{
    if (hum_hist_index > (hum_hist_len - 1))
    {
        hum_hist_index = 0;
    }

    hum_hist[hum_hist_index] = bme280_meas[1];
    Serial.println("Written to humidity history: " + String(bme280_meas[1]) + " at index " + String(hum_hist_index));
}

int humHistIndexRollover(int index)
{
    if (index < 0)
    {
        return hum_hist_len + index;
    }

    else if (index > (hum_hist_len - 1))
    {
        return index - hum_hist_len;
    }

    else
    {
        return index;
    }
}

void resetHumHist()
{
    for (int i = 0; i < hum_hist_len; i++)
    {
        hum_hist[i] = 100;
    }
    Serial.println("Reset humidity history to 100.");
}

bool evaluateHumHist()
{
    float latest = (hum_hist[hum_hist_index] + hum_hist[humHistIndexRollover(hum_hist_index - 1)]) / 2.0;
    float oldest = (hum_hist[humHistIndexRollover(hum_hist_index - hum_hist_len + 1)] +
                    hum_hist[humHistIndexRollover(hum_hist_index - hum_hist_len + 2)]) /
                   2.0;
    // Serial.println(hum_hist_index);
    // Serial.println(humHistIndexRollover(hum_hist_index - 1));
    // Serial.println(humHistIndexRollover(hum_hist_index - hum_hist_len + 1));
    // Serial.println(humHistIndexRollover(hum_hist_index - hum_hist_len + 2));

    // Serial.println(String(latest) + " ----------------- " + String(oldest));

    hum_hist_index++;

    if (latest < oldest)
    {
        return true;
    }

    else
    {
        return false;
    }
}

float exhaustPWMHumControl(float hum)
{
    float diff = hum - target_humidity * 100;

    if (diff <= 0)
    {
        return 0;
    }

    else
    {
        float offset = min_offset_hum_fan + rate_hum_fan * diff;

        if (offset <= hum_fan_threshold)
        {
            Serial.println("Calculated exhaust PMW for humidity control: " + String(offset + idle_pwm_exhaust));
            return float(int((idle_pwm_exhaust + offset) * 100) * 0.01);
        }

        else
        {
            Serial.println("Calculated exhaust PMW offset for humidity control: " + String(hum_fan_threshold));
            return float(int(hum_fan_threshold * 100) * 0.01);
        }
    }
}

void evaluateHumidifierAction()
{
    if ((target_humidity * 100 - hum_target_range) <= bme280_meas[1])
    {
        Serial.println("Humidity higher than minimum target range.");

        float threshold;

        if (hum_control_active)
        {
            threshold = target_humidity * 100;
        }

        else
        {
            threshold = target_humidity * 100 + hum_target_range;
        }

        if (threshold <= bme280_meas[1] && humidifier_status)
        {
            turnOffHumidifier();
            hum_control_active = false;
        }

        else if (threshold <= bme280_meas[1])
        {
            Serial.println("Humidity higher than maximum target range.");
            if ((millis() - hum_hist_timestamp) > hum_hist_interval)
            {
                hum_hist_timestamp = millis();
                writeHumHist();
                if (!evaluateHumHist())
                {
                    try_hum_reduction = false;
                    Serial.println("Abandoned attempt to reduce humidity.");
                }
            }

            if (try_hum_reduction)
            {
                Serial.println("Trying to reduce humidity by increased airflow.");
                target_pwm_exhaust = exhaustPWMHumControl(bme280_meas[1]);
                target_pwm_intake = target_pwm_exhaust - hum_exhaust_intake_diff;
                hum_control_active = true;
            }

            else
            {
                target_pwm_exhaust = idle_pwm_exhaust;
                target_pwm_intake = idle_pwm_intake;
                hum_control_active = false;
                resetHumHist();
            }
        }

        else if (!humidifier_status)
        {
            target_pwm_exhaust = idle_pwm_exhaust;
            target_pwm_intake = idle_pwm_intake;
            try_hum_reduction = true;
            resetHumHist();
        }
    }

    else
    {
        Serial.println("Humidity lower than minimum target range.");
        turnOnHumidifier();
        hum_action_timestamp = millis();
        try_hum_reduction = true;
        resetHumHist();
    }
}

void setupHumidifier()
{
    pinMode(RELAY_HUMIDIFIER, OUTPUT);
    turnOffHumidifier();
    resetHumHist();
    hum_check_cycle = 5000;
}
