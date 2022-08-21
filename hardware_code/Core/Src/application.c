#include "joystick.h"
#include "usbd_hid.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

void send_joystick_report(const gameHID_t *report)
{
    USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)report, sizeof(gameHID_t));
}
