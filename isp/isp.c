#include <stdio.h>
#include "gtest/gtest.h"

#include "isp/isp_common.h"
#include "isp/isp_api.h"
#include "isp/isp_enum.h"
#include "isp/isp_func_def.h"


class isp : public testing::Test { 
public: 
   isp() { 
   } 
   void SetUp() { 
    //ASSERT_EQ(RETURN_OK, FH_AC_Init());
       // code here will execute just before the test ensues 
   }
 
   void TearDown() { 
    //ASSERT_EQ(RETURN_OK, FH_AC_DeInit());
       // code here will be called just after the test completes
       // ok to through exceptions from here if need be
   }
   ~isp()  { 
       // cleanup any pending stuff, but no exceptions allowed
   }
   // put in any custom data members that you need 
};


TEST_F(isp, isp001)
{
    ASSERT_EQ(FH_RET_OK, API_ISP_MemInitEx(4));
    //ASSERT_EQ(0, sample_isp_init());

}

TEST_F(isp, isp002)
{
    FH_UINT32 u32BinAddr, u32BinSize;

    ASSERT_EQ(FH_RET_OK, API_ISP_GetBinAddr(&u32BinAddr, &u32BinSize));
    printf("BinAddr: 0x%x, BinSize: 0x%x\n", u32BinAddr, u32BinSize);
    
}

TEST_F(isp, isp003)
{
    ASSERT_EQ(FH_RET_OK, API_ISP_Pause());    
}


