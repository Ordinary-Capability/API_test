#include <stdio.h>
#include "gtest/gtest.h"

#include "isp/isp_common.h"
#include "isp/isp_api.h"
#include "isp/isp_enum.h"
#include "isp/isp_func_def.h"
#include "dsp_ext/FHAdv_Isp_mpi.h"
#include "sample_opts.h"

FHADV_ISP_SENSOR_INFO_t g_sensor_infos[] = {
    {
        "sensor", NULL, 0, NULL,
        {
        (void *(*)(void))Sensor_Create,
        GetMirrorFlipBayerFormat,
        GetSensorAwbGain,
        GetSensorLtmCurve,
        GetDefaultParam,
        },
    },
};

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


void get_isp_sensor_info(FHADV_ISP_SENSOR_INFO_t **info, int *len)
{
    *info = g_sensor_infos;
    *len = sizeof(g_sensor_infos) / sizeof(FHADV_ISP_SENSOR_INFO_t);
}

TEST_F(isp, isp001)
{
    printf("Test start.\n");
    //ASSERT_EQ(FH_RET_OK, API_ISP_MemInitEx(4));
    ASSERT_EQ(FH_RET_OK, API_ISP_MemInit(1920, 1080));
    //ASSERT_EQ(0, sample_isp_init());
    FH_UINT32 u32BinAddr, u32BinSize;

    ASSERT_EQ(FH_RET_OK, API_ISP_GetBinAddr(&u32BinAddr, &u32BinSize));
    printf("BinAddr: 0x%x, BinSize: 0x%x\n", u32BinAddr, u32BinSize);

    FHADV_ISP_SENSOR_PROBE_INFO_t sensor_probe;
    FHADV_ISP_SENSOR_INFO_t probed_sensor;

    /*
    get_isp_sensor_info(&sensor_probe.sensor_infos, &sensor_probe.sensor_num);
    ASSERT_EQ(0, FHAdv_Isp_SensorInit(&sensor_probe, &probed_sensor));
	API_ISP_SensorRegCb(0, (struct isp_sensor_if*)probed_sensor.sensor_handle);

    API_ISP_SensorInit();
    API_ISP_SetSensorFmt(ISP_FORMAT);

    ASSERT_EQ(0, API_ISP_Init());
    */
}

TEST_F(isp, isp002)
{
    /*
    FH_UINT32 u32BinAddr, u32BinSize;

    ASSERT_EQ(FH_RET_OK, API_ISP_GetBinAddr(&u32BinAddr, &u32BinSize));
    printf("BinAddr: 0x%x, BinSize: 0x%x\n", u32BinAddr, u32BinSize);
    */
    
}

TEST_F(isp, isp003)
{
//    ASSERT_EQ(FH_RET_OK, API_ISP_Pause());    
}


