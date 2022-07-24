#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief cycles the sync reset pin on the ADS130B04. Pull low to reset.
 *  
 * @param value level to set sync reset pin
 */
void set_sync_reset_pin(bool value);

void ads130b04_spi_transfer(uint8_t *txbuffer, uint8_t txlen, uint8_t *rxbuffer, uint8_t rxlen);


#endif /* HAL_GPIO_H */