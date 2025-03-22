
#include "_CONFIG.h"

#include "Mqtt_client.h"
#ifdef MQTT_ENABLED
#include "WiFi.h" // for ESP32
#include <PubSubClient.h>
#include <ArduinoJson.h>

#ifdef MQTT_USE_TLS
#include <WiFiClientSecure.h> // for secure WiFi client
WiFiClientSecure espClient;
#else
WiFiClient espClient;
#endif
PubSubClient MQTTclient(espClient);

#define concat2(first, second) first second
#define concat3(first, second, third) first second third
#define concat4(first, second, third, fourth) first second third fourth
#define concat5(first, second, third, fourth, fifth) first second third fourth fifth

#define MQTT_STATE_ON "ON"
#define MQTT_STATE_OFF "OFF"

#define MQTT_BRIGHTNESS_MIN 0
#define MQTT_BRIGHTNESS_MAX 255

#define TopicHAstatus "homeassistant/status"

int splitCommand(char *topic, char *tokens[], int tokensNumber);
void callback(char *topic, byte *payload, unsigned int length);

void MqttReportBackOnChange();
bool MqttReportDiscovery();

uint32_t lastTimeSent = (uint32_t)(MQTT_REPORT_STATUS_EVERY_SEC * -1000);
uint32_t LastTimeTriedToConnect = 0;
bool MqttConnected = false;
bool discoveryReported = false;

// ========== APPLICATION VARIABLES ==========

// commands from server; states to be returned; last value that was sent
#define TopicLight "back"
bool MqttCommandPower = true;
bool MqttCommandPowerReceived = false;
bool MqttStatusPower = true;
bool MqttStatusPowerLastSent = false;

uint8_t MqttCommandBrightness = -1;
bool MqttCommandBrightnessReceived = false;
uint8_t MqttStatusBrightness = 0;
uint8_t MqttStatusBrightnessLastSent = 0;

char MqttCommandEffect[24] = "";
bool MqttCommandEffectReceived = false;
char MqttStatusEffect[24] = "";
char MqttStatusEffectLastSent[24] = "";

uint32_t MqttCommandColor = 0x000000;
bool MqttCommandColorReceived = false;

#define TopicRainbowSec "rainbow"
float MqttCommandRainbowSec = -1;
bool MqttCommandRainbowSecReceived = false;
float MqttStatusRainbowSec = 0;
float MqttStatusRainbowSecLastSent = 0;

#define TopicDots "dots"
bool MqttCommandDots = true;
bool MqttCommandDotsReceived = false;
bool MqttStatusDots = true;
bool MqttStatusDotsLastSent = false;

// read-only statuses
#define TopicRssi "rssi"
int MqttStatusSignalLevel = 0;
int MqttStatusSignalLevelLastSent = 999;

#define TopicTemperature "temperature"
int MqttStatusTemperture = 0;
int MqttStatusTempertureLastSent = 999;

#define TopicErrorWarning "errors"
int MqttStatusErrorWarning = 0;
int MqttStatusErrorWarningLastSent = 999;

// ===========================================================

double round1(double value)
{
  return (int)(value * 10 + 0.5) / 10.0;
}

