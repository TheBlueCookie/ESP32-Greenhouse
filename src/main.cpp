#include <Arduino.h>

#include <pin_def.h>
#include <sensors.h>
#include <fan_control.h>
#include <connection.h>
#include <light.h>
#include <telegram.h>
#include <credentials.h>

bool first_loop = true;

void setup()
{
  Serial.begin(9600);

  setupWiFi(); // always setup first
  setupSensors();
  setupFans();
  setupLight();
  setupTelegram(); // always setup last
}

void loop()
{
  if (checkWiFi())
  {
    bot.tick();

    if (first_loop)
    {
      measureBME280();
      prepareSoilMeas();
      prepareWaterMeas();
      updateStatus();
      first_loop = false;
    }

    if ((millis() - bme_timestamp) >= bme280_cycle)
    {
      measureBME280();
    }

    if (soil_prep && (millis() - soil_prep_timestamp) >= soil_water_prep)
    {
      measureSoil();
      updateStatus();
    }

    if ((millis() - soil_timestamp) >= soil_cycle && !soil_prep)
    {
      prepareSoilMeas();
    }

    if (water_prep && (millis() - water_prep_timestamp) >= soil_water_prep)
    {
      measureWater();
      updateStatus();
    }

    if ((millis() - water_timestamp) >= water_cycle && !water_prep)
    {
      prepareWaterMeas();
    }

    if (light_status == 0)
    {
      if ((millis() - off_timestamp) >= cycle_off_millis)
      {
        lightOn();
        updateStatus();
      }
    }

    else if (light_status == 1)
    {
      if ((millis() - on_timestamp) >= cycle_on_millis)
      {
        lightOff();
        updateStatus();
      }
    }

    if (current_pwm_circ != target_pwm_circ)
    {
      setPWMCirc(target_pwm_circ);
      updateStatus();
    }

    if (current_pwm_exhaust != target_pwm_exhaust)
    {
      setPWMExhaust(target_pwm_exhaust);
      updateStatus();
    }

    if ((millis() - api_timestamp) >= api_cycle)
    {
      ThingSpeak.writeFields(THINGSPEAK_CHANNEL_ID, THINGSPEAK_API_KEY);
      api_timestamp = millis();
      Serial.println("Uploaded to ThingSpeak.");
    }

    if ((millis() - telegram_timestamp) >= telegram_status_cycle)
    {
      updateStatus();
    }

    if ((millis() - telegram_restart_timestamp) >= telegram_restart_cycle)
    {
      restartBot();
    }

    if (restart)
    {
      bot.tickManual();
      ESP.restart();
    }
  }
}