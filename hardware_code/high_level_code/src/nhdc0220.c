#include "lcd.h"
#ifdef USE_NHDC0220
#include "nhdc0220.h"
#include "hal_nhdc0220.h"

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