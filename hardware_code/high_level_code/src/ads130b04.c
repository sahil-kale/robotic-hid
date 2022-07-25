#include "adc.h"
#ifdef USE_ADS130B04
//In order to avoid the compilier warning "Conversion"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"

#include "hal_ads130b04.h"
#include "ads130b04.h"
#include <stdio.h>
#include <string.h>

//PRIVATE DECLARATIONS
#define ADS130B04_SPI_WORD_SIZE_BYTES (3U)

//PUBLIC DEFINITIONS
void init_adc(void)
{
    set_sync_reset_pin(true);
    for(size_t i = 0; i<2000; i++); //Crude Delay but fast enough for the ADS130B04 to reset.
    set_sync_reset_pin(false);

}

adc_data_t get_adc_data(void)
{
    adc_data_t data = {0};

    //Read the data from the ADS130B04
    //ads130b04_spi_transfer(NULL, 0, (uint8_t*)&data, sizeof(data));

    return data;
}

void writeRegister(uint8_t reg, uint16_t value)
{
    static const uint16_t CMD_WREG = 0x6000U; //0b01100000;
    static const uint16_t CMD_WREG_ADDR_OFFSET = 7U;

    uint8_t txBuffer[ADS130B04_SPI_WORD_SIZE_BYTES] = {0};
    uint8_t rxBuffer[ADS130B04_SPI_WORD_SIZE_BYTES] = {0};

    //Load the txBuffer with the command and the address
    uint16_t command = CMD_WREG;
    uint16_t addr = reg & 0x7FU; //0b01111111;
    command |= (addr << CMD_WREG_ADDR_OFFSET);

    //copy the command to the txBuffer
    txBuffer[0] = (command >> 8) & 0xFF;
    txBuffer[1] = command & 0xFF;

    //Send the command
    ads130b04_spi_transfer(txBuffer, rxBuffer, sizeof(txBuffer));
    //clear the txBuffer
    memset(txBuffer, 0, sizeof(txBuffer));
    //Load the txBuffer with the value to write to the register
    txBuffer[0] = (value >> 8) & 0xFF;
    txBuffer[1] = value & 0xFF;
    //Send the value
    ads130b04_spi_transfer(txBuffer, rxBuffer, sizeof(txBuffer));
    //clear the txBuffer
    memset(txBuffer, 0, sizeof(txBuffer));

    //Loop through 4 more calls to get to response
    for(size_t i = 0; i < 4; i++)
    {
        ads130b04_spi_transfer(txBuffer, rxBuffer, sizeof(txBuffer));
    }

    //read the response
    ads130b04_spi_transfer(txBuffer, rxBuffer, sizeof(txBuffer));
    //uint16_t returnData = uint16_t(rxBuffer[0] << 8) | uint16_t(rxBuffer[1]);

    for(size_t i = 0; i < 5; i++)
    {
        ads130b04_spi_transfer(txBuffer, rxBuffer, sizeof(txBuffer));
    }

}


uint16_t readRegister(uint8_t reg)
{
    static const uint16_t CMD_RREG = 0xA000U; //0b1010 0000 0000 0000
    static const uint16_t CMD_RREG_ADDR_OFFSET = 7U;
    
    uint8_t txBuffer[ADS130B04_SPI_WORD_SIZE_BYTES] = {0};
    uint8_t rxBuffer[ADS130B04_SPI_WORD_SIZE_BYTES] = {0};

    uint16_t command = CMD_RREG;

    uint16_t addr = reg & 0x7FU; //0b0111 1111 as there are only 7 bits allowed for the address.
    command |= (addr << CMD_RREG_ADDR_OFFSET); //append address
    //Copy the data into txBuffer
    txBuffer[0] = (command >> 8) & 0xFF;
    txBuffer[1] = command & 0xFF;

    //Send the command
    ads130b04_spi_transfer(txBuffer, rxBuffer, sizeof(txBuffer));
    memset(txBuffer, 0, sizeof(txBuffer)); //Clear the TX buffer
    //we now need to read the ADC data for 5 cycles (4 channels + CRC)

    for(size_t i = 0; i < 5; i++)
    {
        ads130b04_spi_transfer(txBuffer, rxBuffer, sizeof(txBuffer));
    }

    //Read the response which should contain RREG response
    ads130b04_spi_transfer(txBuffer, rxBuffer, sizeof(txBuffer));
    uint16_t returnValue = rxBuffer[1] | (rxBuffer[0] << 8);
    for(size_t i = 0; i < 5; i++)
    {
        ads130b04_spi_transfer(txBuffer, rxBuffer, sizeof(txBuffer));
    }
    return returnValue;
}

#pragma GCC diagnostic pop
#endif // USE_ADS130B04