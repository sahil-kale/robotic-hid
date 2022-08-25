#include "CppUTest/TestHarness.h"
#include "dfu.h"


// create a test group
TEST_GROUP(dfu_tests){

   
};

//create a test for that test group
TEST (dfu_tests, pass_me){
    CHECK_EQUAL(1,1);
}

