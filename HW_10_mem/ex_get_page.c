// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gfp.h>
#include <linux/ktime.h>
#include <linux/mm.h>

static int __init get_page_init(void)
{
	int order_idx;
	unsigned long block_bytes;
	unsigned long page_addr;
	ktime_t gp_start, gp_end;
	s64 gp_time_ms;

	pr_info("get_free_page: testing up to MAX_ORDER=%d\n", MAX_ORDER);

	for (order_idx = 0; order_idx <= MAX_ORDER; order_idx++) {
		block_bytes = PAGE_SIZE << order_idx;
		pr_info("get_free_page: %lu byte (order=%d)\n", block_bytes, order_idx);

		gp_start = ktime_get();
		page_addr = __get_free_pages(GFP_KERNEL, order_idx);
		gp_end = ktime_get();

		if (page_addr) {
			gp_time_ms = ktime_ms_delta(gp_end, gp_start);
			pr_info("get_free_page: SUCCESS\n");
			pr_info("get_free_page: %lu byte, %lld ms, type: physical (pages)\n",
				block_bytes, gp_time_ms);
			free_pages(page_addr, order_idx);
		} else {
			pr_info("get_free_page: FAIL, err_msg = allocation failed\n");
			pr_info("get_free_page: max successful order = %d (%lu bytes)\n",
				order_idx - 1,
				(order_idx > 0 ? PAGE_SIZE << (order_idx - 1) : 0));
			break;
		}
	}

	return 0;
}

static void __exit get_page_exit(void)
{
	pr_info("get_free_page module unloaded\n");
}

module_init(get_page_init);
module_exit(get_page_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vsevolod Zag");
MODULE_DESCRIPTION("get_free_pages allocation test");