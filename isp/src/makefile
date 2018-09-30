include ../../Rules.make

ifeq ($(DEBUG),0)
  CFLAGS 	+= -O2
else
  CFLAGS 	+= -O0 -g3
endif

LIBS = -ladvapi -ldbi -ldci -ldsp -lisp -lispcore  -lvmm -ljxf22_mipi -lpthread -lvlcview -lmipi -lrt -lgtest

UNITY_ROOT = ../../Unity
RUBY = ruby $(UNITY_ROOT)/auto/generate_test_runner.rb

test_origin_c = isp_api_test.c
test_runner_c = $(test_origin_c:%.c=%_runner.c)

BIN = API_isp

SRCS = $(wildcard *.c)
SRCS += $(wildcard ../../*.c)
SRCS += $(wildcard ../../Unity/src/*.c)
OBJS	= $(SRCS:%.c=%.o)  
DEPENDS	= $(subst .o,.d,$(OBJS))

all: $(test_runner_c) $(BIN) 

%_runner.c: %.c
	$(RUBY) "$<" "$@"

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)


clean:
	rm -f $(BIN)
	rm -f *.o *.d ../*.o ../*.d
	rm -f ../common/*.o ../common/*.d

ifneq "$(MAKECMDGOALS)" "clean"
  -include $(DEPENDS)
endif

%.o: %.c
	#$(CC) $(CFLAGS) -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	$(CC) $(CFLAGS) -c -o "$@" "$<"


.PHONY: clean
