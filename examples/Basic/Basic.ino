#include <Wire.h>
#include "7Semi_MCP4725.h"

MCP4725_7Semi dac;

int analog_pin = A0;

float voltage = 2.50;

void setup() {
  Serial.begin(115200);

  if (!dac.begin(0x62, Wire, 400000)) {
    Serial.println("7Semi MCP4725 initialization failed!");
    while (1)
      ;
  }

  Serial.println("7Semi MCP4725 initialized successfully.");

  // Configure DAC
  dac.setMode(MCP4725_FAST_MODE);  // Fast mode (no EEPROM write)

  // Set reference voltage
  dac.setVRef(3.3f);  // Match your VCC
}

void loop() {

  dac.setVoltage(voltage);

  Serial.print("Set: ");
  Serial.print(voltage, 3);
  float out_voltage = analogRead(analog_pin) * 3.30f / 1023.00f;
  Serial.print(" | Output: ");

  Serial.println(out_voltage, 3);

  delay(100);
}
