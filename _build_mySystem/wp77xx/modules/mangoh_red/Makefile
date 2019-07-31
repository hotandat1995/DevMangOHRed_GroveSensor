obj-m += mangoh_red.o
mangoh_red-objs += mangoh_red_board.o
mangoh_red-objs += mangoh_red_mux.o

src = /home/htandat/WorkSpace/WorkSpace1/linux_kernel_modules/mangoh

ccflags-y += -DDEBUG
ccflags-y += -I/linux_kernel_modules/ltc294x
ccflags-y += -I/linux_kernel_modules/bq27xxx
ccflags-y += -I/linux_kernel_modules/bq24296
ccflags-y += -I/linux_kernel_modules/iot_slot
ccflags-y += -I/linux_kernel_modules/led

KBUILD := /home/htandat/WorkSpace/WorkSpace1/leaf-data/wp77dev/wp77-toolchain/sysroots/armv7a-neon-poky-linux-gnueabi/usr/src/kernel
export CROSS_COMPILE := /home/htandat/WorkSpace/WorkSpace1/leaf-data/wp77dev/wp77-toolchain/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-
export ARCH := arm

all:
	make -C $(KBUILD) M=/home/htandat/WorkSpace/WorkSpace1/_build_mySystem/wp77xx/modules/mangoh_red 'KBUILD_EXTRA_SYMBOLS=/home/htandat/WorkSpace/WorkSpace1/_build_mySystem/wp77xx/modules/bmi160-i2c/Module.symvers /home/htandat/WorkSpace/WorkSpace1/_build_mySystem/wp77xx/modules/bmp280-i2c/Module.symvers /home/htandat/WorkSpace/WorkSpace1/_build_mySystem/wp77xx/modules/bq24296/Module.symvers /home/htandat/WorkSpace/WorkSpace1/_build_mySystem/wp77xx/modules/bq27xxx_battery/Module.symvers /home/htandat/WorkSpace/WorkSpace1/_build_mySystem/wp77xx/modules/led/Module.symvers /home/htandat/WorkSpace/WorkSpace1/_build_mySystem/wp77xx/modules/ltc294x/Module.symvers ' modules

clean:
	 make -C $(KBUILD) M=/home/htandat/WorkSpace/WorkSpace1/_build_mySystem/wp77xx/modules/mangoh_red clean
