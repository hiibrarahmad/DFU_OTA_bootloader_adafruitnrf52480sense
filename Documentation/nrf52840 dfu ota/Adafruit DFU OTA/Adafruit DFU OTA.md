## rerequisites

Before you start, make sure you have:

1. **Adafruit nRF52 board** with **Adafruit UF2 + Secure Bootloader** preloaded
    
2. **BLE-capable device** (iPhone, Android, etc.)
    
3. Firmware compiled into `.hex` or `.bin`
    
4. `adafruit-nrfutil` installed:
    
    bash
    
    CopyEdit
    
    `pip install adafruit-nrfutil`
    
5. Optional: `nRF Connect` mobile app (for OTA testing)
    

---

## 📦 Step 1: Compile Firmware

You can use:

- **Arduino IDE**, PlatformIO (for `.hex`)
    
- **C/C++ with GCC** (`.hex` or `.bin`)
    
- **CircuitPython** doesn't need OTA (just drag-drop)
    

Make sure the firmware **does not overwrite the bootloader region**.

---

## 🔐 Step 2: Create a DFU `.zip` Package

You need to package your firmware into a `.zip` with metadata.

### Command:

bash

CopyEdit

`adafruit-nrfutil dfu genpkg \   --dev-type 0x0052 \   --application your_firmware.hex \   firmware_dfu.zip`

**Optional (for signed DFU):**

bash

CopyEdit

`adafruit-nrfutil keys generate private.key  adafruit-nrfutil dfu genpkg \   --application your_firmware.hex \   --key-file private.key \   firmware_signed.zip`

> 📝 The `--dev-type 0x0052` is standard for Adafruit nRF52 boards.

---

## 📱 Step 3: Install **nRF Connect for Mobile**

- Download **nRF Connect for Mobile** (Android/iOS)
    
- Enable Bluetooth and location services
    

---

## 🔄 Step 4: Enter DFU Mode on the Board

You have several ways to do this:

### 📌 Option 1: Double-Tap RESET

- Double-press the **RESET** button quickly.
    
- Board enters **bootloader mode** (you'll see UF2 drive appear over USB)
    
- If BLE DFU is enabled, it will also advertise a **“DFU-XXXX”** name.
    

### 📌 Option 2: From Application Firmware (auto DFU)

Add this to your firmware code to **automatically reboot into DFU**:

cpp

CopyEdit

`#include <nrf_nvic.h> #include <nrf_sdm.h> #include <nrf_soc.h>  sd_power_gpregret_clr(0, 0xFF); sd_power_gpregret_set(0, BOOTLOADER_DFU_START); NVIC_SystemReset();`

---

## 📡 Step 5: Start OTA Update with Mobile App

### Using **nRF Connect (Android/iOS)**:

1. Open the app
    
2. Scan for Bluetooth devices
    
3. Tap on device named **“DFU-XXXX”** or your board name
    
4. Tap **DFU**
    
5. Choose `firmware_dfu.zip` from file browser
    
6. Start the update
    

> You’ll see progress %, and the board will reboot into the new firmware.

---

## 🔁 Step 6: Repeating DFU OTA Again and Again

Once setup is ready, you can repeat DFU easily:

1. Modify your firmware
    
2. Recompile to get updated `.hex`
    
3. Generate new `.zip` using `adafruit-nrfutil`
    
4. Enter DFU mode (RESET double-tap or software-triggered)
    
5. Use **nRF Connect** to upload new `.zip` file
    

### Bonus Tip:

Make a script like:

bash

CopyEdit

`#!/bin/bash HEX=my_app.hex ZIP=my_app_dfu.zip  adafruit-nrfutil dfu genpkg --dev-type 0x0052 \   --application $HEX \   $ZIP`

Use this after every new build to simplify DFU.

---

## 🔍 Troubleshooting Tips

|Issue|Cause|Fix|
|---|---|---|
|DFU doesn’t start|App didn’t trigger bootloader|Use RESET double-tap or add DFU code in firmware|
|Update fails at 50%|Incorrect memory config|Make sure bootloader, SoftDevice, and app regions don’t overlap|
|Device not advertising DFU|App isn’t entering DFU mode|Check for correct `sd_power_gpregret` logic or manual RESET|
|DFU rollbacks to UF2|Signature or CRC mismatch|Recheck the `.zip` or disable signature in bootloader|
|Can't find `.zip` in mobile|File manager permission issue|Ensure `.zip` is placed in accessible folder|

---

## 🧠 Summary (Cheat Sheet)

|Task|Command|
|---|---|
|Build DFU `.zip`|`adafruit-nrfutil dfu genpkg --dev-type 0x0052 --application file.hex out.zip`|
|Enter DFU (manual)|Double-tap RESET|
|Enter DFU (software)|Use `sd_power_gpregret_set()` + `NVIC_SystemReset()`|
|Upload via mobile|Use **nRF Connect**|
|Repeat|Rebuild → Generate `.zip` → DFU again|

---

If you want, I can help you:

- Modify your firmware to include **auto DFU trigger**
    
- Write scripts to automate `.zip` creation
    
- Debug failed OTA DFU updates with logs