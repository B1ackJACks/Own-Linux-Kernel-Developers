// SPDX-License-Identifier: GPL-2.0-only
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/delay.h>

static void work_cb(struct work_struct *work)
{
	pr_info("Workqueue: run, in_interrupt=%lu, can_sleep=%u\n",
	(unsigned long)in_interrupt(), !in_interrupt());
	msleep(100);
	pr_info("Workqueue: done\n");
}

static DECLARE_WORK(my_work, work_cb);

static int __init ex_workqueue_init(void)
{
	pr_info("Workqueue: init\n");
	schedule_work(&my_work);
	return 0;
}

static void __exit ex_workqueue_exit(void)
{
	flush_work(&my_work);
	pr_info("Workqueue: exit\n");
}

module_init(ex_workqueue_init);
module_exit(ex_workqueue_exit);
MODULE_LICENSE("GPL");