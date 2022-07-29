#include "adc.h"
#ifdef USE_ADS130B04
//In order to avoid the compilier warning "Conversion"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wunused-const-variable"

#include "hal_ads130b04.h"
#include "ads130b04.h"
#include <stdio.h>
#include <string.h>

//PRIVATE DECLARATIONS
#define ADS130B04_SPI_WORD_SIZE_BYTES (3U)

//Register addresses
static const uint16_t ADS130B04_REG_ID = 0x00;
static const uint16_t ADS130B04_REG_STATUS = 0x01;
static const uint16_t ADS130B04_REG_MODE = 0x02;
static const uint16_t ADS130B04_REG_CLOCK = 0x03;
static const uint16_t ADS130B04_REG_GAIN = 0x04;
static const uint16_t ADS130B04_REG_GLOBAL_CHOP_OFFSET = 0x06;

//Other assumptions
static const uint8_t ADS130B04_NUM_CHANNELS = 4U;

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
    uint8_t empty_buffer[ADS130B04_SPI_WORD_SIZE_BYTES] = {0};
    uint8_t working_buffer[ADS130B04_SPI_WORD_SIZE_BYTES] = {0};

    //Initaite a SPI transfer with empty buffer as tx
    ads130b04_spi_transfer(empty_buffer, working_buffer, ADS130B04_SPI_WORD_SIZE_BYTES);
    
    //Copy the status register, which is made up of the 1st 16 bits of working_buffer
    data.status_register = working_buffer[0] | (working_buffer[1] << 8);

    //Clear the working buffer
    memset(working_buffer, 0, ADS130B04_SPI_WORD_SIZE_BYTES);

    for(size_t i = 0; i< ADS130B04_NUM_CHANNELS; i++)
    {
        memset(working_buffer, 0, ADS130B04_SPI_WORD_SIZE_BYTES);
        //Initaite a SPI transfer with empty buffer as tx
        ads130b04_spi_transfer(empty_buffer, working_buffer, ADS130B04_SPI_WORD_SIZE_BYTES);
        //Copy the ADC data, which is made up of the 1st 16 bits of working_buffer, into the appropriate channel
        data.adc_data[i] = working_buffer[1] | (working_buffer[0] << 8);
    }

    //CRC call
    ads130b04_spi_transfer(empty_buffer, working_buffer, ADS130B04_SPI_WORD_SIZE_BYTES);

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

void writeRegisterMasked(uint8_t reg, uint16_t value, uint16_t mask)
{
    uint16_t regValue = readRegister(reg);
    regValue &= ~mask;
    regValue |= (value & mask);
    writeRegister(reg, regValue);
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


/******************/
//Individual register stuff

void setClock(bool use_external, uint8_t OSR)
{
    static const uint8_t CLK_SEL_BIT_POS = 7U;
    static const uint8_t OSR_BIT_POS = 2U;
    static const uint16_t setClockMask = 0x00B4U; //0b10011100;
    uint16_t value_to_write = use_external << CLK_SEL_BIT_POS;
    value_to_write |= ((OSR & 0x07) << OSR_BIT_POS);

    writeRegisterMasked(ADS130B04_REG_CLOCK, value_to_write, setClockMask);
}

void setGlobalChop(uint8_t gc_delay, bool enable)
{
    static const uint8_t GC_DELAY_BIT_POS = 9U;
    static const uint8_t GC_EN_BIT_POS = 8U;
    static const uint16_t setGlobalChopMask = 0x1F00U; //0b0001111100000000;

    uint16_t value_to_write = (gc_delay & 0x0F) << GC_DELAY_BIT_POS;
    value_to_write |= (enable << GC_EN_BIT_POS);
    writeRegisterMasked(ADS130B04_REG_GLOBAL_CHOP_OFFSET, value_to_write, setGlobalChopMask);

}

void setChannelPGA(uint8_t channel, uint16_t pga)
{
    uint8_t channel_offset = channel * 4U;
    uint16_t mask = 0x07U << (channel_offset);
    uint16_t value_to_write = (pga & 0x07) << (channel_offset);

    writeRegisterMasked(ADS130B04_REG_GAIN, value_to_write, mask);
}

#pragma GCC diagnostic pop
#endif // USE_ADS130B04