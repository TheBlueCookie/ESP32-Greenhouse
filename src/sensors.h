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

unsigned int bme280_cycle = 0.25 * 60 * 1000;
unsigned int soil_cycle = 12 * 60 * 60 * 1000;
unsigned int water_cycle = 12 * 60 * 60 * 1000;

unsigned long bme_timestamp = 0;
unsigned long soil_timestamp = 0;
unsigned long water_timestamp = 0;

float bme280_meas[2];
int soil_meas;
int water_meas;

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

void measureSoil()
{
    soil_meas = analogRead(SEN_MOIST_DAT);
    soil_timestamp = millis();

    Serial.println("Measured soil");

    ThingSpeak.setField(API_SOIL_FIELD, soil_meas);
}

void measureWater()
{
    water_meas = analogRead(SEN_WATER_DAT);
    water_timestamp = millis();

    Serial.println("Measured water");

    ThingSpeak.setField(API_WATER_FIELD, water_meas);
}