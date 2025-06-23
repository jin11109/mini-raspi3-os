#ifndef _CRC_H
#define _CRC_H

unsigned int crc32_update(unsigned int crc, unsigned char data);
unsigned int crc32_calculate(const unsigned char *data, unsigned int length);

#endif /*_CRC_H */