#ifndef HAL_NHDC0220_H
#define HAL_NHDC0220_H
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Reset the LCD.
 * 
 */
void hal_nhdc0220_init(void);

/**
 * @brief I2C write bytes command
 * @param data pointer to data array
 * @param len length of data array
 */
void nhdc0220_i2c_write(uint8_t address, uint8_t data[], uint8_t len);


#endif //HAL_NHDC0220_H
