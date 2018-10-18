include ../../Rules.make

ifeq ($(DEBUG),0)
  CFLAGS 	+= -O2
else
  CFLAGS 	+= -O0 -g3
endif

LIBS = -ladvapi -ldbi -ldci -ldsp -lisp -lispcore  -lvmm -ljxf22_mipi -lpthread -lvlcview -lmipi -lrt -lm

UNITY_ROOT = ../../Unity
RUBY = ruby $(UNITY_ROOT)/auto/generate_test_runner.rb

test_origin_c = isp_api_test.c
test_origin_c += isp_api_test2.c

test_runner_c = $(test_origin_c:%.c=%_runner.c)

#BIN = API_isp
BIN = $(test_origin_c:%.c=%.bin)

#SRCS = $(wildcard *.c) 
#SRCS += $(test_runner_c)
SRCS = $(wildcard ../../*.c)
#SRCS += $(wildcard ../../Unity/src/*.c)
OBJS	= $(SRCS:%.c=%.o)  
OBJ_UNITY = $(wildcard ../../Unity/src/*.o)

#DEPENDS	= $(subst .o,.d,$(OBJS))

all: clean_runner $(test_runner_c) $(BIN) 

clean_runner: 
	rm -f *_runner.c

%_runner.c: %.c
	$(RUBY) "$<" "$@"

%.o: %.c
	$(CC) $(CFLAGS) -c -o "$@" "$<"

#$(BIN): $(OBJS)
%.bin: $(OBJS) %.o %_runner.o $(OBJ_UNITY)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm -f $(BIN)
	rm -f *.o *.d ../*.o ../*.d
	rm -f ../common/*.o ../common/*.d

#ifneq "$(MAKECMDGOALS)" "clean"
#  -include $(DEPENDS)
#endif



.PHONY: clean 
