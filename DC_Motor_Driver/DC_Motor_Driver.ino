/*
   Program code to control DC motors with the L298N driver
*/
#define input1 7    //Input pins of the L298N to control DIRECTION of the motor
#define input2 8


void setup() {
  Serial.begin(9600);
  pinMode(input1, OUTPUT);
  pinMode(input2, OUTPUT);
}

void loop() {
  lengthenArm();
  delay(5000);
  shortenArm();
  delay(5000);
}

void lengthenArm() {
  digitalWrite(input1, LOW);
  digitalWrite(input2, HIGH);
}

void shortenArm() {
  digitalWrite(input1, HIGH);
  digitalWrite(input2, LOW);
}

