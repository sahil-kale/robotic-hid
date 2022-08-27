#include "hal_application.h"
#include "gpio.h"
#include "usbd_hid.h"
#include "stm32l1xx_hal.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

const uint8_t num_buttons = 4;
const uint8_t button_bitmask = 0b00001111;

#define GPIO_SW_1_PORT GPIOB
#define GPIO_SW_1_PIN GPIO_PIN_8
#define GPIO_SW_2_PORT GPIOB
#define GPIO_SW_2_PIN GPIO_PIN_9
#define GPIO_SW_3_PORT GPIOB
#define GPIO_SW_3_PIN GPIO_PIN_10
#define GPIO_SW_4_PORT GPIOB
#define GPIO_SW_4_PIN GPIO_PIN_11

#define GPIO_USERRIGHT_PORT GPIOB
#define GPIO_USERRIGHT_PIN GPIO_PIN_12
#define GPIO_USERLEFT_PORT GPIOB
#define GPIO_USERLEFT_PIN GPIO_PIN_13
#define GPIO_USERENTER_PORT GPIOB
#define GPIO_USERENTER_PIN GPIO_PIN_14

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"


static uint32_t determine_page_address_from_sector(uint8_t sector);

void send_usb_hid_report(uint8_t *report_data, uint8_t report_size)
{
    USBD_HID_SendReport(&hUsbDeviceFS, report_data, report_size);
}

static uint32_t determine_page_address_from_sector(uint8_t sector)
{
	return (sector * SECTOR_SIZE) + BOOTLOADER_START_ADDRESS;
}


hal_application_button_state_t update_button_states(void)
{
    //Poll the button states
    hal_application_button_state_t button_state = {0};
    button_state.manipulating_button_state |= !(HAL_GPIO_ReadPin(GPIO_SW_1_PORT, GPIO_SW_1_PIN)) << 0;
    button_state.manipulating_button_state |= !(HAL_GPIO_ReadPin(GPIO_SW_2_PORT, GPIO_SW_2_PIN)) << 1;
    button_state.manipulating_button_state |= !(HAL_GPIO_ReadPin(GPIO_SW_3_PORT, GPIO_SW_3_PIN)) << 2;
    button_state.manipulating_button_state |= !(HAL_GPIO_ReadPin(GPIO_SW_4_PORT, GPIO_SW_4_PIN)) << 3;

    button_state.lcd_button_left = HAL_GPIO_ReadPin(GPIO_USERLEFT_PORT, GPIO_USERLEFT_PIN);
    button_state.lcd_button_right = HAL_GPIO_ReadPin(GPIO_USERRIGHT_PORT, GPIO_USERRIGHT_PIN);
    button_state.lcd_button_enter = HAL_GPIO_ReadPin(GPIO_USERENTER_PORT, GPIO_USERENTER_PIN);

    return button_state;
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

}

application_info_flash_t read_application_info(void)
{
    application_info_flash_t info = {0};
    uint32_t address = determine_page_address_from_sector(APP_INFO_SECTOR);
    memcpy(&info, (void*)(address), sizeof(application_info_flash_t));
    return info;
}

void hal_reset(void)
{
	NVIC_SystemReset();
}

#pragma GCC diagnostic pop
