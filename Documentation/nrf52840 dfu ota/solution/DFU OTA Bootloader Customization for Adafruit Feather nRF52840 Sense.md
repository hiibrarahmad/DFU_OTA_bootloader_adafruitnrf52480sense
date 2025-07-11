# Adafruit Feather nRF52840 Sense: DFU OTA Custom Bootloader Guide (No Rollback)

---

## 🌟 Objective

Completely eliminate firmware rollback during Over-The-Air (OTA) Device Firmware Update (DFU) on the Adafruit Feather nRF52840 Sense by:

1. **Customizing the BLE bootloader** to disable application validation.
    
2. **Building** the bootloader with Adafruit’s tools and Nordic SDK.
    
3. **Flashing** the bootloader via J-Link (nrfjprog).
    
4. **Uploading** an example sketch with BLE UART & DFU services via the Arduino IDE.
    
5. **Wrapping** the compiled firmware into a DFU package using a **single-line** `adafruit-nrfutil` command.
    
6. **Performing** repeated, seamless OTA updates using nRF Connect Mobile without rollback.
    

This guide walks through every command, file change, and verification step.

---

## 🛠️ 1. Environment Setup

### 🔹 Prerequisites

- **Host OS**: Windows 10/11 or Linux/macOS with Git, Python 3, and GNU Make.
    
- **Tools**:
    
    - `adafruit-nrfutil` CLI (Python package)
        
    - Segger **J-Link** or **nrfjprog** tools
        
    - **Arduino IDE** with Adafruit nRF52840 board support
        
    - **nRF Connect** mobile app (iOS/Android)
        

### 🔹 Install Adafruit nRFUtil

```bash
pip3 install --user adafruit-nrfutil
```

- Installs `adafruit-nrfutil` in your user’s Python environment.
    
- Confirm installation:
    

```bash
adafruit-nrfutil version
# Should print version 6.x.x
```

### 🔹 Clone Adafruit Bootloader Source

```bash
git clone https://github.com/adafruit/Adafruit_nRF52_Bootloader.git
cd Adafruit_nRF52_Bootloader
git submodule update --init --recursive
```

- Clones the bootloader repository and initializes submodules (TinyUSB, Nordic SDKs, UF2).
    

---

## 🔧 2. Modify `board.h` (Exact Changes)

File: `src/boards/feather_nrf52840_sense/board.h`

### 2.1 Fix Button Pin Definition

**Before**:

```c
#define BUTTONS_NUMBER 2
#define BUTTON_1       _PINNUM(1, 02)   // <-- Octal literal, breaks parsing
#define BUTTON_2       _PINNUM(0, 10)
```

**After**:

```c
#define BUTTONS_NUMBER 2
#define BUTTON_1       _PINNUM(1, 2)    // Fixed to decimal '2'
#define BUTTON_2       _PINNUM(0, 10)
```

### 2.2 Add Bootloader Configuration to Skip Validation

Insert these lines **just above** the `#endif`:

```c
/*------------------------------------------------------------------*/
/* Bootloader Configuration (Required for DFU)                       */
/*------------------------------------------------------------------*/
#define CFG_UF2_BOOTLOADER      1    // Enable UF2 bootloader mode
#define CFG_BOOTLOADER_VERSION  0x0001
#define BOOT_VALIDATE_APP       0    // 0 = skip app validation to prevent rollback (DEV MODE)
```

- **Reason**: When `BOOT_VALIDATE_APP` is `1`, the bootloader checks CRC/signature/version and rolls back on invalid. Setting it to `0` forces it to always trust the new application.
    

### 2.3 Confirm USB & UF2 Metadata

Verify these remain or are added correctly:

```c
/* USB descriptors */
#define USB_DESC_VID           0x239A
#define USB_DESC_UF2_PID       0x0087
#define USB_DESC_CDC_ONLY_PID  0x0088

/* UF2 info displayed as mass storage */
#define UF2_PRODUCT_NAME       "Adafruit Feather nRF52840 Sense"
#define UF2_VOLUME_LABEL       "FTHRSNSBOOT"
#define UF2_BOARD_ID           "nRF52840-Feather-Sense"
#define UF2_INDEX_URL          "https://www.adafruit.com/product/4516"
```

- Ensures the bootloader shows the correct vendor/device ID and UF2 volume name in USB.
    

---

## 🏗️ 3. Build the Custom Bootloader

### 3.1 Resolve Windows Toolchain Quirks

- On Windows, `cut`, `tr`, and other UNIX tools may be missing. Install **Git Bash** or **GNUWin32** utils.
    
- Ensure `python3` (not `python`) is in PATH.
    

### 3.2 Run `make`

```bash
make BOARD=feather_nrf52840_sense
```

- Targets:
    
    - `_build/build-feather_nrf52840_sense/*.out`
        
    - `*.hex`, `*_nosd.hex`
        
    - `update-*.uf2`
        
    - `*.zip` (bootloader + SoftDevice)
        

### 3.3 Check for Errors

- **Missing SoftDevice**: Verify `lib/softdevice/s140_nrf52_6.1.1/*.hex` exists.
    
- **`hexmerge.py` failed**: Ensure you call with `python3 tools/hexmerge.py ...`.
    

### 3.4 Locate Built Files

```text
_build/build-feather_nrf52840_sense/
  ├─ feather_nrf52840_sense_bootloader-<ver>.hex
  ├─ feather_nrf52840_sense_bootloader-<ver>_nosd.hex
  ├─ update-feather_nrf52840_sense_bootloader-<ver>_nosd.uf2
  ├─ feather_nrf52840_sense_bootloader-<ver>_s140_6.1.1.hex
  └─ feather_nrf52840_sense_bootloader-<ver>_s140_6.1.1.zip
```

