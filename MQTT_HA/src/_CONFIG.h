
#define DEBUG_OUTPUT

#include "Version.h"



// ************* MQTT config *************
#define MQTT_ENABLED                       // enable general MQTT support
#define MQTT_SAVE_PREFERENCES_AFTER_SEC 60 // auto save config X seconds after last MQTT message received

#define MQTT_REPORT_STATUS_EVERY_SEC 15 // report status to MQTT every X seconds
#define MQTT_RECONNECT_WAIT_SEC 10      // wait X seconds before trying to reconnect to MQTT broker

// --- MQTT Home Assistant settings ---
// You will either need a local MQTT broker to use MQTT with Home Assistant (e.g. Mosquitto) or use an internet-based broker with Home Assistant support.
// If not done already, you can set up a local one easily via an Add-On in HA. See: https://www.home-assistant.io/integrations/mqtt/
// Enter the credential data into the MQTT broker settings section below accordingly.

#define MQTT_HOME_ASSISTANT_DISCOVERY                                         // Uncomment if you want HA auto-discovery
#define MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MANUFACTURER "Aljaz"          // Name of the manufacturer shown in HA
#define MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MODEL "MQTT TEST DEVICE"             // Name of the model shown in HA
#define MQTT_HOME_ASSISTANT_DISCOVERY_SW_VERSION VERSION                // Firmware version shown in HA
#define MQTT_HOME_ASSISTANT_DISCOVERY_HW_VERSION "1.0"                     // Hardware version shown in HA

// --- MQTT broker settings ---
// NOTE: If Home Assistant is not enabled, the MQTT support is very limited in the moment!
// You can still use MQTT to control the clock, but only via direct sent MQTT messages, sent from a MQTT client like MQTT Explorer or similar.
// The actual pure MQTT implementation is "emulating" a temperature sensor, so you can use "set temperature" commands to control the clock.
// This is a workaround to have a basic MQTT support.
// For pure MQTT support you can either use any internet-based MQTT broker (i.e. smartnest.cz or HiveMQ) or a local one (i.e. Mosquitto).
// If you choose an internet based one, you will need to create an account, (maybe setting up the device there) and filling in the data below then.
// If you choose a local one, you will need to set up the broker on your local network and fill in the data below.

#define MQTT_BROKER "10.38.11.55"           // Broker host
#define MQTT_PORT 1883                      // Broker port
#define MQTT_USERNAME "MQTT"                // Username
#define MQTT_PASSWORD "DomaceNaprave444"    // Password
#define MQTT_CLIENT "test02"                 // Device Id 
// #define MQTT_USE_TLS                                 // Use TLS for MQTT connection. Setting a root CA certificate is needed!
                                                     // Don't forget to copy the correct certificate file into the 'data' folder and rename it to mqtt-ca-root.pem!
                                                     // Example CA cert (Let's Encrypt CA cert) can be found in the 'data - other graphics' subfolder in the root of this repo