void MqttPublishValues(bool forceUpdateEverything)
{
  if (MQTTclient.connected())
  {
#ifdef MQTT_HOME_ASSISTANT_DISCOVERY
    if (!discoveryReported)
    {
      if (MqttReportDiscovery())
        discoveryReported = true;
      else
      {
        Serial.println("Error sending discovery!");
        delay(2000);
      }
      // after discovery is sent, all values must be updated.
      forceUpdateEverything = true;
    }
#endif

    if (forceUpdateEverything || MqttStatusPower != MqttStatusPowerLastSent || MqttStatusBrightness != MqttStatusBrightnessLastSent)
    {
      JsonDocument state;
      state["state"] = MqttStatusPower == 0 ? MQTT_STATE_OFF : MQTT_STATE_ON;
      state["brightness"] = MqttStatusBrightness;
      state["effect"] = MqttStatusEffect;

      char buffer[256];
      serializeJson(state, buffer);
      const char *topic = concat2(MQTT_CLIENT, concat2("/", TopicLight));
      if (!MQTTclient.publish(topic, buffer, true))
        return;
      MqttStatusPowerLastSent = MqttStatusPower;
      MqttStatusBrightnessLastSent = MqttStatusBrightness;

      Serial.print("TX MQTT: ");
      Serial.print(topic);
      Serial.print(" ");
      Serial.println(buffer);
      state.clear();
    }

    if (forceUpdateEverything || MqttStatusRainbowSec != MqttStatusRainbowSecLastSent)
    {
      JsonDocument state;
      state["state"] = round1(MqttStatusRainbowSec);

      char buffer[256];
      serializeJson(state, buffer);
      const char *topic = concat2(MQTT_CLIENT, concat2("/", TopicRainbowSec));
      if (!MQTTclient.publish(topic, buffer, true))
        return;
      MqttStatusRainbowSecLastSent = MqttStatusRainbowSec;

      Serial.print("TX MQTT: ");
      Serial.print(topic);
      Serial.print(" ");
      Serial.println(buffer);
      state.clear();
    }

    if (forceUpdateEverything || MqttStatusDots != MqttStatusDotsLastSent)
    {
      JsonDocument state;
      state["state"] = MqttStatusDots == 0 ? MQTT_STATE_OFF : MQTT_STATE_ON;

      char buffer[256];
      serializeJson(state, buffer);
      const char *topic = concat2(MQTT_CLIENT, concat2("/", TopicDots));
      if (!MQTTclient.publish(topic, buffer, true))
        return;
      MqttStatusDotsLastSent = MqttStatusDots;

      Serial.print("TX MQTT: ");
      Serial.print(topic);
      Serial.print(" ");
      Serial.println(buffer);
      state.clear();
    }

    if (forceUpdateEverything || (abs(MqttStatusSignalLevel - MqttStatusSignalLevelLastSent) > 6))
    {
      JsonDocument state;
      state["state"] = round1(MqttStatusSignalLevel);

      char buffer[256];
      serializeJson(state, buffer);
      const char *topic = concat2(MQTT_CLIENT, concat2("/", TopicRssi));
      if (!MQTTclient.publish(topic, buffer, true))
        return;
      MqttStatusSignalLevelLastSent = MqttStatusSignalLevel;

      Serial.print("TX MQTT: ");
      Serial.print(topic);
      Serial.print(" ");
      Serial.println(buffer);
      state.clear();
    }

    if (forceUpdateEverything || (abs(MqttStatusTemperture - MqttStatusTempertureLastSent) > 1))
    {
      JsonDocument state;
      state["state"] = round1(MqttStatusTemperture);

      char buffer[256];
      serializeJson(state, buffer);
      const char *topic = concat2(MQTT_CLIENT, concat2("/", TopicTemperature));
      if (!MQTTclient.publish(topic, buffer, true))
        return;
      MqttStatusTempertureLastSent = MqttStatusTemperture;

      Serial.print("TX MQTT: ");
      Serial.print(topic);
      Serial.print(" ");
      Serial.println(buffer);
      state.clear();
    }

    if (forceUpdateEverything || MqttStatusErrorWarning != MqttStatusErrorWarningLastSent)
    {
      JsonDocument state;
      state["state"] = MqttStatusErrorWarning;

      char buffer[256];
      serializeJson(state, buffer);
      const char *topic = concat2(MQTT_CLIENT, concat2("/", TopicErrorWarning));
      if (!MQTTclient.publish(topic, buffer, true))
        return;
      MqttStatusErrorWarningLastSent = MqttStatusErrorWarning;

      Serial.print("TX MQTT: ");
      Serial.print(topic);
      Serial.print(" ");
      Serial.println(buffer);
      state.clear();
    }
  }
}

void MqttReportBackOnChange()
{
  MqttPublishValues(false);
}

void MqttPeriodicReportBackEverything()
{
  if (((millis() - lastTimeSent) > (MQTT_REPORT_STATUS_EVERY_SEC * 1000)) && MQTTclient.connected())
  {
    MqttPublishValues(true);
    lastTimeSent = millis();
  }
}

// ====================================== DISCOVERY ===================================================

// https://www.home-assistant.io/integrations/mqtt/#supported-abbreviations-in-mqtt-discovery-messages

