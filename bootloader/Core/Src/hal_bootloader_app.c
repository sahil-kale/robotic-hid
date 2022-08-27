#ifndef HAL_BOOTLOADER_APP_H
#define HAL_BOOTLOADER_APP_H
#include <stdint.h>
#include <string.h>
#include "hal_bootloader_app.h"

//#define DEBUG_TRIGGER_DFU_MODE
#ifdef DEBUG_TRIGGER_DFU_MODE
#warning DFU mode is being intentionally triggered...
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"

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
	return (sector * SECTOR_SIZE) + BOOTLOADER_START_ADDRESS;
}

application_info_flash_t read_application_info(void)
{
    application_info_flash_t info = {0};
    uint32_t address = determine_page_address_from_sector(APP_INFO_SECTOR);
    memcpy(&info, (void*)(address), sizeof(application_info_flash_t));
#ifdef DEBUG_TRIGGER_DFU_MODE
    info.dfu_request = true;
#endif
    return info;
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

	//Clear flags
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR);

	uint32_t page_error = 0;
	hal_status = HAL_FLASHEx_Erase(&EraseInitStruct, &page_error);

	if(hal_status != HAL_OK)
	{
		return;
	}

	uint32_t* data = (uint32_t*)info;

	//Convert info to a uint32_t data buffer

	for(size_t i = 0; i < sizeof(application_info_flash_t)/sizeof(uint32_t) + ((sizeof(application_info_flash_t) % sizeof(uint32_t)) > 0); i++)
	{
		uint32_t address = determine_page_address_from_sector(APP_INFO_SECTOR) + (i * sizeof(uint32_t));
		hal_status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data[i]);

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

	application_info_flash_t test_info = read_application_info();
	(void)test_info;

}

#pragma GCC diagnostic pop

#endif
