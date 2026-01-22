// SPDX-License-Identifier: GPL-2.0
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/kfifo.h>
#include <linux/string.h>

#define EVENT_SIZE 4

struct security_event {
	pid_t pid;
	char event_type[16];
	char details[32];
};

static DEFINE_KFIFO(sec_events, struct security_event, EVENT_SIZE);

static void init_event(struct security_event *value, pid_t pid,
		       const char *event_type, const char *details)
{
	value->pid = pid;
	strscpy(value->event_type, event_type, sizeof(value->event_type));
	strscpy(value->details, details, sizeof(value->details));
	pr_info("Event Added: %s\n", value->event_type);
}

static int __init mod_init(void)
{
	struct security_event ev1;

	init_event(&ev1, 1234, "EXEC", "/tmp/.malware.bin");
	kfifo_put(&sec_events, ev1);

	struct security_event ev2;

	init_event(&ev2, 5678, "NET_CONN", "192.168.1.100:4444");
	kfifo_put(&sec_events, ev2);

	struct security_event ev3;

	init_event(&ev3, 9012, "FILE_ACCESS", "/etc/shadow");
	kfifo_put(&sec_events, ev3);

	struct security_event ev4;

	init_event(&ev4, 3456, "PRIV_ESC", "sudo exploit");
	kfifo_put(&sec_events, ev4);

	return 0;
}

static void __exit mod_exit(void)
{
	struct security_event value;

	while (!kfifo_is_empty(&sec_events)) {
		if (kfifo_get(&sec_events, &value) != 1)
			break;
		pr_info("Event processed: %s\n", value.event_type);
	}

	pr_info("Module unloaded\n");
}

module_init(mod_init);
module_exit(mod_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vsevolod Zag");