#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ThingSpeak.h>

#include <credentials.h>

WiFiClient client;

unsigned long api_timestamp;
unsigned int api_cycle = 15 * 1000;

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

bool checkWiFi()
{
    int count = 0;
    while (WiFi.status() != WL_CONNECTED && count < 10)
    {
        WiFi.reconnect();
        count++;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        return true;
    }
    else
    {
        return false;
    }
}