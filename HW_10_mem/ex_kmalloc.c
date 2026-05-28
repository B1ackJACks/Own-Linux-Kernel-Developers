// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/ktime.h>

static int __init kmalloc_init(void)
{
	void *buf_ptr;
	ktime_t tm_start, tm_end;
	s64 elapsed_ms;
	unsigned long largest_ok = 0;
	unsigned long low = 1;
	unsigned long high = 64 * 1024 * 1024UL;
	unsigned long test;

	pr_info("kmalloc: starting binary search for max allocation\n");

	buf_ptr = kmalloc(high, GFP_KERNEL);
	if (buf_ptr) {
		kfree(buf_ptr);
		high *= 2;
		while ((buf_ptr = kmalloc(high, GFP_KERNEL)) != NULL) {
			kfree(buf_ptr);
			high *= 2;
		}
	}

	while (low <= high) {
		test = low + (high - low) / 2;
		if (test < 1)
			test = 1;

		pr_info("kmalloc: %lu byte\n", test);
		tm_start = ktime_get();
		buf_ptr = kmalloc(test, GFP_KERNEL);
		tm_end = ktime_get();

		if (buf_ptr) {
			elapsed_ms = ktime_ms_delta(tm_end, tm_start);
			pr_info("kmalloc: SUCCESS\n");
			pr_info("kmalloc: %lu byte, %lld ms, type: physical\n",
				test, elapsed_ms);
			kfree(buf_ptr);
			largest_ok = test;
			low = test + 1;
		} else {
			pr_info("kmalloc: FAIL, err_msg = allocation failed\n");
			high = test - 1;
		}
	}

	if (largest_ok > 0)
		pr_info("kmalloc: maximum allocation size = %lu bytes\n", largest_ok);
	else
		pr_info("kmalloc: could not allocate even 1 byte\n");

	return 0;
}

static void __exit kmalloc_exit(void)
{
	pr_info("kmalloc module unloaded\n");
}

module_init(kmalloc_init);
module_exit(kmalloc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vsevolod Zag");
MODULE_DESCRIPTION("kmalloc allocation test");