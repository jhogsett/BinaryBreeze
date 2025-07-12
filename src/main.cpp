/**
 * VS1053_MIDI Library Test - Wind Chimes
 * 
 * Testing the VS1053_MIDI library with the same wind chime scenarios
 * that were working in the manual implementation.
 * 
 * Hardware:
 * - Arduino Nano
 * - VS1003/VS1053 Module (generic version)
 * 
 * Pin Connections:
 * - MISO: Pin 12
 * - MOSI: Pin 11
 * - SCK: Pin 13
 * - DREQ: Pin 2
 * - XRST: Pin 8
 * - XCS: Pin 6
 * - XDCS: Pin 7
 */

#include <SPI.h>
#include <VS1053_MIDI.h>

// Pin definitions - same as working setup
#define VS1053_XCS   6
#define VS1053_XDCS  7
#define VS1053_DREQ  2
#define VS1053_RESET 8

// Create VS1053_MIDI instance
VS1053_MIDI midi(VS1053_XCS, VS1053_XDCS, VS1053_DREQ, VS1053_RESET);

// Function declaration
void playMelody(uint8_t* notes, uint8_t numNotes, uint8_t notesToPlay, int minDelay, int maxDelay);

void setup() {
  Serial.begin(115200);
  Serial.println("=== VS1053_MIDI Library Test - Wind Chimes ===");
  
  // Initialize SPI
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  Serial.println("SPI initialized");
  
  // Initialize VS1053 with MIDI plugin
  if (midi.begin(true)) {
    Serial.println("✓ VS1053_MIDI Library initialized successfully!");
    Serial.print("Plugin size loaded: ");
    Serial.print(midi.getPluginSize());
    Serial.println(" words");
  } else {
    Serial.println("✗ Library initialization failed!");
    while(1);
  }
  
  // Set volume (same as working setup)
  midi.setMasterVolume(0x20, 0x20);
  delay(100);
  
  Serial.println("=== Ready for Wind Chime Testing ===");
}

void loop() {
  // Electronic Wind Chime - Same scenarios as manual implementation
  Serial.println("=== Electronic Wind Chime - Multi-Instrument Test ===");
  
  // Scenario 1: Gentle breeze - Try different instruments
  Serial.println("Scenario 1: Gentle breeze - Glockenspiel");
  midi.setInstrument(0, GM_Instruments::GLOCKENSPIEL);   // Try Glockenspiel instead of Tubular Bells
  midi.setVolume(0, 90);
  uint8_t gentleNotes[] = {60, 62, 64, 67, 69, 72}; // C pentatonic
  playMelody(gentleNotes, 6, 3, 600, 800);
  
  delay(2000);
  
  // Scenario 2: Medium wind - Marimba
  Serial.println("Scenario 2: Medium wind - Marimba");
  midi.setInstrument(0, GM_Instruments::MARIMBA);  // Marimba
  midi.setVolume(0, 110);
  uint8_t mediumNotes[] = {55, 57, 60, 62, 64, 67}; // Lower, fuller scale
  playMelody(mediumNotes, 6, 4, 400, 600);
  
  delay(2000);
  
  // Scenario 3: Strong wind - Vibraphone
  Serial.println("Scenario 3: Strong wind - Vibraphone");
  midi.setInstrument(0, GM_Instruments::VIBRAPHONE);  // Vibraphone
  midi.setVolume(0, 127);
  uint8_t strongNotes[] = {64, 65, 67, 68, 70, 71, 73, 74}; // Chromatic sequence
  playMelody(strongNotes, 8, 6, 200, 300);
  
  delay(2000);
  
  // Scenario 4: Calm - Celesta
  Serial.println("Scenario 4: Calm - Celesta");
  midi.setInstrument(0, GM_Instruments::CELESTA);   // Celesta
  midi.setVolume(0, 70);
  uint8_t calmNotes[] = {72, 76, 79, 84}; // High, sparse notes
  playMelody(calmNotes, 4, 2, 1200, 1500);
  
  delay(3000);
  
  Serial.println("Wind chime cycle complete - repeating...");
}

// Play a melody with same characteristics as manual implementation
void playMelody(uint8_t* notes, uint8_t numNotes, uint8_t notesToPlay, int minDelay, int maxDelay) {
  for(int i = 0; i < notesToPlay; i++) {
    uint8_t note = notes[random(numNotes)];
    uint8_t velocity = 70 + random(50);  // Velocity 70-120
    
    Serial.print("Playing note: ");
    Serial.print(note);
    Serial.print(" with velocity: ");
    Serial.println(velocity);
    
    // Play note using library
    midi.noteOn(0, note, velocity);
    delay(minDelay + random(maxDelay - minDelay)); // Variable hold time
    midi.noteOff(0, note, velocity);
    delay(100 + random(400));                 // Short pause between notes
  }
}
