#include "application.h"
#include "hal_usb.h"

//Static Game HId inststnace for LCD to update
static gameHID_t most_recent_hid_data = {0};

//Create HID report task loop:
void joystick_task(void const * argument)
{
    while(1)
    {

    }
}

//Create LCD update task loop
void lcd_task(void const * argument)
{
    while(1)
    {
        
    }
}



void send_joystick_report(const gameHID_t *report)
{
    send_usb_hid_report((uint8_t *)report, sizeof(gameHID_t));
}
