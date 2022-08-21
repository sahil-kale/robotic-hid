#ifndef JOYSTICK_H
#define JOYSTICK_H
#include <stdint.h>



typedef struct gameHID {
	int8_t JoyLX; 		
  	int8_t JoyLY; 		
    int8_t JoyRX; 		
    int8_t JoyRY; 		
  	uint8_t Buttons; //Only lower 4 bits are used
} gameHID_t;

typedef enum LCD_PAGES
{
    LCD_PAGE_AXIS = 0,
    LCD_PAGE_BUTTONS = 1,
    LCD_PAGE_INFO = 2,
    LCD_PAGES_MAX = 3,
} LCD_PAGES_E;

void joystick_task(void const * argument);
void lcd_task(void const * argument);

#endif // JOYSTICK_H
