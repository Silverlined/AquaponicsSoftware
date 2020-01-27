#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include "SparkFun_SCD30_Arduino_Library.h"

#define TEMP_SENSOR_MEASURE_DELAY 60000 // 1 minute
#define TEMP_SENSOR_SEND_DELAY 1800000 // 30 minutes
#define waterTempPin A0
#define waterConductivityPin A1
#define loadCellPin A2
#define tempHumiditySensor 7
#define LDR A3
#define LDR_THRESHOLD 200

Adafruit_BMP280 bmp_in;
Adafruit_BMP280 bmp_out;
SCD30 co2_sensor;

float lastAirTempInside = 0;
float lastAirTempOutside = 0;

const byte MA_SIZE = 10;

SoftwareSerial esp8266Serial(10, 11); // RX, TX
char buffer[32];

float waterTempMA[MA_SIZE];
long waterTempMeasureTimer;
long waterTempSendTimer;

long currentTime;

float getAverageWaterTemp() {
  float result = 0;
  for (int i = 0; i < MA_SIZE; i++) {
    result = result + waterTempMA[i];
  }
  result = result / MA_SIZE;
  return result;
}

String constructMessage(String sensor, float value) {
  String message = "";
  message += sensor;
  message += F(" ");
  message += String(value, 2);
  return message;
}

void takeWaterTempReading() {
  int reading = analogRead(waterTempPin);
  float temp = -0.0879 * reading + 69.8; // Make sure the correct fomula is used for the sensor

  // Shift all values in the array 1 place
  for (int i = MA_SIZE - 1; i > 0; i--) {
    waterTempMA[i] = waterTempMA[i - 1];
  }
  // Add the new value to the beginning of the array
  waterTempMA[0] = temp;
}

void sendSensorMeasurement(String tag, float value) {
  String a = constructMessage(tag, value);
  Serial.println(a);
  a.toCharArray(buffer, sizeof(buffer));
  esp8266Serial.write(buffer);
  esp8266Serial.flush();
}

void initBMP280(void) {
  Wire.begin(); //Start the wire hardware that may be supported by your platform
  while (!bmp_in.begin(0x76)) {
    Serial.println("Could not find a valid BMP280 indoor sensor, check wiring!");
  }
  if (!bmp_out.begin(0x77)) {
    Serial.println("Could not find a valid BMP280 outdoor sensor, check wiring!");
  }
  co2_sensor.begin(Wire); //Pass the Wire port to the .begin() function
}

bool getLights() {
  int value = analogRead(LDRpin); // read the value from the LDR]
  (value > LDR_THRESHOLD) ? return true : return false;
}

void measureAirTemp(void) {
  lastAirTempInside = bmp_in.readTemperature();
  lastAirTempOutside = bmp_out.readTemperature();
}

void setup() {
  Serial.begin(115200);
  esp8266Serial.begin(4800);
  initBMP280();
  waterTempMeasureTimer = 0;
  waterTempSendTimer = TEMP_SENSOR_MEASURE_DELAY * MA_SIZE; // Wait until the temp moving average is filled before sending data.
  pinMode(LDR, INPUT);
}

void loop() {
  currentTime = millis();

  if (currentTime > waterTempMeasureTimer) {
    takeWaterTempReading();
    waterTempMeasureTimer = currentTime + TEMP_SENSOR_MEASURE_DELAY;
  }
  if (currentTime > waterTempSendTimer) {
    measureAirTemp();
    sendSensorMeasurement("WTEMP", getAverageWaterTemp());
    sendSensorMeasurement("INTEMP", lastAirTempInside);
    sendSensorMeasurement("OUTTEMP", lastAirTempOutside);
    sendSensorMeasurement("RH", (float) co2_sensor.getHumidity());
    sendSensorMeasurement("CO2", (float) co2_sensor.getCO2());
    sendSensorMeasurement("LIGHT", (float) getLights());
    waterTempSendTimer = currentTime + TEMP_SENSOR_SEND_DELAY;
  }
}
