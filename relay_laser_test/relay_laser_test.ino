#define RELAY_PIN 11

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
}

int c = 0;
bool relayOn = true;

void loop() {
  // put your main code here, to run repeatedly:
  if (c % 100 == 0) {
    digitalWrite(RELAY_PIN, relayOn ? HIGH : LOW);
    relayOn = !relayOn;
  }
  
  char serialStr[128];
  sprintf(serialStr, "Relay %s", relayOn ? "on" : "off");
  Serial.println(serialStr);
  
  c++;
  delay(10);
}
