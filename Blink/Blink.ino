#include <bluefruit.h>

// Active LOW LED pins on nRF52840 DK
#define LED1_PIN 13  // P0.13 (used by Bluefruit status LED by default)
#define LED2_PIN 14  // P0.14
#define LED3_PIN 15  // P0.15
#define LED4_PIN 16  // P0.16

#define BUTTON3_PIN 24  // P0.24 - Button SW3

// BLE Services
BLEDis bledis;
BLEDfu bledfu;
BLEUart bleuart;

void handle_ble_command(char cmd) {
  switch (cmd) {
    case '1':
      digitalWrite(LED1_PIN, LOW);  Serial.println("LED1 ON"); break;
    case '2':
      digitalWrite(LED1_PIN, HIGH); Serial.println("LED1 OFF"); break;
    case '3':
      digitalWrite(LED2_PIN, LOW);  Serial.println("LED2 ON"); break;
    case '4':
      digitalWrite(LED2_PIN, HIGH); Serial.println("LED2 OFF"); break;
    case '5':
      digitalWrite(LED3_PIN, LOW);  Serial.println("LED3 ON"); break;
    case '6':
      digitalWrite(LED3_PIN, HIGH); Serial.println("LED3 OFF"); break;
    case '7':
      digitalWrite(LED4_PIN, LOW);  Serial.println("LED4 ON"); break;
    case '8':
      digitalWrite(LED4_PIN, HIGH); Serial.println("LED4 OFF"); break;
    case 'b':
      bleuart.println(digitalRead(BUTTON3_PIN) == LOW ? "BUTTON3 = PRESSED" : "BUTTON3 = RELEASED");
      break;
    default:
      bleuart.println("Unknown command");
      break;
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("BLE LED + DFU + BUTTON demo");

  // Start BLE stack first
  Bluefruit.begin();
  Bluefruit.autoConnLed(false); // Disable default Bluefruit LED usage on P0.13

  // Reclaim LED1_PIN from Bluefruit and configure all LEDs (active LOW = LOW = ON)
  pinMode(LED1_PIN, OUTPUT); digitalWrite(LED1_PIN, HIGH);
  pinMode(LED2_PIN, OUTPUT); digitalWrite(LED2_PIN, HIGH);
  pinMode(LED3_PIN, OUTPUT); digitalWrite(LED3_PIN, HIGH);
  pinMode(LED4_PIN, OUTPUT); digitalWrite(LED4_PIN, HIGH);

  // Setup BUTTON3 input with pull-up
  pinMode(BUTTON3_PIN, INPUT_PULLUP);

  // BLE configuration
  Bluefruit.setName("nRF52840_DK");
  Bluefruit.setTxPower(4); // Max transmit power

  // BLE services
  bledis.setManufacturer("MindTune");
  bledis.setModel("nRF52840 DK");
  bledis.begin();

  bledfu.begin();    // Enable OTA DFU
  bleuart.begin();   // BLE UART for command reception

  // Start advertising
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.start();

  Serial.println("Ready! Connect via nRF Connect and send 1-8 or 'b'");
}

void loop() {
  if (bleuart.available()) {
    char cmd = bleuart.read();
    handle_ble_command(cmd);
  }
}
