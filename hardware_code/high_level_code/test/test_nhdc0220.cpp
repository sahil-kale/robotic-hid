#include "CppUTest/TestHarness.h"
#include <CppUTestExt/MockSupport_c.h>

extern "C" {
#include "lcd.h"
#include "nhdc0220.h"
#include "hal_nhdc0220.h"
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"

#define LCD_I2C_ADDR (0x78UL)

// create a test group
TEST_GROUP(nhdc0220_test){
   void setup()
    {
        mock_c()->clear();
    }

    void teardown()
    {
        mock_c()->clear(); //if you ever get a bunch of memory leak errors, it's cause you forgot this line
    }
};

TEST(nhdc0220_test, command_func_test)
{
    mock_c()->expectOneCall("nhdc0220_i2c_beginTransmission")->withUnsignedIntParameters("address", LCD_I2C_ADDR)->withBoolParameters("write", true);
    uint8_t expected_data[] = {0x00, 0x12};
    mock_c()->expectOneCall("nhdc0220_i2c_write")->withMemoryBufferParameter("data", expected_data, sizeof(expected_data));
    //Expect end transmission
    mock_c()->expectOneCall("nhdc0220_i2c_endTransmission");


    nhdc0220_command(expected_data[1]);
}

#pragma GCC diagnostic pop
