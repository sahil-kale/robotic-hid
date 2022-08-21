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

static LCD_PAGES_E lcd_page = LCD_PAGE_AXIS;
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
        if(lcd_page != 0)
        {
            lcd_page--;
        }
        else
        {
            lcd_page = LCD_PAGES_MAX - 1;
        }
    }

    if(button_state.lcd_button_right)
    {
        if(lcd_page != LCD_PAGES_MAX - 1)
        {
            lcd_page++;
        }
        else
        {
            lcd_page = 0;
        }
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
    char testArray1[] = "Initializing...";
    write_lcd(testArray1, sizeof(testArray1));
    set_lcd_cursor(1,0);
    char testArray2[] = "Sahil Kale 2022";
    write_lcd(testArray2, sizeof(testArray2));
    osDelay(2000);
    
    
    static char line_one_buffer[20] = {0};
    static char line_two_buffer[20] = {0};
    
    while(1)
    {
        clear_lcd();
        //Update LCD with joystick data from HID report using sprintf to populate buffers

        //Switch case statement for LCD pages
        switch(lcd_page)
        {
            case LCD_PAGE_AXIS:
                sprintf(line_one_buffer, "LX|LY: %d | %d", hid_data.JoyLX, hid_data.JoyLY);
                sprintf(line_two_buffer, "RX|RY: %d | %d", hid_data.JoyRX, hid_data.JoyRY);
                break;
            case LCD_PAGE_BUTTONS:
                //Avoid statement situation
                if(false)
                {
                    uint8_t unused = 0;
                    (void)unused;
                }
                uint16_t button_states_binary = 0;
                //Byte by byte, add each button state to button_states
                for(int i = 0; i < 4; i++)
                {
                    button_states_binary += (hid_data.Buttons >> i) & 0b00000001;
                }
                sprintf(line_one_buffer, "Buttons: %d", button_states_binary);
                sprintf(line_two_buffer, "Sys Operational");
                break;
            case LCD_PAGE_INFO:
                sprintf(line_one_buffer, "Robotic HID");
                sprintf(line_two_buffer, "Sahil Kale 2022");
                break;
            default:
                sprintf(line_one_buffer, "UNKNOWN ERR!");
                sprintf(line_two_buffer, "Discontinue use!");
                break;
        }

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
