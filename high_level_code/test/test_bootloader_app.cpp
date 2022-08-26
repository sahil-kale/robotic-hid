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
}

