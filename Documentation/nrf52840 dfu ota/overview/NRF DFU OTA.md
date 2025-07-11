
## 🔧 What is DFU OTA?

**DFU OTA** allows you to **remotely update firmware** over Bluetooth Low Energy (BLE) or other transport layers (USB, serial, etc.). This is crucial for products already deployed in the field — no need for physical access or JTAG/SWD tools.

---

## 🧠 Core Components of nRF DFU OTA

### 1. **Bootloader**

- Special firmware partition that runs before the main application.
    
- It checks for a valid application or incoming update request.
    
- Types:
    
    - **Legacy Bootloader** (uses SoftDevice)
        
    - **Secure Bootloader (recommended)** using nRF Secure DFU (with cryptographic validation)
        
    - **MCUBoot Bootloader** (for Zephyr-based projects like nRF Connect SDK)
        

### 2. **SoftDevice (for nRF5 SDK only)**

- A precompiled BLE stack from Nordic.
    
- Provides DFU BLE services and security layers.
    
- Deprecated in nRF Connect SDK (Zephyr used instead).
    

### 3. **DFU Transport**

- BLE (most common)
    
- UART
    
- USB
    
- SPI (less common)
    
- ANT (Nordic-specific)
    

### 4. **DFU Services & Protocols**

- **BLE DFU Service** (UUID: `0xFE59`)
    
- **Secure DFU Protocol v1/v2**: Handles packetized firmware transmission, state machine, init packet verification.
    

---

## 📦 Firmware Image Types

Firmware updates are transmitted in `.zip` archives (called **DFU packages**) which may contain:

- `application.hex` or `.bin`
    
- `bootloader.hex` or `.bin`
    
- `softdevice.hex` (if applicable)
    
- `manifest.json` — defines what type of image is included
    
- `init packet` (`.dat`) — metadata, version, CRC, signature
    

---

## 🔐 Security: Signed Updates

Secure DFU supports:

- **SHA256 hash validation**
    
- **ECDSA digital signature** (with public key embedded in bootloader)
    
- **Version checking** to prevent downgrade attacks
    

---

## 🚀 OTA DFU Flow (BLE Example)

### Step-by-step:

1. **Device boots into application**
    
2. BLE advertising includes DFU service UUID
    
3. Mobile app (e.g. nRF Connect) connects
    
4. Sends command to **enter DFU mode**
    
    - Device reboots into bootloader
        
5. App sends `init packet` for validation
    
6. Sends firmware in chunks (16–512 bytes per packet)
    
7. Bootloader receives & validates:
    
    - Signature
        
    - CRC
        
    - Firmware version
        
8. If successful:
    
    - Flashes new image
        
    - Reboots into updated application
        

---

## 📱 Tools Used

### Nordic Tools:

- ✅ **nRF Connect for Mobile** (Android/iOS) – for BLE DFU
    
- ✅ **nRF Util (CLI)** – package creation, flashing, DFU via UART
    
- ✅ **nRF Programmer** – GUI for DFU over USB/JLink
    

### Firmware Helpers:

- `nrfutil pkg generate` – creates DFU `.zip` file
    
- `nrfutil dfu ble` or `dfu serial` – triggers OTA update
    

---

## 🧱 Bootloader Configuration

In `nRF5 SDK`:

- `dfu_bootloader_project` (GCC/Keil)
    
- Configure flash memory layout:
    
    - Bootloader: `0x78000–0x80000` (for 512KB flash)
        
    - Application: `0x26000–0x78000`
        
    - SoftDevice: `0x1000–0x26000`
        

In `nRF Connect SDK (Zephyr)`:

- Uses **MCUBoot**
    
- Configuration in `prj.conf`:
    
    conf
    
    CopyEdit
    
    `CONFIG_BOOTLOADER_MCUBOOT=y CONFIG_MCUBOOT_SIGNATURE_KEY_FILE="my_priv_key.pem"`
    

---

## ⚠️ Common Pitfalls

|Issue|Cause|Fix|
|---|---|---|
|DFU fails at init packet|Mismatched signature/key|Regenerate package with correct key|
|DFU not advertising|App not triggering DFU mode|Add BLE service or button trigger|
|Reboots into bootloader repeatedly|Invalid application image|Verify firmware integrity and flash addresses|
|Rollback to factory firmware|Bootloader detects update failed|Ensure proper `init_packet` and CRC|

---

## ✅ Best Practices

- Use **Secure DFU with signatures** in production.
    
- Store **firmware version** in init packet.
    
- **Test DFU** via BLE and serial before shipping devices.
    
- Keep **bootloader as small and isolated** as possible.
    
- Implement **fail-safe logic** (fallback image or retry mode).
    

---


## 📚 References

- [nRF5 SDK DFU documentation](https://infocenter.nordicsemi.com/)
    
- [nRF Connect SDK (Zephyr) DFU/MCUBoot](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/)
    
- [nRF Util GitHub](https://github.com/NordicSemiconductor/pc-nrfutil)
    
- [MCUBoot GitHub](https://github.com/mcu-tools/mcuboot)