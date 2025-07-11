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
#define BOOT_VALIDATE_APP       0  // <== SKIPS validation to prevent rollback (DEV MODE)

#endif // _FEATHER_NRF52840_SENSE_H
