#define TEMP_SENSOR_PIN A0
#define TEMP_SENSOR_MEASURE_DELAY 60000 // 1 minute
#define TEMP_SENSOR_SEND_DELAY 1800000 // 30 minutes
#define TEMP_SENSOR_MOVING_AVERAGE_SIZE 10

float tempSensorMovingAverage[TEMP_SENSOR_MOVING_AVERAGE_SIZE];
long tempSensorMeasureCounter;
long tempSensorSendCounter;


void setup() {
  Serial.begin(9600);
  tempSensorMeasureCounter = 0;
  tempSensorSendCounter = TEMP_SENSOR_MEASURE_DELAY * TEMP_SENSOR_MOVING_AVERAGE_SIZE; // Wait until the temp moving average is filled before sending data.
}


void loop() {

  long currentTime = millis();

  if (currentTime > tempSensorMeasureCounter) {
    takeTempSensorReading();
    tempSensorMeasureCounter = currentTime + TEMP_SENSOR_MEASURE_DELAY;
  }

  if (currentTime > tempSensorSendCounter) {
    sendTempSensorMeasurement();
    tempSensorSendCounter = currentTime + TEMP_SENSOR_SEND_DELAY;
  }
}


void takeTempSensorReading() {
  int reading = analogRead(TEMP_SENSOR_PIN);
  float temp = -0.0913 * reading + 72; // Make sure the correct fomula is used for the sensor

  // Shift all values in the array 1 place
  for (int i = TEMP_SENSOR_MOVING_AVERAGE_SIZE - 1; i > 0; i--) {
    tempSensorMovingAverage[i] = tempSensorMovingAverage[i - 1];
  }
  // Add the new value to the beginning of the array
  tempSensorMovingAverage[0] = temp;
}


float getAverageTempValue() {
  float result = 0;
  for (int i = 0; i < TEMP_SENSOR_MOVING_AVERAGE_SIZE; i++) {
    result = result + tempSensorMovingAverage[i];
  }
  result = result / TEMP_SENSOR_MOVING_AVERAGE_SIZE;
  return result;
}


void sendTempSensorMeasurement() {
  float averageTemp = getAverageTempValue();
  Serial.println(averageTemp);
}
