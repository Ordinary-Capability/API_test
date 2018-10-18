DEBUG ?= 0
STATIC ?= 1

SAMPLE_TOP_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

#$(info $(SAMPLE_TOP_DIR))

CROSS_COMPILE ?= arm-fullhan-linux-uclibcgnueabi-
CC := $(CROSS_COMPILE)gcc

SDK_INC_DIR = $(SAMPLE_TOP_DIR)/../include
SAMPLE_COMMON_INC_DIR = $(SAMPLE_TOP_DIR)/../demo/common
LIB_PATH = $(SAMPLE_TOP_DIR)/../lib

ifeq ($(STATIC),1)
SDK_LIB_DIR = $(LIB_PATH)/static
else
SDK_LIB_DIR = $(LIB_PATH)/dynamic
endif

INCLUDES =  -I$(SAMPLE_TOP_DIR)/Unity/src -I$(SDK_INC_DIR) -I$(SAMPLE_TOP_DIR) -I$(SAMPLE_COMMON_INC_DIR) 
INCLUDES += -I$(SDK_INC_DIR)/dsp_ext

CFLAGS = -Wall $(INCLUDES)
#CFLAGS = -Wall $(INCLUDES)
LDFLAGS = -L$(SDK_LIB_DIR) -L$(SAMPLE_TOP_DIR)/lib
SDK_LIBS = -ldbi -ldci -ldsp -lisp -lispcore -ladvapi -lvmm

INSTALL_DIR ?= $(SAMPLE_TOP_DIR)/bin

SENSOR ?= jxf22_mipi

ifeq ($(SENSOR),imx138)
SENSOR_LIB = -limx-138
else
SENSOR_LIB = -l$(SENSOR)
endif
CFLAGS += -DSENSOR=\"$(SENSOR)\"
SDK_LIBS += $(SENSOR_LIB)
