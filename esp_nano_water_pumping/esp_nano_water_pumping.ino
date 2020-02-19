#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>

// config:
const char *ssid = "RoboRemoApp";  // You have to connect your phone to this Access Point
const char *pw = "87654321"; // and this is the password
IPAddress ip(192, 168, 0, 1); // Part1 of the RoboRemo connection
IPAddress netmask(255, 255, 255, 0);
const int port = 6000; // Part2 of the RoboRemo connection => part 1+2 together => 192.168.0.1:6000

WiFiServer server(port);
WiFiClient client;

const byte RX_PIN = D5;
const byte TX_PIN = D6;

SoftwareSerial ARDUINO_NANO(RX_PIN, TX_PIN);
char tx_buffer[32];

char buffer[32];
const char EOL = '\n';    //command terminator (end of line)
const char separator = ' ';
String cmd;
float value;

void parseSerial() {
  uint8_t byteCount = client.readBytesUntil(EOL, buffer, sizeof(buffer));   //read until EOL, put all to buffer.
  String _read = String(buffer);    //Use Strings to make character processing easier.
  memset(buffer, 0, sizeof(buffer));    //clear, empty buffer

  // Select cmd from associated data
  int index = _read.indexOf(separator);
  cmd = _read.substring(0, index);
  cmd.trim();
  cmd.toLowerCase();
  Serial.println(cmd);

  // Extract cmd data.
  String data = _read.substring(index + 1);
  data.trim();
  value = data.toFloat();
  Serial.println(value);
}

String constructMessage(String id, float value) {
  String message = "";
  message += id;
  message += F(" ");
  message += String(value, 2);
  message += F("\n");
  return message;
}

void sendCommand() {
  String message = constructMessage(cmd, value);
  Serial.println("Sending: " + message);
  message.toCharArray(tx_buffer, sizeof(tx_buffer));
  ARDUINO_NANO.write(tx_buffer);
  ARDUINO_NANO.flush();
  memset(tx_buffer, 0, sizeof(tx_buffer));
}

void setup() {
  Serial.begin(115200);
  ARDUINO_NANO.begin(4800);

  WiFi.softAPConfig(ip, ip, netmask); // configure ip address for softAP
  WiFi.softAP(ssid, pw);      // configure ssid and password for softAP

  server.begin(); // start TCP server

  Serial.println("ESP8266 RC receiver 1.1 powered by RoboRemo");
  Serial.println((String)"SSID: " + ssid + "  PASS: " + pw);
  Serial.println((String)"RoboRemo app must connect to " + ip.toString() + ":" + port);
}

void loop() {
  if (!client.connected()) {
    client = server.available();
    return;
  }

  //RoboRemo can generate command with a button => "set press action" of "set release action" => "send *cmd".
  // If client is connected.
  if (client.available()) {
    Serial.println("Received from RoboRemo: ");
    parseSerial();
    sendCommand();
  }
}
