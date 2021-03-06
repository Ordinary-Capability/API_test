#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include "types/type_def.h"
#include "dsp/fh_system_mpi.h"
#include "dsp/fh_vpu_mpi.h"
#include "sample_common_isp.h"
#include "dbi/dbi_over_tcp.h"
#include "dsp/fh_venc_mpi.h"
#include "isp/isp_enum.h"
#include "sample_opts.h"
#include "vpss_assistant.h"

#ifndef RT_USING_FINSH
#include "libvlcview.h"
#endif


#define CHANNEL_COUNT 1

#define USE_H265 0

struct channel_info
{
    FH_UINT32 width;
    FH_UINT32 height;
    FH_UINT8 frame_count;
    FH_UINT8 frame_time;
    FH_UINT32 bps;
};


static FH_BOOL g_stop_running = FH_FALSE;
static pthread_t g_thread_isp = 0;
static pthread_t g_thread_stream  = 0;
//static pthread_t g_thread_dbi = 0;
//static struct dbi_tcp_config g_tcp_conf;


void sample_vlcview_exit(void)
{
    if (g_thread_stream != 0)
        pthread_join(g_thread_stream, NULL);
    if (g_thread_isp != 0)
        pthread_join(g_thread_isp, NULL);

    FH_SYS_Exit();
}

void sample_vlcview_handle_sig(FH_SINT32 signo)
{
    printf("Caught %d, program exit abnormally\n", signo);
    g_stop_running = FH_TRUE;
    sample_vlcview_exit();
    exit(EXIT_FAILURE);
}

void *sample_vlcview_get_stream_proc(void *arg)
{
    FH_SINT32 ret, i;
    FH_VENC_STREAM stream;
    unsigned int chan;
    struct vlcview_enc_stream_element stream_element;

    while (!g_stop_running)
    {
#if USE_H265
        ret = FH_VENC_GetStream_Block(FH_STREAM_H265, &stream);
        if (ret == RETURN_OK)
        {
            stream_element.enc_type   = VLCVIEW_ENC_H265;
            stream_element.frame_type = stream.h265_stream.frame_type == FH_FRAME_I ? VLCVIEW_ENC_I_FRAME : VLCVIEW_ENC_P_FRAME;
            stream_element.frame_len  = stream.h265_stream.length;
            stream_element.time_stamp = stream.h265_stream.time_stamp;
            stream_element.nalu_count = stream.h265_stream.nalu_cnt;
            chan                      = stream.h265_stream.chan;
            for (i = 0; i < stream_element.nalu_count; i++)
            {
                stream_element.nalu[i].start = stream.h265_stream.nalu[i].start;
                stream_element.nalu[i].len   = stream.h265_stream.nalu[i].length;
            }
            vlcview_pes_stream_pack(chan, stream_element);
            FH_VENC_ReleaseStream(chan);
        }
#else
        ret = FH_VENC_GetStream_Block(FH_STREAM_H264, &stream);
        if (ret == RETURN_OK)
        {
            stream_element.enc_type   = VLCVIEW_ENC_H264;
            stream_element.frame_type = stream.h264_stream.frame_type == FH_FRAME_I ? VLCVIEW_ENC_I_FRAME : VLCVIEW_ENC_P_FRAME;
            stream_element.frame_len  = stream.h264_stream.length;
            stream_element.time_stamp = stream.h264_stream.time_stamp;
            stream_element.nalu_count = stream.h264_stream.nalu_cnt;
            chan                      = stream.h264_stream.chan;
            for (i = 0; i < stream_element.nalu_count; i++)
            {
                stream_element.nalu[i].start = stream.h264_stream.nalu[i].start;
                stream_element.nalu[i].len   = stream.h264_stream.nalu[i].length;
            }
            vlcview_pes_stream_pack(chan, stream_element);
            FH_VENC_ReleaseStream(chan);
        }
#endif
    }

    return NULL;
}


void usage(char *program_name)
{
    fprintf(stderr, "\nUsage:  %s  <VLC IP address>  [port number (optional, default 1234)]\n\n", program_name);
    fprintf(stderr, "Example:\n");
    fprintf(stderr, "    %s 192.168.1.3\n", program_name);
    fprintf(stderr, "    %s 192.168.1.3 2345\n", program_name);
}

void stop_vpss_assistant()
{
    g_stop_running = FH_TRUE;
    usleep(10000000);
}

