#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <fcntl.h>
#include <gtest/gtest.h>

#include "dsp/fh_common.h"
#include "dsp/fh_audio_mpi.h"

#define MAX_FRAME_COUNT 100000

#define ERR_RETURN(name, ret_val) \
    if (ret_val != RETURN_OK) \
    {\
        fprintf(stderr, name" failed (%d)\n", ret_val);\
        return ret_val;\
    }

static int mode;

void usage(const char *program_name)
{
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage: %s [-c | -p | -l [count] | -t | -r [sample rate] | -b [bit width] | -n [samples per frame]]\n", program_name);
	fprintf(stderr, "\n");
	fprintf(stderr, "Example:\n");
	fprintf(stderr, "       %s -m      --> audio input is mic_in, default is line_in\n", program_name);
	fprintf(stderr, "       %s -j 0    --> set mic_in analog volume(0~3,0:0db,1:10db,2:20db,3:26.4db,default is 0)\n", program_name);
	fprintf(stderr, "       %s -s      --> sterio data(two channels)\n", program_name);
	fprintf(stderr, "       %s -c      --> capture audio to file audio_capture.dat\n", program_name);
	fprintf(stderr, "       %s -p      --> playback audio file audio.dat\n", program_name);
	fprintf(stderr, "       %s -l 500  --> loop back test, cycle 500 times\n", program_name);
	fprintf(stderr, "       %s -t      --> test pause resume function\n", program_name);
	fprintf(stderr, "       %s -r 8000 --> set sample rate\n", program_name);
	fprintf(stderr, "       %s -b 16   --> set bit width\n", program_name);
	fprintf(stderr, "       %s -n 1024 --> set sample numbers per frame\n", program_name);
	fprintf(stderr, "       %s -f audio.dat --> specify the playback or record filename, default is audio.dat\n", program_name);
	fprintf(stderr, "       %s -v 11  --> set analog input volume(0~31,default 11)\n", program_name);
	fprintf(stderr, "       %s -V 28  --> set analog output volume(0~31,default 28)\n", program_name);
	fprintf(stderr, "       %s -e encoding-type --> specify audio encoding type[0:default PCM; 1:G711A; 2:G711U; 3:G726_16K; 4:G726_32K; 5:AAC] \n", program_name);
	fprintf(stderr, "\n");
}

static void signal_handler(int sig)
{
	printf("catch a signal\n");

	if (mode == 0 || mode == 2) {
		FH_AC_AI_Disable();
		printf("[INFO]: FH_AC_AI_Disable ok\n");
	}

	if (mode == 1 || mode == 2) {
		FH_AC_AO_Disable();
		printf("[INFO]: FH_AC_AO_Disable ok\n");
	}

	exit(EXIT_FAILURE);
}

