#include <arduino.h>
#include "chip_info.h"


void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(300);

  GetPrintChipInfo();
  Serial.println(); 

  Serial.println("Free heap memory: " + String(esp_get_free_heap_size()) + " bytes");

  multi_heap_info_t info;
  heap_caps_get_info(&info, MALLOC_CAP_INTERNAL); // internal RAM
  /*
  info.total_free_bytes;   // total currently free in all non-continues blocks
  info.minimum_free_bytes;  // minimum free ever
  info.largest_free_block;   // largest continues block to allocate big array
  */
  Serial.println();
  Serial.println("INTERNAL RAM:");
  Serial.println("Total available memory: " + String(info.total_free_bytes) + " bytes");
  Serial.println("Largest available block: " + String(info.largest_free_block) + " bytes");
  Serial.println("Minimum free ever: " + String(info.minimum_free_bytes) + " bytes");   

  heap_caps_get_info(&info, MALLOC_CAP_SPIRAM);
  /*
  info.total_free_bytes;   // total currently free in all non-continues blocks
  info.minimum_free_bytes;  // minimum free ever
  info.largest_free_block;   // largest continues block to allocate big array
  */
  Serial.println();
  Serial.println("SPI RAM:");
  Serial.println("Total available memory: " + String(info.total_free_bytes) + " bytes");
  Serial.println("Largest available block: " + String(info.largest_free_block) + " bytes");
  Serial.println("Minimum free ever: " + String(info.minimum_free_bytes) + " bytes");   
}

//==================================================================================================================
int vvv;

void loop() {
  delay(500);
  /*
  for (int i = 20; i < NUM_OUPUT_PINS; i++) {
    if (((i < 22) || (i > 25)) && (i != 32) && (i != 33) && (i != 34) && (i != 35) && (i != 36) && (i != 37) && (i != 38)) {
      Serial.printf("Pin %d", i);
      if ((i != 27)) {
        Serial.print("  OUT HIGH");
        pinMode(i, OUTPUT);
        digitalWrite(i, HIGH);
        delay(500);
        Serial.print("   LOW");
        digitalWrite(i, LOW);
      }
      pinMode(i, INPUT);
      delay(500);
      vvv = digitalRead(i);
      Serial.print("   INPUT = ");
      Serial.println(vvv);
      delay(500);
    }
  }
  */
}
