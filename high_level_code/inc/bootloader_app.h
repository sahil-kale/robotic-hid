#ifndef BOOTLOADER_APP_H
#define BOOTLOADER_APP_H
#include "stdint.h"

typedef enum bootloader_state
{
    BOOTLOADER_STATE_RESET,
    BOOTLOADER_STATE_LAUNCH_APP,
    BOOTLOADER_STATE_DFU,
    BOOTLOADER_STATE_HALT,
} bootloader_state_E;

typedef enum halt_condition_causes
{
    BOOTLOADER_ERR_DFU_ERROR,
    BOOTLOADER_ERR_INVALID_FLASH,
} halt_condition_causes_E;

extern bootloader_state_E bootloader_current_state;


/**
 * @brief Determines the appropriate bootloader state from the reset vector
 * 
 * @return bootloader_state_E 
 */
bootloader_state_E determine_bootloader_state_from_reset_vector(void);

/**
 * @brief Runs the bootloader application. Call in own while loop.
 * 
 */
void bootloader_app_run(void);


#endif // BOOTLOADER_APP_H
