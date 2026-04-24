#include <7semi_MCP4725.h>

// Create MCP4725 DAC object using default I2C port (Wire)
MCP4725_7semi dac;

void setup() {
  Serial.begin(115200);  // Initialize serial monitor for debugging
  Wire.begin();          // Initialize I2C bus

  // Begin communication with MCP4725 at address 0x62
  if (dac.begin(0x62)) {
    Serial.println("7semi MCP4725 initialized successfully.");
  } else {
    Serial.println("7semi MCP4725 initialization failed!");
    while (1);  // Halt execution if device is not found
  }

  // Set I2C speed to 400kHz for faster data rate (optional)
  dac.setSpeed(400000);

  // Optional: Set to FAST_MODE or WRITE_DAC_EEPROM
  dac.setMode(FAST_MODE);
}

void loop() {
  uint16_t value = 4000;  // Set DAC output value (0–4095)
  dac.setAnalogOut(value);
  Serial.print("DAC Output: ");
  Serial.println(value * 5.00 / 4095.0, 3);  // Scale to voltage assuming 5V VCC
  delay(1000);  // Wait 1 second before repeating
}
