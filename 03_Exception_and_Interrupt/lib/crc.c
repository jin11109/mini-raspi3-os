# include "crc.h"
// CRC32 parameters (IEEE 802.3)
#define CRC32_POLY 0xEDB88320

unsigned int crc32_update(unsigned int crc, unsigned char data) {
    crc ^= data;
    for (int i = 0; i < 8; i++) {
        if (crc & 1)
            crc = (crc >> 1) ^ CRC32_POLY;
        else
            crc >>= 1;
    }
    return crc;
}

unsigned int crc32_calculate(const unsigned char *data, unsigned int length) {
    unsigned int crc = 0xFFFFFFFF;
    for (unsigned int i = 0; i < length; i++) {
        crc = crc32_update(crc, data[i]);
    }
    return ~crc;
}