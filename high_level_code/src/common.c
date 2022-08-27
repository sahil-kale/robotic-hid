#include "common.h"

const uint32_t APP_START_ADDRESS = 0x0800C000;
const uint8_t DFU_SOF_identifier = 0x01;
const uint8_t APP_SECTOR_START = 12;
const uint8_t APP_SECTOR_END = 31;

const uint8_t APP_INFO_SECTOR = 11;

const uint32_t SECTOR_SIZE = 4096;
const uint32_t PAGE_SIZE = 256;
