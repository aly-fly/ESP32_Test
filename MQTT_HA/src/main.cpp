
#include <Arduino.h>
#include <stdint.h>
#include <stdlib.h>
#include "myWiFi.h"
#include "__CONFIG.h"
#ifdef MQTT_ENABLED
#include "Mqtt_client_HA.h"
#endif
// #include "LED_builtin.h"
#include <FastLED.h>

CRGB leds[1];


void setup()
{
  Serial.begin(115200);
  for (int i = 0; i < 5; i++)
  {
    Serial.print('*');
    delay(500);
  }
  Serial.println();

  // LEDbuiltin_init();
  // LEDbuiltin_ON();
  FastLED.addLeds<NEOPIXEL, WS2812_LED_PIN>(leds, 1);
  leds[0] = CRGB::DarkOrange;
  FastLED.show();

  WifiInit();

#ifdef MQTT_ENABLED
  Serial.println("MQTT start...");
  MqttStart();
  Serial.println("MQTT start Done!");
#endif

  // LEDbuiltin_OFF();
  FastLED.addLeds<NEOPIXEL, WS2812_LED_PIN>(leds, 1);
  leds[0] = CRGB::Navy;
  FastLED.show();

  MqttCommandColor = 0xFFFFFF;

  Serial.println("INIT FINISHED.");
}

// ===============================================================================================================================================================

unsigned long lastMqttCommandExecuted = -1;
bool MqttCommandReceived = false;
uint32_t LEDcolor;

void loop()
{

#ifdef MQTT_ENABLED
  MqttLoopFrequently();

  MqttCommandReceived = false;

  if (MqttCommandPowerReceived)
  {
    MqttCommandPowerReceived = false;
    Serial.printf("CMD: Power = %d\r\n", MqttCommandPower);
    MqttCommandReceived = true;
  }
  if (MqttCommandBrightnessReceived)
  {
    MqttCommandBrightnessReceived = false;
    Serial.printf("CMD: Brightness = %d\r\n", MqttCommandBrightness);
    MqttCommandReceived = true;
  }
  if (MqttCommandColorReceived)
  {
    MqttCommandColorReceived = false;
    Serial.printf("CMD: RGB = %6X\r\n", MqttCommandColor);
    MqttCommandReceived = true;
  }
  if (MqttCommandEffectReceived)
  {
    MqttCommandEffectReceived = false;
    Serial.printf("CMD: Effect = %d (%s)\r\n", MqttCommandEffectNumber, MqttCommandEffect);
    MqttCommandReceived = true;
  }
  if (MqttCommandRainbowSecReceived)
  {
    MqttCommandRainbowSecReceived = false;
    Serial.printf("CMD: Rainbow sec = %.1f\r\n", MqttCommandRainbowSec);
    MqttCommandReceived = true;
  }
  if (MqttCommandDotsReceived)
  {
    MqttCommandDotsReceived = false;
    Serial.printf("CMD: Dots = %d\r\n", MqttCommandDots);
    MqttCommandReceived = true;
  }

  if (MqttCommandReceived)
  {
    MqttStatusTemperture = random(0, 50);
    lastMqttCommandExecuted = millis();

    if (MqttCommandPower)
    {
      LEDcolor = MqttCommandColor;
      FastLED.setBrightness(MqttCommandBrightness);
      // adjustBrightness(&LEDcolor, MqttCommandBrightness);
    }
    else
    {
      LEDcolor = 0;
    }
    leds[0] = LEDcolor;
    FastLED.show();

    /*
    if (MqttCommandDots)
      LEDbuiltin_ON();
    else
      LEDbuiltin_OFF();
    */
  }

  if (lastMqttCommandExecuted != -1)
  {
    if ((millis() - lastMqttCommandExecuted) > (MQTT_SAVE_PREFERENCES_AFTER_SEC * 1000))
    {
      lastMqttCommandExecuted = -1;

      Serial.print("Saving config...");
      // stored_config.save();
      Serial.println(" Done.");
    }
  }
#endif

  MqttStatusBrightness = MqttCommandBrightness;
  MqttStatusPower = MqttCommandPower;
  strncpy(MqttStatusEffect, Effect[MqttCommandEffectNumber].c_str(), sizeof(MqttStatusEffect) - 1);
  MqttStatusEffect[sizeof(MqttStatusEffect) - 1] = '\0';
  MqttStatusRainbowSec = MqttCommandRainbowSec;
  MqttStatusDots = MqttCommandDots;
  MqttStatusRssi = WifiGetSignalLevel();
  //  MqttStatusTemperture = random(0, 50);

#ifdef MQTT_ENABLED
  MqttLoopInFreeTime();
#endif

} // loop
