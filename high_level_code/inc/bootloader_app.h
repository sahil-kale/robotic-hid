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

bootloader_state_E determine_bootloader_state_from_reset_vector(void);

void bootloader_app_run(void);


#endif // BOOTLOADER_APP_H
