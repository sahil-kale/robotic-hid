#include "application.h"
#include "adc.h"
#include "lcd.h"
#include "hal_application.h"
#include "cmsis_os2.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static void send_joystick_report(const gameHID_t *report);
static void buttonlogic(void);

//Static Game HId inststnace for LCD to update
static gameHID_t hid_data = {0};

static LCD_PAGES_E lcd_page = LCD_PAGE_AXIS;

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
    hid_data.JoyLX = (int8_t)((adc_data.adc_data[0]*2)/256);
    hid_data.JoyLY = (int8_t)((adc_data.adc_data[1]*2)/256);
    hid_data.JoyRX = (int8_t)((adc_data.adc_data[2]*2)/256);
    hid_data.JoyRY = (int8_t)((adc_data.adc_data[3]*2)/256);

    buttonlogic();

	send_joystick_report(&hid_data);
    osDelay(5); //200Hz
    }
}

static void buttonlogic(void)
{
    static hal_application_button_state_t hystered_button_state = {0};
    //Sample Buttons:
    hal_application_button_state_t button_state = update_button_states();
    //Set left and right button requests
    if(button_state.lcd_button_left)
    {
        if(!hystered_button_state.lcd_button_left)
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
        hystered_button_state.lcd_button_left = true;
    }
    else
    {
        
        hystered_button_state.lcd_button_left = false;
    }

    if(button_state.lcd_button_right)
    {
        if(!hystered_button_state.lcd_button_right)
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
        hystered_button_state.lcd_button_right = true;
    }
    else
    {
        hystered_button_state.lcd_button_right = false;
    }

    if(button_state.lcd_button_enter)
    {
        hystered_button_state.lcd_button_enter = true;
    }
    else
    {
        hystered_button_state.lcd_button_enter = false;
    }

	hid_data.Buttons = button_state.manipulating_button_state & button_bitmask;
}

//Create LCD update task loop
void lcd_task(void const * argument)
{
    init_lcd();
    set_lcd_cursor(0,0);
    char initArray1[] = "Initializing...";
    write_lcd(initArray1, sizeof(initArray1));
    set_lcd_cursor(1,0);
    char initArray2[] = "Sahil Kale 2022";
    write_lcd(initArray2, sizeof(initArray2));
    osDelay(1000);
    
    
    char line_one_buffer[LCD_NUM_COLS] = {0};
    char line_two_buffer[LCD_NUM_COLS] = {0};
    
    while(1)
    {
        clear_lcd();
        memset(line_one_buffer, 0, sizeof(line_one_buffer));
        memset(line_two_buffer, 0, sizeof(line_two_buffer));
        //Update LCD with joystick data from HID report using sprintf to populate buffers

        //Switch case statement for LCD pages
        switch(lcd_page)
        {
            case LCD_PAGE_AXIS:
                sprintf(line_one_buffer, "LX: %d", hid_data.JoyLX);
                sprintf(line_one_buffer + 10, "LY: %d", hid_data.JoyLY);
                sprintf(line_two_buffer, "RX: %d", hid_data.JoyRX);
                sprintf(line_two_buffer + 10, "RY: %d", hid_data.JoyRY);
                break;
            case LCD_PAGE_BUTTONS:
                //Avoid statement situation
                if(false)
                {
                    uint8_t unused = 0;
                    (void)unused;
                }

                char button_text[] = "Buttons:";
                sprintf(line_one_buffer, "%s", button_text);

                //Append to line one buffer the binary representation of the buttons
                for(size_t i = 0; i < num_buttons; i++)
                {
                    if(hid_data.Buttons & (1 << i))
                    {
                        line_one_buffer[i + sizeof(button_text) + 2] = '1';
                    }
                    else
                    {
                        line_one_buffer[i + sizeof(button_text) + 2] = '0';
                    }
                }

                
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
        set_lcd_cursor(0,0);
        write_lcd(line_one_buffer, sizeof(line_one_buffer));
        set_lcd_cursor(1,0);
        write_lcd(line_two_buffer, sizeof(line_two_buffer));
        osDelay(33); //30Hz
    }
}



static void send_joystick_report(const gameHID_t *report)
{
    send_usb_hid_report((uint8_t *)report, sizeof(gameHID_t));
}
