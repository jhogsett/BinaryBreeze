### More About This Project

#### Core Vision
- It would be nice to have a set of musical scales, ranging from "super duper cheery" to "awfully darn dreadful"
- The sensors will be used to collect environmental information (light, temperature, humidity, pressure, vibration)
- The environmental information will guide which musical scale is chosen, for instance beautiful calm sunny weather should be _cheery_ and cold nasty weather should be _dreadful_.
- The environmental information will also be used to choose which instrument is used, and other musical characteristics that are easily variable to "radiate information about the environment" to the listener through ambient listening.
- It would be nice to use some known algorithms for predicting coming bad weather, such as a sudden pressure drop, and signal the coming bad weather by starting to sound _gloomy_
- I have sensors for light, sound, temperature, humidity, pressure and vibration/acceleration 
- The ultimate device would be housed in a weather-safe enclosure and hung from the roof awning like other wind chimes, with 5V power delivered to it.

#### Open Questions & Design Considerations (TBD)

**Musical Scale Theory:**
- What specific scales should represent the "super duper cheery" to "awfully darn dreadful" spectrum?
  - Major scales (cheery) → Minor scales (dreadful)?
  - Pentatonic, blues, or exotic modal scales for variety?
  - Microtonal or non-Western scales for unique moods?
  - How many distinct mood categories should we support?

**Environmental → Musical Mapping:**
- What are the optimal sensor → musical parameter relationships?
  - Temperature → pitch range, scale selection, or instrument choice?
  - Humidity → reverb effects, note sustain, or harmonic content?
  - Pressure → tempo, note frequency, or overall activity level?
  - Light level → instrument timbre, brightness, or day/night modes?
  - Should mappings be linear, logarithmic, or use more complex curves?

**Weather Prediction Algorithms:**
- Which meteorological algorithms should we implement for weather forecasting?
  - Barometric pressure trend analysis (sudden drops = storms)?
  - Temperature/humidity/pressure combinations for different weather patterns?
  - How far in advance should the system predict weather changes?
  - What "gloomy" musical characteristics best represent approaching bad weather?

**Physical Installation & Hardware:**
- What type of weather-safe enclosure design would work best?
- How should 5V power be delivered safely to an outdoor installation?
- Should the speaker be internal or external to the enclosure?
- What mounting system would work with roof awning installation?
- How can we ensure good sensor exposure while protecting electronics?

**Advanced Features & Behaviors:**
- Should the system have seasonal behavior variations?
- How should time-of-day influence musical choices?
- Would remote monitoring/configuration be valuable?
- Should there be manual override or preset mood modes?
- How can we balance realistic wind chime physics with environmental responsiveness?

**User Experience:**
- How "ambient" should the listening experience be?
- Should there be visual indicators of current environmental conditions?
- How can we ensure the system doesn't become annoying with repetitive patterns?
- Should neighbors be able to hear and enjoy it, or keep it personal?


# VS1053 Electronic Wind Chime Project - Progress Summary

## Project Overview
**Goal**: Create an electronic wind chime that converts environmental data and tilt/vibration into beautiful digital chime sounds using a VS1053 audio module.

**Status**: ✅ **FOUNDATIONAL SUCCESS** - Basic MIDI communication working perfectly!

---

## Hardware Configuration (WORKING)

### Components
- **Arduino Nano** (nanoatmega328new)
- **VS1053 SPI Audio Module** (generic 10-pin module, not Adafruit version)
- **Audio Output**: Headphone jack → amplifier → speaker

### Pin Connections (VERIFIED WORKING)
```
VS1053 → Arduino Nano
MISO   → Pin 12
MOSI   → Pin 11
SCK    → Pin 13
DREQ   → Pin 2
XRST   → Pin 8  (Reset)
XCS    → Pin 6  (Command interface)
XDCS   → Pin 7  (Data interface)
VCC    → 5V
GND    → GND
```

---

## Critical Technical Breakthrough

### Library Issues Discovered
- **Adafruit VS1053 Library**: Incompatible with generic VS1053 modules
- **pschatzmann/arduino-vs1053 Library**: Hangs during `begin()` initialization despite successful SPI communication

### Solution: Manual SPI Implementation ✅
**Key Discovery**: Direct SPI communication works perfectly, bypassing all library issues.

#### Successful Manual Setup Sequence:
1. **Hardware Reset**: Pull XRST low for 10ms, then high
2. **SPI Initialization**: Standard Arduino SPI setup
3. **Communication Test**: Read MODE register (returns 0x4890 - confirms working SPI)
4. **Volume Control**: Write to SCI_VOL register (0x0B)
5. **MIDI Mode**: Enable real-time MIDI mode in MODE register
6. **Direct MIDI**: Send MIDI commands via data interface (XDCS)

#### Working MIDI Function:
```cpp
void sendMIDIDirectly(uint8_t cmd, uint8_t data1, uint8_t data2) {
  while(!digitalRead(VS1053_DREQ));  // Wait for ready
  digitalWrite(VS1053_DCS, LOW);     // Select data interface
  SPI.transfer(cmd);
  SPI.transfer(data1);
  if((cmd & 0xF0) != 0xC0 && (cmd & 0xF0) != 0xD0) {
    SPI.transfer(data2); // Third byte for most commands
  }
  digitalWrite(VS1053_DCS, HIGH);
}
```

