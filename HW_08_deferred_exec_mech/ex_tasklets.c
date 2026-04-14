// SPDX-License-Identifier: GPL-2.0-only
#include <linux/module.h>
#include <linux/interrupt.h>

static void tasklet_cb(struct tasklet_struct *t)
{
	pr_info("Tasklet: run, cpu=%u, in_interrupt=%u\n",
		smp_processor_id(), in_interrupt());
}

static DECLARE_TASKLET(t_normal, tasklet_cb);
static DECLARE_TASKLET(t_hi, tasklet_cb);

static int __init ex_tasklets_init(void)
{
	pr_info("Tasklet: init\n");
	tasklet_disable(&t_normal);
	tasklet_schedule(&t_normal);
	tasklet_enable(&t_normal);
	tasklet_hi_schedule(&t_hi);
	return 0;
}

static void __exit ex_tasklets_exit(void)
{
	tasklet_kill(&t_normal);
	tasklet_kill(&t_hi);
	pr_info("Tasklet: exit\n");
}

module_init(ex_tasklets_init);
module_exit(ex_tasklets_exit);
MODULE_LICENSE("GPL");