//int main(int argc, char const *argv[])
void * vpss_assist()
{
    FH_VPU_SIZE vi_pic;
    FH_VPU_CHN_CONFIG chn_attr;
    //FH_VENC_CHN_CAP cfg_vencmem;
    //FH_VENC_CHN_CONFIG cfg_param;
    FH_SINT32 ret;
    FH_SINT32 i;

//    signal(SIGINT, sample_vlcview_handle_sig);
//    signal(SIGQUIT, sample_vlcview_handle_sig);
//    signal(SIGKILL, sample_vlcview_handle_sig);
//    signal(SIGTERM, sample_vlcview_handle_sig);

    /******************************************
     step  1: init media platform
    ******************************************/
    ret = FH_SYS_Init();
    if (ret != RETURN_OK)
    {
        printf("Error: FH_SYS_Init failed with %d\n", ret);
        goto err_exit;
    }

    /******************************************
     step  2: set vpss resolution
    ******************************************/
    vi_pic.vi_size.u32Width = VIDEO_INPUT_WIDTH;
    vi_pic.vi_size.u32Height = VIDEO_INPUT_HEIGHT;
    ret = FH_VPSS_SetViAttr(&vi_pic);
    if (ret != RETURN_OK)
    {
        printf("Error: FH_VPSS_SetViAttr failed with %d\n", ret);
        goto err_exit;
    }

    /******************************************
     step  3: start vpss
    ******************************************/
    if (FH_VPSS_Enable(VPU_MODE_ISP) != RETURN_OK)
    {
        printf("Error: FH_VPSS_Enable failed with %d\n", ret);
        goto err_exit;
    }

    /******************************************
     step  4: configure vpss channels
    ******************************************/
    for (i = 0; i < CHANNEL_COUNT; i++)
    {
        chn_attr.vpu_chn_size.u32Width = 1920; //g_channel_infos[i].width;
        chn_attr.vpu_chn_size.u32Height = 1080; //g_channel_infos[i].height;
        ret = FH_VPSS_SetChnAttr(i, &chn_attr);
        printf("vpss set channel attr... \n");
        if (ret != RETURN_OK)
        {
            printf("Error: FH_VPSS_SetChnAttr failed with %d\n", ret);
            goto err_exit;
        }

        /******************************************
         step  5: open vpss channel
        ******************************************/
        printf("vpss open channel...\n");
        ret = FH_VPSS_OpenChn(i);
        if (ret != RETURN_OK)
        {
            printf("Error: FH_VPSS_OpenChn failed with %d\n", ret);
            goto err_exit;
        }
#if USE_H265
        FH_VPSS_SetVOMode(i, VPU_VOMODE_SCAN);
#endif

//        /******************************************
//         step  6: create venc channel
//        ******************************************/
//#if USE_H265
//        cfg_vencmem.support_type = FH_NORMAL_H265;
//        cfg_vencmem.max_size.u32Width = g_channel_infos[i].width;
//        cfg_vencmem.max_size.u32Height = g_channel_infos[i].height;
//
//        ret = FH_VENC_CreateChn(i, &cfg_vencmem);
//        if (ret != RETURN_OK)
//        {
//            printf("Error: FH_VENC_CreateChn failed with %d\n", ret);
//            goto err_exit;
//        }
//
//        cfg_param.chn_attr.enc_type = FH_NORMAL_H265;
//        cfg_param.chn_attr.h265_attr.profile = H265_PROFILE_MAIN;
//        cfg_param.chn_attr.h265_attr.i_frame_intterval = 50;
//        cfg_param.chn_attr.h265_attr.size.u32Width = g_channel_infos[i].width;
//        cfg_param.chn_attr.h265_attr.size.u32Height = g_channel_infos[i].height;
//
//        cfg_param.rc_attr.rc_type = FH_RC_H265_VBR;
//        cfg_param.rc_attr.h265_vbr.init_qp = 35;
//        cfg_param.rc_attr.h265_vbr.bitrate = CH0_BIT_RATE;
//        cfg_param.rc_attr.h265_vbr.ImaxQP = 42;
//        cfg_param.rc_attr.h265_vbr.IminQP = 28;
//        cfg_param.rc_attr.h265_vbr.PmaxQP = 42;
//        cfg_param.rc_attr.h265_vbr.PminQP = 28;
//        cfg_param.rc_attr.h265_vbr.maxrate_percent = 200;
//        cfg_param.rc_attr.h265_vbr.IFrmMaxBits = 0;
//        cfg_param.rc_attr.h265_vbr.IP_QPDelta = 0;
//        cfg_param.rc_attr.h265_vbr.I_BitProp = 5;
//        cfg_param.rc_attr.h265_vbr.P_BitProp = 1;
//        cfg_param.rc_attr.h265_vbr.fluctuate_level = 0;
//        cfg_param.rc_attr.h265_vbr.FrameRate.frame_count = g_channel_infos[i].frame_count;
//        cfg_param.rc_attr.h265_vbr.FrameRate.frame_time  = g_channel_infos[i].frame_time;
//#else
//        cfg_vencmem.support_type = FH_NORMAL_H264;
//        cfg_vencmem.max_size.u32Width = g_channel_infos[i].width;
//        cfg_vencmem.max_size.u32Height = g_channel_infos[i].height;
//
//        ret = FH_VENC_CreateChn(i, &cfg_vencmem);
//        if (ret != RETURN_OK)
//        {
//            printf("Error: FH_VENC_CreateChn failed with %d\n", ret);
//            goto err_exit;
//        }
//
//        cfg_param.chn_attr.enc_type = FH_NORMAL_H264;
//        cfg_param.chn_attr.h264_attr.profile = H264_PROFILE_MAIN;
//        cfg_param.chn_attr.h264_attr.i_frame_intterval = 50;
//        cfg_param.chn_attr.h264_attr.size.u32Width = g_channel_infos[i].width;
//        cfg_param.chn_attr.h264_attr.size.u32Height = g_channel_infos[i].height;
//
//        cfg_param.rc_attr.rc_type = FH_RC_H264_VBR;
//        cfg_param.rc_attr.h264_vbr.init_qp = 35;
//        cfg_param.rc_attr.h264_vbr.bitrate = CH0_BIT_RATE;
//        cfg_param.rc_attr.h264_vbr.ImaxQP = 42;
//        cfg_param.rc_attr.h264_vbr.IminQP = 28;
//        cfg_param.rc_attr.h264_vbr.PmaxQP = 42;
//        cfg_param.rc_attr.h264_vbr.PminQP = 28;
//        cfg_param.rc_attr.h264_vbr.maxrate_percent = 200;
//        cfg_param.rc_attr.h264_vbr.IFrmMaxBits = 0;
//        cfg_param.rc_attr.h264_vbr.IP_QPDelta = 0;
//        cfg_param.rc_attr.h264_vbr.I_BitProp = 5;
//        cfg_param.rc_attr.h264_vbr.P_BitProp = 1;
//        cfg_param.rc_attr.h264_vbr.fluctuate_level = 0;
//        cfg_param.rc_attr.h264_vbr.FrameRate.frame_count = g_channel_infos[i].frame_count;
//        cfg_param.rc_attr.h264_vbr.FrameRate.frame_time  = g_channel_infos[i].frame_time;
//#endif
//        ret = FH_VENC_SetChnAttr(i, &cfg_param);
//        if (ret != RETURN_OK)
//        {
//            printf("Error: FH_VENC_SetChnAttr failed with %d\n", ret);
//            goto err_exit;
//        }
//        /******************************************
//         step  7: start venc channel
//        ******************************************/
//        ret = FH_VENC_StartRecvPic(i);
//        if (ret != RETURN_OK)
//        {
//            printf("Error: FH_VENC_StartRecvPic failed with %d\n", ret);
//            goto err_exit;
//        }
//
//        /******************************************
//         step  8: bind vpss channel to venc channel
//        ******************************************/
//        ret = FH_SYS_BindVpu2Enc(i, i);
//        if (ret != RETURN_OK)
//        {
//            printf("Error: FH_SYS_BindVpu2Enc failed with %d\n", ret);
//            goto err_exit;
//        }
    }

    /******************************************
     step  9: init ISP, and then start ISP process thread
//    ******************************************/
//    if (sample_isp_init() != 0)
//    {
//        goto err_exit;
//    }
//    pthread_create(&g_thread_isp, NULL, sample_isp_proc, &g_stop_running);
//    /******************************************
//     step  10: start debug interface thread
//    ******************************************/
//    g_tcp_conf.cancel = &g_stop_running;
//    g_tcp_conf.port = 8888;
//    pthread_create(&g_thread_dbi, NULL, tcp_dbi_thread, &g_tcp_conf);
//
//    /******************************************
//     step  11: initialize vlcview lib
//    ******************************************/
//    ret = vlcview_pes_init();
//    if (ret != 0)
//    {
//        printf("Error: vlcview_pes_init failed with %d\n", ret);
//        goto err_exit;
//    }
//
//    /******************************************
//     step  12: get stream, pack as PES stream and then send to vlc
//    ******************************************/
//    pthread_create(&g_thread_stream, NULL, sample_vlcview_get_stream_proc, NULL);
//    for (i = 0; i < CHANNEL_COUNT; i++)
//        vlcview_pes_send_to_vlc(i, ipaddr, port + i);
//
//    /******************************************
//     step  13: handle keyboard event
//    ******************************************/
//    printf("\nPress Enter key to exit program ...\n");
//    getchar();
//    g_stop_running = 1;

    /******************************************
     step  14: exit process
    ******************************************/
    printf("VPSS start successfully.\n");
    while (!g_stop_running){
        usleep(100000);
    }
err_exit:
//    sample_vlcview_exit();
//    vlcview_pes_uninit();
    FH_SYS_Exit();
    printf("VPSS exit successfully.\n");
    return NULL;
}

void start_vpss_assistant()
{
    g_stop_running = FH_FALSE;
    pthread_t vpss_thread = 0;

    pthread_create(&vpss_thread, NULL, vpss_assist, NULL);
    usleep(5000000);
}
