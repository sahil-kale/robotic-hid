#include "CppUTest/TestHarness.h"
#include <CppUTestExt/MockSupport_c.h>
extern "C" {
#include "bootloader_app.h"
#include "hal_bootloader_app.h"
}

// create a test group
TEST_GROUP(bootloader_test){
   void setup()
    {
        mock_c()->clear();
    }

    void teardown()
    {
        mock_c()->clear(); //if you ever get a bunch of memory leak errors, it's cause you forgot this line
    } 
};

//create a test for that test group
TEST (bootloader_test, go_to_dfu_state){
    //expect a call to be made to the read_application_info function
    application_info_flash_t test_info;
    test_info.application_size = 0x12345678;
    test_info.application_crc = 0x87654321;
    test_info.dfu_request = true;
    test_info.flash_valid = false;
    mock_c()->expectOneCall("read_application_info")->andReturnPointerValue((void*)&test_info);

    bootloader_state_E state = determine_bootloader_state_from_reset_vector();
    CHECK_EQUAL(BOOTLOADER_STATE_DFU, state);
}

TEST(bootloader_test, go_to_launch_app_if_no_dfu_requested)
{
    application_info_flash_t test_info;
    test_info.application_size = 0x12345678;
    test_info.application_crc = 0x87654321;
    test_info.dfu_request = false;
    test_info.flash_valid = true;
    mock_c()->expectOneCall("read_application_info")->andReturnPointerValue((void*)&test_info);
    bootloader_state_E state = determine_bootloader_state_from_reset_vector();
    CHECK_EQUAL(BOOTLOADER_STATE_LAUNCH_APP, state);
}

