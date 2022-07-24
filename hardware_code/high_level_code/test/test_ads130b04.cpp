#include "CppUTest/TestHarness.h"

extern "C" {
#include "adc.h"
}



// create a test group
TEST_GROUP(ads130b04_test){
   
};

//create a test for that test group
TEST(ads130b04_test, sync_reset_test){
    adc_data_t data = get_adc_data();
    CHECK_EQUAL(0, data.adc_data[0]);
}

