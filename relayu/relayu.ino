#define sv1_relay A0
#define sv2_relay A1
#define sv3_relay A2
#define sv4_relay A3

#define sm1_DirectionPin 9
#define sm1_StepsPin 8
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

  // Select actuator from associated data
  int index = _read.indexOf(separator);
  cmd = _read.charAt(0);

  String data = _read.substring(index + 1);
  data.trim();
  value = data.toFloat();
}

void getSerial(void) {
  if (Serial.available()) {
    Serial.println("Received");
    parseSerial();
    switch (cmd) {
      case '1':
        digitalWrite(sv1_relay, LOW);
        break;
      case '2':
        digitalWrite(sv2_relay, LOW);
        break;
      case '3':
        digitalWrite(sv3_relay, LOW);
        break;
      case '4':
        digitalWrite(sv4_relay, LOW);
        break;
      case 'q':
        digitalWrite(sv1_relay, HIGH);
        break;
      case 'w':
        digitalWrite(sv2_relay, HIGH);
        break;
      case 'e':
        digitalWrite(sv3_relay, HIGH);
        break;
      case 'r':
        digitalWrite(sv4_relay, HIGH);
        break;
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(sv1_relay, OUTPUT);
  pinMode(sv2_relay, OUTPUT);
  pinMode(sv3_relay, OUTPUT);
  pinMode(sv4_relay, OUTPUT);
  digitalWrite(sv1_relay, HIGH);
  digitalWrite(sv2_relay, HIGH);
  digitalWrite(sv3_relay, HIGH);
  digitalWrite(sv4_relay, HIGH);
  Serial.begin(115200);
  Serial.println("DONE");
}

void loop() {
  getSerial();
}
