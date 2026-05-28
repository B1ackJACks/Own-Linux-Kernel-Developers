// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <linux/ktime.h>

static int __init my_vmalloc_init(void)
{
	unsigned long v_low = 1;
	unsigned long v_high = 256 * 1024 * 1024UL;
	unsigned long v_test;
	unsigned long v_max = 0;
	void *v_buf;
	ktime_t v_start, v_end;
	s64 v_time_ms;

	pr_info("vmalloc: searching for max allocation size\n");

	while ((v_buf = vmalloc(v_high)) != NULL) {
		vfree(v_buf);
		v_high *= 2;
		if (v_high > 1024 * 1024 * 1024UL)
			break;
	}

	while (v_low <= v_high) {
		v_test = v_low + (v_high - v_low) / 2;
		if (v_test < 1)
			v_test = 1;

		pr_info("vmalloc: %lu byte\n", v_test);
		v_start = ktime_get();
		v_buf = vmalloc(v_test);
		v_end = ktime_get();

		if (v_buf) {
			v_time_ms = ktime_ms_delta(v_end, v_start);
			pr_info("vmalloc: SUCCESS\n");
			pr_info("vmalloc: %lu byte, %lld ms, type: virtual\n",
				v_test, v_time_ms);
			vfree(v_buf);
			v_max = v_test;
			v_low = v_test + 1;
		} else {
			pr_info("vmalloc: FAIL, err_msg = allocation failed\n");
			v_high = v_test - 1;
		}
	}

	if (v_max > 0)
		pr_info("vmalloc: max allocation size = %lu bytes\n", v_max);
	else
		pr_info("vmalloc: could not allocate memory\n");

	return 0;
}

static void __exit my_vmalloc_exit(void)
{
	pr_info("vmalloc module unloaded\n");
}

module_init(my_vmalloc_init);
module_exit(my_vmalloc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vsevolod Zag");
MODULE_DESCRIPTION("vmalloc allocation test");