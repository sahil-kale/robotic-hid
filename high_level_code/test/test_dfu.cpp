#include "CppUTest/TestHarness.h"
#include <CppUTestExt/MockSupport_c.h>

extern "C" {
#include "dfu.h"
#include "hal_dfu.h"
}

// create a test group
TEST_GROUP(dfu_tests){
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
TEST (dfu_tests, dfu_ack_check){
    uint16_t test_data_len = 500; //in hex, 0x0150
    uint8_t compare_buffer[] = {0x01, DFU_PACKET_ACK, 0x01, 0xF4};

    mock_c()->expectOneCall("send_data_to_dfu_host")->withUnsignedLongIntParameters("size", sizeof(compare_buffer))->withMemoryBufferParameter("data", compare_buffer, sizeof(compare_buffer));

    dfu_ack(test_data_len);
}

//Test the DFU assert function that moves it to the DFU error state
TEST(dfu_tests, dfu_assert_test)
{
    dfu_state = DFU_STATE_START;
    bool value = dfu_assert_error(DFU_STATUS_OK);
    CHECK_EQUAL(DFU_STATE_START, dfu_state);
    CHECK_EQUAL(false, value);

    //Now, check what happens in the event of an error
    value = dfu_assert_error(DFU_STATUS_ERROR);
    CHECK_EQUAL(true, value);
    CHECK_EQUAL(DFU_STATE_ERROR, dfu_state);
    
}

//Test the DFU packet handle function