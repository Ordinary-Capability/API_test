#include <stdio.h>
#include <unistd.h>
#include "unity.h"

#include "isp/isp_common.h"
#include "isp/isp_api.h"
#include "isp/isp_enum.h"
#include "isp/isp_func_def.h"
#include "dsp/fh_common.h"
#include "FHAdv_Isp_mpi.h"
#include "sample_opts.h"
#include "vpss_assistant.h"

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

/*
void setUp(void)
{
    printf("Test setup...\n");
    start_vpss_assistant();
    }

void tearDown(void)
{
    printf("\nTest teardown...\n");
    stop_vpss_assistant();
    }
void suiteSetUp(void)
{
    
    printf("Test suite setup...\n");
    start_vpss_assistant();
    }

int suiteTearDown(int num_failures)
{
    printf("Test suite teardown...\n");
    stop_vpss_assistant();
    return num_failures;
    }
*/

void get_isp_sensor_info(FHADV_ISP_SENSOR_INFO_t **info, int *len)
{
    *info = g_sensor_infos;
    *len = sizeof(g_sensor_infos) / sizeof(FHADV_ISP_SENSOR_INFO_t);
}



int isp_init(void)
{
    FH_UINT32 u32BinAddr, u32BinSize;
    ISP_VI_ATTR_S stViAttr;
    FHADV_ISP_SENSOR_PROBE_INFO_t sensor_probe;
    FHADV_ISP_SENSOR_INFO_t probed_sensor;
    
    TEST_ASSERT_EQUAL_INT(FH_RET_OK, API_ISP_MemInit(1920, 1080));
    TEST_ASSERT_EQUAL_INT(FH_RET_OK, API_ISP_GetBinAddr(&u32BinAddr, &u32BinSize));
    get_isp_sensor_info(&sensor_probe.sensor_infos, &sensor_probe.sensor_num);
    TEST_ASSERT_EQUAL_INT(0, FHAdv_Isp_SensorInit(&sensor_probe, &probed_sensor));
	TEST_ASSERT_EQUAL_INT(0, API_ISP_SensorRegCb(0, (struct isp_sensor_if*)probed_sensor.sensor_handle));
    TEST_ASSERT_EQUAL_INT(0, API_ISP_SensorInit());
    TEST_ASSERT_EQUAL_INT(0, API_ISP_SetSensorFmt(ISP_FORMAT));
    TEST_ASSERT_EQUAL_INT(0, API_ISP_Init());
    return 0;
    }

void test_isp_basic(void)
{
//    TEST_IGNORE();
    TEST_ASSERT_EQUAL_INT(FH_RET_OK, API_ISP_MemInit(1920, 1080));
    FH_UINT32 u32BinAddr, u32BinSize;
    TEST_ASSERT_EQUAL_INT(FH_RET_OK, API_ISP_GetBinAddr(&u32BinAddr, &u32BinSize));
    FHADV_ISP_SENSOR_PROBE_INFO_t sensor_probe;
    FHADV_ISP_SENSOR_INFO_t probed_sensor;
    get_isp_sensor_info(&sensor_probe.sensor_infos, &sensor_probe.sensor_num);
    TEST_ASSERT_EQUAL_INT(0, FHAdv_Isp_SensorInit(&sensor_probe, &probed_sensor));
	TEST_ASSERT_EQUAL_INT(0, API_ISP_SensorRegCb(0, (struct isp_sensor_if*)probed_sensor.sensor_handle));
    TEST_ASSERT_EQUAL_INT(0, API_ISP_SensorInit());
    TEST_ASSERT_EQUAL_INT(0, API_ISP_SetSensorFmt(ISP_FORMAT));
    TEST_ASSERT_EQUAL_INT(0, API_ISP_Init());
    TEST_ASSERT_EQUAL_INT(0, API_ISP_Init());
    FILE *param_file;
    FH_SINT8 isp_param_buff[u32BinSize];
    param_file = fopen("jxf22_mipi_attr.hex", "rb");
    TEST_ASSERT_NOT_NULL(param_file);
    fread(isp_param_buff, 1, u32BinSize, param_file);
    fclose(param_file);
    TEST_ASSERT_EQUAL_INT(0, API_ISP_LoadIspParam(isp_param_buff));
    TEST_ASSERT_EQUAL_INT(0, API_ISP_Run());
    TEST_ASSERT_EQUAL_INT(0, API_ISP_Pause());
    TEST_ASSERT_EQUAL_INT(0, API_ISP_Resume());
    TEST_ASSERT_EQUAL_INT(0, API_ISP_KickStart());
    TEST_ASSERT_EQUAL_INT(0, API_ISP_Run());
    TEST_ASSERT_EQUAL_INT(0, API_ISP_Exit());
}