---

## 🔌 4. Flash the Custom Bootloader via J-Link

### 4.1 Erase & Recover Device

```bash
nrfjprog --recover            # Unlock & clear readback protection
nrfjprog --eraseall           # Full chip erase
```

- **Note**: If readback protection is enabled, use `--recover` first.
    

### 4.2 Program the Bootloader

```bash
nrfjprog --program _build/build-feather_nrf52840_sense/feather_nrf52840_sense_bootloader-*.hex --verify --reset
```

- `--program`: hex file path
    
- `--verify`: ensure flash integrity
    
- `--reset`: reboot into newly flashed bootloader
    

### 4.3 Verify USB DFU Bootloader

- Unplug & replug the USB cable into Feather nRF52840 Sense.
    
- Confirm mass storage drive **`FTHRSNSBOOT`** appears.
    
- Confirm new COM port shows (e.g., `COM3`/`ttyACM0`).
    

---

## 👩‍💻 5. Upload Example BLE DFU Sketch via Arduino IDE

### 5.1 Install Board in Arduino IDE

- In **Preferences**, add Adafruit URL: `https://www.adafruit.com/package_adafruit_index.json`.
    
- In **Boards Manager**, install **Adafruit nRF52**.
    
- Select **Feather nRF52840 Sense**.
    

### 5.2 Example Sketch: BLE UART + OTA DFU

Paste the code below in a new sketch:

```cpp
#include <bluefruit.h>

#define LED1_PIN 13
#define LED2_PIN 14
#define LED3_PIN 15
#define LED4_PIN 16
#define BUTTON3_PIN 24

BLEDis    bledis;
BLEUart   bleuart;
BLEDfu    bledfu;

void handleCommand(char cmd) {
  switch (cmd) {
    case '1': digitalWrite(LED1_PIN, LOW);  Serial.println("Custom LED1 ON");  break;
    case '2': digitalWrite(LED1_PIN, HIGH); Serial.println("Custom LED1 OFF"); break;
    case '3': digitalWrite(LED2_PIN, LOW);  Serial.println("LED2 ON");        break;
    case '4': digitalWrite(LED2_PIN, HIGH); Serial.println("LED2 OFF");       break;
    case '5': digitalWrite(LED3_PIN, LOW);  Serial.println("LED3 ON");        break;
    case '6': digitalWrite(LED3_PIN, HIGH); Serial.println("LED3 OFF");       break;
    case '7': digitalWrite(LED4_PIN, LOW);  Serial.println("LED4 ON");        break;
    case '8': digitalWrite(LED4_PIN, HIGH); Serial.println("LED4 OFF");       break;
    case 'b': bleuart.println(digitalRead(BUTTON3_PIN)==LOW?"BUTTON PRESSED":"BUTTON RELEASED");
              break;
    default:  bleuart.println("Unknown cmd"); break;
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  pinMode(LED1_PIN, OUTPUT); pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT); pinMode(LED4_PIN, OUTPUT);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);

  Bluefruit.begin();
  bledis.setManufacturer("Adafruit");
  bledis.setModel("nRF52840 Sense");
  bledis.begin();

  bleuart.begin();
  bledfu.begin();  // Enable OTA DFU service

  Bluefruit.setName("nRF52840_DK");
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.start();

  Serial.println("Ready: send '1'-'8' or 'b' via BLE UART");
}

void loop() {
  if (bleuart.available()) {
    handleCommand(bleuart.read());
  }
}
```

1. **Upload** via USB (verify `FTHRSNSBOOT` volume is not mounted while uploading).
    
2. Open **Serial Monitor** at **115200 baud**.
    
3. Use nRF Connect to connect, send commands `1`–`8` and `b`, see responses.
    

---

## 🗜️ 6. Prepare OTA DFU Package

### 6.1 Export Compiled `.hex`

- In Arduino: **Sketch → Export compiled Binary**.
    
- Locate `Blink.ino.hex` in `Documents/Arduino/Blink/build/adafruit.nrf52.feather52840sense/`.
    

### 6.2 Single-Line `adafruit-nrfutil` Command

```bash
adafruit-nrfutil dfu genpkg --dev-type 0x0052 --application Blink.ino.hex --application-version 0 --sd-req 0xB6 --dfu-ver 0 dfu_app_only.zip
```

- `--dev-type 0x0052`: Device type ID for Feather nRF52840
    
- `--application-version 0`: App version number
    
- `--sd-req 0xB6`: SoftDevice requirement (S140 v6.1.1)
    
- `--dfu-ver 0`: DFU protocol version
    

Output: **`dfu_app_only.zip`**

---

## 📲 7. Perform OTA DFU with nRF Connect Mobile

1. Launch **nRF Connect** app.
    
2. Scan and connect to **nRF52840_DK**.
    
3. Tap **DFU** → **Select file** → pick `dfu_app_only.zip`.
    
4. Tap **Start**. Wait until **100%**.
    
5. Device reboots automatically.
    
6. In **Serial Monitor**, see **"Custom LED1 ON"** when sending `1`.
    

Repeat the process repeatedly with new builds to confirm **no rollback**.

---

## ✅ 8. Verification Checklist

|Step|Expected Result|
|---|---|
|Bootloader flashed|`FTHRSNSBOOT` USB drive appears|
|Initial sketch via USB|BLE UART commands work|
|`adafruit-nrfutil` package|`dfu_app_only.zip` created|
|OTA DFU on mobile|Update reaches 100% without errors|
|Post-DFU Serial|Custom messages appear correctly|
|Repeated DFU|No rollback; always runs latest code|

