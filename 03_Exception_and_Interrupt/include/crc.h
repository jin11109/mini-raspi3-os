#ifndef _INCLUDE_CRC_H
#define _INCLUDE_CRC_H

unsigned int crc32_update(unsigned int crc, unsigned char data);
unsigned int crc32_calculate(const unsigned char *data, unsigned int length);

#endif /* _INCLUDE_CRC_H */
