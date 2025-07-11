# nRF52840 Custom Bootloader and Firmware Update Guide

A comprehensive guide for customizing the Adafruit nRF52840 bootloader to prevent DFU rollback issues and enable reliable OTA firmware updates.

## 📋 Table of Contents

- [🔍 Overview](#-overview)
- [⚠️ Problem Statement](#️-problem-statement)
- [🛠️ Prerequisites](#️-prerequisites)
- [📁 Project Structure](#-project-structure)
- [🔧 Bootloader Changes](#-bootloader-changes)
- [🏗️ Building the Custom Bootloader](#️-building-the-custom-bootloader)
- [💾 Flashing the Bootloader](#-flashing-the-bootloader)
- [📱 Sample Firmware](#-sample-firmware)
- [🔄 Firmware Update Process](#-firmware-update-process)
- [🎮 Usage Instructions](#-usage-instructions)
- [🐛 Troubleshooting](#-troubleshooting)
- [📚 Additional Resources](#-additional-resources)
- [📄 License](#-license)
- [🙏 Acknowledgments](#-acknowledgments)

## 🔍 Overview

This project addresses a critical issue with the Adafruit nRF52840 bootloader where DFU (Device Firmware Update) operations would fail due to automatic rollback mechanisms. The custom bootloader modification disables validation checks that cause rollbacks, ensuring successful OTA updates.

**Key Features:**
- ✅ Prevents firmware rollback during DFU operations
- ✅ Maintains UF2 bootloader compatibility
- ✅ Supports BLE OTA updates
- ✅ Includes sample firmware with LED control and button reading
- ✅ Compatible with nRF Connect and other DFU tools

## ⚠️ Problem Statement

### Original Issue with Adafruit Bootloader

The Adafruit prebuilt bootloader had a critical flaw in its DFU implementation:

1. **Rollback Mechanism**: When performing DFU OTA updates, the bootloader would validate the new firmware
2. **Validation Failure**: If validation failed (even for valid firmware), it would roll back to the previous version
3. **Update Loop**: This created a cycle where the device would:
   - Accept the DFU package
   - Attempt to install it
   - Fail validation
   - Roll back to AdaFruit DFU mode
   - Switch to UF2 mode
   - Repeat the cycle

### Solution Implemented

The custom bootloader modification addresses this by:
- **Disabling App Validation**: Setting `BOOT_VALIDATE_APP = 0` skips the problematic validation step
- **Development Mode**: Enables a development mode that prioritizes successful updates over strict validation
- **Maintained Compatibility**: Keeps all other bootloader features intact

## 🛠️ Prerequisites

### Hardware Requirements
- **nRF52840 Development Board** (Adafruit Feather nRF52840 Sense or compatible)
- **J-Link Debugger** (for bootloader flashing)
- **USB Cable** (for serial communication and UF2 updates)

### Software Requirements

#### Essential Tools
```bash
# ARM GCC Toolchain
sudo apt-get install gcc-arm-none-eabi

# Python 3 and pip
sudo apt-get install python3 python3-pip

# Git
sudo apt-get install git

# Make
sudo apt-get install build-essential
```

#### nRF Tools
```bash
# Install adafruit-nrfutil for DFU operations
pip3 install adafruit-nrfutil

# Install nRF Command Line Tools (includes nrfjprog)
# Download from: https://www.nordicsemi.com/Products/Development-tools/nrf-command-line-tools
```

#### Development Environment
- **Arduino IDE** (with Adafruit nRF52 board package)
- **J-Flash** (part of SEGGER J-Link software package)
- **nRF Connect** mobile app (for BLE testing)

### Board Package Installation
1. Open Arduino IDE
2. Go to File → Preferences
3. Add to Additional Board Manager URLs:
   ```
   https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
   ```
4. Go to Tools → Board → Boards Manager
5. Search for "Adafruit nRF52" and install

## 📁 Project Structure

```
nRF52840-Custom-Bootloader/
├── README.md
├── bootloader/
│   ├── src/
│   │   └── boards/
│   │       └── feather_nrf52840_sense/
│   │           └── board.h
│   ├── Makefile
│   └── build/
├── firmware/
│   ├── BLE_LED_Control.ino
│   └── build/
├── tools/
│   └── scripts/
└── docs/
    └── images/
```

## 🔧 Bootloader Changes

### Header File Modifications

The main changes are in the board configuration header file:

**File**: `src/boards/feather_nrf52840_sense/board.h`

#### Before (Original Adafruit Configuration)
```c
/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ha Thach for Adafruit Industries
 */

#ifndef _FEATHER_NRF52840_SENSE_H
#define _FEATHER_NRF52840_SENSE_H

#define _PINNUM(port, pin)    ((port)*32 + (pin))

/*------------------------------------------------------------------*/
/* LED */
#define LEDS_NUMBER           2
#define LED_PRIMARY_PIN       _PINNUM(1, 9)
#define LED_SECONDARY_PIN     _PINNUM(1, 10)
#define LED_STATE_ON          1

#define LED_NEOPIXEL           _PINNUM(0, 16)
#define NEOPIXELS_NUMBER      1
#define BOARD_RGB_BRIGHTNESS  0x040404

/*------------------------------------------------------------------*/
/* BUTTON */
#define BUTTONS_NUMBER        2
#define BUTTON_1              _PINNUM(1, 02)
#define BUTTON_2              _PINNUM(0, 10)
#define BUTTON_PULL           NRF_GPIO_PIN_PULLUP

//--------------------------------------------------------------------+
// BLE OTA
//--------------------------------------------------------------------+
#define BLEDIS_MANUFACTURER   "Adafruit Industries"
#define BLEDIS_MODEL          "Feather nRF52840 Sense"

//--------------------------------------------------------------------+
// USB
//--------------------------------------------------------------------+
#define USB_DESC_VID           0x239A
#define USB_DESC_UF2_PID       0x0087
#define USB_DESC_CDC_ONLY_PID  0x0088

//------------- UF2 -------------//
#define UF2_PRODUCT_NAME      "Adafruit Feather nRF52840 Sense"
#define UF2_VOLUME_LABEL      "FTHRSNSBOOT"
#define UF2_BOARD_ID          "nRF52840-Feather-Sense"
#define UF2_INDEX_URL         "https://www.adafruit.com/product/4516"

#endif // _FEATHER_NRF52840_SENSE_H
```

#### After (Custom Configuration)
```c
#ifndef _FEATHER_NRF52840_SENSE_H
#define _FEATHER_NRF52840_SENSE_H

#define _PINNUM(port, pin)    ((port)*32 + (pin))

/*------------------------------------------------------------------*/
/* LED
 *------------------------------------------------------------------*/
#define LEDS_NUMBER           2
#define LED_PRIMARY_PIN       _PINNUM(1, 9)
#define LED_SECONDARY_PIN     _PINNUM(1, 10)
#define LED_STATE_ON          1

#define LED_NEOPIXEL           _PINNUM(0, 16)
#define NEOPIXELS_NUMBER       1
#define BOARD_RGB_BRIGHTNESS   0x040404

/*------------------------------------------------------------------*/
/* BUTTON
 *------------------------------------------------------------------*/
#define BUTTONS_NUMBER        2
#define BUTTON_1              _PINNUM(1, 2)
#define BUTTON_2              _PINNUM(0, 10)
#define BUTTON_PULL           NRF_GPIO_PIN_PULLUP

/*------------------------------------------------------------------*/
/* BLE OTA
 *------------------------------------------------------------------*/
#define BLEDIS_MANUFACTURER   "Adafruit Industries"
#define BLEDIS_MODEL          "Feather nRF52840 Sense"

/*------------------------------------------------------------------*/
/* USB
 *------------------------------------------------------------------*/
#define USB_DESC_VID           0x239A
#define USB_DESC_UF2_PID       0x0087
#define USB_DESC_CDC_ONLY_PID  0x0088

/*------------------------------------------------------------------*/
/* UF2 Bootloader Info
 *------------------------------------------------------------------*/
#define UF2_PRODUCT_NAME       "Adafruit Feather nRF52840 Sense"
#define UF2_VOLUME_LABEL       "FTHRSNSBOOT"
#define UF2_BOARD_ID           "nRF52840-Feather-Sense"
#define UF2_INDEX_URL          "https://www.adafruit.com/product/4516"

/*------------------------------------------------------------------*/
/* Bootloader Configuration (Required for DFU)
 *------------------------------------------------------------------*/
#define CFG_UF2_BOOTLOADER      1
#define CFG_BOOTLOADER_VERSION  0x0001
#define BOOT_VALIDATE_APP       0  // <== CRITICAL: Skips validation to prevent rollback

#endif // _FEATHER_NRF52840_SENSE_H
```

### Key Changes Explained

1. **`BOOT_VALIDATE_APP = 0`**: This is the most critical change
   - **Purpose**: Disables application validation during boot
   - **Effect**: Prevents the bootloader from rejecting valid firmware
   - **Risk**: Reduces safety checks (acceptable for development)

2. **`CFG_UF2_BOOTLOADER = 1`**: Enables UF2 bootloader features
   - **Purpose**: Maintains compatibility with UF2 drag-and-drop updates
   - **Effect**: Keeps the familiar UF2 update method working

3. **`CFG_BOOTLOADER_VERSION = 0x0001`**: Sets bootloader version
   - **Purpose**: Version tracking for the custom bootloader
   - **Effect**: Helps identify which bootloader version is installed

### Makefile Modifications

The Makefile changes ensure proper Python usage:

**Change**: `python3` instead of `python`
```makefile
# Before
@python tools/hexmerge.py --overlap=replace -o $@ $< $(MBR_HEX)

# After  
@python3 tools/hexmerge.py --overlap=replace -o $@ $< $(MBR_HEX)
```

This ensures compatibility with systems where Python 3 is not the default.

## 🏗️ Building the Custom Bootloader

### Step 1: Clone the Bootloader Repository

```bash
# Clone the Adafruit nRF52 bootloader repository
git clone https://github.com/adafruit/Adafruit_nRF52_Bootloader.git
cd Adafruit_nRF52_Bootloader

# Initialize submodules
git submodule update --init --recursive
```

### Step 2: Apply Custom Changes

1. **Navigate to the board configuration file**:
   ```bash
   cd src/boards/feather_nrf52840_sense/
   ```

2. **Edit the board.h file**:
   ```bash
   nano board.h
   ```

3. **Apply the changes** shown in the "After" section above

4. **Update the Makefile** (if needed):
   ```bash
   cd ../../../
   nano Makefile
   ```

### Step 3: Build the Bootloader

```bash
# Clean any previous builds
make clean

# Build for the Feather nRF52840 Sense
make BOARD=feather_nrf52840_sense

# Optional: Build with debug information
make BOARD=feather_nrf52840_sense DEBUG=1
```

### Step 4: Verify Build Output

After successful compilation, you should see:
```
_build/build-feather_nrf52840_sense/
├── feather_nrf52840_sense_bootloader-<version>.hex
├── feather_nrf52840_sense_bootloader-<version>_nosd.hex
├── feather_nrf52840_sense_bootloader-<version>_s140_6.1.1.hex
└── update-feather_nrf52840_sense_bootloader-<version>_nosd.uf2
```

**File Descriptions**:
- **`*_nosd.hex`**: Bootloader without SoftDevice (for J-Flash)
- **`*_s140_6.1.1.hex`**: Bootloader with SoftDevice (complete package)
- **`update-*.uf2`**: UF2 file for bootloader updates

## 💾 Flashing the Bootloader

### Method 1: Using J-Flash (Recommended)

#### Step 1: Install J-Link Software
1. Download from [SEGGER J-Link Downloads](https://www.segger.com/downloads/jlink/)
2. Install the complete package (includes J-Flash)

#### Step 2: Connect Hardware
1. Connect J-Link debugger to nRF52840 board
2. Connect USB cable to power the board
3. Ensure proper SWD connections:
   - **SWDIO** → Pin 18
   - **SWDCLK** → Pin 19  
   - **GND** → Ground
   - **VCC** → 3.3V

#### Step 3: Flash with J-Flash
1. **Open J-Flash**
2. **Create New Project**:
   - Target Device: `nRF52840_xxAA`
   - Target Interface: `SWD`
   - Speed: `4000 kHz`

3. **Connect to Target**:
   - Click "Target" → "Connect"
   - Verify connection success

4. **Load Bootloader**:
   - Click "File" → "Open data file"
   - Select: `feather_nrf52840_sense_bootloader-<version>_s140_6.1.1.hex`

5. **Flash the Bootloader**:
   - Click "Target" → "Auto"
   - Wait for completion
   - Verify success message

#### Step 4: Verify Installation
1. **Disconnect J-Link**
2. **Reset the board**
3. **Check for UF2 drive**: A drive named "FTHRSNSBOOT" should appear
4. **Test LED**: The bootloader LED should blink

### Method 2: Using nrfjprog (Command Line)

```bash
# Erase the chip
nrfjprog --eraseall -f nrf52

# Flash the bootloader with SoftDevice
nrfjprog --program _build/build-feather_nrf52840_sense/feather_nrf52840_sense_bootloader-<version>_s140_6.1.1.hex --sectorerase -f nrf52

# Reset the chip
nrfjprog --reset -f nrf52
```

### Method 3: Using UF2 (For Updates Only)

If you already have a UF2 bootloader installed:

1. **Enter bootloader mode**: Double-tap reset button
2. **Copy UF2 file**: Drag `update-*.uf2` to the UF2 drive
3. **Wait for completion**: Drive will disappear and reappear

## 📱 Sample Firmware

### Complete Arduino Sketch

**File**: `BLE_LED_Control.ino`

```cpp
#include <bluefruit.h>

// Pin definitions for nRF52840 DK
#define LED1_PIN 13  // P0.13 - Status LED
#define LED2_PIN 14  // P0.14 - LED2
#define LED3_PIN 15  // P0.15 - LED3  
#define LED4_PIN 16  // P0.16 - LED4

#define BUTTON3_PIN 24  // P0.24 - Button SW3

// BLE Services
BLEDis bledis;    // Device Information Service
BLEDfu bledfu;    // Device Firmware Update Service
BLEUart bleuart;  // BLE UART Service

// Function to handle BLE commands
void handle_ble_command(char cmd) {
  switch (cmd) {
    case '1':
      digitalWrite(LED1_PIN, LOW);   // Turn ON LED1 (Active LOW)
      Serial.println("LED1 ON");
      bleuart.println("LED1 ON");
      break;
    case '2':
      digitalWrite(LED1_PIN, HIGH);  // Turn OFF LED1
      Serial.println("LED1 OFF");
      bleuart.println("LED1 OFF");
      break;
    case '3':
      digitalWrite(LED2_PIN, LOW);   // Turn ON LED2
      Serial.println("LED2 ON");
      bleuart.println("LED2 ON");
      break;
    case '4':
      digitalWrite(LED2_PIN, HIGH);  // Turn OFF LED2
      Serial.println("LED2 OFF");
      bleuart.println("LED2 OFF");
      break;
    case '5':
      digitalWrite(LED3_PIN, LOW);   // Turn ON LED3
      Serial.println("LED3 ON");
      bleuart.println("LED3 ON");
      break;
    case '6':
      digitalWrite(LED3_PIN, HIGH);  // Turn OFF LED3
      Serial.println("LED3 OFF");
      bleuart.println("LED3 OFF");
      break;
    case '7':
      digitalWrite(LED4_PIN, LOW);   // Turn ON LED4
      Serial.println("LED4 ON");
      bleuart.println("LED4 ON");
      break;
    case '8':
      digitalWrite(LED4_PIN, HIGH);  // Turn OFF LED4
      Serial.println("LED4 OFF");
      bleuart.println("LED4 OFF");
      break;
    case 'b':
    case 'B':
      // Read button state
      bool buttonPressed = (digitalRead(BUTTON3_PIN) == LOW);
      String buttonState = buttonPressed ? "PRESSED" : "RELEASED";
      Serial.println("BUTTON3 = " + buttonState);
      bleuart.println("BUTTON3 = " + buttonState);
      break;
    case 'a':
    case 'A':
      // Turn on all LEDs
      digitalWrite(LED1_PIN, LOW);
      digitalWrite(LED2_PIN, LOW);
      digitalWrite(LED3_PIN, LOW);
      digitalWrite(LED4_PIN, LOW);
      Serial.println("ALL LEDs ON");
      bleuart.println("ALL LEDs ON");
      break;
    case 'o':
    case 'O':
      // Turn off all LEDs
      digitalWrite(LED1_PIN, HIGH);
      digitalWrite(LED2_PIN, HIGH);
      digitalWrite(LED3_PIN, HIGH);
      digitalWrite(LED4_PIN, HIGH);
      Serial.println("ALL LEDs OFF");
      bleuart.println("ALL LEDs OFF");
      break;
    case 'h':
    case 'H':
      // Help command
      Serial.println("Commands: 1-8 (LED control), A (all on), O (all off), B (button), H (help)");
      bleuart.println("Commands: 1-8 (LED control), A (all on), O (all off), B (button), H (help)");
      break;
    default:
      Serial.println("Unknown command. Send 'H' for help.");
      bleuart.println("Unknown command. Send 'H' for help.");
      break;
  }
}

void setup() {
  Serial.begin(115200);
  
  // Wait for serial connection (optional, remove for production)
  while (!Serial && millis() < 5000) delay(10);
  
  Serial.println("=================================");
  Serial.println("nRF52840 BLE LED Control + DFU");
  Serial.println("=================================");

  // Initialize Bluefruit
  Serial.println("Initializing Bluefruit...");
  Bluefruit.begin();
  
  // Disable auto connection LED to reclaim LED1_PIN
  Bluefruit.autoConnLed(false);
  
  // Set device name and appearance
  Bluefruit.setName("nRF52840_DK");
  Bluefruit.setTxPower(4);    // Max transmit power for better range

  // Configure LED pins (Active LOW)
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
  
  // Turn off all LEDs initially
  digitalWrite(LED1_PIN, HIGH);
  digitalWrite(LED2_PIN, HIGH);
  digitalWrite(LED3_PIN, HIGH);
  digitalWrite(LED4_PIN, HIGH);

  // Configure button pin with pull-up
  pinMode(BUTTON3_PIN, INPUT_PULLUP);

  // Initialize BLE services
  Serial.println("Configuring BLE services...");
  
  // Device Information Service
  bledis.setManufacturer("MindTune");
  bledis.setModel("nRF52840 DK Custom");
  bledis.setFirmwareRev("1.0.0");
  bledis.setHardwareRev("1.0");
  bledis.begin();

  // DFU Service (for OTA updates)
  bledfu.begin();

  // BLE UART Service
  bleuart.begin();

  // Set up advertising
  Serial.println("Setting up advertising...");
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(bleuart);
  Bluefruit.Advertising.addName();
  
  // Set advertising interval (units of 0.625ms)
  Bluefruit.Advertising.setInterval(32, 244);    // 20ms - 152.5ms
  Bluefruit.Advertising.setFastTimeout(30);      // 30 seconds
  
  // Start advertising
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising
  
  Serial.println("=================================");
  Serial.println("Ready! Advertising as 'nRF52840_DK'");
  Serial.println("Connect with nRF Connect app");
  Serial.println("Commands: 1-8 (LEDs), A (all on), O (all off), B (button), H (help)");
  Serial.println("=================================");
  
  // Visual startup indication
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED1_PIN, LOW);
    delay(100);
    digitalWrite(LED1_PIN, HIGH);
    delay(100);
  }
}

void loop() {
  // Handle BLE UART commands
  if (bleuart.available()) {
    char cmd = bleuart.read();
    handle_ble_command(cmd);
  }
  
  // Handle Serial commands (for debugging)
  if (Serial.available()) {
    char cmd = Serial.read();
    handle_ble_command(cmd);
  }
  
  // Optional: Periodic button status (uncomment if needed)
  /*
  static unsigned long lastButtonCheck = 0;
  if (millis() - lastButtonCheck > 1000) {
    lastButtonCheck = millis();
    if (digitalRead(BUTTON3_PIN) == LOW) {
      Serial.println("Button is pressed");
    }
  }
  */
}
```

### Pin Configuration for Different Boards

**For nRF52840 DK**:
```cpp
#define LED1_PIN 13  // P0.13
#define LED2_PIN 14  // P0.14
#define LED3_PIN 15  // P0.15
#define LED4_PIN 16  // P0.16
#define BUTTON3_PIN 24  // P0.24
```

**For Adafruit Feather nRF52840 Sense**:
```cpp
#define LED1_PIN 3   // Red LED
#define LED2_PIN 4   // Blue LED (if available)
#define BUTTON_PIN 7 // User button
#define NEOPIXEL_PIN 16  // NeoPixel
```

## 🔄 Firmware Update Process

### Step 1: Compile and Export Firmware

1. **Open Arduino IDE**
2. **Load the sketch**: Open `BLE_LED_Control.ino`
3. **Select board**: Tools → Board → Adafruit Feather nRF52840 Sense
4. **Configure settings**:
   - Port: Select your device port
   - Programmer: Default
5. **Compile**: Sketch → Verify/Compile
6. **Export binary**: Sketch → Export Compiled Binary

### Step 2: Locate the HEX File

After compilation, find the `.hex` file:
```
Arduino/BLE_LED_Control/
└── build/
    └── adafruit.nrf52.feather52840sense/
        └── BLE_LED_Control.ino.hex
```

### Step 3: Create DFU Package

Open command prompt/terminal in the build directory:

```bash
# Navigate to the build directory
cd /path/to/Arduino/BLE_LED_Control/build/adafruit.nrf52.feather52840sense/

# Create DFU package
adafruit-nrfutil dfu genpkg --dev-type 0x0052 --application BLE_LED_Control.ino.hex --application-version 1 --sd-req 0xB6 --dfu-ver 0 firmware_update.zip
```

**Command Parameters Explained**:
- `--dev-type 0x0052`: nRF52 device type
- `--application`: Path to your hex file
- `--application-version 1`: Version number (increment for updates)
- `--sd-req 0xB6`: SoftDevice requirement (S140 v6.1.1)
- `--dfu-ver 0`: DFU version
- `firmware_update.zip`: Output file name

### Step 4: Upload via BLE DFU

#### Using nRF Connect Mobile App

1. **Install nRF Connect**:
   - [Android](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp)
   - [iOS](https://apps.apple.com/app/nrf-connect/id1054362403)

2. **Connect to Device**:
   - Open nRF Connect
   - Scan for devices
   - Connect to "nRF52840_DK"

3. **Access DFU Service**:
   - Look for "Secure DFU Service" 
   - Tap the DFU icon (usually in top-right)

4. **Upload Firmware**:
   - Select "Distribution packet (ZIP)"
   - Choose your `firmware_update.zip`
   - Tap "Start"
   - Wait for completion (usually 30-60 seconds)

5. **Verify Update**:
   - Device will restart automatically
   - Reconnect and test functionality

#### Using nRF Connect Desktop

1. **Install nRF Connect for Desktop**
2. **Install Programmer app**
3. **Connect device via USB**
4. **Select your device**
5. **Load the ZIP file**
6. **Click "Write"**

### Step 5: Verify Update Success

1. **Check Serial Output**:
   ```
   =================================
   nRF52840 BLE LED Control + DFU
   =================================
   Initializing Bluefruit...
   Ready! Advertising as 'nRF52840_DK'
   ```

2. **Test BLE Connection**:
   - Reconnect with nRF Connect
   - Send test commands (1, 2, 3, etc.)
   - Verify LED responses

3. **Check Version** (if implemented):
   - Send 'V' command to get firmware version

## 🎮 Usage Instructions

### BLE Commands

| Command | Action | Response |
|---------|--------|----------|
| `1` | Turn ON LED1 | "LED1 ON" |
| `2` | Turn OFF LED1 | "LED1 OFF" |
| `3` | Turn ON LED2 | "LED2 ON" |
| `4` | Turn OFF LED2 | "LED2 OFF" |
| `5` | Turn ON LED3 | "LED3 ON" |
| `6` | Turn OFF LED3 | "LED3 OFF" |
| `7` | Turn ON LED4 | "LED4 ON" |
| `8` | Turn OFF LED4 | "LED4 OFF" |
| `A` or `a` | Turn ON all LEDs | "ALL LEDs ON" |
| `O` or `o` | Turn OFF all LEDs | "ALL LEDs OFF" |
| `B` or `b` | Read button state | "BUTTON3 = PRESSED/RELEASED" |
| `H` or `h` | Show help | Command list |

### Using nRF Connect App

1. **Scan and Connect**:
   - Open nRF Connect
   - Tap "SCAN"
   - Find "nRF52840_DK"
   - Tap "CONNECT"

2. **Find UART Service**:
   - Look for "Nordic UART Service"
   - Expand the service
   - Find TX and RX characteristics

3. **Send Commands**:
   - Tap the RX characteristic (usually has an up arrow)
   - Select "Text" format
   - Type your command (e.g., "1")
   - Tap "SEND"

4. **Receive Responses**:
   - Enable notifications on TX characteristic
   - Responses will appear automatically

### Serial Monitor Testing

You can also test via Arduino Serial Monitor:

1. **Open Serial Monitor** (Tools → Serial Monitor)
2. **Set baud rate** to 115200
3. **Send commands** directly (1, 2, 3, etc.)
4. **View responses** in the monitor

## 🐛 Troubleshooting

### Bootloader Issues

#### Problem: Build fails with "command not found"
**Solution**:
```bash
# Install missing tools
sudo apt-get install gcc-arm-none-eabi
sudo apt-get install python3-pip
pip3 install adafruit-nrfutil
```

#### Problem: "No rule to make target"
**Solution**:
```bash
# Clean and rebuild
make clean
make BOARD=feather_nrf52840_sense
```

#### Problem: J-Flash cannot connect
**Solutions**:
1. **Check connections**: Verify SWD wiring
2. **Power cycle**: Disconnect and reconnect power
3. **Try different speed**: Reduce SWD speed to 1000 kHz
4. **Check J-Link**: Ensure J-Link drivers are installed

### Firmware Issues

#### Problem: Device not advertising
**Solutions**:
1. **Check power**: Ensure adequate power supply
2. **Reset device**: Press reset button
3. **Check serial output**: Look for error messages
4. **Verify bootloader**: Ensure custom bootloader is installed

#### Problem: LEDs not responding
**Solutions**:
1. **Check pin definitions**: Verify LED pins for your board
2. **Test with multimeter**: Check if pins are toggling
3. **Check serial output**: Look for command acknowledgments

#### Problem: DFU update fails
**Solutions**:
1. **Check ZIP file**: Ensure it was created correctly
2. **Verify connection**: Strong BLE connection required
3. **Retry**: DFU can sometimes fail due to interference
4. **Check bootloader**: Ensure custom bootloader prevents rollback

### Common Error Messages

#### "DFU failed: Invalid object"
**Cause**: Incorrect DFU package format
**Solution**: Recreate ZIP with correct parameters:
```bash
adafruit-nrfutil dfu genpkg --dev-type 0x0052 --application firmware.hex --application-version 1 --sd-req 0xB6 --dfu-ver 0 update.zip
```

#### "Connection timeout"
**Cause**: BLE connection lost during update
**Solution**: 
1. Move closer to device
2. Ensure stable power supply
3. Retry the update

#### "Bootloader validation failed"
**Cause**: Old bootloader still doing validation
**Solution**: Ensure custom bootloader is properly flashed

### Debug Tips

1. **Enable Serial Output**: Always use Serial.println() for debugging
2. **Check BLE Status**: Monitor connection state
3. **Use LED Indicators**: Visual feedback for debugging
4. **Test Step by Step**: Isolate issues by testing individual components

## 📚 Additional Resources

### Official Documentation
- [nRF52840 Product Specification](https://infocenter.nordicsemi.com/pdf/nRF52840_PS_v1.1.pdf)
- [Adafruit nRF52 Bootloader GitHub](https://github.com/adafruit/Adafruit_nRF52_Bootloader)
- [Nordic nRF5 SDK Documentation](https://infocenter.nordicsemi.com/index.jsp)

### Tools and Software
- [nRF Connect for Mobile](https://www.nordicsemi.com/Products/Development-tools/nrf-connect-for-mobile)
- [nRF Connect for Desktop](https://www.nordicsemi.com/Products/Development-tools/nrf-connect-for-desktop)
- [SEGGER J-Link Software](https://www.segger.com/downloads/jlink/)
- [nRF Command Line Tools](https://www.nordicsemi.com/Products/Development-tools/nrf-command-line-tools)

### Community Resources
- [Adafruit Forums](https://forums.adafruit.com/)
- [Nordic DevZone](https://devzone.nordicsemi.com/)
- [Arduino Forum](https://forum.arduino.cc/)

### Example Projects
- [Adafruit nRF52 Examples](https://github.com/adafruit/Adafruit_nRF52_Arduino/tree/master/libraries/Bluefruit52Lib/examples)
- [Nordic SDK Examples](https://github.com/NordicSemiconductor/nRF5_SDK/tree/master/examples)

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2024 nRF52840 Custom Bootloader Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## 🙏 Acknowledgments

- **Adafruit Industries** - For the original bootloader and excellent hardware
- **Nordic Semiconductor** - For the nRF52840 SoC and comprehensive SDK
- **Ha Thach** - Original bootloader author
- **SEGGER** - For J-Link debugging tools
- **Arduino Community** - For the development environment and libraries

---

## 📞 Support

If you encounter issues or have questions:

1. **Check the troubleshooting section** above
2. **Search existing issues** in the repository
3. **Create a new issue** with detailed information:
   - Hardware used
   - Software versions
   - Complete error messages
   - Steps to reproduce

**When reporting issues, please include**:
- Board type and revision
- Bootloader version
- Arduino IDE version
- Complete serial output
- Photos of hardware setup (if relevant)

---

*Last updated: [Current Date]*
*Version: 1.0.0* 
