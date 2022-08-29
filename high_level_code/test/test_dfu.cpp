#include "CppUTest/TestHarness.h"
#include <CppUTestExt/MockSupport_c.h>
#include <iostream>

extern "C" {
#include "dfu.h"
#include "hal_dfu.h"
#include "utility.h"
#include "common.h"
#include "hal_bootloader_app.h"
#include <cstring> 
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"

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

    //Expect a call to the application info to be updated
    application_info_flash_t test_application_info;
    test_application_info.application_size = 0x000010BC;
    test_application_info.application_crc = 0x12345678;
    test_application_info.flash_valid = false;
    test_application_info.dfu_request = false;

    mock_c()->expectOneCall("write_application_info")->withMemoryBufferParameter("info", (uint8_t *)&test_application_info, sizeof(test_application_info));


    DFU_STATUS_E returnState = dfu_process_packet((uint8_t *)test_packet);

    mock_c()->checkExpectations();

    //Check if the DFU status struct equals the above parameters
    CHECK_EQUAL(0x000010BC, dfu_state.prog_size);
    CHECK_EQUAL(0x12345678, dfu_state.prog_crc);
    CHECK_EQUAL(DFU_STATE_DATA_EXCHANGE, dfu_state.state);
    CHECK_EQUAL(0, dfu_state.bytes_sent);

    CHECK_EQUAL(DFU_STATUS_OK, returnState);

}

//Test the DFU packet handle function when presented with a data packet
TEST(dfu_tests, dfu_process_data_packet_correctly)
{
    memset(&dfu_state, 0, sizeof(dfu_state));
    dfu_state.state = DFU_STATE_DATA_EXCHANGE;
    dfu_state.bytes_sent = 731;
    dfu_state.prog_size = 0x000010BC;
    dfu_state.prog_crc = 0x12345678;

    uint8_t test_data[300] = {0};

    packet_dfu_header_t test_packet_header;
    test_packet_header.SOF = DFU_SOF_identifier;
    test_packet_header.packet_type = DFU_PACKET_DATA;
    test_packet_header.payload_length = swap_uint16(sizeof(test_data));
    //Copy into test packet
    uint8_t test_packet[sizeof(packet_dfu_header_t) + sizeof(test_data)] = {0};
    std::memcpy(test_packet, &test_packet_header, sizeof(packet_dfu_header_t));
    std::memcpy(test_packet + sizeof(packet_dfu_header_t), test_data, sizeof(test_data));

    //Expect flash to be written
    mock_c()->expectOneCall("hal_dfu_writeflash")->withUnsignedLongIntParameters("address", APP_START_ADDRESS + dfu_state.bytes_sent)->withUnsignedLongIntParameters("size", sizeof(test_data))->withMemoryBufferParameter("data", test_data, sizeof(test_data));


    //Expect acknowledgement to be sent
    uint8_t ack_buffer[4] = {0x01, DFU_PACKET_ACK, 0x00, 0x00};
    //Append payload length to the end of ack_buffer
    ack_buffer[2] = test_packet_header.payload_length & 0xFF;
    ack_buffer[3] = test_packet_header.payload_length >> 8;
    mock_c()->expectOneCall("send_data_to_dfu_host")->withUnsignedIntParameters("size", sizeof(packet_dfu_header_t))->withMemoryBufferParameter("data", ack_buffer, sizeof(ack_buffer));

    DFU_STATUS_E returnState = dfu_process_packet((uint8_t *)test_packet);
    CHECK_EQUAL(DFU_STATE_DATA_EXCHANGE, dfu_state.state);
    CHECK_EQUAL(731 + sizeof(test_data), dfu_state.bytes_sent);
    CHECK_EQUAL(DFU_STATUS_OK, returnState);

}

