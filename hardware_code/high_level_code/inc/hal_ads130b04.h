#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdbool.h>

/**
 * @brief cycles the sync reset pin on the ADS130B04. Pull low to reset.
 *  
 * @param value level to set sync reset pin
 */
void set_sync_reset_pin(bool value);


#endif /* HAL_GPIO_H */