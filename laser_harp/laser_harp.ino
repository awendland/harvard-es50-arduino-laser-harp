/*
 MIDI Laser Harp

 created 2015-11-20
 by Alex Wendland, Wes Fabyan, Edgar Thorton, Tod Jones
 
 */

#include <SoftwareSerial.h>

// Constants
#define NUM_CHANNELS 3
static const uint8_t CHANNEL_PINS[] = {8,9,10};
#define NUM_FEEDS_PER_CHANNEL 4
#define TOTAL_FEEDS NUM_CHANNELS * NUM_FEEDS_PER_CHANNEL
#define NUM_FEEDS_SIDE_A 6
#define NUM_FEEDS_SIDE_B TOTAL_FEEDS - NUM_FEEDS_SIDE_A
static const uint8_t FEED_PINS[] = {A0,A1,A2,A3};
#define LASER_CNTRL_PIN 11
#define MIDI_PIN 3

// Variables
SoftwareSerial midiSerial(2, MIDI_PIN);
uint16_t photoResCutoffs[TOTAL_FEEDS];

// Functions
void playNote(int cmd, int pitch, int velocity);
void readPhotoRes(uint16_t vals[]);

void setup() {
  Serial.begin(9600);
  // Initiate the digital pins
  for (int i = 0; i < NUM_CHANNELS; ++i) {
    pinMode(CHANNEL_PINS[i], OUTPUT);
  }
  // Initiate the analog pins
  for (int i = 0; i < NUM_FEEDS_PER_CHANNEL; ++i) {
    pinMode(FEED_PINS[i], INPUT);
  }
  //  Set MIDI baud rate:
  midiSerial.begin(31250);
  ////////////////////////////////////
  // Run calibration for lasers &   //
  // photoresisters                 //
  ////////////////////////////////////
  // Disable lasers and read these LOW photores values
  digitalWrite(LASER_CNTRL_PIN, LOW);
  uint16_t photoResLowVals[TOTAL_FEEDS];
  readPhotoRes(photoResLowVals);
  // Enable lasers and read these HIGH photores values
  digitalWrite(LASER_CNTRL_PIN, HIGH);
  uint16_t photoResHighVals[TOTAL_FEEDS];
  readPhotoRes(photoResHighVals);
  // Calculate cutoff values
  for (int i = 0; i < TOTAL_FEEDS; ++i) {
    const int offset = (photoResHighVals[i] - photoResLowVals[i]) / 2;
    photoResCutoffs[i] = offset + photoResLowVals[i];
  }
}

void loop() {
  uint16_t photoResVals[TOTAL_FEEDS];
  ////////////////////////////////////
  // Read in photo resister values  //
  ////////////////////////////////////
  readPhotoRes(photoResVals);
  ////////////////////////////////////
  // Process the photo resister     //
  // values                         //
  ////////////////////////////////////
  // Calculate which feeds are blocked
  bool blockedFeeds[TOTAL_FEEDS];
  for (int i = 0; i < TOTAL_FEEDS; ++i) {
    blockedFeeds[i] = photoResVals[i] > photoResCutoffs[i];
  }
  // Check if any cross sections have been triggered
  bool triggered[NUM_FEEDS_SIDE_A * NUM_FEEDS_SIDE_B];
  for (int a = 0; a < NUM_FEEDS_SIDE_A; ++a) {
    for (int b = 0; b < NUM_FEEDS_SIDE_B; ++b) {
      triggered[a * NUM_FEEDS_SIDE_A + b] =
          blockedFeeds[a] && blockedFeeds[NUM_FEEDS_SIDE_A + b];
    }
  }
  ////////////////////////////////////
  // Handle initiated triggered     //
  // cross sections                 //
  ////////////////////////////////////
  bool anyTriggered = false;
  for (int i = 0;
      !anyTriggered && i < NUM_FEEDS_SIDE_A * NUM_FEEDS_SIDE_B;
      anyTriggered = triggered[i], ++i);
  if (anyTriggered) {
    Serial.print("Triggered: ");
    for (int a = 0; a < NUM_FEEDS_SIDE_A; ++a) {
      for (int b = 0; b < NUM_FEEDS_SIDE_B; ++b) {
        if (triggered[a * NUM_FEEDS_SIDE_A + b]) {
          Serial.print(a);
          Serial.print("x");
          Serial.print(b);
          Serial.print(" ");
        }
      }
    }
    Serial.println();
  }
}

void readPhotoRes(uint16_t vals[]) {
  for (uint8_t curChannel = 0; curChannel < NUM_CHANNELS; ++curChannel) {
    ///////////////////////////////////
    // Read in photo resister values //
    // over NUM_CHANNELS iterations  //
    ///////////////////////////////////
    // Activate this channel
    digitalWrite(CHANNEL_PINS[curChannel], HIGH);
    // Read in the inputs for this channel
    for (int i = 0; i < NUM_FEEDS_PER_CHANNEL; ++i) {
      vals[curChannel * NUM_FEEDS_PER_CHANNEL + i] = analogRead(FEED_PINS[i]);
    }
    // Deactive this channel
    digitalWrite(CHANNEL_PINS[curChannel], LOW);
  }
}

//  plays a MIDI note.  Doesn't check to see that
//  cmd is greater than 127, or that data values are  less than 127:
void playNote(int cmd, int pitch, int velocity) {
  midiSerial.write(cmd);
  midiSerial.write(pitch);
  midiSerial.write(velocity);
  
  char serialStr[128];
  sprintf(serialStr, "cmd: %u, pitch: %u, velocity: %u", cmd, pitch, velocity);
  Serial.println(serialStr);
}
