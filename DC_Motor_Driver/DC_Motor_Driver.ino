/*
   Program code to control DC motors with the L298N driver
*/
#define input1 7    //Input pins of the L298N to control DIRECTION of the motor
#define input2 8


float getActiveTime(float distance) {
  return (distance + 0.1) / 3.275;
}

void setup(void) {
  Serial.begin(9600);
  pinMode(input1, OUTPUT);
  pinMode(input2, OUTPUT);
}

void loop(void) {
  controlSerial();
}

void controlSerial(void) {
  if (Serial.available()) {
    Serial.println("Received");
    char cmd = Serial.read();
    switch (cmd){
      case 's': stopArm(); break;
      case 'a': 
      lengthenArm();
      delay((int) (getActiveTime(5.5) * 1000));
      stopArm(); break;
      case 'z': 
      lengthenArm();
      delay((int) (getActiveTime(5.5) * 1000));
      stopArm(); break;
    }
  }
}

void shortenArm(void) {
  digitalWrite(input1, LOW);
  digitalWrite(input2, HIGH);
}

void lengthenArm(void) {
  digitalWrite(input1, HIGH);
  digitalWrite(input2, LOW);
}

void stopArm(void) {
  digitalWrite(input1, LOW);
  digitalWrite(input2, LOW);
}