//Test that DFU packet goes into complete mode when it reaches data size
TEST(dfu_tests, dfu_process_packet_state_change_when_complete)
{
    memset(&dfu_state, 0, sizeof(dfu_state));
    dfu_state.state = DFU_STATE_DATA_EXCHANGE;
    dfu_state.bytes_sent = 0x00001000;
    dfu_state.prog_size = 0x000010BC;
    dfu_state.prog_crc = 0x12345678;

    uint8_t test_data[0xBC] = {0};

    packet_dfu_header_t test_packet_header;
    test_packet_header.SOF = DFU_SOF_identifier;
    test_packet_header.packet_type = DFU_PACKET_DATA;
    test_packet_header.payload_length = swap_uint16(sizeof(test_data));
    //Copy into test packet
    uint8_t test_packet[sizeof(packet_dfu_header_t) + sizeof(test_data)] = {0};
    std::memcpy(test_packet, &test_packet_header, sizeof(packet_dfu_header_t));
    std::memcpy(test_packet + sizeof(packet_dfu_header_t), test_data, sizeof(test_data));
    //Expect flash to be written
    mock_c()->expectOneCall("hal_dfu_writeflash")->withUnsignedLongIntParameters("address", APP_START_ADDRESS + dfu_state.bytes_sent)->withUnsignedLongIntParameters("size", sizeof(test_data))->withMemoryBufferParameter("data", test_data, sizeof(test_data));
    //Expect acknowledgement to be sent
    uint8_t ack_buffer[4] = {0x01, DFU_PACKET_ACK, 0x00, 0x00};
    //Append payload length to the end of ack_buffer
    ack_buffer[2] = test_packet_header.payload_length & 0xFF;
    ack_buffer[3] = test_packet_header.payload_length >> 8;
    mock_c()->expectOneCall("send_data_to_dfu_host")->withUnsignedIntParameters("size", sizeof(packet_dfu_header_t))->withMemoryBufferParameter("data", ack_buffer, sizeof(ack_buffer));

    //Expect call to DFU validate
    mock_c()->expectOneCall("hal_dfu_validate_crc")->withUnsignedLongIntParameters("address", APP_START_ADDRESS)->withUnsignedLongIntParameters("size", dfu_state.prog_size)->withUnsignedLongIntParameters("crc", dfu_state.prog_crc)->andReturnBoolValue(true);
    //expect call to write application info with the following values
    application_info_flash_t test_info;
    test_info.application_size = dfu_state.prog_size;
    test_info.application_crc = dfu_state.prog_crc;
    test_info.flash_valid = true;
    test_info.dfu_request = false;
    mock_c()->expectOneCall("write_application_info")->withMemoryBufferParameter("info", (uint8_t *)&test_info, sizeof(test_info));

    DFU_STATUS_E returnState = dfu_process_packet((uint8_t *)test_packet);
    CHECK_EQUAL(DFU_STATE_COMPLETE, dfu_state.state);
    CHECK_EQUAL(0x000010BC, dfu_state.bytes_sent);
    CHECK_EQUAL(DFU_STATUS_OK, returnState);

}

//Test that DFU panics when it reaches data size and the CRC is invalid
TEST(dfu_tests, dfu_process_packet_state_change_when_complete_crc_invalid)
{
    memset(&dfu_state, 0, sizeof(dfu_state));
    dfu_state.state = DFU_STATE_DATA_EXCHANGE;
    dfu_state.bytes_sent = 0x00001000;
    dfu_state.prog_size = 0x000010BC;
    dfu_state.prog_crc = 0x12345678;

    uint8_t test_data[0xBC] = {0};

    packet_dfu_header_t test_packet_header;
    test_packet_header.SOF = DFU_SOF_identifier;
    test_packet_header.packet_type = DFU_PACKET_DATA;
    test_packet_header.payload_length = swap_uint16(sizeof(test_data));
    //Copy into test packet
    uint8_t test_packet[sizeof(packet_dfu_header_t) + sizeof(test_data)] = {0};
    std::memcpy(test_packet, &test_packet_header, sizeof(packet_dfu_header_t));
    std::memcpy(test_packet + sizeof(packet_dfu_header_t), test_data, sizeof(test_data));
    //Expect flash to be written
    mock_c()->expectOneCall("hal_dfu_writeflash")->withUnsignedLongIntParameters("address", APP_START_ADDRESS + dfu_state.bytes_sent)->withUnsignedLongIntParameters("size", sizeof(test_data))->withMemoryBufferParameter("data", test_data, sizeof(test_data));
    //Expect acknowledgement to be sent
    uint8_t ack_buffer[4] = {0x01, DFU_PACKET_ACK, 0x00, 0x00};
    //Append payload length to the end of ack_buffer
    ack_buffer[2] = test_packet_header.payload_length & 0xFF;
    ack_buffer[3] = test_packet_header.payload_length >> 8;
    mock_c()->expectOneCall("send_data_to_dfu_host")->withUnsignedIntParameters("size", sizeof(packet_dfu_header_t))->withMemoryBufferParameter("data", ack_buffer, sizeof(ack_buffer));

    //Expect call to DFU validate
    mock_c()->expectOneCall("hal_dfu_validate_crc")->withUnsignedLongIntParameters("address", APP_START_ADDRESS)->withUnsignedLongIntParameters("size", dfu_state.prog_size)->withUnsignedLongIntParameters("crc", dfu_state.prog_crc)->andReturnBoolValue(false);

    DFU_STATUS_E returnState = dfu_process_packet((uint8_t *)test_packet);
    CHECK_EQUAL(DFU_STATE_ERROR, dfu_state.state);
    CHECK_EQUAL(DFU_STATUS_ERROR_CRC_FAILED, returnState);
}

