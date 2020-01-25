#include <Wire.h>
#include "Adafruit_TCS34725.h"

/* Example code for the Adafruit TCS34725 breakout library */
#define LED 7

/* Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 3.3V DC
   Connect GROUND to common ground */

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

uint16_t getNitrate(void) {
  return (uint16_t) (368 - 35 * log(b));
}

uint16_t getAmmonia(void) {

}

void setup(void) {
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  initColourSensor();
}

void loop(void) {
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    if (cmd == 'c') {
      takeColour();
    }
  }
}
