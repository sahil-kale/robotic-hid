#include "common.h"

const uint32_t APP_START_ADDRESS = 0x0800B000;
const uint8_t DFU_SOF_identifier = 0x01;
const uint8_t APP_SECTOR_START = 11;
const uint8_t APP_SECTOR_END = 31;

const uint8_t APP_INFO_SECTOR = 10;

const uint32_t SECTOR_SIZE = 4096;
const uint32_t PAGE_SIZE = 256;
