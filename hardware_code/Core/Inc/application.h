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

void send_joystick_report(const gameHID_t *report);
void joystick_task(void const * argument);

#endif // JOYSTICK_H
