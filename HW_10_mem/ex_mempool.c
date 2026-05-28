// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mempool.h>
#include <linux/slab.h>
#include <linux/ktime.h>

static int __init my_mempool_init(void)
{
	unsigned long p_low = 1;
	unsigned long p_high = 8 * 1024 * 1024UL;
	unsigned long p_test;
	unsigned long p_best = 0;
	mempool_t *pool_ptr;
	void *pool_item;
	ktime_t p_start, p_end;
	s64 p_time_ms;
	int min_nr = 1;

	pr_info("mempool: searching max element size\n");

	while (p_low <= p_high) {
		p_test = p_low + (p_high - p_low) / 2;
		if (p_test < 1)
			p_test = 1;

		pool_ptr = mempool_create_kmalloc_pool(min_nr, p_test);
		if (!pool_ptr) {
			pr_info("mempool: pool creation failed for %lu bytes\n", p_test);
			p_high = p_test - 1;
			continue;
		}

		pr_info("mempool: %lu byte (pool created)\n", p_test);
		p_start = ktime_get();
		pool_item = mempool_alloc(pool_ptr, GFP_KERNEL);
		p_end = ktime_get();

		if (pool_item) {
			p_time_ms = ktime_ms_delta(p_end, p_start);
			pr_info("mempool: SUCCESS\n");
			pr_info("mempool: %lu byte, %lld ms, type: physical (mempool)\n",
				p_test, p_time_ms);
			mempool_free(pool_item, pool_ptr);
			p_best = p_test;
			p_low = p_test + 1;
		} else {
			pr_info("mempool: FAIL, err_msg = alloc from pool failed\n");
			p_high = p_test - 1;
		}

		mempool_destroy(pool_ptr);
	}

	if (p_best > 0)
		pr_info("mempool: max element size = %lu bytes\n", p_best);
	else
		pr_info("mempool: could not allocate any element\n");

	return 0;
}

static void __exit my_mempool_exit(void)
{
	pr_info("mempool module unloaded\n");
}

module_init(my_mempool_init);
module_exit(my_mempool_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vsevolod Zag");
MODULE_DESCRIPTION("mempool allocation test");