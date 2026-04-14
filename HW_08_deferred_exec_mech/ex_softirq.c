// SPDX-License-Identifier: GPL-2.0-only
#include <linux/module.h>
#include <linux/interrupt.h>

static struct tasklet_struct demo_bh;

static void softirq_demo(unsigned long data)
{
	pr_info("Softirq: processing in the context of an interrupt\n");

}
static int __init ex_softirq_init(void)
{
	pr_info("Softirq: initalization\n");
	tasklet_init(&demo_bh, softirq_demo, 0);
	tasklet_schedule(&demo_bh);
	return 0;
}

static void __exit ex_softirq_exit(void)
{
	tasklet_kill(&demo_bh);
	pr_info("Softirq: unloading\n");
}

module_init(ex_softirq_init);
module_exit(ex_softirq_exit);
MODULE_LICENSE("GPL");