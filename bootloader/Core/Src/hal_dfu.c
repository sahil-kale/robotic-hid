#include "hal_dfu.h"
#include "dfu.h"
#include <stdint.h>
#include "stm32l1xx_hal.h"
#include "usbd_cdc_if.h"
#include "common.h"
#include "usb_device.h"

#define MAX_USB_PACKET_SIZE (64U)
#define NUM_USB_PACKETS_STORED (10U)

static uint8_t usb_packet_data_buffer[MAX_USB_PACKET_SIZE * NUM_USB_PACKETS_STORED] = {0};
static DFU_data_handle_t data_handles[NUM_USB_PACKETS_STORED] = {0};
static uint8_t usb_packet_buffer_index_start = 0;
static uint8_t usb_packet_buffer_index_end = 0;

static uint32_t determine_page_address_from_sector(uint8_t sector);

static uint32_t determine_page_address_from_sector(uint8_t sector)
{
	return (sector * SECTOR_SIZE) + APP_START_ADDRESS;
}

void append_USB_data_rx_buffer(uint8_t *data, size_t size)
{
    DFU_data_handle_t *data_handle = &data_handles[usb_packet_buffer_index_end];
    //Clear the buffer
    memset(data_handle->data, 0, size);
    memcpy(data_handle->data, data, size); //Copy the data size
    data_handle->size = size; //Copy the data size
    usb_packet_buffer_index_end++; //Increment the buffer index
}

DFU_STATUS_E hal_dfu_init(void)
{
	MX_USB_DEVICE_Init();
    for(size_t i = 0; i < NUM_USB_PACKETS_STORED; i++)
    {
        data_handles[i].data = usb_packet_data_buffer + (i * MAX_USB_PACKET_SIZE);
        data_handles[i].size = 0;
    }
    return DFU_STATUS_OK;
}

DFU_data_handle_t get_data_from_dfu_host(void)
{
    DFU_data_handle_t data_handle = {0};
    if(usb_packet_buffer_index_start == usb_packet_buffer_index_end)
    {
        return data_handle; //Empty buffer
    }
    data_handle = data_handles[usb_packet_buffer_index_start];
    data_handles[usb_packet_buffer_index_start].size = 0; //Clear the data size
    usb_packet_buffer_index_start++;
    return data_handle;
}

DFU_STATUS_E send_data_to_dfu_host(DFU_data_handle_t data)
{
    if(CDC_Transmit_FS(data.data, data.size) != USBD_OK)
    {
        return DFU_STATUS_ERROR;
    }
    
    return DFU_STATUS_OK;

}

DFU_STATUS_E hal_dfu_writeflash(uint32_t address, uint32_t size, const uint8_t *data)
{
    DFU_STATUS_E status = DFU_STATUS_OK;
    do
    {
        HAL_StatusTypeDef hal_status = HAL_FLASH_Unlock();
        if(hal_status != HAL_OK)
        {
            status = DFU_STATUS_ERROR_FLASH_WRITE;
            break;   
        }


        for(uint32_t i = 0; i < size; i++)
        {
            hal_status = HAL_FLASH_Program(FLASH_TYPEPROGRAMDATA_BYTE, address + i, data[i]);
            if(hal_status != HAL_OK)
            {
                status = DFU_STATUS_ERROR_FLASH_WRITE;
                break;
            }
        }

        hal_status = HAL_FLASH_Lock();
        if(hal_status != HAL_OK)
        {
            status = DFU_STATUS_ERROR_FLASH_WRITE;
            break;
        }


    } while (false);

    return status;

}

DFU_STATUS_E hal_dfu_eraseflash(uint32_t baseSector, uint32_t noSectors)
{
    DFU_STATUS_E status = DFU_STATUS_OK;

    do
    {
        //Ensure Flash is not busy
        FLASH_WaitForLastOperation(HAL_MAX_DELAY);

        //Setup erase
        FLASH_EraseInitTypeDef EraseInitStruct = {0};
        EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
        EraseInitStruct.PageAddress = determine_page_address_from_sector(APP_START_ADDRESS);
        EraseInitStruct.NbPages = (APP_SECTOR_END - APP_SECTOR_START + 1) * SECTOR_SIZE/PAGE_SIZE;

        //Unlock Flash
        //Unlock the flash
        HAL_StatusTypeDef hal_status = HAL_FLASH_Unlock();
        if(hal_status != HAL_OK)
        {
            status = DFU_STATUS_ERROR_FLASH_ERASE;
            break;
        }

        //Perform erase
        uint32_t page_error = 0;
        hal_status = HAL_FLASHEx_Erase(&EraseInitStruct, &page_error);

        if(hal_status != HAL_OK)
        {
            status = DFU_STATUS_ERROR_FLASH_ERASE;
            break;
        }


        hal_status = HAL_FLASH_Lock();
        if(hal_status != HAL_OK)
        {
            status = DFU_STATUS_ERROR_FLASH_ERASE;
            break;
        }
    } while (false);

    return status;
}   

bool hal_dfu_validate_crc(uint32_t address, uint32_t size, uint32_t crc)
{
    //TODO
    (void)address;
    (void)size;
    (void)crc;
    return true;
}

uint32_t hal_dfu_gettick(void)
{
    return HAL_GetTick();
}
