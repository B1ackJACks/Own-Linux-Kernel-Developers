// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/rbtree.h>

struct node {
	struct rb_node rb;
	int id;
	char name[32];
};

static struct rb_root root = RB_ROOT;

static struct node *find(int id)
{
	struct rb_node *n = root.rb_node;

	while (n) {
		struct node *cur = rb_entry(n, struct node, rb);

		if (id < cur->id)
			n = n->rb_left;
		else if (id > cur->id)
			n = n->rb_right;
		else
			return cur;
	}
	return NULL;
}

static void insert(struct node *new_node)
{
	struct rb_node **link = &root.rb_node;
	struct rb_node *parent = NULL;

	while (*link) {
		parent = *link;
		struct node *cur = rb_entry(*link, struct node, rb);

		if (new_node->id < cur->id)
			link = &(*link)->rb_left;
		else
			link = &(*link)->rb_right;
	}

	rb_link_node(&new_node->rb, parent, link);
	rb_insert_color(&new_node->rb, &root);
}

static void remove_node(struct node *n)
{
	rb_erase(&n->rb, &root);
	kfree(n);
}

static int __init tree_init(void)
{
	pr_info("[TREE] Adding users...\n");

	struct node *n1 = kmalloc(sizeof(*n1), GFP_KERNEL);
	
    if (!n1)
		return -ENOMEM;

	n1->id = 3;
	strcpy(n1->name, "Alice");
	insert(n1);
	pr_info("  + User %d: %s\n", n1->id, n1->name);

	struct node *n2 = kmalloc(sizeof(*n2), GFP_KERNEL);

    if (!n2)
		return -ENOMEM;

	n2->id = 1;
	strcpy(n2->name, "Bob");
	insert(n2);
	pr_info("  + User %d: %s\n", n2->id, n2->name);

	struct node *n3 = kmalloc(sizeof(*n3), GFP_KERNEL);

    if (!n3)
		return -ENOMEM;

	n3->id = 2;
	strcpy(n3->name, "Charlie");
	insert(n3);
	pr_info("  + User %d: %s\n", n3->id, n3->name);

	pr_info("[TREE] All users (sorted):\n");
	struct rb_node *ptr;

	for (ptr = rb_first(&root); ptr; ptr = rb_next(ptr)) {
		struct node *cur = rb_entry(ptr, struct node, rb);

		pr_info("  -> ID=%d, Name=%s\n", cur->id, cur->name);
	}

	struct node *found = find(2);

	if (found)
		pr_info("[SEARCH] Found: ID=%d, Name=%s\n", found->id, found->name);

	if (found) {
		remove_node(found);
		pr_info("[DELETE] Removed user ID=2\n");
	}

	return 0;
}

static void __exit tree_exit(void)
{
	pr_info("[EXIT] Freeing memory...\n");

	struct rb_node *ptr = rb_first(&root);

	while (ptr) {
		struct node *cur = rb_entry(ptr, struct node, rb);
		struct rb_node *next = rb_next(ptr);

		remove_node(cur);
		ptr = next;
	}
	pr_info("[EXIT] Done.\n");
}

module_init(tree_init);
module_exit(tree_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vsevolod");
MODULE_DESCRIPTION("RB-Tree Demo");