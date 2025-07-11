#include <bluefruit.h>

// Active-LOW LED pins on nRF52840 DK
#define LED1_PIN 13
#define LED2_PIN 14
#define LED3_PIN 15
#define LED4_PIN 16

#define BUTTON3_PIN 24  // P0.24 – SW3

// BLE services
BLEDis  bledis;
BLEDfu  bledfu;
BLEUart bleuart;

// Reuse your existing handler; just make sure it drives the LEDs
// via nrf_gpio and replies via bleuart.println(...)
void handle_ble_command(char cmd) {
  switch (cmd) {
    case '1':
      nrf_gpio_pin_clear(LED1_PIN);  bleuart.println("LED1 ON");
      break;
    case '2':
      nrf_gpio_pin_set(LED1_PIN);    bleuart.println("LED1 OFF");
      break;
    case '3':
      nrf_gpio_pin_clear(LED2_PIN);  bleuart.println("LED2 ON");
      break;
    case '4':
      nrf_gpio_pin_set(LED2_PIN);    bleuart.println("LED2 OFF");
      break;
    case '5':
      nrf_gpio_pin_clear(LED3_PIN);  bleuart.println("LED3 ON");
      break;
    case '6':
      nrf_gpio_pin_set(LED3_PIN);    bleuart.println("LED3 OFF");
      break;
    case '7':
      nrf_gpio_pin_clear(LED4_PIN);  bleuart.println("LED4 ON");
      break;
    case '8':
      nrf_gpio_pin_set(LED4_PIN);    bleuart.println("LED4 OFF");
      break;
    case 'b':
      bleuart.println(
        nrf_gpio_pin_read(BUTTON3_PIN)==0
        ? "BUTTON3 = PRESSED"
        : "BUTTON3 = RELEASED"
      );
      break;
    default:
      bleuart.println("Unknown cmd");
      break;
  }
}

// This is the correct signature for Bluefruit's UART RX callback:
void uart_rx_callback(uint16_t conn_handle) {
  // Pull all available bytes from bleuart into handle_ble_command()
  while (bleuart.available()) {
    char cmd = bleuart.read();
    handle_ble_command(cmd);
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("=== BLE + LED Control Demo ===");

  // Configure LEDs (active LOW) as outputs and switch them OFF
  nrf_gpio_cfg_output(LED1_PIN); nrf_gpio_pin_set(LED1_PIN);
  nrf_gpio_cfg_output(LED2_PIN); nrf_gpio_pin_set(LED2_PIN);
  nrf_gpio_cfg_output(LED3_PIN); nrf_gpio_pin_set(LED3_PIN);
  nrf_gpio_cfg_output(LED4_PIN); nrf_gpio_pin_set(LED4_PIN);

  // Configure button with pull-up
  nrf_gpio_cfg_input(BUTTON3_PIN, NRF_GPIO_PIN_PULLUP);

  // Start BLE
  Bluefruit.begin();
  Bluefruit.setName("nRF52840_DK");
  Bluefruit.autoConnLed(false);

  // Device info
  bledis.setManufacturer("MindTune");
  bledis.setModel("nRF52840 DK");
  bledis.begin();

  // DFU
  bledfu.begin();

  // UART (Nordic UART Service)
  bleuart.begin();
  bleuart.setRxCallback(uart_rx_callback);

  // Advertise all services
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.addService(bledis);
  Bluefruit.Advertising.addService(bledfu);
  Bluefruit.Advertising.addService(bleuart);
  Bluefruit.Advertising.start();

  Serial.println("Advertising Device-Info, DFU & UART services");
}

void loop() {
  // Everything is handled in uart_rx_callback()
  delay(100);
}
