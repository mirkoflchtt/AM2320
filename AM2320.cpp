/**
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Copyright 2016 Ratthanan Nalintasnai
**/
#include <Wire.h>
#include "AM2320.h"

static
uint16_t _crc16(const byte *byte, uint32_t numByte)
{
  uint16_t crc = 0xFFFF;          // 16-bit crc register

  for ( ; numByte>0; numByte--) {
    crc ^= *byte++;                   // exclusive-or crc with first byte

    for (int i = 0; i < 8; i++) {     // perform 8 shifts
      const uint16_t lsb = crc & 0x01;  // extract LSB from crc
      crc >>= 1;                      // shift be one position to the right

      if (lsb == 0) {                 // LSB is 0
        continue;                     // repete the process
      } else {                        // LSB is 1
        crc ^= 0xA001;                // exclusive-or with 1010 0000 0000 0001
      }
    }
  }

  return crc;
}


AM2320::AM2320() {
  // do nothing
}

void  AM2320::begin() {
  Wire.begin();
}

void AM2320::begin(int sda, int scl) {
  Wire.begin(sda, scl);
}

float AM2320::getTemperature() {
  return _temperature;
}

float AM2320::getHumidity() {
  return _humidity;
}

bool AM2320::measure() {
  _errorCode = ERROR_NONE;

  if ( ! _read_registers(0x00, 4)) {
    _errorCode = ERROR_READ_FAILED;
    return false;
  }

  const uint16_t receivedCrc = (((uint16_t)_buf[7]<<8) | _buf[6]);   // pack high and low byte together

  if ( receivedCrc == _crc16(_buf, 6) ) {
    const uint16_t humidity = ((_buf[2] << 8) | _buf[3]);
    _humidity =  humidity * 0.1f;

    const uint16_t temperature = ((_buf[4] & 0x7F) << 8) | _buf[5];
    _temperature  = temperature;
    _temperature *= ( _buf[4] & 0x80 ) ? -0.1f : 0.1f;

    return true;
  }

  _errorCode = ERROR_BAD_CRC;
  return false;
}

int AM2320::getErrorCode() {
  return _errorCode;
}

bool AM2320::_read_registers(int startAddress, int numByte) {
  _wake();                // wake up sensor


  Wire.beginTransmission(AM2320_ADDR);
  Wire.write(0x03);           // function code: 0x03 - read register data
  Wire.write(startAddress);   // begin address
  Wire.write(numByte);        // number of bytes to read

  // send and check result if not success, return error code
  if (Wire.endTransmission(true) != 0) {        
    return false;                           // return sensor not ready code
  }
  delayMicroseconds(1500);                    // as specified in datasheet
  Wire.requestFrom(AM2320_ADDR, numByte + 4); // request bytes from sensor
                                              // see function code description in datasheet    
  for ( int i = 0; i < numByte + 4; i++) {    // read
    _buf[i] = Wire.read();
  }

  return true;
}

void AM2320::_wake() {
  Wire.beginTransmission(AM2320_ADDR);
  Wire.endTransmission();
}
