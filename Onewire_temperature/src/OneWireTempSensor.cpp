#include <OneWire.h>

// https://github.com/PaulStoffregen/OneWire

OneWire  ds(12);  // on this pin (a 4.7K resistor is necessary)

bool sensorFound = false;
byte sensorAddr[8];
bool oldSensorType;
uint32_t tempStartTime = 0;

// Search for a sensor (on the first call only), save address for future use, send Start Convert T command.
bool OneWireTemperatureStart(void) {
  byte i;
  
  if (!sensorFound) {
    if (ds.search(sensorAddr)) {
      Serial.print("Sensor found.");
      if (OneWire::crc8(sensorAddr, 7) != sensorAddr[7]) {
          Serial.println(" Address CRC is not valid!");
          return false;
      }
      Serial.print(" ROM =");
      for (i = 0; i < 8; i++) {
        Serial.write(' ');
        Serial.print(sensorAddr[i], HEX);
        }
      ds.reset_search();
      // the first ROM byte indicates which chip
      switch (sensorAddr[0]) {
        case 0x10:
          Serial.println(" Chip = DS18S20");  // or old DS1820
          oldSensorType = 1;
          break;
        case 0x28:
          Serial.println(" Chip = DS18B20");
          oldSensorType = 0;
          break;
        case 0x22:
          Serial.println(" Chip = DS1822");
          oldSensorType = 0;
          break;
        default:
          Serial.println(" Device is not a DS18x20 family device.");
          return false;
      } 
      sensorFound = true;
      } else {
      Serial.println("Sensor not found !!");
      ds.reset_search();
      return false;
    }
  }

  if (!sensorFound) {
    Serial.println("Sensor not available!");
    return false;
  }

  Serial.println("OneWire: Sending CONVERT T command.");
  ds.reset();
  ds.select(sensorAddr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  tempStartTime = millis();
  return true;
}

// Read temperature sensor value.
// If not enough time has elapsed, wait.
// If search and convert were not performed yet, will do that first.
void OneWireTemperatureRead(void) {
  byte i;
  byte present = 0;
  byte data[9];
  byte dataCRC;
  float celsius;

  if (!sensorFound) {
    Serial.println("Sensor not available!");
    return;
  }

  // check if enough time has elapsed
  if (0 == tempStartTime) { // not started yet
    OneWireTemperatureStart();
  }
  uint32_t time = millis();
  if ((time - tempStartTime) < 900) {
    delay(800 - (time - tempStartTime));
  }

  // we might do a ds.depower() here, but the reset will take care of it.
  
  Serial.println("OneWire: Reading data.");
  present = ds.reset();
  ds.select(sensorAddr);    
  ds.write(0xBE);         // Read Scratchpad

  Serial.print("  Data = ");
  Serial.print(present, HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  dataCRC = OneWire::crc8(data, 8);
  Serial.print(" CRC=");
  Serial.println(dataCRC, HEX);

  if (dataCRC != data[8]) {
      Serial.println(" Data CRC is not valid!");
      return;
  }


  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (oldSensorType) { // old DS18S20
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else { // DS18B20
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.println(" Celsius");
}
