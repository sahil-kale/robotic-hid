#ifndef HAL_DFU_H
#define HAL_DFU_H
#include "stdint.h"

typedef struct
{
    size_t size;
    uint8_t *data;
} DFU_data_handle_t;

/**
 * @brief Initialize the DFU module.
 * 
 * @return DFU_STATUS_E 
 */
DFU_STATUS_E hal_dfu_init(void);

/**
 * @brief Get the data from dfu host via transport layer
 * 
 * @return DFU_data_handle_t 
 */
DFU_data_handle_t get_data_from_dfu_host(void);

/**
 * @brief Send data to the DFU host via transport layer
 * 
 * @param data DFU data handle to send
 * @return DFU_STATUS_E 
 */
DFU_STATUS_E send_data_to_dfu_host(DFU_data_handle_t data);

/**
 * @brief Write data to flash
 * 
 * @param address Address to write to
 * @param size Size of chunk to write
 * @param data pointer to the data to write
 * @return DFU_STATUS_E 
 */
DFU_STATUS_E hal_dfu_writeflash(uint32_t address, uint32_t size, const uint8_t *data);

/**
 * @brief Erase application memory flash
 * 
 * @param baseSector base sector to start erasing from
 * @param noSectors Number of sectors to erase
 * @return DFU_STATE_E 
 */
DFU_STATE_E hal_dfu_eraseflash(uint32_t baseSector, uint32_t noSectors);

bool hal_dfu_validate_crc(uint32_t address, uint32_t size, uint32_t crc);

#endif // HAL_DFU_H
