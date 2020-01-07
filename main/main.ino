#include <SoftwareSerial.h>

#define TEMP_SENSOR_MEASURE_DELAY 60000 // 1 minute
#define TEMP_SENSOR_SEND_DELAY 1800000 // 30 minutes
#define waterTempPin A0
#define waterConductivityPin A1
#define loadCellPin A2
#define tempHumiditySensor 7

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


void setup() {
  Serial.begin(115200);
  esp8266Serial.begin(4800);
  waterTempMeasureTimer = 0;
  waterTempSendTimer = TEMP_SENSOR_MEASURE_DELAY * MA_SIZE; // Wait until the temp moving average is filled before sending data.
}

void loop() {
  currentTime = millis();

  if (currentTime > waterTempMeasureTimer) {
    takeWaterTempReading();
    waterTempMeasureTimer = currentTime + TEMP_SENSOR_MEASURE_DELAY;
  }

  if (currentTime > waterTempSendTimer) {
    sendSensorMeasurement("WTEMP", getAverageWaterTemp());
    waterTempSendTimer = currentTime + TEMP_SENSOR_SEND_DELAY;
  }
}
