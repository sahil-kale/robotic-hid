#ifndef LCD_H
#define LCD_H
#include <stdint.h>
#include <stddef.h>

#define USE_NHDC0220
#define LCD_NUM_ROWS (2U)
#define LCD_NUM_COLS (20U)

/**
 * @brief Initialize the LCD.
 * 
 */
void init_lcd(void);

/**
 * @brief Write a string to the LCD
 * 
 * @param string The string to write to the LCD
 * @param length The length of the string to write to the LCD
 * @param line The line to write the string to
 */
void write_lcd(char *string, size_t length, uint8_t line);

/**
 * @brief Clear the LCD screen
 * 
 */
void clear_lcd(void);

/**
 * @brief Set the lcd backlight brightness
 * 
 * @param brightness 
 */
void set_lcd_backlight(uint8_t brightness);

/**
 * @brief Set the lcd contrast level
 * 
 * @param contrast 
 */
void set_lcd_contrast(uint8_t contrast);

#endif //LCD_H
