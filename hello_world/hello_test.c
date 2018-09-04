

#include <stdio.h>
#include "gtest/gtest.h"
#include "dsp/fh_common.h"
#include "dsp/fh_audio_mpi.h"


class hello_test : public testing::Test { 
public: 
   hello_test() { 
   } 
   void SetUp() { 
	printf("Setup.\n");
       // code here will execute just before the test ensues 
   }
 
   void TearDown() { 
	printf("TearDown.\n");
       // code here will be called just after the test completes
       // ok to through exceptions from here if need be
   }
   ~hello_test()  { 
       // cleanup any pending stuff, but no exceptions allowed
   }
   // put in any custom data members that you need 
};


TEST_F(hello_test, hello001)
{
    if (RETURN_OK == FH_AC_Init()){
        printf("Audio init successfully.\n");
    }else{
        printf("Audio init fail\n");
    }
    printf("hello world!\n");
}

