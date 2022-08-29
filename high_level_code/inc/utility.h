#ifndef UTILITY_H
#define UTILITY_H
#include "stdint.h"
#include "stdbool.h"

#define PACKED __attribute__((packed))

//Functions to byte swap from le to be and vice versa

/**
 * @brief Swap bytes in a 16 bit integer
 * 
 * @param val 
 * @return uint16_t 
 */
uint16_t swap_uint16(uint16_t val);

/**
 * @brief Swap bytes in a 32 bit integer
 * 
 * @param val 
 * @return uint32_t 
 */
uint32_t swap_uint32(uint32_t val);

/**
 * @brief Swap bytes in a 64 bit integer
 * 
 * @param val 
 * @return uint64_t 
 */
uint64_t swap_uint64(uint64_t val);

#endif // UTILITY_H