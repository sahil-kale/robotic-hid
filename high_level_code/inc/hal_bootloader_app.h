#ifndef HAL_BOOTLOADER_APP_H
#define HAL_BOOTLOADER_APP_H
#include <stdint.h>
#include "utility.h"

typedef struct PACKED application_info_flash
{
    bool flash_valid;
    bool dfu_request;
    uint32_t application_size;
    uint32_t application_crc;
} application_info_flash_t;

/**
 * @brief Jump to application
 * 
 */
void hal_jump_to_app(void);

/**
 * @brief Reset the system by jumping to bootloader reset handler
 * 
 */
void hal_reset(void);

/**
 * @brief Read the application info from flash
 * 
 * @param info Pointer to the application info structure to fill in
 */
void write_application_info(application_info_flash_t* info);

/**
 * @brief Write the application info to flash
 * 
 * @return application_info_flash_t 
 */
application_info_flash_t read_application_info(void);

#endif // HAL_BOOTLOADER_APP_H
