#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ThingSpeak.h>

#include <credentials.h>
#include <fan_control.h>

WiFiClient client;

unsigned long api_timestamp;
unsigned int api_cycle = 30 * 1000;

void setupWiFi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(1000);
    }
    Serial.println(WiFi.localIP());

    ThingSpeak.begin(client);
    api_timestamp = millis();
}

void getLatestSettings()
{
    int stat;
    stat = ThingSpeak.readMultipleFields(THINGSPEAK_CHANNEL_ID, THINGSPEAK_API_KEY_READ);

    Serial.println("Tried getting latest PWM data, status=" + String(stat));

    if (stat == 200)
    {
        Serial.println(ThingSpeak.getFieldAsString(API_TEMP_FIELD));
        Serial.println(ThingSpeak.getFieldAsString(API_HUM_FIELD));
        Serial.println(ThingSpeak.getFieldAsString(API_SOIL_FIELD));
        Serial.println(ThingSpeak.getFieldAsString(API_WATER_FIELD));
        Serial.println(ThingSpeak.getFieldAsString(API_LIGHT_FIELD));
        Serial.println(ThingSpeak.getFieldAsString(API_EXHAUST_FIELD));
        Serial.println(ThingSpeak.getFieldAsString(API_CIRC_FIELD));
        target_pwm_exhaust = ThingSpeak.getFieldAsFloat(API_EXHAUST_FIELD) * 0.01;
        target_pwm_circ = ThingSpeak.getFieldAsFloat(API_CIRC_FIELD) * 0.01;
    }
}