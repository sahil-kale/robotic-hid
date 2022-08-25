#include "CppUTest/TestHarness.h"
#include <CppUTestExt/MockSupport_c.h>
#include <iostream>

extern "C" {
#include "dfu.h"
#include "hal_dfu.h"
#include "utility.h"
#include <cstring> 
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
    dfu_state.state = DFU_STATE_START;
    bool value = dfu_assert_error(DFU_STATUS_OK);
    CHECK_EQUAL(DFU_STATE_START, dfu_state.state);
    CHECK_EQUAL(false, value);

    //Now, check what happens in the event of an error
    value = dfu_assert_error(DFU_STATUS_ERROR);
    CHECK_EQUAL(true, value);
    CHECK_EQUAL(DFU_STATE_ERROR, dfu_state.state);
    
}

//Test the DFU packet handle function when presented with a start packet
TEST(dfu_tests, dfu_process_start_packet)
{
    dfu_state.state = DFU_STATE_START;
    packet_dfu_header_t test_packet_header;
    packet_dfu_prog_info_t test_packet_prog_info;


    test_packet_header.SOF = DFU_SOF_identifier;
    test_packet_header.packet_type = DFU_PACKET_START;
    test_packet_header.data_length = swap_uint16(sizeof(packet_dfu_header_t) + sizeof(packet_dfu_prog_info_t));

    test_packet_prog_info.prog_size = swap_uint32(0x000010BC);
    test_packet_prog_info.prog_crc = swap_uint32(0x12345678);
    //Set the reserved fields to 0 using memset
    memset(&test_packet_prog_info.reserved, 0, sizeof(test_packet_prog_info.reserved));

    uint8_t test_packet[sizeof(packet_dfu_header_t) + sizeof(packet_dfu_prog_info_t)] = {0};
    std::memcpy(test_packet, &test_packet_header, sizeof(packet_dfu_header_t));
    std::memcpy(test_packet + sizeof(packet_dfu_header_t), &test_packet_prog_info, sizeof(packet_dfu_prog_info_t));

    //Expect flash to be erashed
    mock_c()->expectOneCall("hal_dfu_eraseflash")->withUnsignedLongIntParameters("baseSector", 0)->withUnsignedLongIntParameters("noSectors", 1);

    //Check if the DFU status struct equals the above parameters
    CHECK_EQUAL(0x000010BC, dfu_state.prog_size);
    CHECK_EQUAL(0x12345678, dfu_state.prog_crc);
    CHECK_EQUAL(DFU_STATE_DATA_EXCHANGE, dfu_state.state);
    CHECK_EQUAL(0, dfu_state.bytes_sent);

    DFU_STATUS_E returnState = dfu_process_packet((uint8_t *)test_packet);
    CHECK_EQUAL(DFU_STATUS_OK, returnState);

}