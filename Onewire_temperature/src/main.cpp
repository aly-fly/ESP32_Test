#include <Arduino.h>



void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(100);
}

void OneWireTemperatureStart(void);
void OneWireTemperatureRead(void);

void loop() {
  OneWireTemperatureStart();
  OneWireTemperatureRead();

  delay (5000);

}



