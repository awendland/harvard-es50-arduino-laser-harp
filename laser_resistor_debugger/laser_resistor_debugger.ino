/*
 MIDI Laser Harp

 Here is an ES6 snippet for easily generating a sequential
 mapping of notes for the NOTES constant:
 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
    const START = 0x5A;
    const SIDE_A = 8;
    const SIDE_B = 8;

    let notes = [];

    for (let i = 0; i < SIDE_A * SIDE_B; ++i) {
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
#define NUM_CHANNELS 4
static const uint8_t CHANNEL_PINS[] = {10,9,8,7};
#define NUM_FEEDS_PER_CHANNEL 4
#define TOTAL_FEEDS (NUM_CHANNELS * NUM_FEEDS_PER_CHANNEL)
#define NUM_FEEDS_SIDE_A 8
#define NUM_FEEDS_SIDE_B (TOTAL_FEEDS - NUM_FEEDS_SIDE_A)
#define NUM_INTERSECTIONS (NUM_FEEDS_SIDE_A * NUM_FEEDS_SIDE_B)
static const uint8_t FEED_PINS[] = {A0,A1,A2,A3};
#define CUTOFF_THRESHOLD 50
#define LASER_CNTRL_PIN 11
#define MIDI_PIN 3
static const uint8_t NOTE_PITCHES[] = {
  0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
  0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,
  0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
  0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,
  0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45,
  0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
  0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55,
  0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D
};
static const uint8_t NOTE_CHANNELS[] = {
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
};

// Variables
SoftwareSerial midiSerial(2, MIDI_PIN);
uint16_t photoResCutoffs[TOTAL_FEEDS];
bool error = false;
int errorCounter = 0;

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
  // Initiate laser control pin
  pinMode(LASER_CNTRL_PIN, OUTPUT);
  digitalWrite(LASER_CNTRL_PIN, HIGH);
}

void loop() {
  uint16_t photoResVals[TOTAL_FEEDS];
  readPhotoRes(photoResVals);
  printIntArr(photoResVals, TOTAL_FEEDS);
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
  Serial.print(str);
  va_end(args);
}


//
// Output the values of an int array to Serial
//
void printIntArr(uint16_t arr[], int sz) {
  for (int i = 0; i < sz; ++i) {
    Serial.print(i);
    Serial.print("=");
    Serial.print(arr[i]);
    Serial.print(" ");
  }
  Serial.println();
}
