#include "adc.h"
#ifdef USE_ADS130B04
#include "hal_ads130b04.h"
#include <stdio.h>

//PRIVATE DECLARATIONS

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

//PRIVATE DEFINITIONS

#endif // USE_ADS130B04