#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("i2c:bq27200");
MODULE_ALIAS("i2c:bq27210");
MODULE_ALIAS("i2c:bq27500");
MODULE_ALIAS("i2c:bq27510");
MODULE_ALIAS("i2c:bq27520");
MODULE_ALIAS("i2c:bq27500-1");
MODULE_ALIAS("i2c:bq27510g1");
MODULE_ALIAS("i2c:bq27510g2");
MODULE_ALIAS("i2c:bq27510g3");
MODULE_ALIAS("i2c:bq27520g1");
MODULE_ALIAS("i2c:bq27520g2");
MODULE_ALIAS("i2c:bq27520g3");
MODULE_ALIAS("i2c:bq27520g4");
MODULE_ALIAS("i2c:bq27521");
MODULE_ALIAS("i2c:bq27530");
MODULE_ALIAS("i2c:bq27531");
MODULE_ALIAS("i2c:bq27541");
MODULE_ALIAS("i2c:bq27542");
MODULE_ALIAS("i2c:bq27546");
MODULE_ALIAS("i2c:bq27742");
MODULE_ALIAS("i2c:bq27545");
MODULE_ALIAS("i2c:bq27421");
MODULE_ALIAS("i2c:bq27425");
MODULE_ALIAS("i2c:bq27426");
MODULE_ALIAS("i2c:bq27441");
MODULE_ALIAS("i2c:bq27621");
MODULE_ALIAS("of:N*T*Cti,bq27200*");
MODULE_ALIAS("of:N*T*Cti,bq27210*");
MODULE_ALIAS("of:N*T*Cti,bq27500*");
MODULE_ALIAS("of:N*T*Cti,bq27510*");
MODULE_ALIAS("of:N*T*Cti,bq27520*");
MODULE_ALIAS("of:N*T*Cti,bq27500-1*");
MODULE_ALIAS("of:N*T*Cti,bq27510g1*");
MODULE_ALIAS("of:N*T*Cti,bq27510g2*");
MODULE_ALIAS("of:N*T*Cti,bq27510g3*");
MODULE_ALIAS("of:N*T*Cti,bq27520g1*");
MODULE_ALIAS("of:N*T*Cti,bq27520g2*");
MODULE_ALIAS("of:N*T*Cti,bq27520g3*");
MODULE_ALIAS("of:N*T*Cti,bq27520g4*");
MODULE_ALIAS("of:N*T*Cti,bq27521*");
MODULE_ALIAS("of:N*T*Cti,bq27530*");
MODULE_ALIAS("of:N*T*Cti,bq27531*");
MODULE_ALIAS("of:N*T*Cti,bq27541*");
MODULE_ALIAS("of:N*T*Cti,bq27542*");
MODULE_ALIAS("of:N*T*Cti,bq27546*");
MODULE_ALIAS("of:N*T*Cti,bq27742*");
MODULE_ALIAS("of:N*T*Cti,bq27545*");
MODULE_ALIAS("of:N*T*Cti,bq27421*");
MODULE_ALIAS("of:N*T*Cti,bq27425*");
MODULE_ALIAS("of:N*T*Cti,bq27426*");
MODULE_ALIAS("of:N*T*Cti,bq27441*");
MODULE_ALIAS("of:N*T*Cti,bq27621*");
