#include "lcd.h"
#ifdef USE_NHDC0220
#include "nhdc0220.h"
#include "hal_nhdc0220.h"
#include <stddef.h>

//Note: Driver inspired by https://github.com/metodn/ArduProjects/blob/master/i2c_lcd_small/ST7036.cpp

static const uint8_t INSTR_DISP_CLEAR = 0x01;
static const uint8_t INSTR_DISP_HOME = 0x02;
static const uint8_t INSTR_ENTRY_MODE_SET = 0x04;
static const uint8_t INSTR_DISP_ON = 0x08;
static const uint8_t FLAG_POS_INSTR_DISP_ON_WITH_ENTIRE_DISPLAY = 2;
static const uint8_t FLAG_POS_INSTR_DISP_ON_WITH_CURSOR_ON = 1;
static const uint8_t FLAG_POS_INSTR_DISP_CURSOR_HOME = 0;
static const uint8_t INSTR_SET_CGRAM_ADDR = 0x40;
static const uint8_t INSTR_SET_DRAM_ADDR = 0x80;

void init_lcd(void)
{
    hal_nhdc0220_init();
}

void write_lcd(char *string, size_t length, uint8_t line)
{

}

void clear_lcd(void)
{

}

void set_lcd_backlight(uint8_t brightness)
{

}

void set_lcd_contrast(uint8_t contrast)
{

}

#endif // USE_NHDC0220