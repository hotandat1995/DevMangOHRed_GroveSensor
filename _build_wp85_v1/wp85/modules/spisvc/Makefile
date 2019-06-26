obj-m += spisvc.o

src = /home/htandat/myWorkSpace/leaf-data/wp85dev/wp85-legato/drivers/spisvc


KBUILD := /home/htandat/myWorkSpace/leaf-data/wp85dev/wp85-toolchain/sysroots/armv7a-neon-poky-linux-gnueabi/usr/src/kernel
export CROSS_COMPILE := /home/htandat/myWorkSpace/leaf-data/wp85dev/wp85-toolchain/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-
export ARCH := arm

all:
	make -C $(KBUILD) M=/home/htandat/myWorkSpace/_build_wp85_v1/wp85/modules/spisvc modules

clean:
	 make -C $(KBUILD) M=/home/htandat/myWorkSpace/_build_wp85_v1/wp85/modules/spisvc clean
