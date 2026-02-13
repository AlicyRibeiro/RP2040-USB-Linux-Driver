#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x5d5b6583, "usb_put_dev" },
	{ 0xcb8b6ec6, "kfree" },
	{ 0x52b15b3b, "__unregister_chrdev" },
	{ 0xd710adbf, "__kmalloc_noprof" },
	{ 0x76a26ca1, "usb_bulk_msg" },
	{ 0xa61fd7aa, "__check_object_size" },
	{ 0x092a35a2, "_copy_to_user" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0xef4e4365, "usb_deregister" },
	{ 0xbd03ed67, "random_kmalloc_seed" },
	{ 0xfed1e3bc, "kmalloc_caches" },
	{ 0x70db3fe4, "__kmalloc_cache_noprof" },
	{ 0xa9289d30, "usb_get_dev" },
	{ 0x11169e9e, "__register_chrdev" },
	{ 0x092a35a2, "_copy_from_user" },
	{ 0xd272d446, "__fentry__" },
	{ 0xaba46e12, "usb_register_driver" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xe8213e80, "_printk" },
	{ 0xba157484, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0x5d5b6583,
	0xcb8b6ec6,
	0x52b15b3b,
	0xd710adbf,
	0x76a26ca1,
	0xa61fd7aa,
	0x092a35a2,
	0xd272d446,
	0xef4e4365,
	0xbd03ed67,
	0xfed1e3bc,
	0x70db3fe4,
	0xa9289d30,
	0x11169e9e,
	0x092a35a2,
	0xd272d446,
	0xaba46e12,
	0xd272d446,
	0xe8213e80,
	0xba157484,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"usb_put_dev\0"
	"kfree\0"
	"__unregister_chrdev\0"
	"__kmalloc_noprof\0"
	"usb_bulk_msg\0"
	"__check_object_size\0"
	"_copy_to_user\0"
	"__stack_chk_fail\0"
	"usb_deregister\0"
	"random_kmalloc_seed\0"
	"kmalloc_caches\0"
	"__kmalloc_cache_noprof\0"
	"usb_get_dev\0"
	"__register_chrdev\0"
	"_copy_from_user\0"
	"__fentry__\0"
	"usb_register_driver\0"
	"__x86_return_thunk\0"
	"_printk\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");

MODULE_ALIAS("usb:vCAFEp4001d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "9ABBABC8564661042308B87");