int main(int argc, char * const argv[])
{
	int ret = 0;
	int c;
	FH_AC_CONFIG ac_config;
	FH_AC_FRAME_S audio_frame;
	FILE *capture_file, *playback_file;
	int count = 0;
	int data_bytes;
	int total_bytes = 0;
	int len_read;
	int loop_count;
	int test_puase_resume = 0;
	FH_AC_SAMPLE_RATE_E sample_rate;
	FH_AC_BIT_WIDTH_E frame_bit;
	int samples;
	int io_type;
	int ch;
	int times = 1;
	FH_AC_PT_TYPE_E enc_type = FH_PT_LPCM;
	FH_AC_IO_TYPE_E ain_select = FH_AC_LINE_IN;
	int micin_vol = 0;
	int enable_aac = 0;
	int channel = 1;
	int g_paused = 0;
	int ain_vol = 85;
	int aout_vol = 28;
	time_t t1, t2;
	char* file = "audio.dat";

	if (argc == 1) {
		usage(argv[0]);
		return -1;
	}

	fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) | O_NONBLOCK);

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGABRT, signal_handler);

	sample_rate = AC_SR_16K;
	frame_bit = AC_BW_16;
	samples = 1024;

	while ((c = getopt(argc, argv, "j:macpsl:tr:b:n:f:x:e:v:V:")) != -1) {
		switch (c) {
		case 'v':
			ain_vol = strtol(optarg, NULL, 0);
			break;
		case 'V':
			aout_vol = strtol(optarg, NULL, 0);
			break;
		case 'm':
			ain_select = FH_AC_MIC_IN;
			break;
		case 'j':
			micin_vol = strtol(optarg, NULL, 0);
			break;
		case 'a':
			enable_aac = 1;
			break;
		case 's':
			channel = 2;
			break;
		case 'c':
			mode = 0;
			break;
		case 'p':
			mode = 1;
			break;
		case 'l':
			mode = 2;
			loop_count = strtol(optarg, NULL, 0);
			break;
		case 't':
			test_puase_resume = 1;
			break;
		case 'r':
			sample_rate = (FH_AC_SAMPLE_RATE_E )strtol(optarg, NULL, 0);
			break;
		case 'b':
			frame_bit = (FH_AC_BIT_WIDTH_E)strtol(optarg, NULL, 0);
			break;
		case 'n':
			samples = strtol(optarg, NULL, 0);
			break;
		case 'x':
			times = strtol(optarg, NULL, 0);
			break;
		case 'e':
			enc_type = (FH_AC_PT_TYPE_E)strtol(optarg, NULL, 0);
			if (enc_type < 0 || enc_type >= FH_PT_MAX)
			{
			    usage(argv[0]);
			    return -2;
			}
			break;
		case 'f':
			file = optarg;
			break;
		default:
			usage(argv[0]);
			return -2;
		}
	}


	if (enc_type != FH_PT_LPCM && frame_bit != 16)
	{
		printf("G711/G726 encoding should set to 16bit sample.\n");
    		return -2;
	}

	if (enable_aac && frame_bit != 16)
	{
		printf("AAC will use 16bit samples!!!\n");
    		return -2;
	}

	ERR_RETURN("FH_AC_Init", FH_AC_Init());
	printf("[INFO]: FH_AC_Init ok\n");

	t1 = time(0);
	
	if (mode == 0) {
		ac_config.io_type = ain_select;
		ac_config.sample_rate = sample_rate;
		ac_config.bit_width = frame_bit;
		ac_config.enc_type  = enc_type;
		ac_config.channels = channel;
		ac_config.period_size = samples;

		ERR_RETURN("FH_AC_Set_Config", FH_AC_Set_Config(&ac_config));
		printf("[INFO]: FH_AC_Set_Config ok\n");

		ERR_RETURN("FH_AC_AI_Enable", FH_AC_AI_Enable());
		printf("[INFO]: FH_AC_AI_Enable ok\n");

		ERR_RETURN("FH_AC_AI_MICIN_SetVol", FH_AC_AI_MICIN_SetVol(micin_vol));
		printf("[INFO]: FH_AC_AI_MICIN_SetVol ok\n");

		ERR_RETURN("FH_AC_AI_SetVol", FH_AC_AI_SetVol(ain_vol));
		printf("[INFO]: FH_AC_AI_SetVol ok\n");

		if (frame_bit == 24)
			data_bytes = ac_config.period_size * 4;
		else
			data_bytes = ac_config.period_size * ac_config.bit_width / 8;
		audio_frame.data = (FH_UINT8 *)malloc(data_bytes);
		if (audio_frame.data == NULL) {
			printf("[ERRO]: failed to malloc audio frame buffer\n");
			ret = -3;
			goto exit;
		}

		capture_file = fopen(file, "wb");
		if (capture_file == NULL) {
			printf("[ERRO]: failed to open %s\n", file);
			ret = -4;
			goto exit;
		}

		printf("[INFO]: start capturing audio data ...\n");
		while (1) {
			t2 = time(0);
			if (t2 - t1 >= 10)
			{
				printf("Sample rate:%d\n", total_bytes / ((frame_bit == 24 ? 32 : frame_bit)/8) / (t2 - t1));
				t1 = t2;
				total_bytes = 0;
			}

			ch = getchar();
			if (ch == 'p' && !g_paused) 
			{
				FH_AC_AI_Pause();
				g_paused = 1;
			}


			if (ch == 'r' && g_paused) {
				FH_AC_AI_Resume();
				g_paused = 0;
			}

			if (g_paused) {
				usleep(10000);
				continue;
			}

			ret = FH_AC_AI_GetFrame(&audio_frame);
			if (ret == 0) {
				if (audio_frame.len <= 0)
				{
					usleep(10000);
					continue;
				}

				fwrite(audio_frame.data, 1, audio_frame.len, capture_file);
				total_bytes += audio_frame.len;
				count++;
				if (count % 100 == 0)
				{
					//printf("read len = %d\n", audio_frame.len);
				}
				if (count == MAX_FRAME_COUNT) {
					fclose(capture_file);
					break;
				}
				if (test_puase_resume) {
					sleep(1);
					ERR_RETURN("FH_AC_AI_Pause", FH_AC_AI_Pause());
					sleep(2);
					ERR_RETURN("FH_AC_AI_Resume", FH_AC_AI_Resume());
				}
			}
		}
		printf("[INFO]: stop capturing audio data\n");
	} else if (mode == 1) {
		ac_config.io_type = FH_AC_LINE_OUT;
		ac_config.sample_rate = sample_rate;
		ac_config.bit_width = frame_bit;
		ac_config.enc_type  = enc_type;
		ac_config.channels = channel;
		ac_config.period_size = samples;

		ERR_RETURN("FH_AC_Set_Config", FH_AC_Set_Config(&ac_config));
		printf("[INFO]: FH_AC_Set_Config ok\n");

		ERR_RETURN("FH_AC_AO_Enable", FH_AC_AO_Enable());
		printf("[INFO]: FH_AC_AO_Enable ok\n");

		ERR_RETURN("FH_AC_AO_SetVol", FH_AC_AO_SetVol(aout_vol));
		printf("[INFO]: FH_AC_AO_SetVol ok\n");

		if (frame_bit == 24)
			data_bytes = ac_config.period_size * 4;
		else
			data_bytes = ac_config.period_size * ac_config.bit_width / 8;
		audio_frame.data = (FH_UINT8 *)malloc(data_bytes);
		if (audio_frame.data == NULL) {
			printf("[ERRO]: failed to malloc audio frame buffer\n");
			ret = -5;
			goto exit;
		}

		playback_file = fopen(file, "rb");
		if (playback_file == NULL) {
			printf("[ERRO]: failed to open %s\n", file);
			ret = -6;
			goto exit;
		}

		printf("[INFO]: start playing audio data ...\n");
		while (1) {
			t2 = time(0);
			if (t2 - t1 >= 10)
			{
				printf("Play rate:%d\n", total_bytes / ((frame_bit == 24 ? 32 : frame_bit)/8) / (t2 - t1));
				t1 = t2;
				total_bytes = 0;
			}

			ch = getchar();
			if (ch == 'p' && !g_paused) 
			{
				FH_AC_AO_Pause();
				g_paused = 1;
			}


			if (ch == 'r' && g_paused) {
				FH_AC_AO_Resume();
				g_paused = 0;
			}

			if (g_paused) {
				usleep(10000);
				continue;
			}

			len_read = fread(audio_frame.data, 1, data_bytes, playback_file);
			if (len_read != data_bytes)
			{
				if (--times <= 0)
					break;
				
				fseek(playback_file, 0, SEEK_SET);
				printf("Play loop again.\n");
				continue;
			}
			if (feof(playback_file)) {
				break;
			}
			if (len_read > 0) {
				audio_frame.len = len_read;
				FH_AC_AO_SendFrame(&audio_frame);
				total_bytes += len_read;
				if (test_puase_resume) {
					sleep(1);
					ERR_RETURN("FH_AC_AO_Pause", FH_AC_AO_Pause());
					sleep(2);
					ERR_RETURN("FH_AC_AO_Resume", FH_AC_AO_Resume());
				}
			}
		}
		printf("[INFO]: stop playing audio data\n");
	} else if (mode == 2) {
		ac_config.sample_rate = sample_rate;
		ac_config.bit_width = frame_bit;
		ac_config.channels = channel;
		ac_config.period_size = samples;
		ac_config.enc_type  = enc_type;

		ac_config.io_type = ain_select;
		ERR_RETURN("FH_AC_Set_Config", FH_AC_Set_Config(&ac_config));
		printf("[INFO]: FH_AC_Set_Config ok\n");

		ERR_RETURN("FH_AC_AI_Enable", FH_AC_AI_Enable());
		printf("[INFO]: FH_AC_AI_Enable ok\n");

		ac_config.io_type = FH_AC_LINE_OUT;
		ERR_RETURN("FH_AC_Set_Config", FH_AC_Set_Config(&ac_config));
		printf("[INFO]: FH_AC_Set_Config ok\n");

		ERR_RETURN("FH_AC_AO_Enable", FH_AC_AO_Enable());
		printf("[INFO]: FH_AC_AO_Enable ok\n");

		ERR_RETURN("FH_AC_AI_MICIN_SetVol", FH_AC_AI_MICIN_SetVol(micin_vol));
		printf("[INFO]: FH_AC_AI_MICIN_SetVol ok\n");

		ERR_RETURN("FH_AC_AI_SetVol", FH_AC_AI_SetVol(ain_vol));
		printf("[INFO]: FH_AC_AI_SetVol ok\n");

		ERR_RETURN("FH_AC_AO_SetVol", FH_AC_AO_SetVol(aout_vol));
		printf("[INFO]: FH_AC_AO_SetVol ok\n");

		if (frame_bit == 24)
			data_bytes = ac_config.period_size * 4;
		else
			data_bytes = ac_config.period_size * ac_config.bit_width / 8;
		audio_frame.data = (FH_UINT8 *)malloc(data_bytes);
		if (audio_frame.data == NULL) {
			printf("[ERRO]: failed to malloc audio frame buffer\n");
			ret = -3;
			goto exit;
		}

		printf("[INFO]: start capturing and then playing back audio data ...\n");
		while (1) {
			ret = FH_AC_AI_GetFrame(&audio_frame);
			if (ret == 0) {
				FH_AC_AO_SendFrame(&audio_frame);
				if (test_puase_resume) {
					sleep(1);
					ERR_RETURN("FH_AC_AO_Pause", FH_AC_AO_Pause());
					sleep(2);
					ERR_RETURN("FH_AC_AO_Resume", FH_AC_AO_Resume());
				}
				if (loop_count > 0) {
					count++;
					if (count == loop_count)
						break;
				}
				if (test_puase_resume) {
					sleep(1);
					ERR_RETURN("FH_AC_AI_Pause", FH_AC_AI_Pause());
					sleep(2);
					ERR_RETURN("FH_AC_AI_Resume", FH_AC_AI_Resume());
				}
			}
		}
		printf("[INFO]: stop loopback\n");
	} else {
		usage(argv[0]);
		return -1;
	}

exit:
	if (mode == 0 || mode == 2) {
		ERR_RETURN("FH_AC_AI_Disable", FH_AC_AI_Disable());
		printf("[INFO]: FH_AC_AI_Disable ok\n");
	}

	if (mode == 1 || mode == 2) {
		ERR_RETURN("FH_AC_AO_Disable", FH_AC_AO_Disable());
		printf("[INFO]: FH_AC_AO_Disable ok\n");
	}
	FH_AC_DeInit();
	return ret;
}

