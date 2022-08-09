#include "hal_nhdc0220.h"
#include "main.h"
#include "i2c.h"

void hal_nhdc0220_init(void)
{
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, 0);
	HAL_Delay(1);
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, 1);
}

void nhdc0220_i2c_write(uint8_t address, uint8_t data[], uint8_t len)
{
	HAL_StatusTypeDef ret;
	ret= HAL_I2C_Master_Transmit(&hi2c1, address, data, len, HAL_MAX_DELAY);
	(void)(ret); //Suppress unused warning
}
