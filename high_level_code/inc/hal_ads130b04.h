#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Initializes the SPI handle and other information required
 * 
 */
void init_hal_ads130b04(void);

/**
 * @brief cycles the sync reset pin on the ADS130B04. Pull low to reset.
 *  
 * @param value level to set sync reset pin
 */
void set_sync_reset_pin(bool value);


/**
 * @brief Set the chip select pin
 * 
 * @param value high is selected, low is deselected
 */
void assert_chip_select(bool value);

/**
 * @brief SPI transfer function for the ADS130B04.
 * 
 * @param txbuffer pointer to the tx buffer
 * @param rxbuffer pointer to the rx buffer
 * @param len length of the transfer in bytes,
 * @note that txBuffer and rxBuffer must be of length len
 */
void ads130b04_spi_transfer(uint8_t *txbuffer, uint8_t *rxbuffer, uint8_t len);


#endif /* HAL_GPIO_H */