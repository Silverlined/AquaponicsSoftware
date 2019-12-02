#include<PubSubClient.h>
#include <ESP8266WiFi.h>

// Wi-Fi configuration
const char* ssid = "ZiggoC6C6A6C";
const char* password = "3YjjjrzteyFk";

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

// MQTT configuration
const char* server_address = "192.168.178.25";
const char* topic = "aquaponics";
const char* clientName = "NodeMCU";

char buffer[32];
const char EOL = '\n';    //command terminator (end of line)
const char separator = ' ';
String sensor;
float value;

void parseSerial(void) {
  uint8_t byteCount = Serial.readBytesUntil(EOL, buffer, sizeof(buffer));   //read until EOL, put all to buffer.
  String _read = String(buffer);    //Use Strings to make character processing easier.
  memset(buffer, 0, sizeof(buffer));    //clear, empty buffer

  // Select sensor from associated data
  int index = _read.indexOf(separator);
  sensor = _read.substring(0, index);
  sensor.trim();
  sensor.toUpperCase();

  // Extract sensor data.
  String data = _read.substring(index+1);
  data.trim();
  value = data.toFloat();
}

void connectWifi(void) {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting to WiFi..");
  }

  Serial.println("Connected to the network");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqttReconnect(void) {
  while (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqtt_client.connect(clientName)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(", try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqttEmit(String topic, String value) {
  Serial.println(mqtt_client.publish((char*) topic.c_str(), (char*) value.c_str()));
}

void setup() {
  Serial.begin(115200);
  connectWifi();
  mqtt_client.setServer(server_address, 1883);
}

void loop() {
  if (!mqtt_client.connected()) {
    mqttReconnect();
  }
  float sensor = (float)((analogRead(A0) * 100) / 4095);
  String message = "temp,site=room1 value=28";
  mqttEmit(topic, message);
  delay(3000);
}
