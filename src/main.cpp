/**
 * VS1003/VS1053 MIDI Bell Test
 * 
 * This sketch demonstrates basic MIDI capabilities of the VS1003/VS1053 audio module.
 * It plays a bell "ding" sound on startup, pauses, and then repeats forever.
 * 
 * Hardware:
 * - Arduino Nano
 * - VS1003/VS1053 Module
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

// Pin definitions for VS1053
#define VS1053_CS     6   // XCS - Chip Select for command interface
#define VS1053_DCS    7   // XDCS - Chip Select for data interface  
#define VS1053_DREQ   2   // DREQ - Data Request pin
#define VS1053_RESET  8   // XRST - Reset pin

// MIDI commands
#define MIDI_NOTE_ON  0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_CHAN_MSG 0xB0
#define MIDI_CHAN_BANK 0x00
#define MIDI_CHAN_VOLUME 0x07
#define MIDI_CHAN_PROGRAM 0xC0

// Bell sound parameters
#define BELL_NOTE 60      // Middle C
#define BELL_VELOCITY 127 // Maximum velocity
#define BELL_CHANNEL 0    // MIDI channel 1 (0-indexed)
#define BELL_INSTRUMENT 14 // Tubular Bells (General MIDI instrument)

// Function declarations
void setupMIDI();
void playBellDing();
void noteOn(uint8_t channel, uint8_t note, uint8_t velocity);
void noteOff(uint8_t channel, uint8_t note, uint8_t velocity);
void sendMIDIDirectly(uint8_t cmd, uint8_t data1, uint8_t data2);

// Direct MIDI sending function (bypasses library)
// Parameters: cmd, note, velocity (corrected order)
void sendMIDIDirectly(uint8_t cmd, uint8_t note, uint8_t velocity) {
  // Wait for DREQ
  while(!digitalRead(VS1053_DREQ));
  
  // Send via data interface
  digitalWrite(VS1053_DCS, LOW);
  
  // Send command byte
  SPI.transfer(cmd);
  // Send note byte  
  SPI.transfer(note);
  // Send velocity byte if needed
  if((cmd & 0xF0) != 0xC0 && (cmd & 0xF0) != 0xD0) {
    SPI.transfer(velocity);
  }
  
  digitalWrite(VS1053_DCS, HIGH);
  
  // Small delay to ensure command is processed
  delay(1);
}

void setup() {
  Serial.begin(115200);
  Serial.println("VS1053 MIDI Starting...");
  
  // Pin setup
  pinMode(VS1053_CS, OUTPUT);
  pinMode(VS1053_DCS, OUTPUT);
  pinMode(VS1053_DREQ, INPUT);
  pinMode(VS1053_RESET, OUTPUT);
  
  digitalWrite(VS1053_CS, HIGH);
  digitalWrite(VS1053_DCS, HIGH);
  digitalWrite(VS1053_RESET, HIGH);
  
  // Initialize SPI
  SPI.begin();
  
  // Reset VS1053
  digitalWrite(VS1053_RESET, LOW);
  delay(10);
  digitalWrite(VS1053_RESET, HIGH);
  delay(500);
  
  // Wait for DREQ
  while(!digitalRead(VS1053_DREQ));
  
  // Set volume only
  digitalWrite(VS1053_CS, LOW);
  SPI.transfer(0x02);
  SPI.transfer(0x0B);
  SPI.transfer(0x20);
  SPI.transfer(0x20);
  digitalWrite(VS1053_CS, HIGH);
  
  delay(100);
  
  // Try original working MODE register setting
  while(!digitalRead(VS1053_DREQ));
  digitalWrite(VS1053_CS, LOW);
  SPI.transfer(0x02);
  SPI.transfer(0x00);
  SPI.transfer(0x48);
  SPI.transfer(0x04);
  digitalWrite(VS1053_CS, HIGH);
  
  delay(100);
  
  Serial.println("Back to original setup");
}

void loop() {
  // Wind chime sequence using the working tubular bell sound
  
  // Random-like wind chime notes (pentatonic scale for pleasant harmony)
  uint8_t chimeNotes[] = {60, 62, 65, 67, 69, 72, 74, 77};  // C, D, F, G, A, C, D, F
  uint8_t velocities[] = {60, 80, 100, 70, 90, 85, 75, 95}; // Varying volumes
  
  // Play a gentle wind chime sequence
  for(int i = 0; i < 5; i++) {
    int noteIndex = (millis() / 1000 + i * 3) % 8;  // Pseudo-random note selection
    uint8_t note = chimeNotes[noteIndex];
    uint8_t velocity = velocities[noteIndex];
    
    sendMIDIDirectly(0x90, note, velocity);  // Note on
    delay(400 + (noteIndex * 100));         // Variable note length
    sendMIDIDirectly(0x80, note, 0);        // Note off
    delay(200 + (noteIndex * 50));          // Variable pause
  }
  
  // Longer pause between wind chime sequences
  delay(3000);
}

void setupMIDI() {
  // This function is no longer used in manual mode
}

void playBellDing() {
  // This function is no longer used in manual mode
}

void noteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
  sendMIDIDirectly(0x90 | channel, note, velocity);
}

void noteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
  sendMIDIDirectly(0x80 | channel, note, velocity);
}
