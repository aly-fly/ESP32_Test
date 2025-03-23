
#define DEBUG_OUTPUT

#include "Version.h"
#include "___CONFIG_SECRETS.h"

#define DEVICE_NAME "test02" // no spaces!

// ************* Hardware *************

#define WS2812_LED_PIN      GPIO_NUM_48
#define SYS_LED_PIN         GPIO_NUM_2

// ************* MQTT config *************
#define MQTT_ENABLED                       // enable general MQTT support
#define MQTT_SAVE_PREFERENCES_AFTER_SEC 60 // auto save config X seconds after last MQTT message received

#define MQTT_REPORT_STATUS_EVERY_SEC 15 // report status to MQTT every X seconds
#define MQTT_RECONNECT_WAIT_SEC 10      // wait X seconds before trying to reconnect to MQTT broker

#define MQTT_RETAIN_STATE_MESSAGES      false  // https://www.home-assistant.io/integrations/mqtt/#using-retained-state-messages

#define MQTT_ALIVE_TOPIC             "connection" // availability_topic :: https://www.home-assistant.io/integrations/mqtt/#availability_topic
#define MQTT_ALIVE_MSG_ONLINE        "online"     // default in HA. If changed, configure "payload_available" and "payload_not_available"
#define MQTT_ALIVE_MSG_OFFLINE       "offline"
#define MQTT_RETAIN_ALIVE_MESSAGES   false

// --- MQTT Home Assistant settings ---
// You will either need a local MQTT broker to use MQTT with Home Assistant (e.g. Mosquitto) or use an internet-based broker with Home Assistant support.
// If not done already, you can set up a local one easily via an Add-On in HA. See: https://www.home-assistant.io/integrations/mqtt/
// Enter the credential data into the MQTT broker settings section below accordingly.

#define MQTT_HOME_ASSISTANT_DISCOVERY                     1             // Uncomment if you want HA auto-discovery
#define MQTT_RETAIN_DISCOVERY_MESSAGES                    false         // https://www.home-assistant.io/integrations/mqtt/#discovery-messages-and-availability
#define MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MANUFACTURER "Aljaz"       // Name of the manufacturer shown in HA
#define MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MODEL        DEVICE_NAME   // Name of the model shown in HA
#define MQTT_HOME_ASSISTANT_DISCOVERY_SW_VERSION          VERSION       // Firmware version shown in HA
#define MQTT_HOME_ASSISTANT_DISCOVERY_HW_VERSION          "1.0"         // Hardware version shown in HA

// --- MQTT broker settings ---
/*  -> enter into the file __CONFIG_SECRETS.h
#define MQTT_BROKER    ...                    // Broker host
#define MQTT_PORT      1883                   // Broker port
#define MQTT_USERNAME  ...                    // Username
#define MQTT_PASSWORD  ...                    // Password
*/
#define MQTT_CLIENT    DEVICE_NAME            // Device Id 
// #define MQTT_USE_TLS                                 // Use TLS for MQTT connection. Setting a root CA certificate is needed!
                                                     // Don't forget to copy the correct certificate file into the 'data' folder and rename it to mqtt-ca-root.pem!
                                                     // Example CA cert (Let's Encrypt CA cert) can be found in the 'data - other graphics' subfolder in the root of this repo
