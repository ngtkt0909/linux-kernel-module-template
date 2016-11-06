/**
 * linux-kernel-module-template
 *
 * Copyright (c) 2016 T. Ngtk
 *
 * Released under the MIT License.
 * https://github.com/ngtkt0909/linux-kernel-module-template/blob/master/LICENSE
 */

#include <linux/module.h>
#include <linux/init.h>

MODULE_AUTHOR("T. Ngtk");
MODULE_LICENSE("MIT");

static int hello_value;

static int hello_init(void)
{
	printk(KERN_INFO "hello_init\n");
	hello_value = 100;
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_INFO "hello_exit\n");
}

module_init(hello_init);
module_exit(hello_exit);

module_param(hello_value, int, S_IRUSR | S_IRGRP | S_IROTH);
