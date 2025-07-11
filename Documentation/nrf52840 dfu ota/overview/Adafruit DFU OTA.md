## What is Adafruit DFU OTA?

Adafruit’s DFU system allows you to **update firmware wirelessly via BLE**, or over **USB using UF2 drag-and-drop** or **serial using `bossac`**. For OTA, BLE DFU is the main method, based on Nordic’s Secure DFU protocol with enhancements for Adafruit hardware.

---

## ⚙️ Adafruit DFU OTA Architecture

### 1. **UF2 Bootloader**

- **UF2** (USB Flashing Format): A bootloader that exposes the device as a USB mass storage device.
    
- Developed by Microsoft, adapted by Adafruit.
    
- Allows you to drag and drop `.uf2` files to flash firmware.
    

### 2. **BLE DFU Bootloader**

- Fork of Nordic’s Secure DFU bootloader (nRF5 SDK).
    
- BLE service used: `0xFE59` (Secure DFU)
    
- Supports:
    
    - Application-only update
        
    - SoftDevice + Application
        
    - Bootloader + Application (only over wired, not recommended for OTA)
        

### 3. **Application Firmware**

- Normally built using **Arduino** or **CircuitPython** SDK
    
- Must include **BLE DFU Service** and be configured to **jump into bootloader**
    

---

## 📦 Firmware Formats Supported

|Format|Use Case|How to Flash|
|---|---|---|
|`.uf2`|USB drag-and-drop (easiest)|Copy to USB Mass Storage|
|`.hex`|Legacy Nordic flashing|`nrfutil` or JLink|
|`.zip`|BLE DFU OTA update|Used by `nrfutil` and nRF Connect app|

---

## 🔁 OTA DFU Flow (BLE)

### Step-by-Step:

1. **Running application** advertises BLE DFU service (UUID `0xFE59`)
    
2. Mobile app connects (e.g. **nRF Connect for Mobile**)
    
3. Sends **DFU start command**
    
4. App reboots into **bootloader** (BLE only bootloader takes over)
    
5. Sends `init packet` with metadata
    
6. Transfers firmware `.bin`
    
7. Bootloader validates:
    
    - Signature (optional)
        
    - CRC
        
    - Version
        
8. If success:
    
    - New firmware is flashed
        
    - Device reboots into new application
        

---

## 🔐 Secure DFU with Adafruit

Adafruit supports:

- **Signature Verification** (ECDSA or None)
    
- **Version Checks**
    
- **Init Packet CRC**
    

Adafruit bootloaders embed public keys and verify `.zip` files built via `nrfutil` or `adafruit-nrfutil`.

---

## 🛠️ Tools Used for Adafruit DFU OTA

### 1. `adafruit-nrfutil`

- Fork of Nordic’s `nrfutil`
    
- Used to:
    
    - Package DFU zip files
        
    - Flash over USB, Serial, or BLE
        
- Install:
    
    bash
    
    CopyEdit
    
    `pip install adafruit-nrfutil`
    

### 2. **nRF Connect App**

- GUI for iOS/Android to perform OTA updates
    
- Upload DFU `.zip` file wirelessly
    

---

## 🔧 DFU Zip Creation

Use this command to generate a DFU-compatible `.zip`:

bash

CopyEdit

`adafruit-nrfutil dfu genpkg \   --dev-type 0x0052 \   --application firmware.hex \   firmware_dfu.zip`

For signed DFU:

bash

CopyEdit

`adafruit-nrfutil keys generate private.key adafruit-nrfutil dfu genpkg \   --application firmware.hex \   --key-file private.key \   firmware_dfu.zip`

---

## 🧪 Entering DFU Bootloader Mode

Adafruit bootloaders support multiple ways to enter DFU:

|Method|Description|
|---|---|
|Double-tap **Reset**|Enters UF2 bootloader mode (USB)|
|**BLE command**|Triggers bootloader jump from application|
|**Serial command**|`adafruit-nrfutil dfu serial --enter`|
|**Button press at boot**|If configured in bootloader build|

In Arduino or C/C++ code:

cpp

CopyEdit

`// Triggers bootloader entry from app sd_power_gpregret_clr(0, 0xFF); sd_power_gpregret_set(0, BOOTLOADER_DFU_START); NVIC_SystemReset();`

---

## 💡 Flash Memory Layout

Typical flash layout for Adafruit Feather nRF52840:

scss

CopyEdit

`0x00000  - MBR (bootloader start) 0x01000  - SoftDevice (if used) 0x26000  - Application 0x78000  - Bootloader 0x7E000  - Bootloader Settings 0x7F000  - MBR Parameters`

Make sure firmware doesn’t overlap with the bootloader!

---

## 📋 Bootloader Source Code

Adafruit bootloader GitHub repo:  
🔗 [https://github.com/adafruit/Adafruit_nRF52_Bootloader](https://github.com/adafruit/Adafruit_nRF52_Bootloader)

Customize:

- `boards/feather_nrf52840_sense/board.h`
    
- `Makefile`, `bootloader_settings.c`, etc.
    

Rebuild with `make BOARD=feather_nrf52840_sense all`

---

## 🧯 Common Issues

|Problem|Fix|
|---|---|
|DFU fails midway|Try updating SoftDevice + Application together|
|App doesn't enter DFU|Add BLE DFU service and reboot logic|
|Firmware rolls back|Bootloader fails validation → reverts to UF2|
|Flash size overflow|Reduce debug logs and use `F()` macro in Arduino|

---

## ✅ Best Practices

- Always test OTA DFU **before field deployment**
    
- Keep bootloader **unchanged unless needed**
    
- **Sign DFU packages** for secure delivery
    
- Use `BOOTLOADER_SETTINGS` area carefully — it’s critical for rollback
    

---

## 📚 References

- [Adafruit nRF52 Bootloader GitHub](https://github.com/adafruit/Adafruit_nRF52_Bootloader)
    
- [Adafruit-nrfutil Tool](https://github.com/adafruit/Adafruit_nRF52_nrfutil)
    
- nRF5 SDK Documentation
    
- nRF Connect Mobile App