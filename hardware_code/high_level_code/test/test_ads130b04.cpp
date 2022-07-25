#include "CppUTest/TestHarness.h"
#include <CppUTestExt/MockSupport_c.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"

extern "C" {
#include "adc.h"
#include "ads130b04.h"
}



// create a test group
TEST_GROUP(ads130b04_test){

    void setup()
    {
        mock_c()->clear();
    }

    void teardown()
    {
        mock_c()->clear(); //if you ever get a bunch of memory leak errors, it's cause you forgot this line
    }
   
};

TEST(ads130b04_test, read_register_message_generation)
{
    //Test Conditions, register = 0, return value = 0x1234
    uint8_t register_value = 0x3FU;
    uint16_t return_value = 0x1234U;
    uint8_t testTxBuffer[3] = {0};
    testTxBuffer[0] = (0xA0U) | (register_value >> 1U);
    testTxBuffer[1] = (register_value & 1U) << 7U;
    uint8_t testRxBuffer[3] = {0x12, 0x34, 0x00};
    uint8_t emptyBuffer[3] = {0};



    mock_c()->ignoreOtherCalls();
    mock_c()->expectOneCall("ads130b04_spi_transfer")
            ->withMemoryBufferParameter("txBuffer", testTxBuffer, sizeof(testTxBuffer))
            ->withOutputParameterReturning("rxBuffer", emptyBuffer, sizeof(emptyBuffer));
    
    for(size_t i = 0; i < 5; i++)
    {
        mock_c()->expectOneCall("ads130b04_spi_transfer")
                ->withMemoryBufferParameter("txBuffer", emptyBuffer, sizeof(emptyBuffer))
                ->withOutputParameterReturning("rxBuffer", emptyBuffer, sizeof(emptyBuffer));
    }

    mock_c()->expectOneCall("ads130b04_spi_transfer")
            ->withMemoryBufferParameter("txBuffer", emptyBuffer, sizeof(emptyBuffer))
            ->withOutputParameterReturning("rxBuffer", testRxBuffer, sizeof(testRxBuffer));

    for(size_t i = 0; i < 5; i++)
    {
        mock_c()->expectOneCall("ads130b04_spi_transfer")
                ->withMemoryBufferParameter("txBuffer", emptyBuffer, sizeof(emptyBuffer))
                ->withOutputParameterReturning("rxBuffer", emptyBuffer, sizeof(emptyBuffer));
    }
    
    uint16_t result = readRegister(register_value);

    CHECK_EQUAL(return_value, result);

}

TEST(ads130b04_test, read_register_message_generation_clock_register)
{
    //Test Conditions, register = 0, return value = 0x1234
    uint8_t register_value = 0x03U;
    uint16_t return_value = 0xFFFFU;
    uint8_t testTxBuffer[3] = {0};
    testTxBuffer[0] = (0xA0U) | (register_value >> 1U);
    testTxBuffer[1] = (register_value & 1U) << 7U;
    uint8_t testRxBuffer[3] = {0xFF, 0xFF, 0x00};
    uint8_t emptyBuffer[3] = {0};



    mock_c()->ignoreOtherCalls();
    mock_c()->expectOneCall("ads130b04_spi_transfer")
            ->withMemoryBufferParameter("txBuffer", testTxBuffer, sizeof(testTxBuffer))
            ->withOutputParameterReturning("rxBuffer", emptyBuffer, sizeof(emptyBuffer));
    
    for(size_t i = 0; i < 5; i++)
    {
        mock_c()->expectOneCall("ads130b04_spi_transfer")
                ->withMemoryBufferParameter("txBuffer", emptyBuffer, sizeof(emptyBuffer))
                ->withOutputParameterReturning("rxBuffer", emptyBuffer, sizeof(emptyBuffer));
    }

    mock_c()->expectOneCall("ads130b04_spi_transfer")
            ->withMemoryBufferParameter("txBuffer", emptyBuffer, sizeof(emptyBuffer))
            ->withOutputParameterReturning("rxBuffer", testRxBuffer, sizeof(testRxBuffer));

    for(size_t i = 0; i < 5; i++)
    {
        mock_c()->expectOneCall("ads130b04_spi_transfer")
                ->withMemoryBufferParameter("txBuffer", emptyBuffer, sizeof(emptyBuffer))
                ->withOutputParameterReturning("rxBuffer", emptyBuffer, sizeof(emptyBuffer));
    }
    
    uint16_t result = readRegister(register_value);

    CHECK_EQUAL(return_value, result);

}


//create a test for that test group
TEST(ads130b04_test, write_register_message_generation){
    //Test conditions, register = 0x03, value = 0x1234
    uint8_t register_value = 0x03U;
    uint16_t value = 0x1234U;

    uint8_t testTxBuffer[3] = {0};
    testTxBuffer[0] = (0x60U) | (register_value >> 1U);
    testTxBuffer[1] = (register_value & 1U) << 7U;

    uint8_t testTxBufferWithValue[3] = {0};
    testTxBufferWithValue[0] = value >> 8U & 0xFF;
    testTxBufferWithValue[1] = value & 0xFF;

    uint8_t emptyBuffer[3] = {0};

    //This driver doesn't care about return data. Assert that it calls the SPI driver with the correct data.
    mock_c()->ignoreOtherCalls();
    mock_c()->expectOneCall("ads130b04_spi_transfer")
            ->withMemoryBufferParameter("txBuffer", testTxBuffer, sizeof(testTxBuffer))
            ->withOutputParameterReturning("rxBuffer", emptyBuffer, sizeof(emptyBuffer));

    mock_c()->expectOneCall("ads130b04_spi_transfer")
            ->withMemoryBufferParameter("txBuffer", testTxBufferWithValue, sizeof(testTxBufferWithValue))
            ->withOutputParameterReturning("rxBuffer", emptyBuffer, sizeof(emptyBuffer));

    for(size_t i = 0; i < 10; i++)
    {
        mock_c()->expectOneCall("ads130b04_spi_transfer")
                ->withMemoryBufferParameter("txBuffer", emptyBuffer, sizeof(emptyBuffer))
                ->withOutputParameterReturning("rxBuffer", emptyBuffer, sizeof(emptyBuffer));
    }

    writeRegister(register_value, value);
}

#pragma GCC diagnostic pop
