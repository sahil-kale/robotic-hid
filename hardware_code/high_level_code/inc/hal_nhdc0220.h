#ifndef HAL_NHDC0220_H
#define HAL_NHDC0220_H
#include <stdint.h>
#include <stdbool.h>

void hal_nhdc0220_init(void);

/**
 * @brief begin I2C transaction
 * 
 * @param address I2C addr
 * @param write sets the R/W bit. 
 */
void nhdc0220_i2c_beginTransmission(uint8_t address, bool write);

/**
 * @brief I2C write bytes command
 * 
 * @param data pointer to data array
 * @param len length of data array
 * @note ensure that beginTransmission has been called before calling this function
 */
void nhdc0220_i2c_write(uint8_t data[], uint8_t len);

/**
 * @brief Ends an I2C transaction from beginTransmission
 * 
 */
void nhdc0220_i2c_endTransmission(void);

#endif //HAL_NHDC0220_H
