#define sm1_DirectionPin 9
#define sm1_StepsPin 8
//#define sm2_DirectionPin 6
//#define sm2_StepsPin 7

const int STEPS_PER_REVOLUTION = 1600;
char buffer[32];
const char EOL = '\n';    //command terminator (end of line)
const char separator = ' ';
char cmd;
float value;

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

void setup() {
  pinMode(sm1_DirectionPin, OUTPUT);
  pinMode(sm1_StepsPin, OUTPUT);
  digitalWrite(sm1_DirectionPin, LOW);
  digitalWrite(sm1_StepsPin, LOW);
  
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  rotate(-5, sm1_DirectionPin, sm1_StepsPin);
  rotate(5, sm1_DirectionPin, sm1_StepsPin);
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
