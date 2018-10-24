#include <stdio.h>
#include <unistd.h>
#include "unity.h"


#include "types/type_def.h"
#include "dsp/fh_system_mpi.h"


void test_vpss_sys1(void)
{
    TEST_ASSERT_EQUAL_INT(0, FH_SYS_Init());
    TEST_ASSERT_EQUAL_INT(-1015, FH_SYS_BindVpu2Enc(0, 0));
    TEST_ASSERT_EQUAL_INT(0, FH_SYS_Exit());
    }







