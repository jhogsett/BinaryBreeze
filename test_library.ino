/**
 * Test the VS1053_MIDI library with a simple wind chime sketch
 * This should sound identical to your current main.cpp wind chime
 */

#include <SPI.h>
#include <VS1053_MIDI.h>

// Pin definitions - same as your working setup
#define VS1053_XCS   6
#define VS1053_XDCS  7
#define VS1053_DREQ  2
#define VS1053_RESET 8

VS1053_MIDI midi(VS1053_XCS, VS1053_XDCS, VS1053_DREQ, VS1053_RESET);

void setup() {
  Serial.begin(115200);
  Serial.println("VS1053_MIDI Library Test - Wind Chimes");
  
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  
  if (midi.begin(true)) {
    Serial.println("Library initialized successfully!");
  } else {
    Serial.println("Initialization failed!");
    while(1);
  }
  
  midi.setMasterVolume(0x20, 0x20);
  Serial.println("Starting wind chime test...");
}

void loop() {
  // Test the same 4 scenarios as your main.cpp
  
  // Scenario 1: Tubular Bells
  Serial.println("Scenario 1: Gentle breeze - Tubular Bells");
  midi.setInstrument(0, GM_TUBULAR_BELLS);  // Same as instrument 14
  midi.setVolume(0, 90);
  playRandomNotes(3, 600, 800);
  delay(2000);
  
  // Scenario 2: Marimba  
  Serial.println("Scenario 2: Medium wind - Marimba");
  midi.setInstrument(0, GM_MARIMBA);  // Same as instrument 12
  midi.setVolume(0, 110);
  playRandomNotes(4, 400, 600);
  delay(2000);
  
  // Scenario 3: Vibraphone
  Serial.println("Scenario 3: Strong wind - Vibraphone");
  midi.setInstrument(0, GM_VIBRAPHONE);  // Same as instrument 11
  midi.setVolume(0, 127);
  playRandomNotes(6, 200, 300);
  delay(2000);
  
  // Scenario 4: Celesta
  Serial.println("Scenario 4: Calm - Celesta");
  midi.setInstrument(0, GM_CELESTA);  // Same as instrument 8
  midi.setVolume(0, 70);
  playRandomNotes(2, 1200, 1500);
  delay(3000);
}

void playRandomNotes(int noteCount, int minDelay, int maxDelay) {
  uint8_t notes[] = {60, 62, 64, 67, 69, 72, 76, 79, 84}; // Similar range to your code
  
  for(int i = 0; i < noteCount; i++) {
    uint8_t note = notes[random(sizeof(notes))];
    uint8_t velocity = 70 + random(50);
    
    midi.noteOn(0, note, velocity);
    delay(minDelay + random(maxDelay - minDelay));
    midi.noteOff(0, note, velocity);
    delay(100 + random(400));
  }
}
