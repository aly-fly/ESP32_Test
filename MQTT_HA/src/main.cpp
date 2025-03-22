
#include <Arduino.h>
#include <stdint.h>
#include <stdlib.h>
#include "myWiFi.h"
#include "_CONFIG.h"
#ifdef MQTT_ENABLED
#include "Mqtt_client_HA.h"
#endif

void setup()
{
  Serial.begin(115200);
  for (int i = 0; i < 5; i++)
  {
    Serial.print('*');
    delay(500);
  }
  Serial.println();

  WifiInit();

#ifdef MQTT_ENABLED
  Serial.println("MQTT start...");
  MqttStart();
  Serial.println("MQTT start Done!");
#endif

  Serial.println("INIT FINISHED.");
}

// ===============================================================================================================================================================

unsigned long lastMqttCommandExecuted = -1;
bool MqttCommandReceived = false;

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
MqttStatusRainbowSec = MqttCommandRainbowSec;
MqttStatusDots = MqttCommandDots;
MqttStatusRssi = WifiGetSignalLevel();
//  MqttStatusTemperture = random(0, 50);

#ifdef MQTT_ENABLED
  MqttLoopInFreeTime();
#endif

} // loop
