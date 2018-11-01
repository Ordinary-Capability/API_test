#include <stdio.h>
#include <unistd.h>

#include "unity.h"
#include "dsp/fh_common.h"
#include "dsp/fh_system_mpi.h"


void test_sys_getVersion(void)
{
   FH_VERSION_INFO stVerInfo;

   TEST_ASSERT_EQUAL_INT(0, FH_SYS_Init());
   TEST_ASSERT_EQUAL_INT(0, FH_SYS_GetVersion(&stVerInfo));
   printf(" build_data: %u\n sw_version: %u\n hw_version: %u\n",
       stVerInfo.build_date, stVerInfo.sw_version, stVerInfo.hw_version);
    }
