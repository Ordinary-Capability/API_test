#include <stdio.h>
#include <unistd.h>
#include "unity.h"


#include "types/type_def.h"
#include "dsp/fh_system_mpi.h"
#include "dsp/fh_venc_mpi.h"
#include "dsp/fh_vpu_mpi.h"


#define WIDTH  1920
#define HEIGHT 1088


FH_VENC_CHN_ATTR g_chn_attr = {
        .enc_type = FH_NORMAL_H264,
        {
        .h264_attr = {H264_PROFILE_MAIN, 50, {WIDTH, HEIGHT}}
        }
    };

FH_VENC_RC_ATTR g_rc_attr = {
        .rc_type = FH_RC_H264_VBR,
        {
            .h264_vbr = {
                .init_qp = 35,
                .bitrate = 4096 << 10,
                .IminQP = 28,
                .ImaxQP = 42,
                .PminQP = 28,
                .PmaxQP = 42,
                .FrameRate = {25, 1},
                .maxrate_percent = 200,
                .IFrmMaxBits = 0,
                .IP_QPDelta = 0,
                .I_BitProp = 5,
                .P_BitProp = 1,
                .fluctuate_level = 0
                }
        }
    };

void _bind_vpu2vnec(FH_UINT32 vpu_chn, FH_UINT32 venc_chn)
{
    return ;
    FH_VENC_CHN_CAP cfg_vencmem = {FH_NORMAL_H264, {WIDTH, HEIGHT}};
    TEST_ASSERT_EQUAL_INT(0, FH_SYS_Init());
    TEST_ASSERT_EQUAL_INT(0, FH_VPSS_SetViAttr(&cfg_vencmem.max_size));
    TEST_ASSERT_EQUAL_INT(0, FH_VPSS_Enable(0));
    TEST_ASSERT_EQUAL_INT(0, FH_VPSS_SetChnAttr(vpu_chn, &cfg_vencmem.max_size));
    TEST_ASSERT_EQUAL_INT(0, FH_VPSS_OpenChn(vpu_chn));
    FH_VENC_CHN_CONFIG cfg_param = {g_chn_attr, g_rc_attr};
    TEST_ASSERT_EQUAL_INT(0, FH_VENC_CreateChn(venc_chn, &cfg_vencmem)); //venc channel 0
    TEST_ASSERT_EQUAL_INT(0, FH_VENC_SetChnAttr(venc_chn, &cfg_param));
    TEST_ASSERT_EQUAL_INT(0, FH_VENC_StartRecvPic(venc_chn));
    TEST_ASSERT_EQUAL_INT(0, FH_SYS_BindVpu2Enc(vpu_chn, venc_chn));
    TEST_ASSERT_EQUAL_INT(0, FH_SYS_Exit());
}

void test_vpss_sys1(void)
{
    TEST_IGNORE();
    TEST_ASSERT_EQUAL_INT(0, FH_SYS_Init());
    TEST_ASSERT_EQUAL_INT(-1015, FH_SYS_BindVpu2Enc(0, 0));
    TEST_ASSERT_EQUAL_INT(0, FH_SYS_Exit());
    }


void test_vpss_bindVpuChn0(void)
{
    TEST_IGNORE();
    FH_UINT32 vpu_chn=0, venc_chn=0;
    FH_VENC_CHN_CAP cfg_vencmem = {FH_NORMAL_H264, {WIDTH, HEIGHT}};

    TEST_ASSERT_EQUAL_INT(0, FH_SYS_Init());
    TEST_ASSERT_EQUAL_INT(0, FH_VPSS_SetViAttr(&cfg_vencmem.max_size));
    TEST_ASSERT_EQUAL_INT(0, FH_VPSS_Enable(0));
    TEST_ASSERT_EQUAL_INT(0, FH_VPSS_SetChnAttr(vpu_chn, &cfg_vencmem.max_size));
    TEST_ASSERT_EQUAL_INT(0, FH_VPSS_OpenChn(vpu_chn));

    FH_VENC_CHN_CONFIG cfg_param = {g_chn_attr, g_rc_attr};
    TEST_ASSERT_EQUAL_INT(0, FH_VENC_CreateChn(venc_chn, &cfg_vencmem)); //venc channel 0
    TEST_ASSERT_EQUAL_INT(0, FH_VENC_SetChnAttr(venc_chn, &cfg_param));
    TEST_ASSERT_EQUAL_INT(0, FH_VENC_StartRecvPic(venc_chn));
    TEST_ASSERT_EQUAL_INT(0, FH_SYS_BindVpu2Enc(vpu_chn, venc_chn));

    TEST_ASSERT_EQUAL_INT(0, FH_SYS_Exit());
    }

void test_vpss_bindVpuChn1(void)
{
    TEST_IGNORE();
    FH_UINT32 vpu_chn=1, venc_chn=0;
    FH_VENC_CHN_CAP cfg_vencmem = {FH_NORMAL_H264, {WIDTH, HEIGHT}};

    TEST_ASSERT_EQUAL_INT(0, FH_SYS_Init());
    TEST_ASSERT_EQUAL_INT(0, FH_VPSS_SetViAttr(&cfg_vencmem.max_size));
    TEST_ASSERT_EQUAL_INT(0, FH_VPSS_Enable(0));
    TEST_ASSERT_EQUAL_INT(0, FH_VPSS_SetChnAttr(vpu_chn, &cfg_vencmem.max_size));
    TEST_ASSERT_EQUAL_INT(0, FH_VPSS_OpenChn(vpu_chn));

    FH_VENC_CHN_CONFIG cfg_param = {g_chn_attr, g_rc_attr};
    TEST_ASSERT_EQUAL_INT(0, FH_VENC_CreateChn(venc_chn, &cfg_vencmem)); //venc channel 0
    TEST_ASSERT_EQUAL_INT(0, FH_VENC_SetChnAttr(venc_chn, &cfg_param));
    TEST_ASSERT_EQUAL_INT(0, FH_VENC_StartRecvPic(venc_chn));
    TEST_ASSERT_EQUAL_INT(0, FH_SYS_BindVpu2Enc(vpu_chn, venc_chn));

    TEST_ASSERT_EQUAL_INT(0, FH_SYS_Exit());
    }


