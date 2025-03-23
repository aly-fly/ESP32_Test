#ifndef mqtt_client_H_
#define mqtt_client_H_

#include <Arduino.h>
#include "__CONFIG.h"

#ifdef MQTT_ENABLED
extern bool MqttConnected;

// functions
void MqttStart();
void MqttLoopFrequently();
void MqttLoopInFreeTime();

// ========== APPLICATION VARIABLES ==========

// commands from server; states to be returned; last value that was sent
extern bool MqttCommandPower;
extern bool MqttCommandPowerReceived;
extern bool MqttStatusPower;

extern uint8_t MqttCommandBrightness;
extern bool MqttCommandBrightnessReceived;
extern uint8_t MqttStatusBrightness;

extern char MqttCommandEffect[24];
extern bool MqttCommandEffectReceived;
extern char MqttStatusEffect[24];
#define NumEffects  4
extern String Effect[NumEffects];
extern int MqttCommandEffectNumber;

extern uint32_t MqttCommandColor;
extern bool MqttCommandColorReceived;

extern float MqttCommandRainbowSec;
extern bool MqttCommandRainbowSecReceived;
extern float MqttStatusRainbowSec;

extern bool MqttCommandDots;
extern bool MqttCommandDotsReceived;
extern bool MqttStatusDots;

// read-only statuses
extern int MqttStatusRssi;

extern int MqttStatusTemperture;

extern int MqttStatusErrorWarning;

// ===========================================================

#endif // MQTT_ENABLED

#endif /* mqtt_client_H_ */
