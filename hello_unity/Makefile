
UNITY_ROOT = ../Unity
RUBY = ruby $(UNITY_ROOT)/auto/generate_test_runner.rb

test_origin_c = src/hello_world.c
test_origin_c += src/111.c

test_runner_c = $(test_origin_c:%.c=%_runner.c)

all: $(test_runner_c) build_bin

%_runner.c: %.c 
	$(RUBY) "$<" "$@" 

build_bin:
	@scons -j16 --buildlib=libkernel
	@scons -j16 --buildlib=libdriver
	@scons -j16



menuconfig:
	@scons --menuconfig

clean:
	@scons -c
	@rm -rf build
	@rm -f rtthread.elf rtthread.bin rtthread.map rtthread.dis rtthread.s
	@rm -f rtt_stage[12].img
	@rm -f lib*_gcc.a

distclean: clean
	@rm -f rtconfig_kernel.h
	@rm -f rtconfig_driver.h
	@rm -f rtconfig_platform.h
	@rm -f .config
	@rm -f flash.img

%_defconfig:
	@scons --defconfig=$@
