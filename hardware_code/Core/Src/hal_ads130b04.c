#include "hal_ads130b04.h"
#include "main.h"
#include "spi.h"
#include "gpio.h"

extern SPI_HandleTypeDef hspi1;

void init_hal_ads130b04(void)
{
	//Do nothing for now?
}

void set_sync_reset_pin(bool value)
{
	HAL_GPIO_WritePin(ADC_RESET_GPIO_Port, ADC_RESET_Pin, value);
}

void assert_chip_select(bool value)
{
	HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, !value);
}

void ads130b04_spi_transfer(uint8_t *txbuffer, uint8_t *rxbuffer, uint8_t len)
{
	HAL_SPI_TransmitReceive(&hspi1, txbuffer, rxbuffer, len, 1);
}
