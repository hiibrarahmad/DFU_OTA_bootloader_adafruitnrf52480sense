# Adafruit nRF52840 Bootloader

A UF2 bootloader for the Adafruit Feather nRF52840 Sense development board, featuring USB Mass Storage Device (MSD) support and Bluetooth Low Energy (BLE) Device Firmware Update (DFU) capabilities.

## Overview

This bootloader enables easy firmware updates through multiple methods:
- **UF2 (USB Flashing Format)** - Drag and drop firmware files
- **BLE DFU** - Over-the-air updates via Bluetooth
- **Serial DFU** - Updates via UART interface

## Hardware Support

**Primary Target:** Adafruit Feather nRF52840 Sense
- **MCU:** Nordic nRF52840 (ARM Cortex-M4F)
- **Flash:** 1MB internal flash
- **RAM:** 256KB
- **Connectivity:** USB, BLE 5.0
- **Sensors:** Various onboard sensors (accelerometer, gyroscope, magnetometer, etc.)

### Board Features
- 2x Status LEDs (pins P1.09, P1.10)
- 1x NeoPixel RGB LED (pin P0.16)
- 2x User buttons (pins P1.02, P0.10)
- USB connector for power and programming

## Prerequisites

### Development Tools
- **GCC ARM Embedded Toolchain** (`arm-none-eabi-gcc`)
- **Nordic nRF5 SDK** (included as submodule)
- **Python 3.x** with required packages
- **Git** with submodules support

### Flashing Tools
Choose one of the following:
- **nrfjprog** (Nordic's official tool) - Default
- **pyocd** - Alternative open-source tool

### Optional Tools
- **adafruit-nrfutil** - For creating DFU packages
- **nRF Connect** mobile app - For BLE DFU updates

## Getting Started

### 1. Clone Repository
```bash
git clone --recursive https://github.com/adafruit/Adafruit_nRF52_Bootloader.git
cd Adafruit_nRF52_Bootloader
```

### 2. Install Dependencies
```bash
# Install ARM GCC toolchain (example for Ubuntu/Debian)
sudo apt-get install gcc-arm-none-eabi

# Install Python packages
pip install adafruit-nrfutil
```

### 3. Build Bootloader
```bash
make BOARD=feather_nrf52840_sense
```

### 4. Flash Bootloader
```bash
# Erase chip first
make BOARD=feather_nrf52840_sense erase

# Flash SoftDevice
make BOARD=feather_nrf52840_sense flash-sd

# Flash bootloader
make BOARD=feather_nrf52840_sense flash
```

## Build System

### Make Targets
| Target | Description |
|--------|-------------|
| `all` | Build all bootloader variants |
| `flash` | Flash bootloader to device |
| `flash-sd` | Flash SoftDevice only |
| `erase` | Erase entire chip |
| `clean` | Clean build files |

### Build Configuration
```bash
# Debug build with RTT logging
make BOARD=feather_nrf52840_sense DEBUG=1

# Use pyocd instead of nrfjprog
make BOARD=feather_nrf52840_sense FLASHER=pyocd

# Build with specific SoftDevice version
make BOARD=feather_nrf52840_sense SD_VERSION=7.3.0
```

## Usage

### UF2 Bootloader Mode
1. **Double-click** the reset button on your board
2. Board appears as **FTHRSNSBOOT** USB drive
3. **Drag and drop** your `.uf2` firmware file
4. Board automatically reboots with new firmware

### BLE DFU Updates
1. Use **nRF Connect** mobile app
2. Connect to device advertising as **DfuTarg**
3. Select DFU service and upload `.zip` firmware package
4. Follow app instructions to complete update

### Creating DFU Packages
```bash
# Create DFU package from HEX file
adafruit-nrfutil dfu genpkg --dev-type 0x0052 --dev-revision 52840 --application your_app.hex your_app_dfu.zip
```

## Examples

### Blink Example
The included Blink example demonstrates:
- BLE connectivity and services
- LED control via BLE commands
- Button input reading
- Basic nRF52840 peripheral usage

```bash
# Build and flash the example
cd Blink/
# Follow Arduino IDE or platformio build instructions
```

## Project Structure

```
├── src/                    # Bootloader source code
│   ├── boards/            # Board-specific configurations
│   ├── usb/               # USB MSD and UF2 implementation
│   └── dfu_*.c           # DFU services
├── lib/                   # External libraries
│   ├── sdk/              # Nordic nRF5 SDK
│   ├── tinyusb/          # TinyUSB stack
│   └── softdevice/       # Nordic SoftDevice binaries
├── linker/               # Linker scripts
├── tools/                # Build and utility scripts
├── Blink/                # Example Arduino project
└── board.h              # Board configuration header
```

## Configuration

### Board Configuration (`board.h`)
- LED pin assignments
- Button configurations
- USB descriptors
- BLE device information
- UF2 bootloader settings

### Bootloader Features
- **UF2 Family ID:** `0xADA52840`
- **Volume Label:** `FTHRSNSBOOT`
- **Bootloader Size:** 28KB (debug) / 40KB (release)
- **Application Start:** Configurable based on SoftDevice

## Troubleshooting

### Common Issues

**Build fails with "arm-none-eabi-gcc not found"**
```bash
# Install ARM GCC toolchain
sudo apt-get install gcc-arm-none-eabi
# Or download from ARM developer website
```

**Flash fails with "No J-Link device found"**
```bash
# Use pyocd as alternative
make BOARD=feather_nrf52840_sense FLASHER=pyocd flash
```

**Device not entering bootloader mode**
- Check reset button double-click timing
- Ensure bootloader was flashed correctly
- Try erasing and reflashing completely

### Debug Mode
```bash
# Build with debug symbols and RTT logging
make BOARD=feather_nrf52840_sense DEBUG=1
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test on hardware
5. Submit a pull request

## License

This project inherits licenses from multiple components:
- **Bootloader code:** Various (see individual files)
- **Nordic SDK:** Nordic Semiconductor License
- **TinyUSB:** MIT License
- **Adafruit Libraries:** MIT/BSD License

## Resources

- [Adafruit Feather nRF52840 Sense Guide](https://www.adafruit.com/product/4516)
- [UF2 Bootloader Documentation](https://github.com/Microsoft/uf2)
- [Nordic nRF52840 Documentation](https://infocenter.nordicsemi.com/topic/struct_nrf52/struct/nrf52840.html)
- [TinyUSB Documentation](https://docs.tinyusb.org/)

## Support

- **Adafruit Forums:** [forums.adafruit.com](https://forums.adafruit.com/)
- **GitHub Issues:** For bug reports and feature requests
- **Discord:** Adafruit Discord server for community support