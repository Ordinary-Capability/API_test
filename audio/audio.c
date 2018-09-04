

#include <stdio.h>
#include "gtest/gtest.h"
#include "dsp/fh_common.h"
#include "dsp/fh_audio_mpi.h"


class audio : public testing::Test { 
public: 
   audio() { 
   } 
   void SetUp() { 
    ASSERT_EQ(RETURN_OK, FH_AC_Init());
       // code here will execute just before the test ensues 
   }
 
   void TearDown() { 
    ASSERT_EQ(RETURN_OK, FH_AC_DeInit());
       // code here will be called just after the test completes
       // ok to through exceptions from here if need be
   }
   ~audio()  { 
       // cleanup any pending stuff, but no exceptions allowed
   }
   // put in any custom data members that you need 
};


TEST_F(audio, audio001)
{
    FH_AC_CONFIG ac_config={
           FH_AC_LINE_IN,
           (FH_AC_SAMPLE_RATE_E)8000,
           (FH_AC_BIT_WIDTH_E)16,
           FH_PT_LPCM,
           2,
           1024};
    ASSERT_EQ(RETURN_OK, FH_AC_Set_Config(&ac_config));
}

TEST_F(audio, audio002)
{
    FH_AC_CONFIG ac_config={
           FH_AC_LINE_IN,
           (FH_AC_SAMPLE_RATE_E)8000,
           (FH_AC_BIT_WIDTH_E)16,
           (FH_AC_PT_TYPE_E)(FH_PT_MAX + 1), //cause para err
           2,
           1024};
    ASSERT_EQ(PARAM_ERR, FH_AC_Set_Config(&ac_config));
}