bool MqttReportDiscovery()
{
#ifdef MQTT_HOME_ASSISTANT_DISCOVERY
  char json_buffer[2048];
  JsonDocument discovery;

  // Back Light - LIGHT
  discovery["device"]["identifiers"][0] = MQTT_CLIENT;
  discovery["device"]["manufacturer"] = MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MANUFACTURER;
  discovery["device"]["model"] = MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MODEL;
  discovery["device"]["name"] = MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MODEL;
  discovery["device"]["sw_version"] = MQTT_HOME_ASSISTANT_DISCOVERY_SW_VERSION;
  discovery["device"]["hw_version"] = MQTT_HOME_ASSISTANT_DISCOVERY_HW_VERSION;
  discovery["device"]["connections"][0][0] = "mac";
  discovery["device"]["connections"][0][1] = WiFi.macAddress();
  discovery["unique_id"] = concat3(MQTT_CLIENT, "_", TopicLight);
  discovery["object_id"] = concat3(MQTT_CLIENT, "_", TopicLight);
  discovery["name"] = "Back light";
  discovery["schema"] = "json";
  discovery["state_topic"] = concat3(MQTT_CLIENT, "/", TopicLight);
  discovery["json_attributes_topic"] = concat3(MQTT_CLIENT, "/", TopicLight);
  discovery["command_topic"] = concat4(MQTT_CLIENT, "/", TopicLight, "/set");
  discovery["brightness"] = true;
  discovery["brightness_scale"] = MQTT_BRIGHTNESS_MAX;
  discovery["supported_color_modes"][0] = "rgb";
  discovery["effect"] = true;
  for (size_t i = 0; i < 5; i++)
  {
    discovery["effect_list"][i] = String(i);
  }
  serializeJson(discovery, json_buffer);
  const char *topic1 = concat5("homeassistant/light/", MQTT_CLIENT, "_", TopicLight, "/light/config");
  delay(250);
  if (!MQTTclient.publish(topic1, json_buffer, true))
    return false;
  Serial.print("TX MQTT: ");
  Serial.print(topic1);
  Serial.print(" ");
  Serial.println(json_buffer);
  discovery.clear();

  // Rainbow duration - NUMBER
  discovery["device"]["identifiers"][0] = MQTT_CLIENT;
  discovery["device"]["manufacturer"] = MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MANUFACTURER;
  discovery["device"]["model"] = MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MODEL;
  discovery["device"]["name"] = MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MODEL;
  discovery["device"]["sw_version"] = MQTT_HOME_ASSISTANT_DISCOVERY_SW_VERSION;
  discovery["device"]["hw_version"] = MQTT_HOME_ASSISTANT_DISCOVERY_HW_VERSION;
  discovery["device"]["connections"][0][0] = "mac";
  discovery["device"]["connections"][0][1] = WiFi.macAddress();
  discovery["unique_id"] = concat3(MQTT_CLIENT, "_", TopicRainbowSec);
  discovery["object_id"] = concat3(MQTT_CLIENT, "_", TopicRainbowSec);
  // discovery["entity_category"] = "control";
  discovery["name"] = "Rainbow, sec";
  discovery["state_topic"] = concat3(MQTT_CLIENT, "/", TopicRainbowSec);
  discovery["json_attributes_topic"] = concat3(MQTT_CLIENT, "/", TopicRainbowSec);
  discovery["command_topic"] = concat4(MQTT_CLIENT, "/", TopicRainbowSec, "/set");
  discovery["command_template"] = "{\"state\":{{value}}}";
  discovery["step"] = 0.1;
  discovery["min"] = 0.2;
  discovery["max"] = 10;
  discovery["mode"] = "slider";
  discovery["value_template"] = "{{ value_json.state }}";
  serializeJson(discovery, json_buffer);
  const char *topic2 = concat5("homeassistant/number/", MQTT_CLIENT, "_", TopicRainbowSec, "/number/config");
  delay(250);
  if (!MQTTclient.publish(topic2, json_buffer, true))
    return false;
  Serial.print("TX MQTT: ");
  Serial.print(topic2);
  Serial.print(" ");
  Serial.println(json_buffer);
  discovery.clear();

  // Dots - SWITCH
  discovery["device"]["identifiers"][0] = MQTT_CLIENT;
  discovery["device"]["manufacturer"] = MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MANUFACTURER;
  discovery["device"]["model"] = MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MODEL;
  discovery["device"]["name"] = MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MODEL;
  discovery["device"]["sw_version"] = MQTT_HOME_ASSISTANT_DISCOVERY_SW_VERSION;
  discovery["device"]["hw_version"] = MQTT_HOME_ASSISTANT_DISCOVERY_HW_VERSION;
  discovery["device"]["connections"][0][0] = "mac";
  discovery["device"]["connections"][0][1] = WiFi.macAddress();
  discovery["unique_id"] = concat3(MQTT_CLIENT, "_", TopicDots);
  discovery["object_id"] = concat3(MQTT_CLIENT, "_", TopicDots);
  // discovery["entity_category"] = "control";
  discovery["name"] = "Show dots";
  discovery["state_topic"] = concat3(MQTT_CLIENT, "/", TopicDots);
  discovery["json_attributes_topic"] = concat3(MQTT_CLIENT, "/", TopicDots);
  discovery["command_topic"] = concat4(MQTT_CLIENT, "/", TopicDots, "/set");
  discovery["value_template"] = "{{ value_json.state }}";
  discovery["state_on"] = "ON";
  discovery["state_off"] = "OFF";
  discovery["payload_on"] = "{\"state\":\"ON\"}";
  discovery["payload_off"] = "{\"state\":\"OFF\"}";
  serializeJson(discovery, json_buffer);
  const char *topic3 = concat5("homeassistant/switch/", MQTT_CLIENT, "_", TopicDots, "/switch/config");
  delay(250);
  if (!MQTTclient.publish(topic3, json_buffer, true))
    return false;
  Serial.print("TX MQTT: ");
  Serial.print(topic3);
  Serial.print(" ");
  Serial.println(json_buffer);
  discovery.clear();

  // rssi - SENSOR
  discovery["device"]["identifiers"][0] = MQTT_CLIENT;
  discovery["device"]["manufacturer"] = MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MANUFACTURER;
  discovery["device"]["model"] = MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MODEL;
  discovery["device"]["name"] = MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MODEL;
  discovery["device"]["sw_version"] = MQTT_HOME_ASSISTANT_DISCOVERY_SW_VERSION;
  discovery["device"]["hw_version"] = MQTT_HOME_ASSISTANT_DISCOVERY_HW_VERSION;
  discovery["device"]["connections"][0][0] = "mac";
  discovery["device"]["connections"][0][1] = WiFi.macAddress();
  discovery["unique_id"] = concat3(MQTT_CLIENT, "_", TopicRssi);
  discovery["object_id"] = concat3(MQTT_CLIENT, "_", TopicRssi);
  discovery["name"] = "RSSI";
  discovery["state_topic"] = concat3(MQTT_CLIENT, "/", TopicRssi);
  discovery["json_attributes_topic"] = concat3(MQTT_CLIENT, "/", TopicRssi);
  discovery["value_template"] = "{{ value_json.state }}";
  discovery["unit_of_measurement"] = "dBm";
  serializeJson(discovery, json_buffer);
  const char *topic4 = concat5("homeassistant/sensor/", MQTT_CLIENT, "_", TopicRssi, "/sensor/config");
  delay(250);
  if (!MQTTclient.publish(topic4, json_buffer, true))
    return false;
  Serial.print("TX MQTT: ");
  Serial.print(topic4);
  Serial.print(" ");
  Serial.println(json_buffer);
  discovery.clear();

  // temperature - SENSOR
  discovery["device"]["identifiers"][0] = MQTT_CLIENT;
  discovery["device"]["manufacturer"] = MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MANUFACTURER;
  discovery["device"]["model"] = MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MODEL;
  discovery["device"]["name"] = MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MODEL;
  discovery["device"]["sw_version"] = MQTT_HOME_ASSISTANT_DISCOVERY_SW_VERSION;
  discovery["device"]["hw_version"] = MQTT_HOME_ASSISTANT_DISCOVERY_HW_VERSION;
  discovery["device"]["connections"][0][0] = "mac";
  discovery["device"]["connections"][0][1] = WiFi.macAddress();
  discovery["unique_id"] = concat3(MQTT_CLIENT, "_", TopicTemperature);
  discovery["object_id"] = concat3(MQTT_CLIENT, "_", TopicTemperature);
  discovery["name"] = "Motor temperature";
  discovery["state_topic"] = concat3(MQTT_CLIENT, "/", TopicTemperature);
  discovery["json_attributes_topic"] = concat3(MQTT_CLIENT, "/", TopicTemperature);
  discovery["value_template"] = "{{ value_json.state }}";
  discovery["unit_of_measurement"] = "deg C";
  serializeJson(discovery, json_buffer);
  const char *topic5 = concat5("homeassistant/sensor/", MQTT_CLIENT, "_", TopicTemperature, "/sensor/config");
  delay(250);
  if (!MQTTclient.publish(topic5, json_buffer, true))
    return false;
  Serial.print("TX MQTT: ");
  Serial.print(topic5);
  Serial.print(" ");
  Serial.println(json_buffer);
  discovery.clear();

  // errors - SENSOR
  discovery["device"]["identifiers"][0] = MQTT_CLIENT;
  discovery["device"]["manufacturer"] = MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MANUFACTURER;
  discovery["device"]["model"] = MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MODEL;
  discovery["device"]["name"] = MQTT_HOME_ASSISTANT_DISCOVERY_DEVICE_MODEL;
  discovery["device"]["sw_version"] = MQTT_HOME_ASSISTANT_DISCOVERY_SW_VERSION;
  discovery["device"]["hw_version"] = MQTT_HOME_ASSISTANT_DISCOVERY_HW_VERSION;
  discovery["device"]["connections"][0][0] = "mac";
  discovery["device"]["connections"][0][1] = WiFi.macAddress();
  discovery["unique_id"] = concat3(MQTT_CLIENT, "_", TopicErrorWarning);
  discovery["object_id"] = concat3(MQTT_CLIENT, "_", TopicErrorWarning);
  discovery["name"] = "Error status";
  discovery["state_topic"] = concat3(MQTT_CLIENT, "/", TopicErrorWarning);
  discovery["json_attributes_topic"] = concat3(MQTT_CLIENT, "/", TopicErrorWarning);
  discovery["value_template"] = "{{ value_json.state }}";
  //discovery["unit_of_measurement"] = "";
  serializeJson(discovery, json_buffer);
  const char *topic6 = concat5("homeassistant/sensor/", MQTT_CLIENT, "_", TopicErrorWarning, "/sensor/config");
  delay(250);
  if (!MQTTclient.publish(topic6, json_buffer, true))
    return false;
  Serial.print("TX MQTT: ");
  Serial.print(topic6);
  Serial.print(" ");
  Serial.println(json_buffer);
  discovery.clear();

#endif
  return true;
}

