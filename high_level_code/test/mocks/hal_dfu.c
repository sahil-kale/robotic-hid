#include <CppUTestExt/MockSupport_c.h>
#include "hal_dfu.h"

DFU_data_handle_t get_data_from_dfu_host(void)
{
    DFU_data_handle_t default_return;
    mock_c()->actualCall("get_data_from_dfu_host");

    return *(DFU_data_handle_t *)mock_c()->returnPointerValueOrDefault(&default_return);
}

void send_data_to_dfu_host(DFU_data_handle_t data)
{
    mock_c()->actualCall("send_data_to_dfu_host")->withUnsignedLongIntParameters("size", data.size)->withMemoryBufferParameter("data", data.data, data.size);
}