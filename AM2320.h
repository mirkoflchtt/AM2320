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

#ifndef AM2303_H
#define AM2303_H

#include <Arduino.h>
#include <stdint.h>

// address of AM2320
#define AM2320_ADDR 0x5C

// maximum number of bytes that can be read consequtively before
// sensor splits out error
#define MAX_BYTES_READ 10

enum {
  ERROR_NONE          = 0x0,
  ERROR_READ_FAILED   = 0x1,
  ERROR_BAD_CRC       = 0x2,
};

class AM2320 {
    public:
        AM2320();

        // initialize AM2320
        void begin();
        void begin(int sda, int scl);

        bool measure();

        float getTemperature();
        float getHumidity();

        int getErrorCode();
        
    private:
        byte _buf[ MAX_BYTES_READ ];
        float _temperature;
        float _humidity;
        int _errorCode;        
        void _wake();
        bool _read_registers(int startAddress, int numByte);

};

#endif    /*AM2303_H*/
