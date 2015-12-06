#define NUM_CHANNELS 2
const int CHANNEL_PINS[] = {9, 10};
#define READ_PIN A0

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int i = 0; i < NUM_CHANNELS; ++i) {
    pinMode(CHANNEL_PINS[i], OUTPUT);
  }
  pinMode(READ_PIN, INPUT);
}

int c = 0;
int activeChannel = 0;

void loop() {
  // put your main code here, to run repeatedly:
  if (c % 100 == 0) {
    for (int i = 0; i < NUM_CHANNELS; ++i) {
      digitalWrite(CHANNEL_PINS[i], i == activeChannel ? HIGH : LOW);
    }
    activeChannel = (activeChannel + 1) % NUM_CHANNELS;
  }
  
  char serialStr[128];
  sprintf(serialStr, "Channel %d\t Read: %d", activeChannel, analogRead(READ_PIN));
  Serial.println(serialStr);
  
  c++;
  delay(10);
}
