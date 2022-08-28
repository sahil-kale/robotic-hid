#ifndef COMMON_H
#define COMMON_H
#include "stdint.h"
#include "utility.h"

typedef struct PACKED application_info_flash
{
    bool flash_valid;
    bool dfu_request;
    uint32_t application_size;
    uint32_t application_crc;
} application_info_flash_t;

extern const uint32_t APP_START_ADDRESS;
extern const uint32_t BOOTLOADER_START_ADDRESS;
extern const uint8_t DFU_SOF_identifier;
extern const uint8_t APP_SECTOR_START;
extern const uint8_t APP_SECTOR_END;
extern const uint8_t APP_INFO_SECTOR;

extern const uint8_t app_version_number_major;
extern const uint8_t app_version_number_minor;

extern const uint32_t SECTOR_SIZE;
extern const uint32_t PAGE_SIZE;

#endif // COMMON_H
