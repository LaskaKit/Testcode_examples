# GDEH037E01 EPD Display IDF Project

This is an ESP-IDF project for controlling the GDEH037E01 E-ink display (4.37" 480x720) with both ESP32 and ESP32-S3 microcontrollers.

## Project Structure

```
GDEH037E01/
├── CMakeLists.txt           # Root project CMake config
├── main/
│   ├── CMakeLists.txt       # Main component CMake config
│   ├── GDEH037E01_idf.c     # Main application source code
│   └── image.h              # Image data header
├── .gitignore               # Git ignore file
└── README.md                # This file
```

## Prerequisites

- ESP-IDF v4.4 or later installed
- ESP32 or ESP32-S3 development board
- GDEH037E01 E-ink display module

## Board Selection

Edit `main/GDEH037E01_idf.c` line 13:

```c
// For ESP32-S3
#define BOARD_ESP32S3 1

// For ESP32
#define BOARD_ESP32S3 0
```

### ESP32-S3 Pin Configuration
- MOSI: GPIO 11
- CLK:  GPIO 12
- BUSY: GPIO 5
- DC:   GPIO 3
- CS:   GPIO 2
- RST:  GPIO 4

### ESP32 Pin Configuration
- MOSI: GPIO 23
- CLK:  GPIO 18
- BUSY: GPIO 14 (A14)
- DC:   GPIO 16 (A16)
- CS:   GPIO 17 (A17)
- RST:  GPIO 15 (A15)

## Building and Flashing

### 1. Set ESP-IDF environment
```bash
. $IDF_PATH/export.sh
```

### 2. Configure the project
```bash
idf.py menuconfig
```

### 3. Build the project
```bash
idf.py build
```

### 4. Flash to device
```bash
idf.py -p COM3 flash monitor
```

Replace `COM3` with your actual serial port.

## Features

- Support for both ESP32 and ESP32-S3
- Hardware SPI communication (2MHz)
- Automatic BUSY pin polarity detection
- Full-screen image display
- Full-screen white fill
- Deep sleep mode
- FreeRTOS task-based architecture

## Application Flow

1. **Initialize**: Set up GPIO, SPI, and EPD display
2. **Detect BUSY polarity**: Automatically detect if BUSY is active-high or low
3. **Initialize EPD**: Configure display mode (normal or water-ripple)
4. **Display Image**: Show image from `image_data_sixcolor`
5. **Clear Screen**: Fill entire display with white
6. **Deep Sleep**: Enter low-power mode

## Customization

### Change Image
Replace `image_data_sixcolor` in the display function with your own image data.

### Change Colors
Modify the `EpdColorByte` enum values (lines 44-51).

### SPI Speed
Change `clock_speed_hz` in `epd_hardware_init()` function (line 328).

## Troubleshooting

### Display not responding
1. Check physical connections
2. Verify pin assignments match your board
3. Check BUSY pin polarity detection messages in serial monitor

### Compilation errors
1. Ensure ESP-IDF is properly installed
2. Check `IDF_PATH` environment variable
3. Verify all required components are installed

## License

This project is provided as-is for educational and development purposes.