void test_vpss_bindVpuChn2(void)
{
    TEST_IGNORE();
    FH_UINT32 vpu_chn=2;
    FH_VENC_CHN_CAP cfg_vencmem = {FH_NORMAL_H264, {WIDTH, HEIGHT}};

    TEST_ASSERT_EQUAL_INT(0, FH_SYS_Init());
    TEST_ASSERT_EQUAL_INT(0, FH_VPSS_SetViAttr(&cfg_vencmem.max_size));
    TEST_ASSERT_EQUAL_INT(0, FH_VPSS_Enable(0));
    TEST_ASSERT_NOT_EQUAL(0, FH_VPSS_SetChnAttr(vpu_chn, &cfg_vencmem.max_size));
    TEST_ASSERT_EQUAL_INT(0, FH_SYS_Exit());
    }

void test_vpss_bindVencChn_01(void)
{
    _bind_vpu2vnec(1, 0);
    }
void test_vpss_bindVencChn_02(void)
{
    _bind_vpu2vnec(1, 1);
    }
void test_vpss_bindVencChn_03(void)
{
    _bind_vpu2vnec(1, 2);
    }
void test_vpss_bindVencChn_04(void)
{
    _bind_vpu2vnec(1, 3);
    }
void test_vpss_bindVencChn_05(void)
{
    _bind_vpu2vnec(1, 4);
    }
void test_vpss_bindVencChn_06(void)
{
    _bind_vpu2vnec(1, 5);
    }
void test_vpss_bindVencChn_07(void)
{
    _bind_vpu2vnec(1, 6);
    }
void test_vpss_bindVencChn_08(void)
{
    _bind_vpu2vnec(1, 7);
    }
void test_vpss_bindVencChn_09(void)
{
    _bind_vpu2vnec(1, 8);
    }
void test_vpss_bindVencChn_10(void)
{
    _bind_vpu2vnec(0, 0);
    }
void test_vpss_bindVencChn_11(void)
{
    _bind_vpu2vnec(0, 1);
    }
void test_vpss_bindVencChn_12(void)
{
    _bind_vpu2vnec(0, 2);
    }
void test_vpss_bindVencChn_13(void)
{
    _bind_vpu2vnec(0, 3);
    }
void test_vpss_bindVencChn_14(void)
{
    _bind_vpu2vnec(0, 4);
    }
void test_vpss_bindVencChn_15(void)
{
    _bind_vpu2vnec(0, 5);
    }
void test_vpss_bindVencChn_16(void)
{
    _bind_vpu2vnec(0, 6);
    }
void test_vpss_bindVencChn_17(void)
{
    _bind_vpu2vnec(0, 7);
    }
void test_vpss_bindVencChn_18(void)
{
    _bind_vpu2vnec(0, 8);
    }

void test_vpss_bindBgm(void)
{
    TEST_IGNORE();
    FH_UINT32 vpu_chn = 0, venc_chn = 0;
    FH_SIZE size = {WIDTH >> 3, HEIGHT >> 3};
    FH_VENC_CHN_CAP cfg_vencmem = {FH_NORMAL_H264, {WIDTH, HEIGHT}};
    TEST_ASSERT_EQUAL_INT(0, FH_SYS_Init());
    TEST_ASSERT_EQUAL_INT(0, FH_VPSS_SetViAttr(&cfg_vencmem.max_size));
    TEST_ASSERT_EQUAL_INT(0, FH_VPSS_Enable(0));
    TEST_ASSERT_EQUAL_INT(0, FH_VPSS_SetChnAttr(vpu_chn, &cfg_vencmem.max_size));
    TEST_ASSERT_EQUAL_INT(0, FH_VPSS_OpenChn(vpu_chn));
    //TEST_ASSERT_EQUAL_INT(0, FH_VPSS_SetVOMode(vpu_chn, 0));
    FH_VENC_CHN_CONFIG cfg_param = {g_chn_attr, g_rc_attr};
    TEST_ASSERT_EQUAL_INT(0, FH_VENC_CreateChn(venc_chn, &cfg_vencmem)); //venc channel 0
    TEST_ASSERT_EQUAL_INT(0, FH_VENC_SetChnAttr(venc_chn, &cfg_param));
    TEST_ASSERT_EQUAL_INT(0, FH_VENC_StartRecvPic(venc_chn));
    TEST_ASSERT_EQUAL_INT(0, FH_SYS_BindVpu2Enc(vpu_chn, venc_chn));

    TEST_ASSERT_EQUAL_INT(0, FH_BGM_InitMem(240, 180));
    TEST_ASSERT_EQUAL_INT(0, FH_BGM_SetConfig(&size));
    TEST_ASSERT_EQUAL_INT(0, FH_BGM_Enable());
    TEST_ASSERT_EQUAL_INT(0, FH_SYS_BindVpu2Bgm());
    }







