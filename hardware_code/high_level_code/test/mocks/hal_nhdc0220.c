#include "hal_nhdc0220.h"
#include <CppUTestExt/MockSupport_c.h>

void hal_nhdc0220_init(void)
{
    mock_c()->actualCall("hal_nhdc0220_init");
}

void nhdc0220_i2c_beginTransmission(uint8_t address, bool write)
{
    mock_c()->actualCall("nhdc0220_i2c_beginTransmission")->withUnsignedIntParameters("address", address)->withBoolParameters("write", write);
}

void nhdc0220_i2c_write(uint8_t data[], uint8_t len)
{
    mock_c()->actualCall("nhdc0220_i2c_write")->withMemoryBufferParameter("data", data, len);
}

void nhdc0220_i2c_endTransmission(void)
{
    mock_c()->actualCall("nhdc0220_i2c_endTransmission");
}
