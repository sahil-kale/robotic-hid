#include "dfu.h"
#include "hal_dfu.h"
#ifdef ENABLE_OTA_DFU
#include "string.h"
#include "utility.h"

//Disable Wconversion warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"

const uint8_t DFU_SOF_identifier = 0x01;
DFU_STATE_INFORMATION_T dfu_state;

DFU_STATUS_E dfu_init(void)
{
    //Set all of DFU state to 0
    memset(&dfu_state, 0, sizeof(DFU_STATE_INFORMATION_T));
    dfu_state.state = DFU_STATE_IDLE;

    DFU_STATUS_E status = DFU_STATUS_OK;
    status = hal_dfu_init();
    (void)dfu_assert_error(status);
    return status;
}

DFU_STATUS_E dfu_ack(uint16_t data_len)
{

    packet_dfu_header_t packet_header;
    packet_header.SOF = DFU_SOF_identifier;
    packet_header.packet_type = DFU_PACKET_ACK;
    packet_header.data_length = swap_uint16(data_len);


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

#pragma GCC diagnostic pop

#endif // ENABLE_OTA_DFU
