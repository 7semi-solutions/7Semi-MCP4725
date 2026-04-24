# 7Semi MCP4725 Arduino Library

Arduino driver for the Microchip MCP4725 12-bit DAC.

The MCP4725 provides a high-resolution analog voltage output using a digital I²C interface, making it ideal for signal generation, calibration, and analog control applications.

## Features

- 12-bit DAC output (0–4095)

- Fast mode

- EEPROM write mode

- Power-down modes support

---

## Connections / Wiring

The MCP4725 communicates using **I²C interface**.

## I²C Connection

| MCP4725 Pin | MCU Pin   | Notes          |
| ----------- | --------- | -------------- |
| VCC         | 3.3V / 5V | Supply voltage |
| GND         | GND       | Common ground  |
| SDA         | SDA       | I²C data       |
| SCL         | SCL       | I²C clock      |
| A0          | GND / VCC | Address select |

---

### I²C Notes

- Default I²C address: `0x62`

- Supported bus speeds:
  - 100 kHz
  - 400 kHz (recommended)

---

## Installation

- Arduino Library Manager

  1. Open Arduino IDE  
  2. Go to Library Manager  
  3. Search for **7Semi MCP4725**  
  4. Click Install  

- Manual Installation

  1. Download this repository as ZIP  
  2. Arduino IDE → Sketch → Include Library → Add .ZIP Library  

---

## Library Overview

### Setting Voltage

```cpp
dac.setVoltage(2.5);
```

- Sets DAC output in volts

### Setting Raw DAC Value

```cpp
dac.setAnalogOut(2048);
```

- Range: 0 → 4095

### Getting Output Voltage

```cpp
float voltage;

dac.getVoltage(voltage);
```

- Returns current output voltage

### Setting Reference Voltage

```cpp
dac.setVRef(3.3);
```

- Must match actual supply voltage for accuracy

### DAC Mode Selection

```cpp
uint64_t serial;

dac.setMode(MCP4725_FAST_MODE);
```

- MCP4725_FAST_MODE → Fast update (volatile)
- MCP4725_WRITE_DAC_EEPROM → Persistent output

### EEPROM Operations

```cpp
uint16_t value;

dac.readEEPROM(value);
```

### Reads stored DAC value

```cpp
uint16_t value;

dac.readEEPROM(value);
```

### Power-down Modes

```cpp
dac.setPowerDown(1);
```

- 0 → Normal operation
- 1 → 1kΩ pull-down
- 2 → 100kΩ pull-down
- 3 → 500kΩ pull-down
