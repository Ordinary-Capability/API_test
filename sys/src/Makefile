include ../../Rules.make

ifeq ($(DEBUG),0)
  CFLAGS 	+= -O2
else
  CFLAGS 	+= -O0 -g3
endif

UNITY_ROOT = ../../Unity
RUBY = ruby $(UNITY_ROOT)/auto/generate_test_runner.rb
LIBS = -ladvapi -ldbi -ldci -ldsp -lisp -lispcore  -lvmm -ljxf22_mipi -lpthread -lvlcview -lmipi -lrt -lm

test_origin_c = sys_api_test.c

test_runner_c = $(test_origin_c:%.c=%_runner.c)

BIN = $(test_origin_c:%.c=%.bin)

SRCS = $(wildcard ../../*.c)
OBJS	= $(SRCS:%.c=%.o)  
SRC_UNITY = $(wildcard ../../Unity/src/*.c)
OBJ_UNITY = $(SRC_UNITY:%.c=%.o)

all: clean_runner $(test_runner_c) $(BIN) 

clean_runner: 
	rm -f *_runner.c

%_runner.c: %.c
	$(RUBY) "$<" "$@"

%.o: %.c
	$(CC) $(CFLAGS) -c -o "$@" "$<"

%.bin: $(OBJS) %.o %_runner.o $(OBJ_UNITY)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm -f $(BIN)
	rm -f *.o *.d ../*.o ../*.d
	rm -f ../common/*.o ../common/*.d

