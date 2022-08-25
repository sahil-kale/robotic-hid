#ifndef HAL_DFU_H
#define HAL_DFU_H
#include "stdint.h"

typedef struct
{
    size_t size;
    uint8_t *data;
} DFU_data_handle_t;

DFU_data_handle_t get_data_from_dfu_host(void);

void send_data_to_dfu_host(DFU_data_handle_t data);

#endif // HAL_DFU_H
