#include "CppUTest/TestHarness.h"
#include <CppUTestExt/MockSupport_c.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"


// create a test group
TEST_GROUP(nhdc0220_test){
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
TEST(nhdc0220_test, pass_me){
    CHECK_EQUAL(1,1);
}

#pragma GCC diagnostic pop
