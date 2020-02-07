/*
   RoboRemoEasySketch by Laurens Korste from boktorrobotica.nl
   Modify this sketch and upload it to a ESP8266 divice sush as NodeMCU, Wemos D1, Wemos D1 MINI, ESP12 etc..
   You can uses all ASSII characters so more then 200 different actions.
   Whith this sketch it is not possible to sent numbers bigger then 9 or strings (words) unless you modify the sketch.
   If you want to sent strings of big numbers, look for another sketch for example on roboremo.com

   This is a WiFiServer. From divice setting choice RoboRemo as WiFi-connection. If you like you can add a password
   If sketch uploaded, start the Arduino serial monitor where you can read the IP-adres then:
   From the RoboRemo-app choice menu => connect => Internet (TCP) => other (if not exist) => fillin IP-adres
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Adafruit_BMP280.h>

#define RELAY D4

// config:
const char *ssid = "RoboRemoApp";  // You have to connect your phone to this Access Point
const char *pw = "87654321"; // and this is the password
IPAddress ip(192, 168, 0, 1); // Part1 of the RoboRemo connection
IPAddress netmask(255, 255, 255, 0);
const int port = 6000; // Part2 of the RoboRemo connection => part 1+2 together => 192.168.0.1:6000

WiFiServer server(port);
WiFiClient client;

Adafruit_BMP280 tempSensor;

char buffer[32];
const char EOL = '\n';    //command terminator (end of line)
const char separator = ' ';
String param;
int setPoint;

int currentTemp;
bool heaterState;

void parseSerial() {
  uint8_t byteCount = client.readBytesUntil(EOL, buffer, sizeof(buffer));   //read until EOL, put all to buffer.
  String _read = String(buffer);    //Use Strings to make character processing easier.
  memset(buffer, 0, sizeof(buffer));    //clear, empty buffer

  // Select param from associated data
  int index = _read.indexOf(separator);
  param = _read.substring(0, index);
  param.trim();
  param.toLowerCase();
  Serial.println(param);

  // Extract param data.
  String data = _read.substring(index + 1);
  data.trim();
  setPoint = data.toInt();
  setPoint = setPoint;
  Serial.println(setPoint);
}

String constructMessage(String id, float value) {
  String message = "";
  message += id;
  message += F(" ");
  message += String(value, 2);
  message += F("\n");
  return message;
}

void initBMP280(void) {
  Wire.begin();
  while (!tempSensor.begin(0x77)) {
    Serial.println("Could not find a valid BMP280 indoor sensor, check wiring!");
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.softAPConfig(ip, ip, netmask); // configure ip address for softAP
  WiFi.softAP(ssid, pw);      // configure ssid and password for softAP

  server.begin(); // start TCP server

  Serial.println("ESP8266 RC receiver 1.1 powered by RoboRemo");
  Serial.println((String)"SSID: " + ssid + "  PASS: " + pw);
  Serial.println((String)"RoboRemo app must connect to " + ip.toString() + ":" + port);

  setPoint = 0;
  heaterState = false;
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);
  initBMP280();
}

void loop() {
  float currentTemp = tempSensor.readTemperature();
  // Keep looking for a client.
  if (!client.connected()) {
    client = server.available();
    return;
  }

  //RoboRemo can generate command with a button => "set press action" of "set release action" => "send *cmd".
  // If client is connected.
  if (client.available()) {
    parseSerial();
    if (currentTemp > setPoint + 0.2) {
      heaterState = false;
      digitalWrite(RELAY, LOW);
    } else if (currentTemp < setPoint - 0.2) {
      heaterState = true;
      digitalWrite(RELAY, HIGH);
    }
  }
  client.print(constructMessage("plot", (float) currentTemp));
  client.print(constructMessage("log", (float) currentTemp));
  if (heaterState) {
    client.print("led 1\n");
  } else {
    client.print("led 0\n");
  }
  delay(200);
}
