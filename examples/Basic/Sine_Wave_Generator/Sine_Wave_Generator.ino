/***************************************************************************
  7Semi MCP4725 DAC - Sine Wave Example
  ----------------------------------------
  Generates analog sine wave using MCP4725 DAC

  Library:
    - 7semi_MCP4725

  Wiring:
    - VCC → 3.3V / 5V
    - GND → GND
    - SDA → SDA
    - SCL → SCL
    - A0  → GND (0x60) / VCC (0x61)

  Notes:
    - Vout = (DAC / 4095) × Vref
    - FAST_MODE recommended for waveform generation
***************************************************************************/

#include <7Semi_MCP4725.h>

MCP4725_7Semi dac;

const int samples = 100;
float angle = 0.0f;

void setup() {
  Serial.begin(115200);

  // Initialize DAC
  if (!dac.begin(0x62, Wire, 400000)) {
    Serial.println("MCP4725 not detected!");
    while (1)
      ;
  }

  // Configure DAC
  dac.setMode(MCP4725_FAST_MODE);
  dac.setVRef(3.3f);
}

// Generate sine wave
void loop() {
  for (int i = 0; i < samples; i++) {
    // Generate sine value (-1 → +1)
    float sine = sin(angle);

    // Convert to 0 → Vref
    float voltage = (sine + 1.0f) * (3.3f / 2.0f);

    // Output to DAC
    dac.setVoltage(voltage);

    // Plot SIne wave on plotter
    float vout = analogRead(A0) * 3.3f / 1023.0f;

    Serial.println(vout, 3);

    // Update angle
    angle += (2.0f * PI / samples);

    delayMicroseconds(100);
  }
  //  Reset angle after full cycle
  angle = 0.0f;
}