#ifdef MQTT_USE_TLS
bool loadCARootCert()
{
  const char *filename = "/mqtt-ca-root.pem";
  Serial.println("Loading CA Root Certificate");

  // Check if the PEM file exists
  if (!SPIFFS.exists(filename))
  {
    Serial.println("ERROR: File not found mqtt-ca-root.pem");
    return false;
  }

  // Open the PEM file in read mode
  File file = SPIFFS.open(filename, "r");
  if (!file)
  {
    Serial.println("ERROR: Failed to open mqtt-ca-root.pem");
    return false;
  }

  // Get the size of the file
  size_t size = file.size();
  if (size == 0)
  {
    Serial.println("ERROR: Empty mqtt-ca-root.pem");
    file.close();
    return false;
  }

  // Use the loadCA() method to load the certificate directly from the file stream
  bool result = espClient.loadCACert(file, size);

  file.close();

  if (result)
  {
    Serial.println("CA Root Certificate loaded successfully");
  }
  else
  {
    Serial.println("ERROR: Failed to load CA Root Certificate");
  }

  return result;
}
#endif // tls

void MqttStart()
{
#ifdef MQTT_ENABLED
  MqttConnected = false;
  if (((millis() - LastTimeTriedToConnect) > (MQTT_RECONNECT_WAIT_SEC * 1000)) || (LastTimeTriedToConnect == 0))
  {
    LastTimeTriedToConnect = millis();
    MQTTclient.setSocketTimeout(5); // default 15 seconds, reduce for LAN
    MQTTclient.setServer(MQTT_BROKER, MQTT_PORT);
    MQTTclient.setCallback(callback);
    MQTTclient.setBufferSize(3000);
#ifdef MQTT_USE_TLS
    bool result = loadCARootCert();
    if (!result)
    {
      return; // load certificate failed -> do not continue
    }
#endif

    Serial.println("");
    Serial.println("Connecting to MQTT...");
    if (MQTTclient.connect(MQTT_CLIENT, MQTT_USERNAME, MQTT_PASSWORD))
    {
      Serial.println("MQTT connected");
      MqttConnected = true;
    }
    else
    {
      if (MQTTclient.state() == 5)
      {
        Serial.println("Error: Connection not allowed by broker, possible reasons:");
        Serial.println("- Device is already online. Wait some seconds until it appears offline");
        Serial.println("- Wrong Username or password. Check credentials");
        Serial.println("- Client Id does not belong to this username, verify ClientId");
      }
      else
      {
        Serial.println("Error: Not possible to connect to Broker!");
        Serial.print("Error code:");
        Serial.println(MQTTclient.state());
      }
      return; // do not continue if not connected
    }

    const char *topic1 = concat4(MQTT_CLIENT, "/", TopicLight, "/set");
    MQTTclient.subscribe(topic1);

    const char *topic2 = concat4(MQTT_CLIENT, "/", TopicRainbowSec, "/set");
    MQTTclient.subscribe(topic2);

    const char *topic3 = concat4(MQTT_CLIENT, "/", TopicDots, "/set");
    MQTTclient.subscribe(topic3);

    MQTTclient.subscribe(TopicHAstatus);
  }
#endif
}

