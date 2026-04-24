#ifndef _7SEMI_MCP4725_H_
#define _7SEMI_MCP4725_H_

#include <Wire.h>
#include <Arduino.h>

/**
 * MCP4725 Command Modes
 *
 * - FAST_MODE
 *   - Writes DAC register only
 *   - No EEPROM write
 *   - Faster operation
 *
 * - WRITE_DAC_EEPROM
 *   - Writes DAC + EEPROM
 *   - Retains value after power cycle
 *   - Slower due to EEPROM write time
 */
enum MCP4725_Mode
{
  MCP4725_FAST_MODE = 0x40,
  MCP4725_WRITE_DAC_EEPROM = 0x60
};

class MCP4725_7Semi
{

public:
  MCP4725_7Semi();

  /**
   * Initialize MCP4725 device
   *
   * i2cAddress:
   * - Device I2C address
   * - 0x60 → A0 = GND
   * - 0x61 → A0 = VCC
   *
   * i2cPort:
   * - I2C interface (Wire, Wire1, etc.)
   *
   * i2cSpeed:
   * - Communication speed in Hz
   * - Typical:
   *   - 100000  → standard mode
   *   - 400000  → fast mode
   *
   * returns:
   * - true  → device detected
   * - false → no response / invalid state
   */
  bool begin(uint8_t i2cAddress = 0x60, TwoWire &i2cPort = Wire, uint32_t i2cSpeed = 400000);

  /**
   * Set I2C clock speed
   *
   * speed:
   * - 100000  → standard mode
   * - 400000  → fast mode (recommended)
   */
  bool setI2CSpeed(uint32_t speed);

  /**
   * Set DAC output value (12-bit)
   *
   * voltage:
   * - Range: 0 → 4095
   * - 0     → 0V output
   * - 4095  → reference voltage output
   *
   * returns:
   * - true  → write successful
   * - false → I2C error
   */
  bool setDACRaw(uint16_t voltage);

  /**
   * Set DAC output using voltage
   *
   * voltage:
   * - Input voltage in volts
   * - Range: 0 → reference_voltage
   *
   * working:
   * - Validates reference voltage
   * - Clamps input within safe range
   * - Converts voltage → 12-bit DAC value
   * - Sends value to DAC using raw write
   *
   * conversion:
   * - DAC = (Vout / Vref) × 4095
   *
   * example:
   * - Vref = 5.0V
   * - setVoltage(2.5) → ~2048 (mid-scale)
   *
   * returns:
   * - true  → write successful
   * - false → invalid reference voltage or I2C error
   */
  bool setVoltage(float voltage);

  /**
   * Set reference voltage for DAC
   *
   * vRef:
   * - Must be > 0
   * - Typical values:
   *   - 5.0  → Arduino UNO
   *   - 3.3  → ESP32 / STM32
   *
   * returns:
   * - true  → valid value set
   * - false → invalid input
   */
  bool setVRef(float vRef);

  /**
   * Get current output voltage
   *
   * voltage:
   * - Output parameter (in volts)
   * - Reads DAC register
   * - Converts 12-bit value → voltage using reference
   *
   * returns:
   * - true  → success
   * - false → read error or invalid reference
   */
  bool getVoltage(float &voltage);

  /**
   * Read current DAC value
   *
   * - Reads live DAC register (not EEPROM)
   * - Extracts DAC register value
   *
   * returns:
   * - true  → read successful
   * - false → I2C error
   */
  bool readDAC(uint16_t &value);

  /**
   * Read EEPROM stored value
   *
   * - Reads persistent DAC value from EEPROM
   * - Extracts EEPROM register value
   *
   * returns:
   * - true  → read successful
   * - false → I2C error
   */
  bool readEEPROM(uint16_t &value);

  /**
   * Check EEPROM write status
   *
   * - Uses MCP4725 status byte (RDY bit)
   * - More accurate than ACK polling
   *
   * returns:
   * - true  → ready
   * - false → EEPROM busy
   */
  bool isEEPROMStatus(bool &status);

  /**
   * Set power-down mode
   *
   * modes:
   * - 0 → normal operation
   * - 1 → 1k pull-down
   * - 2 → 100k pull-down
   * - 3 → 500k pull-down
   *
   * returns:
   * - true  → success
   * - false → invalid mode or I2C error
   */
  bool setPowerDown(uint8_t mode);

  /**
   * Set DAC write mode
   *
   * modes:
   * - FAST_MODE
   *   - Updates DAC register only
   *   - Faster operation
   *   - Does NOT save after power cycle
   *
   * - WRITE_DAC_EEPROM
   *   - Writes to DAC + EEPROM
   *   - Retains value after power loss
   *   - Slower due to EEPROM write delay
   *
   * returns:
   * - true  → mode accepted
   * - false → invalid mode
   */
  bool setMode(MCP4725_Mode mode);

  /**
   * Get current DAC write mode
   *
   * returns:
   * - MCP4725_FAST_MODE
   *   → DAC register only (fast, volatile)
   *
   * - MCP4725_WRITE_DAC_EEPROM
   *   → DAC + EEPROM (persistent, slower)
   */
  uint8_t getMode();

private:
 
  TwoWire *i2c;
  uint8_t i2c_address = 0x60;
  uint32_t i2c_speed = 400000;

  uint8_t write_mode = MCP4725_FAST_MODE;

  float reference_voltage = 5.0f;

  bool writeReg(uint8_t *data, uint8_t len);
  bool readReg(uint8_t *data, uint8_t len);

};

#endif