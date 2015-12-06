#define RELAY_PIN 11
#define CHANNEL_PIN 10
#define PHOTO_PIN A0
#define LED_PIN 13

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(CHANNEL_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(PHOTO_PIN, INPUT);
  digitalWrite(CHANNEL_PIN, HIGH);
  delay(1000);
}

int cutoffLevel = -1;

void loop() {
  // put your main code here, to run repeatedly:
  if (cutoffLevel < 0) {
    digitalWrite(RELAY_PIN, LOW);
    delay(100);
    int photoWhenLow = analogRead(PHOTO_PIN);
    Serial.print("Low: ");
    Serial.println(photoWhenLow);
    delay(2000);
    digitalWrite(RELAY_PIN, HIGH);
    delay(100);
    int photoWhenHigh = analogRead(PHOTO_PIN);
    Serial.print("High: ");
    Serial.println(photoWhenHigh);
    cutoffLevel = (photoWhenHigh - photoWhenLow) / 2 + photoWhenLow;
    
    char serialStr[128];
    sprintf(serialStr, "Low: %d, High: %d, Cutoff: %d",
            photoWhenLow, photoWhenHigh, cutoffLevel);
    Serial.println(serialStr);
  }
  
  int photoLevel = analogRead(PHOTO_PIN);
  bool isTripped = photoLevel < cutoffLevel;
  
  char serialStr[128];
  sprintf(serialStr, "=--%s--o [%d]", isTripped ? "X" : "-", photoLevel);
  Serial.println(serialStr);
  
  delay(10);
}
