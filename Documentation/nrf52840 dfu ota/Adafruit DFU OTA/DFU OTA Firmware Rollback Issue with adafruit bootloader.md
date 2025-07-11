## Detailed Problem Statement: DFU OTA Firmware Rollback Issue

During DFU OTA updates using the Adafruit Secure Bootloader on the **Feather nRF52840 Sense**, we observed that the **firmware update process completes the upload**, but the **device rolls back to the bootloader or enters UF2 mode** instead of booting into the new application. This behavior is **repeatable and consistent**, and it indicates that the **bootloader is rejecting the newly flashed application image after update**, triggering a rollback or safe recovery mode.

### ✅ DFU OTA Flow Behavior Observed:

1. The device enters DFU mode successfully (either via button, software trigger, or double-tap reset).
    
2. A valid `.zip` DFU package is uploaded via the nRF Connect app (or `adafruit-nrfutil`).
    
3. The update completes with a 100% progress bar.
    
4. The device **reboots**, but instead of running the new firmware, it:
    
    - Either **returns to the DFU bootloader mode** (`DFU-XXXX` BLE name),
        
    - Or **enters UF2 mass storage mode** via USB,
        
    - Or simply **fails to advertise as expected** — indicating that the bootloader rejected the app and flagged it as invalid.
        

---

## ⚠️ Technical Cause (Confirmed from Debugging Yesterday)

The issue is caused by **bootloader-level validation logic**, which fails after DFU OTA completes, specifically due to:

> 🔸 **Missing or incorrect bootloader settings and app metadata update** post-flashing.

### As we confirmed:

- The Adafruit bootloader expects the **bootloader settings page** (`bootloader_settings.c`) to be updated **automatically after successful DFU**.
    
- However, this update **was not taking place correctly** with your setup, due to a mismatch between the format of the DFU package or the build configuration of the firmware.
    
- As a result, the bootloader **could not verify the new application image**, and treated it as **invalid**, leading to rollback.
    
- This mechanism is part of **Adafruit’s fail-safe rollback feature**, designed to **protect against bricking** the device after a corrupt OTA update.
    

### Internal validation check (inside bootloader):

c

CopyEdit

`if (!nrf_dfu_app_is_valid()) {     enter_dfu_mode(); // or enter UF2 mode }`

---

## 🔍 Root Trigger from Our Work Yesterday

The **prebuilt Adafruit bootloader** you were using includes a rollback safety mechanism that kicks in **if any of the following validations fail after DFU**:

|Checkpoint|Status in your case|
|---|---|
|App CRC matches `init_packet`|✅ Yes|
|Signature verified|✅ Yes or disabled|
|App flash location valid|✅ Yes (`0x26000`)|
|**Bootloader settings page written**|❌ **Failed (this was the root issue)**|
|App starts without immediate crash|✅ Yes|

The **bootloader settings page**, located just before the bootloader in flash (`0x7E000`), was not properly updated during DFU. This meant that on reboot, the bootloader couldn't find a "valid app" flag, so it **assumed the update failed** and reverted.

This **rollback is intentional** by design — the bootloader avoids loading potentially corrupted or unsafe firmware if validation fails.

---

## 📝 Summary Statement of Issue (For Documentation or Reports)

> **The DFU OTA firmware rollback issue** on the Adafruit nRF52840 Feather Sense board was caused by the failure of the bootloader to detect a valid application after an otherwise successful DFU transfer. Although the `.zip` file uploads and flashes correctly, the bootloader’s post-update validation fails because the `bootloader settings` region (used to mark a valid application) was not properly written. As a result, the bootloader considers the application invalid and enters recovery mode (either DFU BLE mode or UF2 mass storage), rolling back to ensure system integrity. This behavior aligns with the Adafruit bootloader’s fail-safe design, which reverts to a known-good state to prevent bricking the device in case of firmware issues.**