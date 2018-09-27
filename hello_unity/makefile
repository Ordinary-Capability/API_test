
CROSS_COMPILE ?= arm-fullhan-linux-uclibcgnueabi-
C_COMPILER = $(CROSS_COMPILE)gcc


UNITY_ROOT = ../Unity

CLEANUP = rm -fr

CFLAGS = -Wall

TARGET = helloworld



INC_DIRS = -I$(UNITY_ROOT)/src -I../


SYMBOLS = 

SRC_FILES = $(UNITY_ROOT)/src/unity.c  hello_world.c hello_world_runner.c


LIB_DIRS = 

LIBS = -lm

all: clean default

default: $(SRC_FILES)
	$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SRC_FILES) -o $(TARGET)  $(LIB_DIRS)  $(LIBS) 

hello_world_runner.c: hello_world.c
	ruby $(UNITY_ROOT)/auto/generate_test_runner.rb hello_world.c hello_world_runner.c

clean: 
	$(CLEANUP) *.o
	$(CLEANUP) $(TARGET)
