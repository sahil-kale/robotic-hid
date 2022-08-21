#include "application.h"
#include "adc.h"
#include "hal_usb.h"
#include "cmsis_os2.h"

//Static Game HId inststnace for LCD to update
static gameHID_t hid_data = {0};

//Create HID report task loop:
void joystick_task(void const * argument)
{
    init_adc();
    while(1)
    {
    //Get ADC report:
    adc_data_t adc_data;
    adc_data = get_adc_data();

    //Scale ADC value from -16384 to 16384 to -127 to 127:
    hid_data.JoyLX = (int8_t)(adc_data.adc_data[0]/256);
    hid_data.JoyLY = (int8_t)(adc_data.adc_data[1]/256);
    hid_data.JoyRX = (int8_t)(adc_data.adc_data[2]/256);
    hid_data.JoyRY = (int8_t)(adc_data.adc_data[3]/256);
	//hid_data.Buttons = counter1 & 0b00001111;
	send_joystick_report(&hid_data);
    osDelay(5); //200Hz
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
