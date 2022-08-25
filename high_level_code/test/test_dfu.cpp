#include "CppUTest/TestHarness.h"
#include <CppUTestExt/MockSupport_c.h>
#include <iostream>

extern "C" {
#include "dfu.h"
#include "hal_dfu.h"
#include "utility.h"
#include "common.h"
#include <cstring> 
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"

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
    uint16_t test_payload_len = 500; //in hex, 0x0150
    uint8_t compare_buffer[] = {0x01, DFU_PACKET_ACK, 0x01, 0xF4};

    mock_c()->expectOneCall("send_data_to_dfu_host")->withUnsignedLongIntParameters("size", sizeof(compare_buffer))->withMemoryBufferParameter("data", compare_buffer, sizeof(compare_buffer));

    dfu_ack(test_payload_len);
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
    mock_c()->ignoreOtherCalls();
    //Set DFU state struct to 0 using memset
    memset(&dfu_state, 0, sizeof(dfu_state));
    dfu_state.state = DFU_STATE_START;
    packet_dfu_header_t test_packet_header;
    packet_dfu_prog_info_t test_packet_prog_info;


    test_packet_header.SOF = DFU_SOF_identifier;
    test_packet_header.packet_type = DFU_PACKET_START;
    test_packet_header.payload_length = swap_uint16(sizeof(packet_dfu_header_t) + sizeof(packet_dfu_prog_info_t));

    test_packet_prog_info.prog_size = swap_uint32(0x000010BC);
    test_packet_prog_info.prog_crc = swap_uint32(0x12345678);
    //Set the reserved fields to 0 using memset
    memset(&test_packet_prog_info.reserved, 0, sizeof(test_packet_prog_info.reserved));

    //Copy into test packet
    uint8_t test_packet[sizeof(packet_dfu_header_t) + sizeof(packet_dfu_prog_info_t)] = {0};
    std::memcpy(test_packet, &test_packet_header, sizeof(packet_dfu_header_t));
    std::memcpy(test_packet + sizeof(packet_dfu_header_t), &test_packet_prog_info, sizeof(packet_dfu_prog_info_t));

    //Expect flash to be erashed
    mock_c()->expectOneCall("hal_dfu_eraseflash")->withUnsignedLongIntParameters("baseSector", APP_SECTOR_START)->withUnsignedLongIntParameters("noSectors", APP_SECTOR_END - APP_SECTOR_START + 1);
    //Expect acknowledgement to be sent
    uint8_t ack_buffer[4] = {0x01, DFU_PACKET_ACK, 0x00, 0x00};
    //Append payload length to the end of ack_buffer
    ack_buffer[2] = test_packet_header.payload_length & 0xFF;
    ack_buffer[3] = test_packet_header.payload_length >> 8;

    mock_c()->expectOneCall("send_data_to_dfu_host")->withUnsignedIntParameters("size", sizeof(packet_dfu_header_t))->withMemoryBufferParameter("data", ack_buffer, sizeof(ack_buffer));

    DFU_STATUS_E returnState = dfu_process_packet((uint8_t *)test_packet);

    //Check if the DFU status struct equals the above parameters
    CHECK_EQUAL(0x000010BC, dfu_state.prog_size);
    CHECK_EQUAL(0x12345678, dfu_state.prog_crc);
    CHECK_EQUAL(DFU_STATE_DATA_EXCHANGE, dfu_state.state);
    CHECK_EQUAL(0, dfu_state.bytes_sent);

    

    CHECK_EQUAL(DFU_STATUS_OK, returnState);

}

//test to ensure that DFU invalid packet type is handled correctly when in wrong state (this case, it will be in start and sent a data packet)


#pragma GCC diagnostic pop