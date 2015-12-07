/*
 MIDI Laser Harp

 Here is an ES6 snippet for easily generating a sequential
 mapping of notes for the NOTES constant:
 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
    const START = 0x5A;
    const SIDE_A = 6;

    let notes = [];

    for (let i = 0; i < 36; ++i) {
        if (i % SIDE_A === 0) notes.push([]);
        notes[notes.length - 1].push(
                "0x" + (START + i)
                    .toString(16)
                    .toUpperCase());
    }

    console.log(notes
        .map(l => l.join(", "))
        .join(",\n"));
 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

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
#define NUM_INTERSECTIONS NUM_FEEDS_SIDE_A * NUM_FEEDS_SIDE_B
static const uint8_t FEED_PINS[] = {A0,A1,A2,A3};
#define LASER_CNTRL_PIN 11
#define MIDI_PIN 3
static const uint8_t NOTES[] = {
  0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65,
  0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
  0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71,
  0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
  0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D
};

// Variables
SoftwareSerial midiSerial(2, MIDI_PIN);
uint16_t photoResCutoffs[TOTAL_FEEDS];

// Functions
void playNote(int cmd, int pitch, int velocity);
void readPhotoRes(uint16_t vals[]);
void printf(int sz, char* format, ...);

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
  // Handle triggered cross         //
  // sections                       //
  ////////////////////////////////////
  // Play the notes corresponding to the triggered intersections
  for (int i = 0; i < NUM_INTERSECTIONS; ++i) {
    if (triggered[i]) {
      playNote(0x90, NOTES[i], 0x45);
    }
  }
  // Output the triggered intersections to Serial
  bool anyTriggered = false;
  for (int i = 0;
      !anyTriggered && i < NUM_INTERSECTIONS;
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

//
// Reads in the current photoresister values
//   This will cycle through NUM_CHANNELS in order to
//   access all available feeds. Values will be read into
//   the uint16_t array, `vals`.
//
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

//
// Prints a formatted string to Serial
//
void printf(int sz, char* format, ...) {
  va_list args;
  va_start(args, format);
  char str[sz];
  vsnprintf(str, sz, format, args);
  Serial.println(str);
  va_end(args);
}

//
// Plays a MIDI note.
//
void playNote(int cmd, int pitch, int velocity) {
  midiSerial.write(cmd);
  midiSerial.write(pitch);
  midiSerial.write(velocity);
  
  printf(128, "cmd: %u, pitch: %u, velocity: %u", cmd, pitch, velocity);
}
