#ifndef DFU_H
#define DFU_H
#define ENABLE_OTA_DFU
#include "stdint.h"

#ifdef UNITTEST
#define PACKED 
#else
#define PACKED __attribute__((packed))
#endif

extern DFU_STATE_E dfu_state;

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

typedef struct PACKED
{
    uint8_t SOF;
    DFU_PACKET_TYPE_E packet_type;
    uint16_t data_length;
    uint8_t* data;
} packet_dfu_t;

typedef struct PACKED
{
    uint32_t prog_size;
    uint32_t data_size;
    uint8_t reserved[4];
} packet_dfu_prog_info_t;

typedef struct PACKED
{
    uint8_t SOF;
    uint8_t packet_type;
    uint16_t data_length_received;
} packet_dfu_ack_t;

DFU_STATUS_E dfu_init(void);
DFU_STATUS_E dfu_run(void);

DFU_STATUS_E dfu_ack(uint16_t data_len);
DFU_STATUS_E dfu_process_packet(packet_dfu_t* packet);

#endif // DFU_H