/*
void test_isp_getversion(void)
{
    TEST_IGNORE();
    ISP_VERSION IspVer;
    TEST_ASSERT_EQUAL_INT(0, API_ISP_GetVersion(&IspVer));
    printf("%d.%d.%u.%s\n", &IspVer.u32SdkVer, IspVer.FH_UINT32ChipVer, IspVer.u08SdkSubVer, IspVer.u08BuildTime);
    }

void test_isp_sensorRegAndUnreg(void)
{
    TEST_IGNORE();
    TEST_ASSERT_EQUAL_INT(FH_RET_OK, API_ISP_MemInit(1920, 1080));
    FH_UINT32 u32BinAddr, u32BinSize;
    TEST_ASSERT_EQUAL_INT(FH_RET_OK, API_ISP_GetBinAddr(&u32BinAddr, &u32BinSize));
    FHADV_ISP_SENSOR_PROBE_INFO_t sensor_probe;
    FHADV_ISP_SENSOR_INFO_t probed_sensor;
    get_isp_sensor_info(&sensor_probe.sensor_infos, &sensor_probe.sensor_num);
    TEST_ASSERT_EQUAL_INT(0, FHAdv_Isp_SensorInit(&sensor_probe, &probed_sensor));
	TEST_ASSERT_EQUAL_INT(0, API_ISP_SensorRegCb(0, (struct isp_sensor_if*)probed_sensor.sensor_handle));
    TEST_ASSERT_NOT_NULL(probed_sensor.funcs.sensor_create);
    TEST_ASSERT_EQUAL_INT(0, API_ISP_SensorUnRegCb(0));
    TEST_ASSERT_NULL(probed_sensor.funcs.sensor_create);
    }


void test_isp_pause_and_resume(void)
{
    TEST_IGNORE();
    TEST_ASSERT_EQUAL_INT(FH_RET_OK, API_ISP_MemInit(1920, 1080));
    FH_UINT32 u32BinAddr, u32BinSize;

    TEST_ASSERT_EQUAL_INT(FH_RET_OK, API_ISP_GetBinAddr(&u32BinAddr, &u32BinSize));

    FHADV_ISP_SENSOR_PROBE_INFO_t sensor_probe;
    FHADV_ISP_SENSOR_INFO_t probed_sensor;

    get_isp_sensor_info(&sensor_probe.sensor_infos, &sensor_probe.sensor_num);
    TEST_ASSERT_EQUAL_INT(0, FHAdv_Isp_SensorInit(&sensor_probe, &probed_sensor));
	TEST_ASSERT_EQUAL_INT(0, API_ISP_SensorRegCb(0, (struct isp_sensor_if*)probed_sensor.sensor_handle));
    TEST_ASSERT_EQUAL_INT(0, API_ISP_SensorInit());
    TEST_ASSERT_EQUAL_INT(0, API_ISP_SetSensorFmt(ISP_FORMAT));
    TEST_ASSERT_EQUAL_INT(0, API_ISP_Init());
    FILE *param_file;
    FH_SINT8 isp_param_buff[u32BinSize];
    param_file = fopen("jxf22_mipi_attr.hex", "rb");
    TEST_ASSERT_NOT_NULL(param_file);
    fread(isp_param_buff, 1, u32BinSize, param_file);
    fclose(param_file);
    TEST_ASSERT_EQUAL_INT(0, API_ISP_LoadIspParam(isp_param_buff));

    int i = 0;
    for(i = 0; i<1; i++){
        printf("ISP pause and resume loop %d...\n", i);
        TEST_ASSERT_EQUAL_INT(0, API_ISP_Run());
        TEST_ASSERT_EQUAL_INT(0, API_ISP_Pause());
        TEST_ASSERT_EQUAL_INT(0, API_ISP_Resume());
        TEST_ASSERT_EQUAL_INT(0, API_ISP_KickStart());
    }
    TEST_ASSERT_EQUAL_INT(0, API_ISP_Run());
    TEST_ASSERT_EQUAL_INT(0, API_ISP_Exit());
}
void test_isp_ViAttr(void)
{
    TEST_IGNORE();
    FH_UINT32 u32BinAddr, u32BinSize;
    FH_UINT16 u16Tmp = 1600;
    ISP_VI_ATTR_S stViAttr;
    FHADV_ISP_SENSOR_PROBE_INFO_t sensor_probe;
    FHADV_ISP_SENSOR_INFO_t probed_sensor;
    
    TEST_ASSERT_EQUAL_INT(FH_RET_OK, API_ISP_MemInit(1920, 1080));
    TEST_ASSERT_EQUAL_INT(FH_RET_OK, API_ISP_GetBinAddr(&u32BinAddr, &u32BinSize));
    get_isp_sensor_info(&sensor_probe.sensor_infos, &sensor_probe.sensor_num);
    TEST_ASSERT_EQUAL_INT(0, FHAdv_Isp_SensorInit(&sensor_probe, &probed_sensor));
	TEST_ASSERT_EQUAL_INT(0, API_ISP_SensorRegCb(0, (struct isp_sensor_if*)probed_sensor.sensor_handle));
    TEST_ASSERT_EQUAL_INT(0, API_ISP_SensorInit());
    TEST_ASSERT_EQUAL_INT(0, API_ISP_SetSensorFmt(ISP_FORMAT));
    TEST_ASSERT_EQUAL_INT(0, API_ISP_Init());
    TEST_ASSERT_EQUAL_INT(0, API_ISP_GetViAttr(&stViAttr));
    stViAttr.u16PicWidth = u16Tmp;
    TEST_ASSERT_EQUAL_INT(0, API_ISP_SetViAttr(&stViAttr));
    TEST_ASSERT_EQUAL_INT(0, API_ISP_GetViAttr(&stViAttr));
    TEST_ASSERT_EQUAL_INT(u16Tmp, stViAttr.u16PicWidth);
    }

void test_isp_DetectPicSize(void)
{
    TEST_IGNORE();
    TEST_ASSERT_EQUAL_INT(0, isp_init());
    TEST_ASSERT_EQUAL_INT(0, API_ISP_DetectPicSize());
    }

void test_getHWmoduleCfg(void)
{
    FH_UINT32 u32modulecfg;
    TEST_ASSERT_EQUAL_INT(0, isp_init());
    TEST_ASSERT_EQUAL_INT(0, API_ISP_Get_HWmodule_cfg(&u32modulecfg));
    printf("modulecfg: %u\n", u32modulecfg);
    }
void test_setHWmoduleCfg(void)
{
    FH_UINT32 u32modulecfg = 0x605c;
    FH_UINT32 u32modulecfg_out;
    TEST_ASSERT_EQUAL_INT(0, isp_init());
    TEST_ASSERT_EQUAL_INT(0, API_ISP_Set_HWmodule_cfg(u32modulecfg));
    //TEST_ASSERT_EQUAL_INT(0, API_ISP_Get_HWmodule_cfg(&u32modulecfg_out));
    //printf("modulecfg: 0x%x\n", u32modulecfg_out);
    }

*/
/*
void test_sample1(void)
{
    TEST_IGNORE();
    }
void test_sample2(void)
{
    TEST_IGNORE();
    }
void test_sample3(void)
{
    TEST_IGNORE();
    }
void test_sample4(void)
{
    TEST_IGNORE();
    }
void test_sample5(void)
{
    TEST_IGNORE();
    }
void test_sample6(void)
{
    TEST_IGNORE();
    }
*/
