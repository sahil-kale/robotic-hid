#include "lcd.h"
#ifdef USE_NHDC0220
#include "nhdc0220.h"
#include "hal_nhdc0220.h"
#include <stddef.h>
#include <string.h> //for memcpy

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-const-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"

//Note: Driver inspired by https://github.com/metodn/ArduProjects/blob/master/i2c_lcd_small/ST7036.cpp

static const uint8_t LCD_ADDR = 0x78;

//Command byte options:
static const uint8_t CONTROL_BYTE_CMD = 0x00;
static const uint8_t CONTROL_BYTE_RAM_DATA = 0x40;

static const uint8_t INSTR_DISP_CLEAR = 0x01;
static const uint8_t INSTR_CURSOR_HOME = 0x02;
static const uint8_t INSTR_ENTRY_MODE_SET = 0x04;
static const uint8_t INSTR_DISP_ON = 0x08;
static const uint8_t FLAG_POS_INSTR_DISP_ON_WITH_ENTIRE_DISPLAY = 2;
static const uint8_t FLAG_POS_INSTR_DISP_ON_WITH_CURSOR_ON = 1;
static const uint8_t FLAG_POS_INSTR_DISP_CURSOR_HOME = 0;
static const uint8_t INSTR_SET_CGRAM_ADDR = 0x40;
static const uint8_t INSTR_SET_DRAM_ADDR = 0x80;

//Array for line offsets, 1st line starts at 0x00, 2nd line starts at 0x40
static const uint8_t dram_line_offsets[] = { 0x00, 0x40 };

void init_lcd(void)
{
    hal_nhdc0220_init();
}

void write_lcd(char *string, size_t length)
{
    //Write string to LCD

    //Dynamically allocate new array
    uint8_t *data = malloc(length+1);
    if (data == NULL)
    {
        abort();
    }
    //Add CONTROL_BYTE_RAM_DATA to first byte
    data[0] = CONTROL_BYTE_RAM_DATA;
    //Copy string to array
    memcpy(data + 1, string, length);
    //Write array to LCD
    nhdc0220_i2c_write(LCD_ADDR, true, data, length+1);

    //Free array
    free(data);

}

void clear_lcd(void)
{
    //Call command with INSTR_DISP_CLEAR
    nhdc0220_command(INSTR_DISP_CLEAR);
}

void set_lcd_backlight(uint8_t brightness)
{
    return; //not implemented
}

void set_lcd_contrast(uint8_t contrast)
{
    return; //not implemented
}

void nhdc0220_command(uint8_t val)
{
    static uint8_t data[2] = {0};

    data[0] = CONTROL_BYTE_CMD;
    data[1] = val;
    nhdc0220_i2c_write(LCD_ADDR, true, data, sizeof(data));
}

void set_lcd_cursor(uint8_t row, uint8_t col)
{
    uint8_t instr = INSTR_SET_DRAM_ADDR;
    instr |= dram_line_offsets[row];
    instr |= col;
    nhdc0220_command(instr);
}

#pragma GCC diagnostic pop

#endif // USE_NHDC0220