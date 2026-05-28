// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/ktime.h>

static int __init my_kmem_cache_init(void)
{
	unsigned long c_low = 1;
	unsigned long c_high = 8 * 1024 * 1024UL;
	unsigned long c_test;
	unsigned long c_best = 0;
	struct kmem_cache *cache_obj;
	void *obj_buf;
	ktime_t c_start, c_end;
	s64 c_time_ms;

	pr_info("kmem_cache: searching max object size\n");

	while (c_low <= c_high) {
		c_test = c_low + (c_high - c_low) / 2;
		if (c_test < 1)
			c_test = 1;

		cache_obj = kmem_cache_create("my_test_cache", c_test, 0,
					      SLAB_HWCACHE_ALIGN, NULL);
		if (!cache_obj) {
			pr_info("kmem_cache: cache creation failed for %lu bytes\n", c_test);
			c_high = c_test - 1;
			continue;
		}

		pr_info("kmem_cache: %lu byte (cache created)\n", c_test);
		c_start = ktime_get();
		obj_buf = kmem_cache_alloc(cache_obj, GFP_KERNEL);
		c_end = ktime_get();

		if (obj_buf) {
			c_time_ms = ktime_ms_delta(c_end, c_start);
			pr_info("kmem_cache: SUCCESS\n");
			pr_info("kmem_cache: %lu byte, %lld ms, type: physical (slab)\n",
				c_test, c_time_ms);
			kmem_cache_free(cache_obj, obj_buf);
			c_best = c_test;
			c_low = c_test + 1;
		} else {
			pr_info("kmem_cache: FAIL, err_msg = alloc from cache failed\n");
			c_high = c_test - 1;
		}

		kmem_cache_destroy(cache_obj);
	}

	if (c_best > 0)
		pr_info("kmem_cache: max object size = %lu bytes\n", c_best);
	else
		pr_info("kmem_cache: could not allocate any object\n");

	return 0;
}

static void __exit my_kmem_cache_exit(void)
{
	pr_info("kmem_cache module unloaded\n");
}

module_init(my_kmem_cache_init);
module_exit(my_kmem_cache_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vsevolod Zag");
MODULE_DESCRIPTION("kmem_cache allocation test");