//test to ensure that DFU invalid packet type is handled correctly when in wrong state (this case, it will be in start and sent a data packet)
TEST(dfu_tests, dfu_process_packet_wrong_state)
{
    //Set DFU state struct to 0 using memset
    memset(&dfu_state, 0, sizeof(dfu_state));
    dfu_state.state = DFU_STATE_START;
    packet_dfu_header_t test_packet_header;
    uint8_t test_data[300] = {0};
    test_packet_header.SOF = DFU_SOF_identifier;
    test_packet_header.packet_type = DFU_PACKET_DATA;
    test_packet_header.payload_length = swap_uint16(sizeof(test_data));
    //Copy into test packet
    uint8_t test_packet[sizeof(packet_dfu_header_t) + sizeof(test_data)] = {0};
    std::memcpy(test_packet, &test_packet_header, sizeof(packet_dfu_header_t));
    std::memcpy(test_packet + sizeof(packet_dfu_header_t), test_data, sizeof(test_data));

    DFU_STATUS_E returnState = dfu_process_packet((uint8_t *)test_packet);
    CHECK_EQUAL(DFU_STATUS_ERROR_INVALID_STATE_PACKET_RX, returnState);
    CHECK_EQUAL(DFU_STATE_ERROR, dfu_state.state);

}

TEST(dfu_tests, dfu_start_pkt_received_in_wrong_state)
{
    //Set DFU state struct to 0 using memset
    memset(&dfu_state, 0, sizeof(dfu_state));
    dfu_state.state = DFU_STATE_DATA_EXCHANGE;

    packet_dfu_header_t test_packet_header;
    packet_dfu_prog_info_t test_packet_prog_info;
    test_packet_header.SOF = DFU_SOF_identifier;
    test_packet_header.packet_type = DFU_PACKET_START;
    test_packet_header.payload_length = swap_uint16(sizeof(packet_dfu_header_t) + sizeof(packet_dfu_prog_info_t));

    test_packet_prog_info.prog_size = swap_uint32(0x000010BC);
    test_packet_prog_info.prog_crc = swap_uint32(0x12345678);

    //Copy into test packet
    uint8_t test_packet[sizeof(packet_dfu_header_t) + sizeof(packet_dfu_prog_info_t)] = {0};
    std::memcpy(test_packet, &test_packet_header, sizeof(packet_dfu_header_t));
    std::memcpy(test_packet + sizeof(packet_dfu_header_t), &test_packet_prog_info, sizeof(packet_dfu_prog_info_t));

    DFU_STATUS_E returnState = dfu_process_packet((uint8_t *)test_packet);
    CHECK_EQUAL(DFU_STATUS_ERROR_INVALID_STATE_PACKET_RX, returnState);
    CHECK_EQUAL(DFU_STATE_ERROR, dfu_state.state);
}

//Test to ensure process packet throws invalid error when presented with a wrongly-typed data packet type
TEST(dfu_tests, dfu_packet_invalid_type)
{
    memset(&dfu_state, 0, sizeof(dfu_state));
    dfu_state.state = DFU_STATE_START;
    uint8_t test_packet[10] = {0x01, 0xFE, 0x00, 0x00}; //0xFE is not a valid packet type
    DFU_STATUS_E returnState = dfu_process_packet(test_packet);
    CHECK_EQUAL(DFU_STATUS_ERROR_INVALID_PACKET_TYPE, returnState);
    CHECK_EQUAL(DFU_STATE_ERROR, dfu_state.state);
}


