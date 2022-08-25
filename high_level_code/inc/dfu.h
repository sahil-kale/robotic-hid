#ifndef DFU_H
#define DFU_H
#define ENABLE_OTA_DFU
#include "stdint.h"
#include "stdbool.h"

#ifdef UNITTEST
#define PACKED 
#else
#define PACKED __attribute__((packed))
#endif

typedef enum DFU_STATE
{
    DFU_STATE_IDLE = 0,
    DFU_STATE_START,
    DFU_STATE_DATA_EXCHANGE,
    DFU_STATE_VALIDATE,
    DFU_STATE_COMPLETE,
    DFU_STATE_ERROR
} DFU_STATE_E;

typedef enum DFU_STATUS
{
    DFU_STATUS_OK,
    DFU_STATUS_ERROR,
    DFU_STATUS_TIMEOUT,
    DFU_STATUS_INVALID_PARAMETER,
    DFU_STATUS_ERROR_INVALID_STATE_PACKET_RX,
    DFU_STATUS_ERROR_INVALID_PACKET_TYPE,
    DFU_STATUS_ERROR_CRC_FAILED,
    DFU_STATUS_UNKNOWN_ERROR
} DFU_STATUS_E;

typedef struct DFU_STATE_INFORMATION
{
    DFU_STATE_E state;
    uint32_t address;
    uint32_t prog_size;
    uint32_t bytes_sent;
    uint32_t prog_crc;
} DFU_STATE_INFORMATION_T;

typedef enum DFU_PACKET_TYPE
{
    DFU_PACKET_START,
    DFU_PACKET_DATA,
    DFU_PACKET_ACK
} DFU_PACKET_TYPE_E;

typedef struct PACKED packet_dfu_header
{
    uint8_t SOF;
    uint8_t packet_type;
    uint16_t payload_length;
} packet_dfu_header_t;

typedef struct PACKED packet_dfu_prog_info
{
    uint32_t prog_size;
    uint32_t prog_crc;
    uint8_t reserved[4];
} packet_dfu_prog_info_t;

extern DFU_STATE_INFORMATION_T dfu_state;

/**
 * @brief Initialize the DFU module.
 * 
 * @return DFU_STATUS_E 
 */
DFU_STATUS_E dfu_init(void);

/**
 * @brief Runs the DFU state machine.
 * 
 * @return DFU_STATUS_E 
 */
DFU_STATUS_E dfu_run(void);

/**
 * @brief Acknowledges the last packet received from the DFU host.
 * 
 * @param payload_len length of the last packet received from the DFU host.
 * @return DFU_STATUS_E 
 */
DFU_STATUS_E dfu_ack(uint16_t payload_len);

/**
 * @brief Processes the incoming DFU packet and stores required information
 * 
 * @param packet Header of the DFU packet
 * @return DFU_STATUS_E 
 */
DFU_STATUS_E dfu_process_packet(uint8_t* packet);

/**
 * @brief DFU assert error is responsible for moving DFU into error state
 * 
 * @param status status to assert
 * @return if the status is not DFU_STATUS_OK, then the DFU state is set to DFU_STATE_ERROR and the function returns true. Otherwise, the function returns false.
 */
bool dfu_assert_error(DFU_STATUS_E status);

#endif // DFU_H