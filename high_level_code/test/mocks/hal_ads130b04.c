#include <CppUTestExt/MockSupport_c.h>
#include "hal_ads130b04.h"

void set_sync_reset_pin(bool value)
{
    mock_c()->actualCall("set_sync_reset_pin")->withBoolParameters("value", value);
}

void ads130b04_spi_transfer(uint8_t *txBuffer, uint8_t *rxBuffer, uint8_t len)
{
    mock_c()->actualCall("ads130b04_spi_transfer")->withMemoryBufferParameter("txBuffer", txBuffer, len)->withOutputParameter("rxBuffer", rxBuffer);


}

void assert_chip_select(bool value)
{
    mock_c()->actualCall("assert_chip_select")->withBoolParameters("value", value);
}

void init_hal_ads130b04(void)
{
    mock_c()->actualCall("init_hal_ads130b04");
}