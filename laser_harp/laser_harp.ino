/*
 MIDI note player

 If this circuit is connected to a MIDI synth, it will play
 the notes F#-0 (0x1E) to F#-5 (0x5A) in sequence, on loop.


 The circuit:
 * digital in 3 connected to MIDI jack pin 5
 * MIDI jack pin 2 connected to ground
 * MIDI jack pin 4 connected to +5V through 220-ohm resistor
 Attach a MIDI cable to the jack, then to a MIDI synth, and play music.

 created 2015-11-20
 by Alex Wendland
 
 */

#include <SoftwareSerial.h>

// Constants
#define NUM_CHANNELS 3
#define NUM_FEEDS_PER_CHANNEL 4
#define CHANNEL_START_PIN 2
#define TOTAL_FEEDS NUM_CHANNELS * NUM_FEEDS_PER_CHANNEL
#define NUM_FEEDS_SIDE_A 6
#define NUM_FEEDS_SIDE_B 6
static const uint8_t FEED_PINS[] = {A1,A2,A3,A4};

// Variables
uint8_t curChannel = 0;
uint16_t photoResVals[TOTAL_FEEDS];
SoftwareSerial midiSerial(2, 3);

void setup() {
  Serial.begin(9600);
  // Initiate the digital pins
  for (int i = 0; i < NUM_CHANNELS; ++i) {
    pinMode(CHANNEL_START_PIN + i, OUTPUT);
  }
  // Initiate the analog pins
  for (int i = 0; i < NUM_FEEDS_PER_CHANNEL; ++i) {
    pinMode(FEED_PINS[i], INPUT);
  }
  //  Set MIDI baud rate:
  midiSerial.begin(31250);
}

void loop() {
  ///////////////////////////////////
  // Read in photo resister values //
  // over NUM_CHANNELS cycles      //
  ///////////////////////////////////
  // Read in the current channel
  curChannel = (curChannel + 1) % NUM_CHANNELS;
  // Activate this channel
  digitalWrite(CHANNEL_START_PIN + curChannel, HIGH);
  // Read in the inputs for this channel
  for (int i = 0; i < NUM_FEED_PER_CHANNEL; ++i) {
    photoResVals[curChannel * NUM_FEED_PER_CHANNEL + i] = analogRead(FEED_PINS[i]);
  }
  // Deactive this channel
  digitalWrite(CHANNEL_START_PIN + curChannel, LOW);
  ////////////////////////////////////
  // Process the photo resister     //
  // values after each full read    //
  ////////////////////////////////////
  if (curChannel == 2) {
    // Check if any cross sections have been triggered
    for (int a = 0; a < NUM_FEEDS_SIDE_A; ++a) {
      for (int b = 0; b < NUM_FEEDS_SIDE_B; ++b) {
        photoResVals[i]
      }
    }
  }
  ////////////////////////////////////
  // Handle initiated notes         //
  ////////////////////////////////////
  // TODO
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
