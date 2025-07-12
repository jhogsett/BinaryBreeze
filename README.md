# VS1053 MIDI Bell Test

This Arduino project demonstrates the MIDI capabilities of a VS1003/VS1053 audio module connected to an Arduino Nano. The project plays a bell "ding" sound on startup, pauses, and then repeats the sequence forever.

## Hardware Requirements

- Arduino Nano
- VS1003/VS1053 Audio Module (generic version with 10 pins)
- Jumper wires for connections
- Optional: Headphones or speakers connected to the audio module

## Pin Connections

Connect the VS1003/VS1053 module to the Arduino Nano as follows:

| VS1053 Pin | Arduino Nano Pin | Description |
|------------|------------------|-------------|
| 5V         | 5V               | Power supply |
| DGND       | GND              | Ground |
| MISO       | 12               | SPI Master In Slave Out |
| MOSI       | 11               | SPI Master Out Slave In |
| SCK        | 13               | SPI Clock |
| DREQ       | 2                | Data Request |
| XRST       | 8                | Reset |
| XCS        | 6                | Chip Select (Command) |
| XDCS       | 7                | Chip Select (Data) |

## Software Dependencies

This project uses PlatformIO and requires the following library:
- `pschatzmann/arduino-vs1053` (loaded directly from GitHub)

The library dependency is automatically managed by PlatformIO via the `platformio.ini` configuration file.

## Building and Uploading

1. Open this project in VS Code with the PlatformIO extension installed
2. Connect your Arduino Nano to your computer via USB
3. Build the project: `Ctrl+Alt+B` (or use PlatformIO: Build from the command palette)
4. Upload to the Arduino: `Ctrl+Alt+U` (or use PlatformIO: Upload from the command palette)

Alternatively, you can use the PlatformIO CLI:
```bash
# Build the project
pio run

# Upload to the Arduino
pio run --target upload

# Build and upload in one command
pio run --target upload
```

## Expected Behavior

1. On startup, the Arduino will initialize the VS1053 module
2. It will load the appropriate MIDI patches (VS1003 or VS1053 depending on detected chip)
3. The module will configure MIDI settings and select the Tubular Bells instrument
4. The module will play a bell "ding" sound
5. After a 2-second pause, the sequence repeats indefinitely
6. Serial output at 115200 baud provides status messages

## Troubleshooting

- **No sound output**: Check all pin connections and ensure the audio module has proper power
- **Initialization fails**: Verify the XRST and XCS pin connections
- **Upload fails**: Ensure the correct COM port is selected in PlatformIO
- **Compilation errors**: Make sure the arduino-vs1053 library is properly installed
- **MIDI not working**: The library automatically detects VS1003 vs VS1053 and loads appropriate patches

## Serial Monitor Output

Connect to the serial monitor at 115200 baud to see status messages:
```
VS1053 MIDI Bell Test Starting...
VS1053 MIDI initialized successfully!
Setup complete. Starting bell sequence...
Playing bell ding...
Pausing...
Repeating...
```

## Library Information

This project uses the `pschatzmann/arduino-vs1053` library which:
- Supports both VS1003 and VS1053 chips
- Automatically detects chip version and loads appropriate MIDI patches
- Works with generic VS1053 modules (not Adafruit-specific)
- Provides proper SPI-based MIDI communication
- Is designed for Arduino Uno/Nano compatibility

## Customization

You can modify the following parameters in `src/main.cpp`:
- `BELL_NOTE`: Change the MIDI note number (60 = Middle C)
- `BELL_VELOCITY`: Adjust note velocity (0-127)
- `BELL_INSTRUMENT`: Select different General MIDI instrument (0-127)
- `BELL_CHANNEL`: Change MIDI channel (0-15)
- Timing delays in the `loop()` function

## License

This project is provided as-is for educational and testing purposes.
