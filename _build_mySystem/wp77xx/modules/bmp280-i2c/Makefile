obj-m += bmp280-i2c.o

src = /home/htandat/WorkSpace/WorkSpace1/linux_kernel_modules/bmp280


KBUILD := /home/htandat/WorkSpace/WorkSpace1/leaf-data/wp77dev/wp77-toolchain/sysroots/armv7a-neon-poky-linux-gnueabi/usr/src/kernel
export CROSS_COMPILE := /home/htandat/WorkSpace/WorkSpace1/leaf-data/wp77dev/wp77-toolchain/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-
export ARCH := arm

all:
	make -C $(KBUILD) M=/home/htandat/WorkSpace/WorkSpace1/_build_mySystem/wp77xx/modules/bmp280-i2c 'KBUILD_EXTRA_SYMBOLS=/home/htandat/WorkSpace/WorkSpace1/_build_mySystem/wp77xx/modules/bmp280/Module.symvers ' modules

clean:
	 make -C $(KBUILD) M=/home/htandat/WorkSpace/WorkSpace1/_build_mySystem/wp77xx/modules/bmp280-i2c clean
