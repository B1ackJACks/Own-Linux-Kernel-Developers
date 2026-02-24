// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/bsearch.h>
#include <linux/sort.h>
#include <linux/kernel.h>

#define SIZE_ARRAY 5

struct threat_entry {
	u32 ip_address;
	char country_code[4];
};

static struct threat_entry *threat_array;

static int bin_cmp_ip(const void *key, const void *elem)
{
	u32 k = ((const struct threat_entry *)key)->ip_address;
	u32 e = ((const struct threat_entry *)elem)->ip_address;

	return (k > e) - (k < e);
}

static int sort_cmp_ip(const void *a, const void *b)
{
	u32 ip_a = ((const struct threat_entry *)a)->ip_address;
	u32 ip_b = ((const struct threat_entry *)b)->ip_address;

	return (ip_a > ip_b) - (ip_a < ip_b);
}

static int __init mod_init(void)
{
	threat_array = kmalloc_array(SIZE_ARRAY, sizeof(struct threat_entry), GFP_KERNEL);
	if (!threat_array)
		return -ENOMEM;

	threat_array[0] = (struct threat_entry){0xC0A80164, "CN"};
	threat_array[1] = (struct threat_entry){0xC0A80132, "RU"};
	threat_array[2] = (struct threat_entry){0xC0A80119, "US"};
	threat_array[3] = (struct threat_entry){0xC0A8010A, "DE"};
	threat_array[4] = (struct threat_entry){0xC0A801F0, "BR"};

	pr_info("[INIT] Initial array of threats:\n");
	for (int i = 0; i < SIZE_ARRAY; i++) {
		pr_info("  [%d] IP=%pI4, Country=%s\n",
			i, &threat_array[i].ip_address, threat_array[i].country_code);
	}

	sort(threat_array, SIZE_ARRAY, sizeof(struct threat_entry), sort_cmp_ip, NULL);

	pr_info("[SORT] Array sorted by IP address for binary search.\n");

	u32 target_ip = 0xC0A80119;
	struct threat_entry key = {.ip_address = target_ip};

	struct threat_entry *found = bsearch(&key, threat_array, SIZE_ARRAY,
					     sizeof(struct threat_entry), bin_cmp_ip);

	if (found) {
		pr_info("[SEARCH] SUCCESS: Element found!\n");
		pr_info("  -> Target IP: %pI4\n", &found->ip_address);
		pr_info("  -> Country: %s\n", found->country_code);
	} else {
		pr_info("[SEARCH] FAILED: Element with IP %pI4 not found.\n", &target_ip);
	}

	return 0;
}

static void __exit mod_exit(void)
{
	kfree(threat_array);
	pr_info("[EXIT] Memory freed. Module unloaded.\n");
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vsevolod Zag");
MODULE_DESCRIPTION("Binary Search Demo");