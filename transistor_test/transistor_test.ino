#define CONTROL_PIN 8
#define READ_PIN A0

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(CONTROL_PIN, OUTPUT);
  pinMode(READ_PIN, INPUT);
}

int c = 0;
bool channelOn = false;

void loop() {
  // put your main code here, to run repeatedly:
  if (c % 100 == 0) {
    digitalWrite(CONTROL_PIN, channelOn ? LOW : HIGH);
    channelOn = !channelOn;
  }
  
  char serialStr[128];
  sprintf(serialStr, "Channel %s\t Read: %d", channelOn ? "on" : "off", analogRead(READ_PIN));
  Serial.println(serialStr);
  
  c++;
  delay(10);
}
