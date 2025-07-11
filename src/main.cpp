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
  // Debug output
  Serial.print("MIDI: 0x");
  Serial.print(cmd, HEX);
  Serial.print(" Note:");
  Serial.print(note);
  if((cmd & 0xF0) != 0xC0 && (cmd & 0xF0) != 0xD0) {
    Serial.print(" Vel:");
    Serial.print(velocity);
  }
  Serial.println();
  
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
  Serial.println("VS1053 MIDI Bell Test Starting...");
  
  // Test pin setup first
  Serial.println("Setting up pins...");
  pinMode(VS1053_CS, OUTPUT);
  pinMode(VS1053_DCS, OUTPUT);
  pinMode(VS1053_DREQ, INPUT);
  pinMode(VS1053_RESET, OUTPUT);
  
  // Set initial pin states
  digitalWrite(VS1053_CS, HIGH);
  digitalWrite(VS1053_DCS, HIGH);
  digitalWrite(VS1053_RESET, HIGH);
  
  Serial.println("Pin setup complete");
  Serial.print("DREQ pin state: ");
  Serial.println(digitalRead(VS1053_DREQ) ? "HIGH" : "LOW");
  
  // Initialize SPI
  SPI.begin();
  Serial.println("SPI initialized");
  
  // Test reset sequence
  Serial.println("Testing reset sequence...");
  digitalWrite(VS1053_RESET, LOW);
  delay(10);
  digitalWrite(VS1053_RESET, HIGH);
  delay(500);
  
  Serial.print("DREQ after reset: ");
  Serial.println(digitalRead(VS1053_DREQ) ? "HIGH" : "LOW");
  
  // Wait for DREQ to go high (indicates VS1053 is ready)
  Serial.println("Waiting for DREQ to go HIGH...");
  int timeout = 0;
  while(!digitalRead(VS1053_DREQ) && timeout < 1000) {
    delay(10);
    timeout++;
  }
  
  if(digitalRead(VS1053_DREQ)) {
    Serial.println("DREQ is HIGH - VS1053 should be ready");
  } else {
    Serial.println("DREQ never went HIGH - possible power or connection issue");
    Serial.println("Check:");
    Serial.println("1. 5V power connection to VS1053");
    Serial.println("2. Ground connections");
    Serial.println("3. DREQ wire to pin 2");
    while(1) {
      delay(1000);
      Serial.println("Halted - fix hardware issues");
    }
  }
  
  // Manual SPI test - try to read MODE register
  Serial.println("Testing manual SPI communication...");
  
  // Select command interface
  digitalWrite(VS1053_CS, LOW);
  
  // Send read command for MODE register (address 0x00)
  SPI.transfer(0x03); // Read command
  SPI.transfer(0x00); // MODE register address
  uint16_t mode = SPI.transfer(0x00) << 8; // High byte
  mode |= SPI.transfer(0x00); // Low byte
  
  // Deselect
  digitalWrite(VS1053_CS, HIGH);
  
  Serial.print("MODE register value: 0x");
  Serial.println(mode, HEX);
  
  if(mode == 0x0000 || mode == 0xFFFF) {
    Serial.println("Invalid MODE register value - SPI communication failed");
    Serial.println("Check SPI wiring:");
    Serial.println("  MISO -> Pin 12");
    Serial.println("  MOSI -> Pin 11"); 
    Serial.println("  SCK -> Pin 13");
    Serial.println("  XCS -> Pin 6");
    while(1) {
      delay(1000);
      Serial.println("Halted - fix SPI wiring");
    }
  } else {
    Serial.println("SPI communication successful!");
    Serial.println("Bypassing library initialization and doing manual setup...");
    
    // Manual VS1053 setup since the library hangs
    Serial.println("Setting up VS1053 manually...");
    
    // Wait for DREQ
    while(!digitalRead(VS1053_DREQ));
    
    // Set volume manually by writing to SCI_VOL register (0x0B)
    digitalWrite(VS1053_CS, LOW);
    SPI.transfer(0x02); // Write command
    SPI.transfer(0x0B); // VOL register
    SPI.transfer(0x20); // Left volume (0x20 = moderate volume - your original working setting)
    SPI.transfer(0x20); // Right volume (0x20 = moderate volume - your original working setting)
    digitalWrite(VS1053_CS, HIGH);
    
    Serial.println("Volume set");
    
    // Try to load MIDI patches manually
    Serial.println("Loading MIDI patches...");
    
    // Simple approach - just try to send MIDI commands directly
    // Set up for real-time MIDI mode
    while(!digitalRead(VS1053_DREQ));
    
    Serial.println("Setting MODE register for MIDI...");
    digitalWrite(VS1053_CS, LOW);
    SPI.transfer(0x02); // Write command
    SPI.transfer(0x00); // MODE register
    SPI.transfer(0x48); // SM_SDINEW (bit 11) + SM_RESET (bit 2) - enable real-time MIDI mode
    SPI.transfer(0x04); // Lower byte - additional MIDI flags
    digitalWrite(VS1053_CS, HIGH);
    
    delay(100);
    
    // Read back MODE register to verify
    Serial.println("Reading back MODE register...");
    digitalWrite(VS1053_CS, LOW);
    SPI.transfer(0x03); // Read command
    SPI.transfer(0x00); // MODE register address
    uint16_t newMode = SPI.transfer(0x00) << 8; // High byte
    newMode |= SPI.transfer(0x00); // Low byte
    digitalWrite(VS1053_CS, HIGH);
    
    Serial.print("New MODE register value: 0x");
    Serial.println(newMode, HEX);
    
    delay(100);
    
    Serial.println("Manual setup complete! Trying MIDI...");
    
    // Simple MIDI test - start with basic commands
    delay(100);
    
    // Set volume first
    sendMIDIDirectly(0xB0, 0x07, 127); // Channel 1 volume to maximum
    delay(50);
    
    // Try different MIDI channels - some might have different default instruments
    Serial.println("Testing different MIDI channels...");
    
    // Try Channel 2 - higher pitched notes
    Serial.println("Testing Channel 2...");
    sendMIDIDirectly(0xB1, 0x07, 127); // Channel 2 volume
    sendMIDIDirectly(0x91, 84, 127); // High C (note 84) - much higher pitch
    delay(300);
    sendMIDIDirectly(0x81, 84, 0);   // Note off
    delay(200);
    
    // Try Channel 3 - even higher
    Serial.println("Testing Channel 3...");
    sendMIDIDirectly(0xB2, 0x07, 127); // Channel 3 volume
    sendMIDIDirectly(0x92, 96, 127); // Very high C (note 96)
    delay(300);
    sendMIDIDirectly(0x82, 96, 0);   // Note off
    delay(200);
    
    // Try Channel 4
    Serial.println("Testing Channel 4...");
    sendMIDIDirectly(0xB3, 0x07, 127); // Channel 4 volume
    sendMIDIDirectly(0x93, 72, 127); // High note 72
    delay(300);
    sendMIDIDirectly(0x83, 72, 0);   // Note off
    delay(500);
    
    // Try a simple note without program change first
    Serial.println("Testing basic note...");
    sendMIDIDirectly(0x90, 60, 127); // Note on
    delay(500);
    sendMIDIDirectly(0x80, 60, 0); // Note off
    delay(200);
    
    Serial.println("MIDI test sent!");
  }
  
  Serial.println("Starting MIDI bell test loop...");
  delay(1000);
}

