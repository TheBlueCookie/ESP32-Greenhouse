#include <Arduino.h>

#include <pin_def.h>
#include <sensors.h>
#include <fan_control.h>
#include <connection.h>
#include <light.h>
#include <telegram.h>
#include <credentials.h>

bool first_loop = true;
int short_recon = 0;
int long_recon = 0;

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
  if (WiFi.status() == WL_CONNECTED)
  {
    bot.tick();

    if (first_loop)
    {
      delay(api_cycle);
      getLatestSettings();
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

    if ((millis() - lamp_timestamp) >= cycle_on_m && next_status == 0)
    {
      if (manual_light_status == 0)
      {
        lamp_timestamp = millis();
        next_status = 1;
        Serial.println(1);
      }
      if (light_status == 1)
      {
        lightOff();
        updateStatus();
        Serial.println(2);
      }
    }

    if ((millis() - lamp_timestamp) >= cycle_off_m && next_status == 1)
    {
      if (manual_light_status == 1)
      {
        lamp_timestamp = millis();
        next_status = 0;
        Serial.println(3);
      }
      if (light_status == 0)
      {
        lightOn();
        updateStatus();
        Serial.println(4);
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
      ThingSpeak.writeFields(THINGSPEAK_CHANNEL_ID, THINGSPEAK_API_KEY_WRITE);
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

  else if (short_recon < 12) // try reconnecting every 10 seconds for up to 2 minutes
  {
    Serial.println("Attempting to reconnect in short intervals, attempt=" + String(short_recon));
    WiFi.reconnect();
    short_recon++;
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("Connection restored.");
      short_recon = 0;
    }
    delay(10 * 1000);
  }

  else if (long_recon < 10) // try reconnection every 60 seconds for up to 10 minutes
  {
    Serial.println("Attempting to reconnect in long intervals, attempt=" + String(long_recon));
    WiFi.reconnect();
    long_recon++;
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("Connection restored.");
      long_recon = 0;
    }
    delay(60 * 1000);
  }

  else
  {
    Serial.println("Reconnecting attemps failed. Restarting...");
    ESP.restart();
  }
}