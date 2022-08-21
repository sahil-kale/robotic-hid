#include "hal_usb.h"
#include "usbd_hid.h"

extern USBD_HandleTypeDef hUsbDeviceFS;


void send_usb_hid_report(uint8_t *report_data, uint8_t report_size)
{
    USBD_HID_SendReport(&hUsbDeviceFS, report_data, report_size);
}