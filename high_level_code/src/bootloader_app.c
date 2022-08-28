#include "bootloader_app.h"
#include "hal_bootloader_app.h"
#include "hal_dfu.h"
#include "lcd.h"
#include "dfu.h"
#include <stdio.h>
#include "common.h"

//#define RESET_ON_HALT

static void display_halt_condition_message(halt_condition_causes_E cause, uint16_t err_code);

bootloader_state_E bootloader_current_state = BOOTLOADER_STATE_RESET;

void bootloader_app_init(void)
{
    
}

bootloader_state_E determine_bootloader_state_from_reset_vector(void)
{
    bootloader_state_E state = BOOTLOADER_STATE_RESET;

    application_info_flash_t info = read_application_info();
    if(info.dfu_request)
    {
        state = BOOTLOADER_STATE_DFU;
    }
    else
    {
        state = BOOTLOADER_STATE_LAUNCH_APP;
    }

    return state;
}

void bootloader_app_run(void)
{
    static bool halt_info_processed = false;
    static halt_condition_causes_E cause = 0;
    static uint16_t err_code = 0;
    switch(bootloader_current_state)
    {
        case BOOTLOADER_STATE_RESET:;
            bootloader_current_state = determine_bootloader_state_from_reset_vector();
            break;
        case BOOTLOADER_STATE_LAUNCH_APP:;
            application_info_flash_t info = read_application_info();
            if(info.flash_valid && hal_dfu_validate_crc(APP_START_ADDRESS, info.application_size, info.application_crc))
            {
                hal_jump_to_app();
            }
            else
            {
                cause = BOOTLOADER_ERR_INVALID_FLASH;
                bootloader_current_state = BOOTLOADER_STATE_HALT;
            }
            break;
        case BOOTLOADER_STATE_DFU:;
            dfu_init();

            clear_lcd();
            set_lcd_cursor(0, 0);
            char starting_dfu_msg[] = "Starting DFU";
            set_lcd_cursor(0, 0);
            write_lcd(starting_dfu_msg, sizeof(starting_dfu_msg));

            DFU_STATUS_E dfu_curr_status = DFU_STATUS_OK;
            do
            {
                dfu_curr_status = dfu_run();
                if(dfu_state.state == DFU_STATE_COMPLETE || dfu_state.state == DFU_STATE_ERROR)
                {
                	break;
                }
            } while((bootloader_current_state == BOOTLOADER_STATE_DFU) && (dfu_curr_status == DFU_STATUS_OK));
            if(dfu_curr_status != DFU_STATUS_OK)
            {
                cause = BOOTLOADER_ERR_DFU_ERROR;
                err_code = dfu_curr_status;
                bootloader_current_state = BOOTLOADER_STATE_HALT;
            }
            else if(dfu_curr_status == DFU_STATUS_OK)
            {
                hal_reset();
            }
            break;
        case BOOTLOADER_STATE_HALT:;
            if(!halt_info_processed)
            {
                halt_info_processed = true;
                application_info_flash_t halt_app_info = {0};
                halt_app_info.dfu_request = true;
                halt_app_info.flash_valid = false;
                write_application_info(&halt_app_info);
                display_halt_condition_message(cause, err_code);
            }
            else
            {
                #ifdef RESET_ON_HALT
                hal_reset();
                #else
                while(true)
                {
                    //infinite loop
                }
                #endif
            }
            
            break;
        default:
            bootloader_current_state = BOOTLOADER_STATE_HALT;
            break;
    }
}

static void display_halt_condition_message(halt_condition_causes_E cause, uint16_t err_code)
{
    char msg[20];
    switch(cause)
    {
        case BOOTLOADER_ERR_DFU_ERROR:
            sprintf(msg, "DFU error: %d", err_code);
            break;
        case BOOTLOADER_ERR_INVALID_FLASH:
            sprintf(msg, "Invalid flash");
            break;
        default:
            sprintf(msg, "Unknown error");
            break;
    }
    clear_lcd();
    set_lcd_cursor(0, 0);
    write_lcd(msg, sizeof(msg));

}
