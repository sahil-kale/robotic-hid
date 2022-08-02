#ifndef ADC_H
#define ADC_H
#include <stdint.h>

#define USE_ADS130B04
#define NUM_ADC_CHANNELS (4UL)

//Define struct to return ADC data in
typedef struct
{
    uint16_t status_register;
    int16_t adc_data[NUM_ADC_CHANNELS];
} adc_data_t;


/**
 * @brief Initalize the ADC
 * 
 */
void init_adc(void);

/**
 * @brief Read the ADC and return the data. Note that this initates a call to read the analog values
 * 
 * @param adc_data_ptr adc_data_t populated with adc_data
 */
adc_data_t get_adc_data(void);

#endif
