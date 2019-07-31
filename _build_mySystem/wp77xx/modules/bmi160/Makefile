obj-m += bmi160.o
bmi160-objs += bmi160_core.o

src = /home/htandat/WorkSpace/WorkSpace1/linux_kernel_modules/bmi160

ccflags-y += -DCONFIG_BMI160
ccflags-y += -DCONFIG_BMI160_I2C
ccflags-y += -DCONFIG_IIO
ccflags-y += -DCONFIG_IIO_BUFFER
ccflags-y += -DCONFIG_IIO_TRIGGERED_BUFFER
ccflags-y += -DREGMAP

KBUILD := /home/htandat/WorkSpace/WorkSpace1/leaf-data/wp77dev/wp77-toolchain/sysroots/armv7a-neon-poky-linux-gnueabi/usr/src/kernel
export CROSS_COMPILE := /home/htandat/WorkSpace/WorkSpace1/leaf-data/wp77dev/wp77-toolchain/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-
export ARCH := arm

all:
	make -C $(KBUILD) M=/home/htandat/WorkSpace/WorkSpace1/_build_mySystem/wp77xx/modules/bmi160 modules

clean:
	 make -C $(KBUILD) M=/home/htandat/WorkSpace/WorkSpace1/_build_mySystem/wp77xx/modules/bmi160 clean
