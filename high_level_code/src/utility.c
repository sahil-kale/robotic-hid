#include "utility.h"

uint16_t swap_uint16(uint16_t val)
{
    return (val << 8) | (val >> 8);
}

uint32_t swap_uint32(uint32_t val)
{
    return (val << 24) | ((val << 8) & 0x00FF0000) | ((val >> 8) & 0x0000FF00) | (val >> 24);
}

uint64_t swap_uint64(uint64_t val)
{
    return ((uint64_t)swap_uint32(val) << 32) | swap_uint32(val >> 32);
}