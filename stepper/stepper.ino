#include<Servo.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"

#define sm1_DirectionPin 9
#define sm1_StepsPin 8
#define sm2_DirectionPin 7
#define sm2_StepsPin 6
#define sm3_DirectionPin 5
#define sm3_StepsPin 4
const int STEPS_PER_REVOLUTION = 1600;

#define dcm_input1 10    //Input pins of the L298N to control DIRECTION of the motor
#define dcm_input2 11
#define servo_pin 12
Servo servo;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);
uint16_t r, g, b, c;

char buffer[32];
const char EOL = '\n';    //command terminator (end of line)
const char separator = ' ';
char cmd;
float value;

void takeColour(void) {
  tcs.getRawData(&r, &g, &b, &c);

  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
  Serial.println(" ");
}

float getNitrate(void) {  // Calculates the concentration of Nitrate (mg/L) based on carried out calibration.
  return (2706 * exp(-0.00038 * b));
}

// Not Finished
// void startNitrateAssay(void) {
//   lengthenArm();
//   delay((int) (getActiveTime(4) * 1000));
//   stopArm();
//   //First Titrator
//   rotate(5, sm1_DirectionPin, sm1_StepsPin);

//   lengthenArm();
//   delay((int) (getActiveTime(4) * 1000));
//   stopArm();
//   //Second Titrator
//   rotate(5, sm2_DirectionPin, sm2_StepsPin);

//   lengthenArm();
//   delay((int) (getActiveTime(17) * 1000));
//   stopArm();
//   //Colorimeter
//   //delay(3000 * 60);
//   takeColour();
//   Serial.println("Nitrate assay has finished:");
//   Serial.print("The concentration is: ");
// }

void initColourSensor(void) {
  if (tcs.begin()) {
    Serial.println("Connected to Colorimeter");
  } else {
    Serial.println("No TCS34725 found ... check your connections and restart");
    while (1);
  }
}

void parseSerial(void) {
  uint8_t byteCount = Serial.readBytesUntil(EOL, buffer, sizeof(buffer));   //read until EOL, put all to buffer.
  String _read = String(buffer);    //Use Strings to make character processing easier.
  memset(buffer, 0, sizeof(buffer));    //clear, empty buffer

  // Select actuator from associated data
  int index = _read.indexOf(separator);
  cmd = _read.charAt(0);

  String data = _read.substring(index + 1);
  data.trim();
  value = data.toFloat();
}

void lengthenArm(void) {
  digitalWrite(dcm_input1, LOW);
  digitalWrite(dcm_input2, HIGH);
}

void shortenArm(void) {
  digitalWrite(dcm_input1, HIGH);
  digitalWrite(dcm_input2, LOW);
}

void stopArm(void) {
  digitalWrite(dcm_input1, LOW);
  digitalWrite(dcm_input2, LOW);
}

float getActiveTime(float distance) {
  return (distance + 2.1) / 3.275;
}

float getTurns(float mL) {
  return 69 * mL + 2.5;
}

void getSerial(void) {
  if (Serial.available()) {
    Serial.println("Received");
    parseSerial();
    switch (cmd) {
      // Linear DC Motor Control
      case 's': stopArm(); break;
      case 'a':
        lengthenArm();
        delay((int) (getActiveTime(value) * 1000));
        stopArm(); break;
      case 'z':
        shortenArm();
        delay((int) (getActiveTime(value) * 1000));
        stopArm(); break;
      case '1':
        rotate(getTurns(value), sm1_DirectionPin, sm1_StepsPin);
        break;
      case '2':
        rotate(getTurns(value), sm2_DirectionPin, sm2_StepsPin);
        break;
      case '3':
        rotate(getTurns(value), sm3_DirectionPin, sm3_StepsPin);
        break;
      case '4':
        servo.write(value);
        break;
      case '5':
        takeColour();
        Serial.print("Nitrate Concentration (mg/L): ");
        Seria.prinln(getNitrate());
        break;
    }
  }
}

void rotate(int turns, byte directionPin, byte stepsPin) {
  int direction;
  int steps = turns * STEPS_PER_REVOLUTION; 
  if (steps > 0) {
    direction = HIGH;
  } else {
    direction = LOW;
  }
  steps = abs(steps);
  digitalWrite(directionPin, direction);
  /*Steppin'*/
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepsPin, HIGH);
    delayMicroseconds(200);
    digitalWrite(stepsPin, LOW);
    delayMicroseconds(200);
  }
  delay(500);
}

void setup() {
  pinMode(dcm_input1, OUTPUT);
  pinMode(dcm_input2, OUTPUT);
  pinMode(servo_pin, OUTPUT);

  pinMode(sm1_DirectionPin, OUTPUT);
  pinMode(sm1_StepsPin, OUTPUT);
  pinMode(sm2_DirectionPin, OUTPUT);
  pinMode(sm2_StepsPin, OUTPUT);
  pinMode(sm3_DirectionPin, OUTPUT);
  pinMode(sm3_StepsPin, OUTPUT);
  digitalWrite(sm1_DirectionPin, LOW);
  digitalWrite(sm1_StepsPin, LOW);
  digitalWrite(sm2_DirectionPin, LOW);
  digitalWrite(sm2_StepsPin, LOW);
  digitalWrite(sm3_DirectionPin, LOW);
  digitalWrite(sm3_StepsPin, LOW);

  initColourSensor(); 

  servo.attach(servo_pin);
  servo.write(180);

  Serial.begin(115200);
}

void loop() {
  getSerial();
}
