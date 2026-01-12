
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

typedef struct {
    int id;
    char name[32];
    int priority;
    struct list_head list;
} task_entry;

static LIST_HEAD(task_list);

static void free_memory(void) {
    task_entry *pos, *tmp;

    list_for_each_entry_safe(pos, tmp, &task_list, list) {
        pr_info("Removed task: %s\n", pos->name);
        list_del(&pos->list);
        kfree(pos);
    }
}

static void print_list(struct list_head *head) {
    task_entry *pos;

    list_for_each_entry(pos, head, list) {
        pr_info("%d: %s\n", pos->priority, pos->name);
    }
}

static int add_task_sorted(struct list_head *head, int id, const char *name,
                           int priority) {
    task_entry *task, *pos;
    char inserted = 0;

    task = kmalloc(sizeof(*task), GFP_KERNEL);
    if (!task)
        return -ENOMEM;

    task->id = id;
    strlcpy(task->name, name, sizeof(task->name));
    task->priority = priority;
    INIT_LIST_HEAD(&task->list);

    list_for_each_entry(pos, head, list) {
        if (task->priority < pos->priority) {
            __list_add(&task->list, pos->list.prev, &pos->list);
            inserted = 1;
            break;
        }
    }

    if (!inserted)
        list_add_tail(&task->list, head);

    pr_info("Added task: %s (ID=%d, prio=%d)\n", task->name, task->id,
            task->priority);
    print_list(head);
    return 0;
}

static int build_scheduler(void) {
    if (add_task_sorted(&task_list, 1, "breakfast", 3) < 0)
        goto cleanup;
    if (add_task_sorted(&task_list, 2, "dinner", 1) < 0)
        goto cleanup;
    if (add_task_sorted(&task_list, 3, "working", 2) < 0)
        goto cleanup;
    return 0;

cleanup:
    free_memory();
    return -1;
}

static int __init mod_init(void) {
    pr_info("Module_start\n");
    if (build_scheduler() < 0) {
        pr_err("Scheduler build failed\n");
        return -1;
    }
    return 0;
}

static void __exit mod_exit(void) {
    free_memory();
    pr_info("Module_exit\n");
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(
    "Priority-based task scheduler using sorted kernel linked list");
MODULE_AUTHOR("VSEVOLOD_ZAG");