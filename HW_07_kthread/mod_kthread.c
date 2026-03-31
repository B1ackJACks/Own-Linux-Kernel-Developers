// SPDX-License-Identifier: GPL-2.0
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/rwsem.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/slab.h>



#define NUM_READERS 3
#define NUM_WRITERS 1

static int shared_data;
static struct rw_semaphore rw_sem;
static struct task_struct *threads[NUM_READERS + NUM_WRITERS];

/* Reader thread - acquires read lock (multiple readers allowed) */
static int reader_thread(void *data)
{
	int id = *(int *)data;

	while (!kthread_should_stop()) {
		down_read(&rw_sem);

		pr_info("Reader %d, value = %d\n", id, shared_data);

		msleep(1000);

		up_read(&rw_sem);
		msleep(500);
	}

	pr_info("Reader %d: finished work\n", id);
	kfree(data);
	return 0;
}

/* Writer thread - acquires write lock (exclusive access) */
static int writer_thread(void *data)
{
	int id = *(int *)data;
	static int writer_counter;
	while (!kthread_should_stop()) {
		down_write(&rw_sem);

		writer_counter++;
		shared_data = writer_counter;
		pr_info("Writer %d: New value = %d\n", id, shared_data);

		msleep(2000);

		up_write(&rw_sem);
		msleep(1500);
	}

	pr_info("Writer %d: Finished work\n", id);
	kfree(data);
	return 0;
}

/* Module initialization - creates reader and writer threads */
static int __init rw_module_init(void)
{
	int thread_idx = 0;
	int i;

	pr_info("Module initialization started...\n");

	init_rwsem(&rw_sem);

	for (i = 0; i < NUM_READERS; i++) {
		int *id = kmalloc(sizeof(int), GFP_KERNEL);

		if (!id) {
			pr_err("Error alloc of memory\n");
			return -ENOMEM;
		}
		*id = i + 1;

		threads[thread_idx] = kthread_run(reader_thread, id, "reader_%d", i + 1);

		if (IS_ERR(threads[thread_idx])) {
			pr_err("Error creating reader thread\n");
			kfree(id);
			return PTR_ERR(threads[thread_idx]);
		}
		thread_idx++;
	}

	for (i = 0; i < NUM_WRITERS; i++) {
		int *id = kmalloc(sizeof(int), GFP_KERNEL);

		if (!id) {
			pr_err("Error alloc of memory\n");
			return -ENOMEM;
		}
		*id = i + 1;

		threads[thread_idx] = kthread_run(writer_thread, id, "writer_%d", i + 1);

		if (IS_ERR(threads[thread_idx])) {
			pr_err("Error creating writer thread\n");
			kfree(id);
			return PTR_ERR(threads[thread_idx]);
		}
		thread_idx++;
	}

	pr_info("%d threads created\n", thread_idx);
	return 0;
}

/* Module exit - stops all threads */
static void __exit rw_module_exit(void)
{
	int total_threads = NUM_READERS + NUM_WRITERS;
	int i;

	pr_info("Stopping threads...\n");

	for (i = 0; i < total_threads; i++) {
		if (threads[i] && !IS_ERR(threads[i]))
			kthread_stop(threads[i]);
	}

	pr_info("Module unloaded\n");
}

module_init(rw_module_init);
module_exit(rw_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vsevolod Zag");
MODULE_DESCRIPTION("RW Semaphore Example");