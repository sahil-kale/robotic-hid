#ifndef HAL_APPLICATION_H
#define HAL_APPLICATION_H
#include <stdint.h>
#include <stdbool.h>

extern const uint8_t num_buttons;
extern const uint8_t button_bitmask;

typedef struct {
    uint8_t manipulating_button_state;
    bool lcd_button_left;
    bool lcd_button_right;
    bool lcd_button_enter;

} hal_application_button_state_t;

void send_usb_hid_report(uint8_t *report_data, uint8_t report_size);

hal_application_button_state_t update_button_states(void);

#endif