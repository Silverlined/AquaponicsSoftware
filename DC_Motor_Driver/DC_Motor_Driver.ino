/*
   Program code to control DC motors with the L298N driver
*/
#define input1 10    //Input pins of the L298N to control DIRECTION of the motor
#define input2 11

char buffer[32];
const char EOL = '\n';    //command terminator (end of line)
const char separator = ' ';
char cmd;
float value;

int getActiveTime(float distance) {
  return (int) (distance * 483 - 24.4);
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

void setup(void) {
  Serial.begin(115200);
  pinMode(input1, OUTPUT);
  pinMode(input2, OUTPUT);
}

void loop(void) {
  controlSerial();
}

void controlSerial(void) {
  if (Serial.available()) {
    Serial.println("Received");
    parseSerial();
    switch (cmd) {
      case 's': stopArm(); break;
      case 'a':
        lengthenArm();
        delay((int) (value * 1000));
        stopArm(); break;
      case 'z':
        shortenArm();
        delay((int) (value * 1000));
        stopArm(); break;
    }
  }
}

void lengthenArm(void) {
  digitalWrite(input1, LOW);
  digitalWrite(input2, HIGH);
}

void shortenArm(void) {
  digitalWrite(input1, HIGH);
  digitalWrite(input2, LOW);
}

void stopArm(void) {
  digitalWrite(input1, LOW);
  digitalWrite(input2, LOW);
}
