#include "hal_bootloader_app.h"
#include <CppUTestExt/MockSupport_c.h>

void hal_reset(void)
{
    mock_c()->actualCall("hal_reset");
}

void hal_jump_to_app(void)
{
    mock_c()->actualCall("hal_jump_to_app");
}

void write_application_info(application_info_flash_t* info)
{
    mock_c()->actualCall("write_application_info")->withMemoryBufferParameter("info", (uint8_t *)info, sizeof(application_info_flash_t));
}

application_info_flash_t read_application_info()
{
    mock_c()->actualCall("read_application_info");

    //default value
    application_info_flash_t default_return;        
    return *(application_info_flash_t *)mock_c()->returnPointerValueOrDefault(&default_return);
}
