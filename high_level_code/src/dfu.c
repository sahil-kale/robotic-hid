#include "dfu.h"
#include "hal_dfu.h"
#ifdef ENABLE_OTA_DFU

//Disable Wconversion warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"

static const uint8_t SOF_identifier = 0x01;
DFU_STATE_E dfu_state;

DFU_STATUS_E dfu_init(void)
{
    DFU_STATUS_E status = DFU_STATUS_OK;
    status = hal_dfu_init();
    (void)dfu_assert_error(status);
    return status;
}

DFU_STATUS_E dfu_ack(uint16_t data_len)
{
    //Convert data len from little endian to big endian with 2 8 bit bytes
    uint8_t data_len_be[2];
    data_len_be[0] = (data_len & 0xFF00) >> 8;
    data_len_be[1] = data_len & 0xFF;

    packet_dfu_header_t packet_header;
    packet_header.SOF = SOF_identifier;
    packet_header.packet_type = DFU_PACKET_ACK;
    packet_header.data_length = data_len_be[0] + data_len_be[1]*256;


    DFU_data_handle_t ack_data;
    ack_data.data = (uint8_t *)(&packet_header);
    ack_data.size = sizeof(packet_header);

    return send_data_to_dfu_host(ack_data);
}

bool dfu_assert_error(DFU_STATUS_E status)
{
    if(status != DFU_STATUS_OK)
    {
        dfu_state = DFU_STATE_ERROR;
    }

    return (status != DFU_STATUS_OK);
}

#pragma GCC diagnostic pop

#endif // ENABLE_OTA_DFU
