
#ifndef _CRC_H
#define _CRC_H

#define CRC_INVERT8(x)(x ^ 0xFF)
#define CRC_INVERT16(x)(x ^ 0xFFFF)
#define CRC_INVERT32(x)(x ^ 0xFFFFFFFF)
#define CRC_INVERT64(x)(x ^ 0xFFFFFFFFFFFFFFFF)

uint32_t* crc32_init_table();
uint32_t crc32(uint8_t* buffer, uint32_t length);
uint32_t crc32_append(uint8_t* buffer, uint32_t length, uint32_t initstate);

#endif /*_CRC_H*/
