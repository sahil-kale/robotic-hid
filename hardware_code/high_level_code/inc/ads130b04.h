#ifndef ADS130B04_H
#define ADS130B04_H
#include "adc.h" //Get ADC data types

//Note: this driver is inspired by https://github.com/LucasEtchezuri/Arduino-ADS131M04

/**
 * @brief Set the Channel PGA gain
 * 
 * @param channel channel to set gain for
 * @param pga programmable gain amplifier gain
 */
void setChannelPGA(uint8_t channel, uint16_t pga);

/**
 * @brief Write to single register
 * 
 * @param reg register address
 * @param value Value to write to register
 */
void writeRegister(uint8_t reg, uint16_t value);

/**
 * @brief write to multiple registers
 * 
 * @param reg register address
 * @param value value to write to register
 * @param mask Mask to apply to register to only set bits that are set in mask
 */
void writeRegisterMasked(uint8_t reg, uint8_t value, uint8_t mask);

/**
 * @brief Read from single register
 * 
 * @param reg register address
 * @return uint16_t value read from register
 */
void readRegister(uint8_t reg, uint16_t *value);

/**
 * @brief Read from multiple registers
 * 
 * @param reg register address
 * @param value value to write to register
 * @param mask Mask to apply to register to only set bits that are set in mask
 */
void readRegisterMasked(uint8_t reg, uint8_t *value, uint8_t mask);

/**
 * @brief Set the Global Chop variable
 * 
 * @param enable 
 */
void setGlobalChop(bool enable);

/**
 * @brief Set the Clock Source bit
 * 
 * @param use_external Use external clock
 * @param OSR oversample rate
 * 
 * @note this driver assumes that the clock ONLY uses the high power mode. 
 */
void setClock(bool use_external, uint8_t OSR);

#endif /* ADS130B04_H */