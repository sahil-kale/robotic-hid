/**
 * @file ads130b04.h
 * @author sahil kale
 * @brief 
 * @version 0.1
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 * Note: this driver is inspired by https://github.com/LucasEtchezuri/Arduino-ADS131M04
 * 
 * NOTE: This driver only accomodates for 24-bit word size as it is the default.
 * 
 */

#ifndef ADS130B04_H
#define ADS130B04_H
#include "adc.h" //Get ADC data types

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
 * @return uint16_t value of register
 */
uint16_t readRegister(uint8_t reg);

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

/**
 * @brief Set the Channel PGA gain
 * 
 * @param channel channel to set gain for
 * @param pga programmable gain amplifier gain
 */
void setChannelPGA(uint8_t channel, uint16_t pga);

#endif /* ADS130B04_H */