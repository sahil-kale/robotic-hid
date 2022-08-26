#include "dfu.h"
#include "hal_dfu.h"
#ifdef ENABLE_OTA_DFU
#include "string.h"
#include "utility.h"
#include "common.h"
#include "stdio.h"
#include "hal_bootloader_app.h"

//Disable Wconversion warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"


DFU_STATE_INFORMATION_T dfu_state;
const uint32_t DFU_TIMEOUT_START = 30000;

DFU_STATUS_E dfu_init(void)
{
    //Set all of DFU state to 0
    memset(&dfu_state, 0, sizeof(DFU_STATE_INFORMATION_T));
    dfu_state.state = DFU_STATE_START;

    DFU_STATUS_E status = DFU_STATUS_OK;
    status = hal_dfu_init();
    (void)dfu_assert_error(status);
    return status;
}

DFU_STATUS_E dfu_ack(uint16_t payload_len)
{

    packet_dfu_header_t packet_header;
    packet_header.SOF = DFU_SOF_identifier;
    packet_header.packet_type = DFU_PACKET_ACK;
    packet_header.payload_length = swap_uint16(payload_len);


    DFU_data_handle_t ack_data;
    ack_data.data = (uint8_t *)(&packet_header);
    ack_data.size = sizeof(packet_header);

    return send_data_to_dfu_host(ack_data);
}

bool dfu_assert_error(DFU_STATUS_E status)
{
    if(status != DFU_STATUS_OK)
    {
        dfu_state.state = DFU_STATE_ERROR;
    }

    return (status != DFU_STATUS_OK);
}

uint32_t dfu_time_last_received = 0;

DFU_STATUS_E dfu_run()
{
    DFU_STATUS_E status = DFU_STATUS_OK;

    if(dfu_time_last_received == 0)
    {
        dfu_time_last_received = hal_dfu_gettick();
    }

    DFU_data_handle_t data = get_data_from_dfu_host();
    bool is_data_valid = false;
    if(data.size != 0 && data.data != NULL && data.data[0] == DFU_SOF_identifier)
    {
        is_data_valid = true;
        dfu_time_last_received = hal_dfu_gettick();   
    }

    switch(dfu_state.state)
    {
        case DFU_STATE_START:;
            if(is_data_valid)
            {
                status = dfu_process_packet(data.data);
            }
            break;

        case DFU_STATE_DATA_EXCHANGE:;
            if(is_data_valid)
            {
                status = dfu_process_packet(data.data);
            }
            break;

        case DFU_STATE_COMPLETE:;
            //Write new application info to flash
            application_info_flash_t new_info;
            new_info.application_size = dfu_state.prog_size;
            new_info.application_crc = dfu_state.prog_crc;
            new_info.flash_valid = true;
            new_info.dfu_request = false;
            write_application_info(&new_info);

            hal_reset();
            break;

        case DFU_STATE_ERROR:;
            //Do nothing, assume state has been set
            break;
        
        default:
            status = DFU_STATUS_UNKNOWN_ERROR;
            break;

    }

    uint32_t current_time = hal_dfu_gettick();
    if((current_time > dfu_time_last_received + DFU_TIMEOUT_START) && !(is_data_valid))
    {
        status = DFU_STATUS_ERROR_HOST_TIMEOUT;
    }
    
    dfu_assert_error(status);
    return status;
}

DFU_STATUS_E dfu_process_packet(uint8_t* buffer)
{
    packet_dfu_header_t *packet_header = (packet_dfu_header_t *)buffer;
    packet_header->payload_length = swap_uint16(packet_header->payload_length);
    uint8_t *payload = buffer + sizeof(packet_dfu_header_t);

    DFU_STATUS_E status = DFU_STATUS_OK;
    switch(packet_header->packet_type)
    {
        case DFU_PACKET_START:
            //Check if we are in start state
             if(dfu_state.state != DFU_STATE_START)
             {
                 status = DFU_STATUS_ERROR_INVALID_STATE_PACKET_RX;
                 break;
             }

            //Create pointer to program information
            packet_dfu_prog_info_t *prog_info = (packet_dfu_prog_info_t *)payload;
            prog_info->prog_size = swap_uint32(prog_info->prog_size);
            prog_info->prog_crc = swap_uint32(prog_info->prog_crc);

            //Update the master dfu struct
            dfu_state.prog_size = prog_info->prog_size;
            dfu_state.prog_crc = prog_info->prog_crc;
            dfu_state.bytes_sent = 0;

            //Erase flash
            status = hal_dfu_eraseflash(APP_SECTOR_START, APP_SECTOR_END - APP_SECTOR_START + 1);


            //Send ACK
            status = dfu_ack(packet_header->payload_length);
            dfu_state.state = DFU_STATE_DATA_EXCHANGE;

            break;

        case DFU_PACKET_DATA:
            //Check if we are in data exchange state
            if(dfu_state.state != DFU_STATE_DATA_EXCHANGE)
            {
                status = DFU_STATUS_ERROR_INVALID_STATE_PACKET_RX;
                break;
            }

            status = hal_dfu_writeflash(APP_START_ADDRESS+dfu_state.bytes_sent, packet_header->payload_length, payload);
            if(status == DFU_STATUS_OK)
            {
                dfu_state.bytes_sent += packet_header->payload_length;
                status = dfu_ack(packet_header->payload_length);
            }

            if(dfu_state.bytes_sent == dfu_state.prog_size)
            {
                //For simplicity, Validate state is included in the data exchange state in here as it only occurs once in the context of DFU

                if(hal_dfu_validate_crc(APP_START_ADDRESS, dfu_state.prog_size, dfu_state.prog_crc))
                {
                    status = DFU_STATUS_OK;
                    dfu_state.state = DFU_STATE_COMPLETE;
                }
                else
                {
                    status = DFU_STATUS_ERROR_CRC_FAILED;
                }

            }

            break;

        default:
            status = DFU_STATUS_ERROR_INVALID_PACKET_TYPE;
            break;

    }

    (void)dfu_assert_error(status);
    return status;
}

#pragma GCC diagnostic pop

#endif // ENABLE_OTA_DFU
