#include <Wire.h>
#include <Servo.h>
#include "Adafruit_TCS34725.h"

/* TCS34725 RGB Color sensor
   Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 3.3V DC
   Connect GROUND to common ground */

#define dcm_input1 2    //Input pins of the L298N to control DIRECTION of the motor
#define dcm_input2 3
//Stepper Motors
#define sm1_DirectionPin 4
#define sm1_StepsPin 5
#define sm2_DirectionPin 6
#define sm2_StepsPin 7
#define sm3_DirectionPin 8
#define sm3_StepsPin 9
//Colorimeter
#define LED 10
#define servo_Pin 11

char buffer[32];
const char EOL = '\n';    //command terminator (end of line)
const char separator = ' ';
char cmd;
float value;

Servo servo;

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

uint16_t r, g, b, c;

void initColourSensor(void) {
  if (tcs.begin()) {
    Serial.println("Connected to Colorimeter");
  } else {
    Serial.println("No TCS34725 found ... check your connections and restart");
    while (1);
  }
}

void takeColour(void) {
  digitalWrite(LED, HIGH);
  tcs.getRawData(&r, &g, &b, &c);

  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
  Serial.println(" ");
  digitalWrite(LED, LOW);
}

int getNitrate(void) {  // Calculates the concentration of Nitrate (mg/L) based on carried out calibration.
  return (int) (368 - 35 * log(b));
}

void parseSerial(void) {
  uint8_t byteCount = Serial.readBytesUntil(EOL, buffer, sizeof(buffer));   //read until EOL, put all to buffer.
  String _read = String(buffer);    //Use Strings to make character processing easier.
  memset(buffer, 0, sizeof(buffer));    //clear, empty buffer

  // Select sensor from associated data
  int index = _read.indexOf(separator);
  cmd = _read.charAt(0);

  String data = _read.substring(index + 1);
  data.trim();
  value = data.toFloat();
}

void setup(void) {
  pinMode(dcm_input1, OUTPUT);
  pinMode(dcm_input2, OUTPUT);
  pinMode(sm1_DirectionPin, OUTPUT);
  pinMode(sm1_StepsPin, OUTPUT);
  pinMode(sm2_DirectionPin, OUTPUT);
  pinMode(sm2_StepsPin, OUTPUT);
  pinMode(sm3_DirectionPin, OUTPUT);
  pinMode(sm3_StepsPin, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(servo_Pin, OUTPUT);
  servo.attach(servo_Pin);
  // initColourSensor();
  Serial.begin(115200);
}

void loop(void) {
  //getSerial();

//  Troubleshoot Stepper Motors
  rotate(1600, 0.5, sm1_DirectionPin, sm1_StepsPin);
  delay(1000);
  rotate(-1600, 0.5, sm1_DirectionPin, sm1_StepsPin);
  delay(1000);
//  rotate(1600, 0.5, sm2_DirectionPin, sm2_StepsPin);
//  rotate(-1600, 0.5, sm2_DirectionPin, sm2_StepsPin);
}

float getActiveTime(float distance) {
  return (distance + 0.1) / 3.275;
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

void rotate(int steps, float speed, byte motorPin, byte stepsPin) {
  int direction;
  if (steps > 0) {
    direction = HIGH;
  } else {
    direction = LOW;
  }
  speed = 1 / speed * 70; //Calculating speed
  steps = abs(steps);
  digitalWrite(motorPin, direction);
  /*Steppin'*/
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepsPin, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepsPin, LOW);
    delayMicroseconds(2000);
  }
}

void startNitrateAssay(void) {
  lengthenArm();
  delay((int) (getActiveTime(4) * 1000));
  stopArm();
  //First Titrator
  rotate(40000, 0.5, sm1_DirectionPin, sm1_StepsPin);

  lengthenArm();
  delay((int) (getActiveTime(4) * 1000));
  stopArm();
  //Second Titrator
  rotate(40000, 0.5, sm2_DirectionPin, sm2_StepsPin);

  lengthenArm();
  delay((int) (getActiveTime(17) * 1000));
  stopArm();
  //Colorimeter
  //  delay(3000 * 60);
  //  takeColour();
  Serial.println("Nitrate assay has finished:");
  Serial.print("The concentration is: ");
  Serial.println(getNitrate());
}


void getSerial(void) {
  if (Serial.available()) {
    Serial.println("Received");
    parseSerial();
    switch (cmd) {
      // Water Testing
      case 'w': startNitrateAssay(); break;
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
        rotate(40000, 0.5, sm1_DirectionPin, sm1_StepsPin);
        break;
      case '2':
        rotate(40000, 0.5, sm2_DirectionPin, sm2_StepsPin);
        break;
      case '3':
        rotate(40000, 0.5, sm3_DirectionPin, sm3_StepsPin);
        break;
      case '4':
        servo.write(value);
        break;
    }
  }
}
