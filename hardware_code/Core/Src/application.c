#include "application.h"
#include "adc.h"
#include "lcd.h"
#include "hal_application.h"
#include "cmsis_os2.h"
#include <stdbool.h>
#include <stdio.h>

static void send_joystick_report(const gameHID_t *report);
static void buttonlogic(void);

//Static Game HId inststnace for LCD to update
static gameHID_t hid_data = {0};

static bool left_button_down = false;
static bool right_button_down = false;
static bool enter_button_down = false;

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

    buttonlogic();

	send_joystick_report(&hid_data);
    osDelay(5); //200Hz
    }
}

static void buttonlogic(void)
{
    //Sample Buttons:
    hal_application_button_state_t button_state = update_button_states();
    //Set left and right button requests
    if(button_state.lcd_button_left)
    {
        left_button_down = true;
    }

    if(button_state.lcd_button_right)
    {
        right_button_down = true;
    }

    if(button_state.lcd_button_enter)
    {
        enter_button_down = true;
    }

	hid_data.Buttons = button_state.manipulating_button_state & 0b00001111;
}

//Create LCD update task loop
void lcd_task(void const * argument)
{
    init_lcd();
    set_lcd_cursor(0,0);
    char testArray[] = "Starting...";
    write_lcd(testArray, sizeof(testArray));
    
    static char line_one_buffer[20] = {0};
    static char line_two_buffer[20] = {0};
    
    while(1)
    {
        //Update LCD with joystick data from HID report using sprintf to populate buffers
        sprintf(line_one_buffer, "LX/LY: %d/%d", hid_data.JoyLX, hid_data.JoyLY);
        sprintf(line_two_buffer, "RX/RY: %d/%d", hid_data.JoyRX, hid_data.JoyRY);
        set_lcd_cursor(0,0);
        write_lcd(line_one_buffer, sizeof(line_one_buffer));
        set_lcd_cursor(1,0);
        write_lcd(line_two_buffer, sizeof(line_two_buffer));
        osDelay(100); //10Hz
    }
}



static void send_joystick_report(const gameHID_t *report)
{
    send_usb_hid_report((uint8_t *)report, sizeof(gameHID_t));
}
