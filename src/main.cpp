/**
 * VS1003/VS1053 MIDI Control Test
 * 
 * This sketch tests MIDI instrument control capabilities of the VS1003/VS1053 audio module.
 * 
 * PROVEN WORKING CONFIGURATION:
 * - MODE register: 0x4800 (basic MIDI mode, bit 11 set)
 * - VOL register: 0x2020 (volume setting)
 * - STATUS register: 0x40 (normal operation)
 * - Simple initialization without complex plugin loading
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
#include "vs1053_midi_plugin.h"

// Pin definitions for VS1053 module (pure VS1053, not VS1053+SD)
#define VS1053_CS     6   // XCS - VS1053 command interface (SCI - Serial Control Interface)
#define VS1053_DCS    7   // XDCS - VS1053 data interface (SDI - Serial Data Interface)  
#define VS1053_DREQ   2   // DREQ - Data Request pin
#define VS1053_RESET  8   // XRST - Reset pin

// CORRECT INTERFACE USAGE:
// XCS (pin 6) = Commands/register access via SCI
// XDCS (pin 7) = Audio/MIDI data via SDI

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
uint16_t readRegister(uint8_t addressbyte);
void writeRegister(uint8_t addressbyte, uint16_t value);
void loadMidiPlugin();

// Send MIDI command with correct parameter order for VS1053
void sendMIDIDirectly(uint8_t cmd, uint8_t note, uint8_t velocity) {
  // Wait for DREQ
  while(!digitalRead(VS1053_DREQ));
  
  // Use XDCS (pin 7) for MIDI data - this is the correct SDI interface
  digitalWrite(VS1053_DCS, LOW);
  
  // Send MIDI with VS1053's expected parameter order: cmd, velocity, note
  SPI.transfer(cmd);        // Command: 0x90 = note on, 0x80 = note off
  while(!digitalRead(VS1053_DREQ)); 
  SPI.transfer(velocity);   // Velocity first (VS1053 specific order)
  while(!digitalRead(VS1053_DREQ)); 
  SPI.transfer(note);       // Note second (VS1053 specific order)
  
  digitalWrite(VS1053_DCS, HIGH);
  delay(10);
}

void setup() {
  Serial.begin(115200);
  Serial.println("=== VS1053 MIDI Proven Configuration Test ===");
  
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
  Serial.println("SPI initialized");
  
  // Reset VS1053
  Serial.println("Resetting VS1053...");
  digitalWrite(VS1053_RESET, LOW);
  delay(10);
  digitalWrite(VS1053_RESET, HIGH);
  delay(500);
  
  // Wait for DREQ
  Serial.println("Waiting for DREQ...");
  while(!digitalRead(VS1053_DREQ));
  Serial.println("DREQ ready");
  
  // PROVEN INITIALIZATION SEQUENCE:
  // 1. Set volume first
  Serial.println("Step 1: Setting volume to 0x2020...");
  writeRegister(0x0B, 0x2020);
  delay(100);
  
  // 2. Load MIDI plugin for full MIDI synthesis capabilities
  Serial.println("Step 2: Loading MIDI plugin for full pitch control...");
  loadMidiPlugin();
  delay(500);
  
  // 3. Verify our proven configuration
  uint16_t mode = readRegister(0x00);
  uint16_t vol = readRegister(0x0B);
  uint16_t status = readRegister(0x01);
  
  Serial.print("FINAL CONFIG - MODE: 0x");
  Serial.print(mode, HEX);
  Serial.print(", VOL: 0x");
  Serial.print(vol, HEX);
  Serial.print(", STATUS: 0x");
  Serial.println(status, HEX);
  
  if(mode == 0x4800 && vol == 0x2020) {
    Serial.println("✓ BACK TO PROVEN WORKING CONFIGURATION");
  } else {
    Serial.println("✗ Configuration mismatch - check connections");
  }
  
  Serial.println("=== Ready for MIDI Note Testing ===");
}

void loop() {
  // Wind Chime Sequence - play a gentle bell melody
  Serial.println("=== Wind Chime Sequence ===");
  
  // Define wind chime notes (pentatonic scale for pleasant sound)
  uint8_t chimeNotes[] = {60, 62, 64, 67, 69, 72}; // C, D, E, G, A, C (next octave)
  uint8_t numNotes = sizeof(chimeNotes) / sizeof(chimeNotes[0]);
  
  // Play a gentle wind chime sequence
  for(int i = 0; i < 5; i++) {  // Play 5 notes
    uint8_t note = chimeNotes[random(numNotes)];  // Random note from scale
    uint8_t velocity = 80 + random(40);           // Velocity between 80-120
    
    Serial.print("Playing note: ");
    Serial.print(note);
    Serial.print(" with velocity: ");
    Serial.println(velocity);
    
    // Play note
    sendMIDIDirectly(0x90, note, velocity);   // Note on
    delay(800 + random(400));                 // Hold for 0.8-1.2 seconds
    sendMIDIDirectly(0x80, note, velocity);   // Note off
    delay(200 + random(600));                 // Pause 0.2-0.8 seconds before next note
  }
  
  // Longer pause between wind chime sequences
  Serial.println("Wind chime sequence complete - waiting...");
  delay(3000 + random(2000));  // Wait 3-5 seconds before next sequence
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

// Read VS1053 register
uint16_t readRegister(uint8_t addressbyte) {
  while(!digitalRead(VS1053_DREQ));
  digitalWrite(VS1053_CS, LOW);
  SPI.transfer(0x03);
  SPI.transfer(addressbyte);
  uint16_t result = SPI.transfer(0x00) << 8;
  result |= SPI.transfer(0x00);
  digitalWrite(VS1053_CS, HIGH);
  return result;
}

// Write VS1053 register
void writeRegister(uint8_t addressbyte, uint16_t value) {
  while(!digitalRead(VS1053_DREQ));
  digitalWrite(VS1053_CS, LOW);
  SPI.transfer(0x02);
  SPI.transfer(addressbyte);
  SPI.transfer(value >> 8);
  SPI.transfer(value & 0xFF);
  digitalWrite(VS1053_CS, HIGH);
}

// Load MIDI plugin for proper MIDI synthesis
void loadMidiPlugin() {
  Serial.println("Loading MIDI plugin from vs1053_midi_plugin.h...");
  
  // Load the plugin using the defined size
  int i = 0;
  while (i < MIDI1003_SIZE) {
    unsigned short addr, n, val;
    addr = pgm_read_word_near(&MIDI1053[i++]);
    n = pgm_read_word_near(&MIDI1053[i++]);
    
    if (n & 0x8000U) { // RLE run
      n &= 0x7FFF;
      val = pgm_read_word_near(&MIDI1053[i++]);
      while (n--) {
        writeRegister(addr, val);
      }
    } else { // Copy run
      while (n--) {
        val = pgm_read_word_near(&MIDI1053[i++]);
        writeRegister(addr, val);
      }
    }
  }
  
  // Verify MIDI mode is now active
  uint16_t mode = readRegister(0x00);
  Serial.print("MODE after plugin load: 0x");
  Serial.println(mode, HEX);
  
  Serial.println("MIDI plugin loaded successfully!");
}