int splitCommand(char *topic, char *tokens[], int tokensNumber)
{
  int mqttClientLength = strlen(MQTT_CLIENT);
  int topicLength = strlen(topic);
  int finalLength = topicLength - mqttClientLength + 2;
  char *command = (char *)malloc(finalLength);

  strncpy(command, topic + (mqttClientLength + 1), finalLength - 2);

  const char s[2] = "/";
  int pos = 0;
  tokens[0] = strtok(command, s);
  while (pos < tokensNumber - 1 && tokens[pos] != NULL)
  {
    pos++;
    tokens[pos] = strtok(NULL, s);
  }

  free(command);

  return pos;
}

void checkMqtt()
{
  MqttConnected = MQTTclient.connected();
  if (!MqttConnected)
  {
    MqttStart();
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{ // A new message has been received
#ifdef DEBUG_OUTPUT
  Serial.print("Received MQTT topic: ");
  Serial.print(topic); // long output
#endif
  int commandNumber = 10;
  char *command[commandNumber];
  commandNumber = splitCommand(topic, command, commandNumber);

  char message[length + 1];
  strncpy(message, (char *)payload, length);
  message[length] = '\0';

  if (commandNumber < 2)
  {
    Serial.println("Detected number of commands in MQTT message is lower then 2! -> Ignoring message because it is not valid!");
    return;
  }

  Serial.println();
  Serial.print("RX MQTT: ");
  Serial.print(topic);
  Serial.print(" ");
  Serial.println(message);

  // resend all discovery messages if HA is restarted
  if (strcmp(topic, TopicHAstatus) == 0 && strcmp(message, "online") == 0)
  {
    discoveryReported = false;
    Serial.println("HA was restarted. Resending discovery messages.");
  }

  if (strcmp(command[0], TopicLight) == 0 && strcmp(command[1], "set") == 0)
  {
    JsonDocument doc;
    deserializeJson(doc, payload, length);

    if (doc["state"].is<const char *>())
    {
      MqttCommandPower = strcmp(doc["state"], MQTT_STATE_ON) == 0;
      MqttCommandPowerReceived = true;
    }
    if (doc["brightness"].is<int>())
    {
      MqttCommandBrightness = doc["brightness"];
      MqttCommandBrightnessReceived = true;
    }
    if (doc["effect"].is<const char *>())
    {
      // MqttCommandEffect = doc["effect"];
      MqttCommandEffectReceived = true;
    }

    // RX MQTT: test02/back/set {"state":"ON","color":{"r":255,"g":114,"b":152}}
    if (doc["color"]["r"].is<int>())
    {
      byte r = doc["color"]["r"];
      byte g = doc["color"]["g"];
      byte b = doc["color"]["b"];
      MqttCommandColor = (r << 16) | (g << 8) | (b);
      Serial.printf("RGB = %d, %d, %d\r\n", r, g, b);
      MqttCommandColorReceived = true;
    }
    doc.clear();
  }

  if (strcmp(command[0], TopicRainbowSec) == 0 && strcmp(command[1], "set") == 0)
  {
    JsonDocument doc;
    deserializeJson(doc, payload, length);

    if (doc["state"].is<float>())
    {
      MqttCommandRainbowSec = doc["state"];
      MqttCommandRainbowSecReceived = true;
    }
    doc.clear();
  }

  if (strcmp(command[0], TopicDots) == 0 && strcmp(command[1], "set") == 0)
  {
    JsonDocument doc;
    deserializeJson(doc, payload, length);

    if (doc["state"].is<const char *>())
    {
      MqttCommandDots = strcmp(doc["state"], MQTT_STATE_ON) == 0;
      MqttCommandDotsReceived = true;
    }
    doc.clear();
  }
}

void MqttLoopFrequently()
{
#ifdef MQTT_ENABLED
  MQTTclient.loop();
  checkMqtt();
#endif
}

void MqttLoopInFreeTime()
{
#ifdef MQTT_ENABLED
  MqttReportBackOnChange();
  MqttPeriodicReportBackEverything();
#endif
}

#endif // mqtt
