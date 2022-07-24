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
#define ADS130B04_SPI_WORD_SIZE_BYTES (24U)

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



uint16_t readRegister(uint8_t reg)
{
    static const uint16_t CMD_RREG = 0xA000U; //0b1010 0000 0000 0000
    static const uint16_t CMD_RREG_ADDR_OFFSET = 13U;
    
    uint8_t txBuffer[ADS130B04_SPI_WORD_SIZE_BYTES] = {0};
    uint8_t rxBuffer[ADS130B04_SPI_WORD_SIZE_BYTES] = {0};

    uint16_t command = CMD_RREG;

    uint16_t addr = reg;
    command |= (addr << CMD_RREG_ADDR_OFFSET); //append address
    //Copy the data into txBuffer, but can't use memset as this is little endian
    txBuffer[0] = (command >> 8) & 0xFF;
    txBuffer[1] = command & 0xFF;

    //Send the command
    ads130b04_spi_transfer(txBuffer, sizeof(txBuffer), rxBuffer, sizeof(rxBuffer));
    memset(txBuffer, 0, sizeof(txBuffer)); //Clear the TX buffer
    //we now need to read the ADC data for 5 cycles (4 channels + CRC)
    for(size_t i = 0; i < 5; i++)
    {
        ads130b04_spi_transfer(txBuffer, sizeof(txBuffer), rxBuffer, sizeof(rxBuffer));
    }

    //Read the response which should contain RREG response
    ads130b04_spi_transfer(txBuffer, sizeof(txBuffer), rxBuffer, sizeof(rxBuffer));
    uint16_t returnValue = rxBuffer[0] | (rxBuffer[1] << 8);
    for(size_t i = 0; i < 5; i++)
    {
        ads130b04_spi_transfer(txBuffer, sizeof(txBuffer), rxBuffer, sizeof(rxBuffer));
    }
    return returnValue;
}

#pragma GCC diagnostic pop
#endif // USE_ADS130B04