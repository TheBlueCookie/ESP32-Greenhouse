#pragma once

#include <Arduino.h>
#include <Adafruit_BME280.h>
#include <Wire.h>
#include <connection.h>
#include <ThingSpeak.h>

#include <pin_def.h>
#include <credentials.h>

// BME 280
TwoWire I2CBME = TwoWire(0);
Adafruit_BME280 bme;

unsigned int bme280_cycle = 2 * 1000;
unsigned int soil_water_prep = 5 * 1000;
unsigned int soil_cycle = 1 * 60 * 60 * 1000 + soil_water_prep;
unsigned int water_cycle = 1 * 60 * 60 * 1000 + soil_water_prep;

unsigned long bme_timestamp = 0;
unsigned long soil_timestamp = 0;
unsigned long water_timestamp = 0;
unsigned long soil_prep_timestamp = 0;
unsigned long water_prep_timestamp = 0;

float bme280_meas[2];
int soil_meas;
int water_meas;

bool soil_prep = false;
bool water_prep = false;

void setupSensors()
{
    // soil moisture
    pinMode(SEN_MOIST_VCC, OUTPUT);

    // water detection
    pinMode(SEN_WATER_VCC, OUTPUT);

    // bme 280
    I2CBME.begin(SEN_BME280_SDA, SEN_BME280_SCL, 100000);
    bme.begin(0x76, &I2CBME);
}

void measureBME280()
{

    bme280_meas[0] = bme.readTemperature() - 1.5;
    bme280_meas[1] = bme.readHumidity();

    bme_timestamp = millis();

    Serial.println("Measured BME280");

    ThingSpeak.setField(API_TEMP_FIELD, bme280_meas[0]);
    ThingSpeak.setField(API_HUM_FIELD, bme280_meas[1]);
}

void prepareSoilMeas()
{
    digitalWrite(SEN_MOIST_VCC, HIGH);
    soil_prep = true;
    soil_prep_timestamp = millis();
}

void measureSoil()
{
    if (soil_prep)
    {
        soil_meas = analogRead(SEN_MOIST_DAT);
        digitalWrite(SEN_MOIST_VCC, LOW);
        soil_prep = false;
        soil_timestamp = millis();

        Serial.println("Measured soil");

        ThingSpeak.setField(API_SOIL_FIELD, soil_meas);
    }
}

void prepareWaterMeas()
{
    digitalWrite(SEN_WATER_VCC, HIGH);
    water_prep = true;
    water_prep_timestamp = millis();
}

void measureWater()
{
    if (water_prep)
    {
        water_meas = analogRead(SEN_WATER_DAT);
        digitalWrite(SEN_WATER_VCC, LOW);
        water_prep = false;
        water_timestamp = millis();

        Serial.println("Measured water");

        ThingSpeak.setField(API_WATER_FIELD, water_meas);
    }
}