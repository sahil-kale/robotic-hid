#ifndef HAL_BOOTLOADER_H
#define HAL_BOOTLOADER_H
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "stm32l1xx_hal.h"

static uint32_t determine_page_address_from_sector(uint8_t sector);

void hal_jump_to_app(void)
{
	void (*app_reset_handler)(void) = (void*)(*((volatile uint32_t*)(APP_START_ADDRESS + 4U)));
	app_reset_handler();
}

void hal_reset(void)
{
	NVIC_SystemReset();
}

static uint32_t determine_page_address_from_sector(uint8_t sector)
{
	return (sector * SECTOR_SIZE) + APP_START_ADDRESS;
}

void write_application_info(application_info_flash_t* info)
{
	//Ensure Flash is not busy
	FLASH_WaitForLastOperation(HAL_MAX_DELAY);

	//Setup erase
	FLASH_EraseInitTypeDef EraseInitStruct = {0};
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = determine_page_address_from_sector(APP_INFO_SECTOR);
	EraseInitStruct.NbPages = SECTOR_SIZE/PAGE_SIZE;

	//Unlock the flash
	HAL_StatusTypeDef hal_status = HAL_FLASH_Unlock();
	if(hal_status != HAL_OK)
	{
		return;
	}

	uint32_t page_error = 0;
	hal_status = HAL_FLASHEx_Erase(&EraseInitStruct, &page_error);

	if(hal_status != HAL_OK)
	{
		return;
	}

	uint8_t* data = (uint8_t*)info;
	for(size_t i = 0; i < sizeof(application_info_flash_t); i++)
	{
		hal_status = HAL_FLASH_Program(FLASH_TYPEPROGRAMDATA_BYTE, (uint32_t)(determine_page_address_from_sector(APP_INFO_SECTOR) + i), data[i]);

		if(hal_status != HAL_OK)
		{
			break;
		}
	}

	hal_status = HAL_FLASH_Lock();
	if(hal_status != HAL_OK)
	{
		return;
	}

}

application_info_flash_t read_application_info(void)
{
    application_info_flash_t info = {0};
    memcpy(&info, (void*)(determine_page_address_from_sector(APP_INFO_SECTOR)), sizeof(application_info_flash_t));
    return info;
}

#endif
