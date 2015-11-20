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

SoftwareSerial midiSerial(2, 3);

void setup() {
  Serial.begin(9600);
  //  Set MIDI baud rate:
  midiSerial.begin(31250);
}

void loop() {
  // play notes from F#-0 (0x1E) to F#-5 (0x5A):
  for (int noteDelta = 0x0; true; noteDelta = (noteDelta + 1) % (0x5A - 0x1E)) {
    int note = 0x1E + noteDelta;
    //Note on channel 1 (0x90), some note value (note), middle velocity (0x45):
    noteOn(0x90, note, 0x45);
    delay(50);
    //Note on channel 1 (0x90), some note value (note), silent velocity (0x00):
    noteOn(0x90, note, 0x00);
    delay(50);
  }
}

//  plays a MIDI note.  Doesn't check to see that
//  cmd is greater than 127, or that data values are  less than 127:
void noteOn(int cmd, int pitch, int velocity) {
  midiSerial.write(cmd);
  midiSerial.write(pitch);
  midiSerial.write(velocity);
  
  char serialStr[128];
  sprintf(serialStr, "cmd: %u, pitch: %u, velocity: %u", cmd, pitch, velocity);
  Serial.println(serialStr);
}