---

## Current Status: MIDI Audio Working! 🎵

### Confirmed Working Features
- ✅ **SPI Communication**: MODE register reads correctly (0x4890)
- ✅ **Hardware Reset**: DREQ pin responds properly
- ✅ **MIDI Output**: Audible bell/chime sounds every 5 seconds
- ✅ **Volume Control**: Can adjust output levels
- ✅ **Note Control**: MIDI note on/off commands working

### Audio Characteristics
- **Current Sound**: "Strummed Sitar" - actually quite beautiful for wind chimes!
- **Volume**: Very low (needs external amplification)
- **Timing**: Consistent 5-second intervals
- **Quality**: Clean MIDI synthesis

---

## Next Phase: Electronic Wind Chime Development

### Required Hardware Additions
1. **Motion Sensing**:
   - MPU6050 (accelerometer/gyroscope) for tilt and vibration detection
   - I2C interface (pins A4/A5 on Nano)

2. **Environmental Sensors**:
   - BME280 or DHT22: Temperature, humidity, pressure
   - BH1750 or LDR: Light level sensing
   - Optional: Wind speed sensor for outdoor installation

3. **Audio Enhancement**:
   - External audio amplifier (current output very low)
   - Speaker or resonating chamber

### Software Architecture Plan
```
Environmental Data → Algorithm → MIDI Parameters → VS1053 → Audio
    ↓                    ↓            ↓              ↓        ↓
Temperature      → Note pitch    → MIDI notes   → Synthesis → Chimes
Humidity         → Reverb/delay  → CC messages  → Effects   → Ambience  
Pressure         → Timing        → Note timing  → Rhythm    → Natural feel
Light Level      → Instrument    → Program change → Timbre  → Day/night moods
Tilt/Vibration   → Trigger       → Note on/off  → Events   → Wind response
```

### MIDI Instruments for Wind Chimes
- **Instrument 14**: Tubular Bells (current default)
- **Instrument 9**: Glockenspiel
- **Instrument 10**: Music Box
- **Instrument 12**: Vibraphone  
- **Instrument 13**: Marimba
- **Instrument 112**: Tinkle Bell
- **Instrument 113**: Agogo Bells

### Wind Chime Physics Simulation
1. **Pendulum Motion**: Model chime movement based on tilt sensor
2. **Random Intervals**: Natural wind patterns (not mechanical timing)
3. **Harmonic Resonance**: Multiple chimes with related frequencies
4. **Environmental Response**: 
   - Light breezes → gentle single notes
   - Strong movement → cascading chime sequences
   - Temperature changes → instrument shifts
   - Barometric pressure → overall activity level

---

## Technical Configuration Files

### PlatformIO Configuration (Working)
```ini
[env:nanoatmega328]
platform = atmelavr
board = nanoatmega328new
framework = arduino
monitor_speed = 115200
; No external libraries needed - using manual SPI implementation
```

### Key Code Structure
- **Manual SPI Setup**: Direct register manipulation
- **MIDI Communication**: Real-time MIDI via data interface
- **Hardware Diagnostics**: Comprehensive connection testing
- **Volume Control**: Direct SCI register writes
- **Error Handling**: Timeout protection and diagnostic messages

---

## Development Roadmap

### Phase 1: Sensor Integration (Next Steps)
- [ ] Add MPU6050 accelerometer for tilt/vibration sensing
- [ ] Implement basic environmental sensors (BME280)
- [ ] Create sensor data reading and filtering algorithms

### Phase 2: Music Mapping
- [ ] Develop algorithms to convert sensor data to musical parameters
- [ ] Implement multiple MIDI instrument selections
- [ ] Create natural timing patterns (non-mechanical intervals)

### Phase 3: Wind Chime Physics
- [ ] Simulate realistic pendulum motion from accelerometer data
- [ ] Add harmonic relationships between multiple virtual chimes
- [ ] Implement environmental responsiveness (weather patterns)

### Phase 4: Advanced Features
- [ ] Day/night modes with different instrument palettes
- [ ] Seasonal adjustments to chime behaviors
- [ ] Remote monitoring/configuration via WiFi (ESP32 upgrade path)

---

## Critical Success Factors Learned

1. **Manual SPI is Superior**: Libraries cause more problems than they solve
2. **Hardware Verification First**: Always test basic connectivity before complex features
3. **Direct Register Access**: More reliable than abstraction layers
4. **Incremental Testing**: Build up complexity gradually
5. **Audio Amplification Essential**: VS1053 output level very low

---

## Files Structure
```
VS1053test/
├── platformio.ini          # PlatformIO configuration
├── src/main.cpp            # Working MIDI implementation
├── README.md               # Hardware setup and wiring guide
├── .vscode/tasks.json      # Build/upload/monitor tasks
└── PROJECT_SUMMARY.md      # This document
```

---

## Ready for Next Development Phase

The foundation is solid and proven. The manual SPI approach provides complete control over the VS1053, and the basic MIDI functionality demonstrates that the hardware setup is correct. 

**Next session goals**:
1. Add sensor libraries to PlatformIO
2. Implement basic accelerometer reading
3. Create first sensor → MIDI mapping
4. Test environmental responsiveness

**10 years of planning → Now ready for implementation! 🎐**
