#include <CppUTestExt/MockSupport_c.h>
#include "hal_ads130b04.h"

void set_sync_reset_pin(bool value)
{
    mock_c()->actualCall("set_sync_reset_pin")->withBoolParameters("value", value);
}

void ads130b04_spi_transfer(uint8_t *txbuffer, uint8_t txlen, uint8_t *rxbuffer, uint8_t rxlen)
{
    mock_c()->actualCall("ads130b04_spi_transfer")->withPointerParameters("txbuffer", txbuffer)->withUnsignedIntParameters("txlen", txlen)->withOutputParameter("rxbuffer", rxbuffer)->withOutputParameter("rxlen", &rxlen);


}
