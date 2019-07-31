obj-m += bq27xxx_battery.o
bq27xxx_battery-objs += bq27xxx_battery_i2c.o
bq27xxx_battery-objs += devres_backport.o
bq27xxx_battery-objs += bq27xxx_battery_source.o
bq27xxx_battery-objs += power_supply_backport.o

src = /home/htandat/WorkSpace/WorkSpace1/linux_kernel_modules/bq27xxx


KBUILD := /home/htandat/WorkSpace/WorkSpace1/leaf-data/wp77dev/wp77-toolchain/sysroots/armv7a-neon-poky-linux-gnueabi/usr/src/kernel
export CROSS_COMPILE := /home/htandat/WorkSpace/WorkSpace1/leaf-data/wp77dev/wp77-toolchain/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-
export ARCH := arm

all:
	make -C $(KBUILD) M=/home/htandat/WorkSpace/WorkSpace1/_build_mySystem/wp77xx/modules/bq27xxx_battery modules

clean:
	 make -C $(KBUILD) M=/home/htandat/WorkSpace/WorkSpace1/_build_mySystem/wp77xx/modules/bq27xxx_battery clean
