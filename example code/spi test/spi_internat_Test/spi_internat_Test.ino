#include <Arduino.h>
#include <Adafruit_TinyUSB.h>   // Pull in TinyUSB CDC so Serial works
#include <nrf_gpio.h>           // nRF5 GPIO HAL

// ——— Feather nRF52840 Sense Pinout ———

// P1.13  — PIN_SCK  (bit-bang SPI clock)
// P1.15  — PIN_MOSI (bit-bang SPI MOSI)
// P1.14  — PIN_MISO (bit-bang SPI MISO)
// USB    — CDC serial port for debugging


static constexpr uint8_t PIN_SCK    = 45;           // P1.13
static constexpr uint8_t PIN_MOSI   = 47;           // P1.15
static constexpr uint8_t PIN_MISO   = 46;           // P1.14
static constexpr uint8_t TEST_VAL   = 0x5A;         // Pattern to loop back

//=============================================================================
// setup()
// — Initialize GPIOs, USB serial.
//=============================================================================
void setup() {
 
  // Start USB CDC serial at 115200 baud
  Serial.begin(115200);
  while (!Serial) delay(10);  // Wait for host to open port

  Serial.println(F("=== Bit-bang SPI Loopback Test ==="));

  // Configure bit-bang pins using Nordic HAL:
  // SCK and MOSI as outputs, MISO as input (no pull)
  nrf_gpio_cfg_output(PIN_SCK);
  nrf_gpio_cfg_output(PIN_MOSI);
  nrf_gpio_cfg_input(PIN_MISO, NRF_GPIO_PIN_NOPULL);

  // Ensure clock idles low
  nrf_gpio_pin_clear(PIN_SCK);
}

//=============================================================================
// bb_spi_transfer(out)
// — Manually clock out one byte over MOSI, read back MISO.
// — Returns the byte sampled on MISO.
//=============================================================================
uint8_t bb_spi_transfer(uint8_t out) {
  uint8_t in = 0;

  for (int8_t bit = 7; bit >= 0; --bit) {
    // 1) Drive MOSI with the next bit (MSB first)
    nrf_gpio_pin_write(PIN_MOSI, (out >> bit) & 1);
    delayMicroseconds(1);

    // 2) Pulse clock high
    nrf_gpio_pin_set(PIN_SCK);
    delayMicroseconds(1);

    // 3) Sample MISO on the rising edge
    in |= (nrf_gpio_pin_read(PIN_MISO) << bit);

    // 4) Pull clock low again
    nrf_gpio_pin_clear(PIN_SCK);
    delayMicroseconds(1);
  }

  return in;
}

//=============================================================================
// loop()
// perform the loopback test, print result.
//=============================================================================
void loop() {
  // — Perform SPI loopback —
  uint8_t received = bb_spi_transfer(TEST_VAL);

  // — Print PASS/FAIL to serial —
  if (received == TEST_VAL) {
    Serial.println(F("PASS  ↔ 0x5A"));
  } else {
    // hex-print both sent and received values
    Serial.printf("FAIL: sent 0x%02X, got 0x%02X\n", TEST_VAL, received);
  }

  // Short pause before repeating
  delay(500);
}
