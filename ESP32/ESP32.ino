#define RXD1 7  // D7 on XIAO (GPIO 7)
#define TXD1 6  // D6 on XIAO (GPIO 6)

#include <Arduino.h>
#include "MediboundDeviceBLE.h"

using namespace Medibound;

// Medibound BLE setup
MediboundDeviceBLE* device = nullptr;

// UART read buffer
String uartMessage = "";
int eegValue = 0;
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 1000;

void setup() {
  pinMode(RXD1, INPUT);
  pinMode(TXD1, OUTPUT);

  Serial.begin(115200); // USB debug
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1); // UART from Feather

  delay(1000);
  Serial.println("ESP32-C3 XIAO + Medibound BLE Ready");

  // Initialize Medibound device
  device = new MediboundDeviceBLE(
    "U3r363WxrZhK2teKqmk4",      // API Key
    "YukDh+8gFu2yrvPbaOqiVH746SrFoD9EyovX0BlZaiY=", // Secret Key
    "KLgOUrWKgMUKdKQ8bHqa",      // Device ID
    "Medibound EEG Device",      // Custom BLE name
    DeviceMode::CONTINUOUS
  );

  // Start BLE session
  device->onStart([]() {
    Serial.println("Medibound BLE session started.");
  });
}

void loop() {
  // Read and parse UART input from Feather
  while (Serial1.available()) {
    char c = Serial1.read();

    if (c == '\n') {
      uartMessage.trim(); // remove whitespace
      eegValue = uartMessage.toInt();

      Serial.print("Received EEG Value: ");
      Serial.println(eegValue);

      // Send to Medibound
      unsigned long currentTime = millis();
      if (currentTime - lastUpdate >= updateInterval) {
        lastUpdate = currentTime;

        std::vector<MediboundVariable> data;
        data.push_back(MediboundVariable("eegValue", {float(eegValue)}));

        Serial.println("Sending data to Medibound...");
        device->setData(data);
      }

      uartMessage = ""; // reset buffer
    } else if (c != '\r') {
      uartMessage += c;
    }
  }

  // Keep BLE stack alive
  delay(5);
}
