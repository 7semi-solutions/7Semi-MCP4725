#include "7Semi_MCP4725.h"

MCP4725_7Semi::MCP4725_7Semi(){
  write_mode = MCP4725_FAST_MODE;
  reference_voltage = 5.00f;
}

bool MCP4725_7Semi::begin(uint8_t i2cAddress, TwoWire &i2cPort, uint32_t i2cSpeed)
{
  i2c_address = i2cAddress;
  i2c_speed = i2cSpeed;

  i2c = &i2cPort;

  if (!i2c)
    return false;

  // Initialize I2C bus
  i2c->begin();

  // Check device presence
  i2c->beginTransmission(i2c_address);
  return (i2c->endTransmission() == 0);
}

bool MCP4725_7Semi::setI2CSpeed(uint32_t speed)
{
  i2c_speed = speed;
  return true;
}

bool MCP4725_7Semi::setDACRaw(uint16_t voltage)
{

  // Clamp input to valid 12-bit range
  if (voltage > 4095)
    voltage = 4095;

  /**
   * Prepare data data
   *
   * data[0] → command / mode
   * data[1] → D11–D4
   * data[2] → D3–D0 
   */
  uint8_t data[3];
  data[0] = write_mode;
  data[1] = voltage >> 4;
  data[2] = (voltage & 0x0F) << 4;

  return writeReg(data, 3);
}

bool MCP4725_7Semi::setVoltage(float voltage)
{
  // Validate reference voltage
  if (reference_voltage <= 0.0f)
    return false;

  // Clamp input voltage to valid range
  if (voltage < 0.0f)
    voltage = 0.0f;

  if (voltage > reference_voltage)
    voltage = reference_voltage;

  /**
   * Convert voltage to 12-bit DAC value
   *
   * - 0V        → 0
   * - Vref      → 4095
   */
  uint16_t value = (uint16_t)((voltage / reference_voltage) * 4095.0f);

  // Write to DAC
  return setDACRaw(value);
}


bool MCP4725_7Semi::setVRef(float vRef)
{

  // Validate input
  if (vRef <= 0.0f)
    return false;

  reference_voltage = vRef;

  return true;
}

bool MCP4725_7Semi::getVoltage(float &voltage)
{

  // Validate reference voltage
  if (reference_voltage <= 0.0f)
    return false;

  // Read DAC value
  uint16_t value;

  if (!readDAC(value))
    return false;

  /**
   * Convert to voltage
   *
   * Vout = (DAC / 4095) * Vref
   */
  voltage = ((float)value / 4095.0f) * reference_voltage;

  return true;
}

bool MCP4725_7Semi::readDAC(uint16_t &value)
{

  uint8_t buffer[5];

  if (!readReg(buffer, 5))
    return false;

  /**
   * MCP4725 format:
   * buffer[0] → status
   * buffer[1] → DAC high (D11–D4)
   * buffer[2] → DAC low  (D3–D0 << 4)
   */

  value = ((buffer[1] << 4) | (buffer[2] >> 4));

  return true;
}

bool MCP4725_7Semi::readEEPROM(uint16_t &value)
{

  // Buffer for MCP4725 response
  uint8_t buffer[5];

  if (!readReg(buffer, 5))
    return false;

  /**
   * MCP4725 format:
   * buffer[3] → EEPROM high (D11–D4)
   * buffer[4] → EEPROM low  (D3–D0 << 4)
   */
  value = ((buffer[3] << 4) | (buffer[4] >> 4));

  return true;
}

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
bool MCP4725_7Semi::setMode(MCP4725_Mode mode)
{

  if (mode == MCP4725_FAST_MODE || mode == MCP4725_WRITE_DAC_EEPROM)
  {
    write_mode = mode;
    return true;
  }

  return false;
}

/**
 * Check EEPROM write status
 *
 * - Uses MCP4725 status byte (RDY bit)
 * - More accurate than ACK polling
 *
 * working:
 * - Reads 5-byte response
 * - Checks RDY bit (bit 7)
 *
 * returns:
 * - true  → EEPROM busy
 * - false → ready
 */
bool MCP4725_7Semi::isEEPROMStatus(bool &status)
{
  uint8_t data[5];

  // Read device status
  if (!readReg(data, 5))
  {
    return false;
  }

  /**
   * RDY bit:
   * - 1 → ready
   * - 0 → busy
   */
  status = (data[0] & 0x80);

  return true;
}

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
bool MCP4725_7Semi::setPowerDown(uint8_t mode)
{

  // Validate mode
  if (mode > 3)
    return false;

  /**
   * Prepare command buffer
   *
   * buffer[0] → command + power bits
   * buffer[1] → data high (0)
   * buffer[2] → data low  (0)
   */
  uint8_t buffer[3];
  buffer[0] = MCP4725_FAST_MODE | (mode << 1); // Use fast write format
  buffer[1] = 0x00;
  buffer[2] = 0x00;

  // Send command
  return writeReg(buffer, 3);
}

uint8_t MCP4725_7Semi::getMode()
{
  return write_mode;
}

bool MCP4725_7Semi::writeReg(uint8_t *data, uint8_t len)
{
  if (!i2c)
    return false;

  i2c->beginTransmission(i2c_address);

  for (uint8_t i = 0; i < len; i++)
  {
    i2c->write(data[i]);
  }

  return i2c->endTransmission();
}

bool MCP4725_7Semi::readReg(uint8_t *data, uint8_t len)
{

  if (!i2c)
    return false;

  i2c->requestFrom(i2c_address, len);

  if (i2c->available() < len)
    return false;

  for (uint8_t i = 0; i < len; i++)
  {
    data[i] = i2c->read();
  }

  return true;
}
