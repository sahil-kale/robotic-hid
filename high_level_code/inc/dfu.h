#ifndef DFU_H
#define DFU_H
#define ENABLE_OTA_DFU
#include "stdint.h"

typedef enum
{
    DFU_STATE_START,
    DFU_STATE_DATA_EXCHANGE,
    DFU_STATE_COMPLETE,
    DFU_STATE_ERROR
} DFU_STATE_E;

typedef enum
{
    DFU_STATUS_OK,
    DFU_STATUS_ERROR,
    DFU_STATUS_TIMEOUT,
    DFU_STATUS_INVALID_PARAMETER,
    DFU_STATUS_UNKNOWN_ERROR
} DFU_STATUS_E;

typedef enum
{
    DFU_PACKET_START,
    DFU_PACKET_DATA,
    DFU_PACKET_ACK
} DFU_PACKET_TYPE_E;

typedef struct {
    uint32_t prog_size;
    uint32_t data_size;
    uint8_t reserved[4];
} packet_dfu_prog_info_t;

typedef struct
{
    uint8_t SOF;
    uint8_t packet_type;
    uint16_t data_length;
    packet_dfu_prog_info_t prog_info;
} packet_dfu_start_t;

typedef struct
{
    uint8_t SOF;
    uint8_t packet_type;
    uint16_t data_length;
    uint8_t data[];
} packet_dfu_data_t;

typedef struct
{
    uint8_t SOF;
    uint8_t packet_type;
    uint16_t data_length_received;
} packet_dfu_ack_t;

#endif // DFU_H