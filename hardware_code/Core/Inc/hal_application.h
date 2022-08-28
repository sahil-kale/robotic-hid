#ifndef HAL_APPLICATION_H
#define HAL_APPLICATION_H
#include <stdint.h>
#include <stdbool.h>
#include "common.h"

extern const uint8_t num_buttons;
extern const uint8_t button_bitmask;

typedef struct {
    uint8_t manipulating_button_state;
    bool lcd_button_left;
    bool lcd_button_right;
    bool lcd_button_enter;

} hal_application_button_state_t;

void send_usb_hid_report(uint8_t *report_data, uint8_t report_size);

/**
 * @brief Report the actual button states
 * 
 * @return hal_application_button_state_t 
 */
hal_application_button_state_t update_button_states(void);

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

/**
 * @brief Reset the chip
 * 
 */
void hal_reset(void);

#endif