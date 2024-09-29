// https://github.com/junkfix/esp32-ds18b20

/*
#include <OneWireESP32.h>

const uint8_t MaxDevs = 2;

float currTemp[MaxDevs];

void ReadTemp_junkfix_esp32_ds18b20(void){
	OneWire32 ds(13); //gpio pin
	
	uint64_t addr[MaxDevs];
	
	//uint64_t addr[] = {
	//	0x183c01f09506f428,
	//	0xf33c01e07683de28,
	//};
	
	//to find addresses
	uint8_t devices = ds.search(addr, MaxDevs);
	for (uint8_t i = 0; i < devices; i += 1) {
		Serial.printf("%d: 0x%llx,\n", i, addr[i]);
		//char buf[20]; snprintf( buf, 20, "0x%llx,", addr[i] ); Serial.println(buf);
	}
	//end


		ds.request();
		delay(800);
		for(byte i = 0; i < MaxDevs; i++){
			uint8_t err = ds.getTemp(addr[i], currTemp[i]);
			if(err){
				const char *errt[] = {"", "CRC", "BAD","DC","DRV"};
				Serial.print(i); Serial.print(": "); Serial.println(errt[err]);
			}else{
				Serial.print(i); Serial.print(": "); Serial.println(currTemp[i]);
			}
		}
}




void tempTask(void *pvParameters){
    for (;;)
    {
    ReadTemp_junkfix_esp32_ds18b20();
    delay(5000);
    }  
}

void setup() {
	delay(1000);
	Serial.begin(115200);
	xTaskCreatePinnedToCore(tempTask, "tempTask", 2048,  NULL,  1,  NULL, 0);
}

*/

