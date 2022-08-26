#include "CppUTest/TestHarness.h"
#include <CppUTestExt/MockSupport_c.h>
extern "C" {
    #include "bootloader_app.h"
    #include "hal_bootloader_app.h"
    #include "hal_dfu.h"
    #include "common.h"
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

//Test that the launch app function is called if the application is valid and no dfu request is made
TEST(bootloader_test, launch_app_if_valid_app)
{
    application_info_flash_t test_info;
    test_info.application_size = 0x12345678;
    test_info.application_crc = 0x87654321;
    test_info.dfu_request = false;
    test_info.flash_valid = true;
    mock_c()->expectOneCall("read_application_info")->andReturnPointerValue((void*)&test_info);

    //Expect a call for valid CRC
    mock_c()->expectOneCall("hal_dfu_validate_crc")->withUnsignedLongIntParameters("address", APP_START_ADDRESS)->withUnsignedLongIntParameters("size", test_info.application_size)->withUnsignedLongIntParameters("crc", test_info.application_crc)->andReturnBoolValue(true);

    //Expect a call to launch app
    mock_c()->expectOneCall("hal_jump_to_app");

    bootloader_current_state = BOOTLOADER_STATE_LAUNCH_APP;

    bootloader_app_run();

    mock_c()->checkExpectations();
}

//Test that the bootloader moves to the HALT state if the application is invalid
TEST(bootloader_test, halt_if_invalid_app)
{
    application_info_flash_t test_info;
    test_info.application_size = 0x12345678;
    test_info.application_crc = 0x87654321;
    test_info.dfu_request = false;
    test_info.flash_valid = false;
    mock_c()->expectOneCall("read_application_info")->andReturnPointerValue((void*)&test_info);
    bootloader_current_state = BOOTLOADER_STATE_LAUNCH_APP;
    bootloader_app_run();
    CHECK_EQUAL(BOOTLOADER_STATE_HALT, bootloader_current_state);
}

//Test that the bootloader in HALT status turns DFU request on
TEST(bootloader_test, dfu_request_on_halt)
{
    mock_c()->ignoreOtherCalls();
    application_info_flash_t test_info;
    test_info.application_size = 0x00;
    test_info.application_crc = 0x00;
    test_info.dfu_request = true;
    test_info.flash_valid = false;
    mock_c()->expectOneCall("write_application_info")->withMemoryBufferParameter("info", (uint8_t *)&test_info, sizeof(test_info));
    bootloader_current_state = BOOTLOADER_STATE_HALT;
    bootloader_app_run();
    mock_c()->checkExpectations();
}
