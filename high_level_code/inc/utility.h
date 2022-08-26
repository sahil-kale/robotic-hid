#ifndef UTILITY_H
#define UTILITY_H
#include "stdint.h"
#include "stdbool.h"

#define PACKED __attribute__((packed))

//Functions to byte swap from le to be and vice versa
uint16_t swap_uint16(uint16_t val);
uint32_t swap_uint32(uint32_t val);
uint64_t swap_uint64(uint64_t val);

#endif // UTILITY_H