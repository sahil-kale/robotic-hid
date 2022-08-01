#include "CppUTest/TestHarness.h"
#include <CppUTestExt/MockSupport_c.h>

extern "C" {
#include "lcd.h"
#include "nhdc0220.h"
#include "hal_nhdc0220.h"
#include <cstring> //for memcpy
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
    uint8_t expected_data[] = {0x00, 0x12};
    mock_c()->expectOneCall("nhdc0220_i2c_write")
    ->withUnsignedIntParameters("address", LCD_I2C_ADDR)
    ->withBoolParameters("write", true)
    ->withMemoryBufferParameter("data", expected_data, sizeof(expected_data));


    nhdc0220_command(expected_data[1]);
}

TEST(nhdc0220_test, write_lcd)
{
    mock_c()->clear();
    //Test data
    char test_data[] = "Hello World!";

    //Create expected buffer, consisting of the RAM write command and the test data
    uint8_t expected_data[sizeof(test_data) + 1];
    expected_data[0] = 0x40;
    memcpy(expected_data + 1, test_data, sizeof(test_data));
    mock_c()->expectOneCall("nhdc0220_i2c_write")
    ->withUnsignedIntParameters("address", LCD_I2C_ADDR)
    ->withBoolParameters("write", true)
    ->withMemoryBufferParameter("data", expected_data, sizeof(expected_data));
    
    write_lcd(test_data, sizeof(test_data));
}

TEST(nhdc0220_test, set_lcd_cursor_test)
{
    //Test if the set_lcd_cursor function is called correctly
    uint8_t test_row = 1;
    uint8_t test_col = 2;
    //1 << 7 to hex is 
    uint8_t expected_data[] = {0x00, 0x80+0x40+2};
    mock_c()->expectOneCall("nhdc0220_i2c_write")
    ->withUnsignedIntParameters("address", LCD_I2C_ADDR)
    ->withBoolParameters("write", true)
    ->withMemoryBufferParameter("data", expected_data, sizeof(expected_data));

    set_lcd_cursor(test_row, test_col);
}

TEST(nhdc0220_test, clear_lcd_test)
{
    //Test if the clear_lcd function is called correctly
    uint8_t expected_data[] = {0x00, 0x01};
    mock_c()->expectOneCall("nhdc0220_i2c_write")
    ->withUnsignedIntParameters("address", LCD_I2C_ADDR)
    ->withBoolParameters("write", true)
    ->withMemoryBufferParameter("data", expected_data, sizeof(expected_data));

    clear_lcd();
}

#pragma GCC diagnostic pop
