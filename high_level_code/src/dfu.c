#include "dfu.h"
#include "hal_dfu.h"
#ifdef ENABLE_OTA_DFU
#include "string.h"
#include "utility.h"
#include "common.h"

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

DFU_STATUS_E dfu_run()
{
    static uint32_t start_tick = 0;
    if(start_tick == 0)
    {
        start_tick = hal_dfu_gettick();
    }

    DFU_STATUS_E status = DFU_STATUS_OK;
    switch(dfu_state.state)
    {
        case DFU_STATE_START:;

            DFU_data_handle_t data = get_data_from_dfu_host();
            if(data.size != 0 && data.data != NULL)
            {
                //Do something with the data
            }
            else if(hal_dfu_gettick() > start_tick + DFU_TIMEOUT_START)
            {
                status = DFU_STATUS_ERROR_HOST_TIMEOUT;
            }

            break;

        case DFU_STATE_DATA_EXCHANGE:;
            break;

        case DFU_STATE_COMPLETE:;
            break;

        case DFU_STATE_ERROR:;
            break;
        
        default:
            break;

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
