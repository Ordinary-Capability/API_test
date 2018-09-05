include ../Rules.make

LIBS = -ldbi -ldci -ldsp -lisp -lispcore -ladvapi -lvmm -ljxf22_mipi -lpthread -lvlcview -lmipi -lrt -lgtest

ifeq ($(DEBUG),0)
  CFLAGS 	+= -O2
else
  CFLAGS 	+= -O0 -g3
endif

BIN = API_isp

SRCS = $(wildcard *.c)
SRCS += $(wildcard ../*.c)
OBJS	= $(SRCS:%.c=%.o)  
DEPENDS	= $(subst .o,.d,$(OBJS))

all: $(BIN) install

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

install:
	-mkdir -p $(INSTALL_DIR)
	-cp -a $(BIN) $(INSTALL_DIR)

clean:
	rm -f $(BIN)
	rm -f *.o *.d ../*.o ../*.d
	rm -f ../common/*.o ../common/*.d

ifneq "$(MAKECMDGOALS)" "clean"
  -include $(DEPENDS)
endif

%.o: %.c
	$(CC) $(CFLAGS) -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

.PHONY: clean
