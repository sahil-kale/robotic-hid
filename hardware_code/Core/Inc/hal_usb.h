#ifndef HAL_USB_H
#define HAL_USB_H
#include <stdint.h>

void send_usb_hid_report(uint8_t *report_data, uint8_t report_size);

#endif