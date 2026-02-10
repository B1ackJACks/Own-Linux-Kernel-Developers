// SPDX-License-Identifier: GPL-2.0
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/bitmap.h>
#include <linux/module.h>

#define BUSY 1
#define FREE 0

struct block_manager {
	unsigned long *bitmap;
	int total_blocks;
};

struct block_manager bmng;

static int check_size(unsigned long *map, int n, int bmng_size, int mode)
{
	int k = 0;

	for (int i = 0; i < bmng_size && k < n; i++) {
		if (test_bit(i, map) == mode)
			k++;
		else
			k = 0;
	}

	if (!(k < n))
		return 1; // success
	else
		return 0; // failed
}

static int alloc_blocks(struct block_manager *bmng, int n)
{
	unsigned long *map = bmng->bitmap;
	int bmng_size = bmng->total_blocks;
	int k = 0;

	if (check_size(map, n, bmng_size, FREE)) {
		for (int i = 0; i < bmng_size && k < n; i++) {
			if (!test_bit(i, map)) {
				set_bit(i, map);
				k++;
			}
		}
		return 0;
	}
	return -1;
}

static int free_blocks(struct block_manager *bmng)
{
	unsigned long *map = bmng->bitmap;
	int bmng_size = bmng->total_blocks;

	for (int i = 0; i < bmng_size; i++)
		clear_bit(i, map);

	return 0;
}

static void dump_state(struct block_manager *bmng)
{
	unsigned long *map = bmng->bitmap;
	int bmng_size = bmng->total_blocks;
	pr_info("%d",test_bit(bmng_size-1, map));
	for (int i = bmng_size - 2; i > -1; i--)
		pr_cont("%d", test_bit(i, map));

	pr_cont("\n");
}

static int __init mod_init(void)
{
	bmng.bitmap = bitmap_zalloc(16, GFP_KERNEL);
	if (!bmng.bitmap)
		return -ENOMEM;

	bmng.total_blocks = 16;

	int err = alloc_blocks(&bmng, 1);
	if (err)
		return -1;

	dump_state(&bmng);
	free_blocks(&bmng);

	err = alloc_blocks(&bmng, 3);
	if (err)
		return -1;

	dump_state(&bmng);
	free_blocks(&bmng);

	err = alloc_blocks(&bmng, 5);
	if (err)
		return -1;

	dump_state(&bmng);
	free_blocks(&bmng);

	return 0;
}

static void __exit mod_exit(void)
{
	bitmap_free(bmng.bitmap);
}

module_init(mod_init);
module_exit(mod_exit);
MODULE_LICENSE("GPL");