void loop() {
  Serial.println("=== CORRECTED Wind Chime Test ===");
  
  // Now with proper note/velocity order, create a beautiful wind chime sequence
  
  // First chime - Middle C, moderate volume
  Serial.println("Playing Middle C (60)");
  sendMIDIDirectly(0x90, 60, 80);   // Note 60, Velocity 80
  delay(400);
  sendMIDIDirectly(0x80, 60, 0);    // Note off
  delay(200);
  
  // Second chime - Perfect Fifth (G), softer
  Serial.println("Playing G (67)");
  sendMIDIDirectly(0x90, 67, 60);   // Note 67, Velocity 60
  delay(500);
  sendMIDIDirectly(0x80, 67, 0);    // Note off
  delay(300);
  
  // Third chime - High C, very soft
  Serial.println("Playing High C (72)");
  sendMIDIDirectly(0x90, 72, 40);   // Note 72, Velocity 40
  delay(600);
  sendMIDIDirectly(0x80, 72, 0);    // Note off
  delay(400);
  
  // Fourth chime - Very high E, whisper soft
  Serial.println("Playing High E (76)");
  sendMIDIDirectly(0x90, 76, 30);   // Note 76, Velocity 30
  delay(800);
  sendMIDIDirectly(0x80, 76, 0);    // Note off
  delay(500);
  
  Serial.println("=== Beautiful wind chime sequence complete! ===");
  Serial.println("You should now hear different pitches AND volumes!");
  delay(4000);
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