/**** STATE MACHINE TESTS ****/
TEST(dfu_tests, dfu_state_machine_stalls_on_host_timeout)
{
    dfu_time_last_received = 0;
    mock_c()->ignoreOtherCalls();
    
    //Set DFU state struct to 0 using memset
    memset(&dfu_state, 0, sizeof(dfu_state));
    dfu_state.state = DFU_STATE_START;

    DFU_data_handle_t empty_data;
    empty_data.data = NULL;
    empty_data.size = 0;

    //Expect a call to gettick to be made
    mock_c()->expectOneCall("hal_dfu_gettick")->andReturnUnsignedIntValue(12); //Inital Call
    mock_c()->expectOneCall("hal_dfu_gettick")->andReturnUnsignedIntValue(25);
    mock_c()->expectOneCall("get_data_from_dfu_host")->andReturnPointerValue(&empty_data);
    mock_c()->expectOneCall("hal_dfu_gettick")->andReturnUnsignedIntValue(DFU_TIMEOUT_START + 200); //Timeout
    mock_c()->expectOneCall("get_data_from_dfu_host")->andReturnPointerValue(&empty_data);

    DFU_STATUS_E status = dfu_run();
    CHECK_EQUAL(DFU_STATE_START, dfu_state.state);
    
    status = dfu_run();
    CHECK_EQUAL(DFU_STATUS_ERROR_HOST_TIMEOUT, status);
    CHECK_EQUAL(DFU_STATE_ERROR, dfu_state.state);

}

//Ensure that a timeout error is thrown when the host sends a packet that is not of bootloader type (i.e. not consisting of the SOF)
TEST(dfu_tests, dfu_state_machine_timesout_on_incorrect_packet)
{
    dfu_time_last_received = 0;
    mock_c()->ignoreOtherCalls();
    
    //Set DFU state struct to 0 using memset
    memset(&dfu_state, 0, sizeof(dfu_state));
    dfu_state.state = DFU_STATE_START;

    uint8_t bad_data_buf[] = {0x02, 0x05, 0x00, 0x04};

    DFU_data_handle_t bad_data;
    bad_data.data = bad_data_buf;
    bad_data.size = sizeof(bad_data_buf);

    //Expect a call to gettick to be made
    mock_c()->expectOneCall("hal_dfu_gettick")->andReturnUnsignedIntValue(12); //Inital Call
    mock_c()->expectOneCall("hal_dfu_gettick")->andReturnUnsignedIntValue(25);
    mock_c()->expectOneCall("get_data_from_dfu_host")->andReturnPointerValue(&bad_data);
    mock_c()->expectOneCall("hal_dfu_gettick")->andReturnUnsignedIntValue(DFU_TIMEOUT_START + 12 + 1); //Timeout
    mock_c()->expectOneCall("get_data_from_dfu_host")->andReturnPointerValue(&bad_data);

    DFU_STATUS_E status = dfu_run();
    CHECK_EQUAL(DFU_STATE_START, dfu_state.state);

    status = dfu_run();
    CHECK_EQUAL(DFU_STATUS_ERROR_HOST_TIMEOUT, status);
    CHECK_EQUAL(DFU_STATE_ERROR, dfu_state.state);
}

TEST(dfu_tests, sets_app_parameters_on_completion)
{
    mock_c()->ignoreOtherCalls();
    memset(&dfu_state, 0, sizeof(dfu_state));
    dfu_state.state = DFU_STATE_COMPLETE;
    dfu_state.prog_size = 0x12345678;
    dfu_state.prog_crc = 0x87654321;

    application_info_flash_t test_info;
    test_info.application_size = dfu_state.prog_size;
    test_info.application_crc = dfu_state.prog_crc;
    test_info.dfu_request = false;
    test_info.flash_valid = true;
    mock_c()->expectOneCall("write_application_info")->withMemoryBufferParameter("info", (uint8_t *)&test_info, sizeof(test_info));

    DFU_STATUS_E status = dfu_run();
    CHECK_EQUAL(DFU_STATUS_OK, status);
}

//Test that HAL DFU reset is called when the state is set to complete
TEST(dfu_tests, reset_test)
{
    //Set the dfu struct to 0 using memset
    memset(&dfu_state, 0, sizeof(dfu_state));
    dfu_state.state = DFU_STATE_COMPLETE;
    mock_c()->ignoreOtherCalls();
    mock_c()->expectOneCall("hal_reset");
}

#pragma GCC diagnostic pop