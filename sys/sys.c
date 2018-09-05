

#include <stdio.h>
#include "gtest/gtest.h"
#include "dsp/fh_common.h"
#include "dsp/fh_system_mpi.h"


class sys : public testing::Test { 
public: 
   sys() { 
   } 
   void SetUp() { 
       // code here will execute just before the test ensues 
   }
 
   void TearDown() { 
       // code here will be called just after the test completes
       // ok to through exceptions from here if need be
   }
   ~sys()  { 
       // cleanup any pending stuff, but no exceptions allowed
   }
   // put in any custom data members that you need 
};


TEST_F(sys, sys001)
{
    ASSERT_EQ(RETURN_OK, FH_SYS_Init());
}

TEST_F(sys, sys002)
{
    ASSERT_EQ(RETURN_OK, FH_SYS_Exit());
}
