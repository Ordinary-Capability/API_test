#include <stdio.h>
#include "unity.h"


void setUp(void)
{
    printf("Test setup\n");
    }

void tearDown(void)
{
    printf("Test tear down.\n");
    }

void test_helloworld(void)
{
#ifdef RT_USING_FINSH
    printf("Hello rtos.\n");
#else
    printf("Hello linux.\n");
#endif
    }

