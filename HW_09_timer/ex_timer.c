// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

struct timer_list my_timer;
static int period_sec = 3;
static int k = 1;

void print_hello(struct timer_list *t)
{
	(void)t;
	pr_info("%d Hello, world!\n", k);
	if (k != 10) {
		k++;
		mod_timer(&my_timer, jiffies + msecs_to_jiffies(period_sec * 1000));
	}
}

static int __init timer_func_init(void)
{
	timer_setup(&my_timer, print_hello, 0);
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(period_sec * 1000));
	return 0;
}

static void __exit timer_func_exit(void)
{
	del_timer_sync(&my_timer);
}

module_init(timer_func_init);
module_exit(timer_func_exit);
MODULE_AUTHOR("Vsevolod Zag");
MODULE_LICENSE("GPL");