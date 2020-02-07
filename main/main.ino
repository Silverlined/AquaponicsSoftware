#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include "SparkFun_SCD30_Arduino_Library.h"
#include "HX711.h"
#include <Wire.h>

#define TEMP_SENSOR_MEASURE_DELAY 60000 // 1 minute
#define TEMP_SENSOR_SEND_DELAY 1800000 // 30 minutes
#define waterTempPin A0
#define waterConductivityPin A1
#define loadCellPin A2
#define tempHumiditySensor 7
#define LDR A3
#define LDR_THRESHOLD 200

#define BackLeft 3
#define BackRight 4
#define FrontLeft 5
#define FrontRight 6
#define CLK 9 //clock pin

HX711 scale;
HX711 scale1;
HX711 scale2;
HX711 scale3;

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

void initLoadCells(void) {
  scale.begin(FrontLeft, CLK);
  scale.set_scale();
  //  scale.tare(); //Reset the scale to 0
  scale1.begin(FrontRight, CLK);
  scale1.set_scale();
  //  scale1.tare(); //Reset the scale to 0
  scale2.begin(BackLeft, CLK);
  scale2.set_scale();
  //  scale2.tare(); //Reset the scale to 0
  scale3.begin(BackRight, CLK);
  scale3.set_scale();
  //  scale3.tare(); //Reset the scale to 0
}

float getWeight() {
  float zero_factor = scale.read_average(); //reading the raw value from the load cell, FL
  float zero_factor1 = scale1.read_average(); //FR
  float zero_factor2 = scale2.read_average(); //BL
  float zero_factor3 = scale3.read_average(); //BR

  //for mean calibration (unsure)= (zero_factor + 2929.6)/-24279
  float Weight1 = (zero_factor - 1923.5) / -23058; //converting to kg & calibrating it, FL
  float Weight2 = (zero_factor1 - 16736) / -23007; //FR
  float Weight3 = (zero_factor2 + 17795) / -25892; //BL
  float Weight4 = (zero_factor3 + 12583) / -25160; //BR

  float Total = abs(Weight1) + abs(Weight2) + abs(Weight3) + abs(Weight4); //combining 4 load cell value and make the total weight value
  return Total;
}


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
  int value = analogRead(LDR); // read the value from the LDR]
  return (value > LDR_THRESHOLD) ? 1 : 0;
}

void measureAirTemp(void) {
  lastAirTempInside = bmp_in.readTemperature();
  lastAirTempOutside = bmp_out.readTemperature();
}

void setup() {
  Serial.begin(115200);
  esp8266Serial.begin(4800);
  initBMP280();
  initLoadCells();
  pinMode(LDR, INPUT);
  waterTempMeasureTimer = 0;
  waterTempSendTimer = TEMP_SENSOR_MEASURE_DELAY * MA_SIZE; // Wait until the temp moving average is filled before sending data.
  //  Unit test
  //  Serial.println(getWeight());
  //  Serial.println("SETUP DONE");
  //  measureAirTemp();
  //  Serial.println(lastAirTempInside);
  //  Serial.println(lastAirTempOutside);
  //  Serial.println(co2_sensor.getHumidity());
  //  Serial.println(co2_sensor.getCO2());
  //  Serial.println(analogRead(LDR));
  //  measureAirTemp();
  //  sendSensorMeasurement("INTEMP", lastAirTempInside);
  //  delay(1000);
  //  sendSensorMeasurement("OUTTEMP", lastAirTempOutside);
  //  delay(1000);
  //  sendSensorMeasurement("RH", (float) co2_sensor.getHumidity());
  //  delay(1000);
  //  sendSensorMeasurement("CO2", (float) co2_sensor.getCO2());
  //  delay(1000);
  //  sendSensorMeasurement("LIGHT", (float) getLights());
  //  delay(1000);
  //  sendSensorMeasurement("WEIGHT", (float) getWeight());
  //  delay(1000);
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
    delay(1000);
    sendSensorMeasurement("INTEMP", lastAirTempInside);
    delay(1000);
    lastAirTempOutside = 20.59;
    sendSensorMeasurement("OUTTEMP", lastAirTempOutside);
    delay(1000);
    sendSensorMeasurement("RH", (float) co2_sensor.getHumidity());
    delay(1000);
    sendSensorMeasurement("CO2", (float) co2_sensor.getCO2());
    delay(1000);
    sendSensorMeasurement("LIGHT", (float) getLights());
    delay(1000);
    sendSensorMeasurement("pH", 8.74);
    delay(1000);
    sendSensorMeasurement("EC", 340.00);
    delay(1000);
    sendSensorMeasurement("WEIGHT", (float) getWeight());
    delay(1000);

    waterTempSendTimer = currentTime + TEMP_SENSOR_SEND_DELAY;
    delay(1000);
  }
}
