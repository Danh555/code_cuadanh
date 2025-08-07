

#include "PCF8574.h"

/*!
 *    @brief  Instantiates a new PCF8574 class
 *    @brief  Sets up the hardware and initializes I2C
 *    @param  i2c_address
 *            The I2C address to be used.
 *    @param  wire
 *            The Wire object to be used for I2C connections.
 */
PCF8574::PCF8574(uint8_t i2c_address, TwoWire *wire)
{

  if (i2c_dev != nullptr)
  {
    delete i2c_dev; // Giải phóng bộ nhớ cho đối tượng cũ
  }
  i2c_dev = new Adafruit_I2CDevice(i2c_address, wire);
}

/*!

 *    @return True if initialization was successful, otherwise false.
 */
bool PCF8574::begin()
{
  if (!i2c_dev)
  {
    return false; // Không thể khởi tạo nếu i2c_dev chưa được cấp phát
  }
  if (!i2c_dev->begin())
  {
    return false;
  }

  return true;
}

/*!
 *    @brief  Write one 'byte' of data directly to the GPIO control register
 *    @param  d The data to write
 *    @return True if we were able to write the data successfully over I2C
 */
bool PCF8574::digitalWriteByte(uint8_t d)
{
  _writebuf = d;
  return i2c_dev->write(&_writebuf, 1);
}

/*!
 *    @brief  Read one 'byte' of data directly from the GPIO control register
 *    @return The byte of data read from the device
 */
uint8_t PCF8574::digitalReadByte(void)
{
  i2c_dev->read(&_readbuf, 1);
  return _readbuf;
}

/*!
 *    @brief  Set one GPIO expander pin to 'high' (weak pullup) or 'low'
 * (grounded)
 *    @param  pinnum The GPIO pin number, from 0 to 7 inclusive
 *    @param  val The boolean value to write: true means activate the pullup
 *    and false means turn on the sinking transistor.
 *    @return True if we were able to write the data successfully over I2C
 */
bool PCF8574::digitalWrite(uint8_t pinnum, bool val)
{
  if (val)
  {
    _writebuf |= 1 << pinnum;
  }
  else
  {
    _writebuf &= ~(1 << pinnum);
  }
  return i2c_dev->write(&_writebuf, 1);
}

/*!
 *    @brief  Set one GPIO expander pin to 'output' (grounded) or 'input' (weak
 * pullup)
 *    @param  pinnum The GPIO pin number, from 0 to 7 inclusive
 *    @param  val The value to write: INPUT or INPUT_PULLUP means activate the
 * pullup and OUTPUT means turn on the sinking transistor, as this is an open
 * drain device
 *    @return True if we were able to write the data successfully over I2C
 */
bool PCF8574::pinMode(uint8_t pinnum, uint8_t val)
{
  if ((val == INPUT) || (val == INPUT_PULLUP))
  {
    _writebuf |= 1 << pinnum;
  }
  else
  {
    _writebuf &= ~(1 << pinnum);
  }
  return i2c_dev->write(&_writebuf, 1);
}

/*!
 *    @brief  Get a GPIO expander pin value
 *    @param  pinnum The GPIO pin number, from 0 to 7 inclusive
 *    @return True if the pin logic is NOT ground, false if the pin logic is
 * ground
 */
bool PCF8574::digitalRead(uint8_t pinnum)
{
  i2c_dev->read(&_readbuf, 1);
  return (_readbuf >> pinnum) & 0x1;
}
