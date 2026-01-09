#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>

#define MAX_SIZE 14

static char my_str[MAX_SIZE] = "_____________";
static int idx = 0;
static char ch_val = 'H';

static int int_idx_set(const char *val, const struct kernel_param *kp)
{
    int ret, tmp;
    ret = kstrtoint(val, 10, &tmp);
    if (ret)
        return ret;
    if (tmp < 0 || tmp > 12) {
        pr_err("idx %d out of range [0-12]\n", tmp);
        return -EINVAL;
    }
    *(int *)kp->arg = tmp;
    return 0;
}

static int int_idx_get(char *buf, const struct kernel_param *kp)
{
    return sprintf(buf, "%d\n", idx);
}

static const struct kernel_param_ops int_idx_params = {
    .set = int_idx_set,
    .get = int_idx_get,
};

module_param_cb(int_idx, &int_idx_params, &idx, 0644);
MODULE_PARM_DESC(int_idx, "Position index for character insertion (0-12)");

static int char_val_set(const char *val, const struct kernel_param *kp)
{
    int ret, tmp;
    ret = kstrtoint(val, 10, &tmp);
    if (ret)
        return ret;
    if (tmp < 32 || tmp > 126) {
        pr_err("Invalid ASCII code %d (allowed 32-126)\n", tmp);
        return -EINVAL;
    }
    if (idx < 0 || idx > 12) {
        pr_err("Index %d invalid during char write\n", idx);
        return -EINVAL;
    }
    *(char *)kp->arg = (char)tmp;
    my_str[idx] = (char)tmp;
    if (idx == 12)
        my_str[13] = '\0';
    pr_debug("Set char '%c' (ASCII %d) at idx %d\n", (char)tmp, tmp, idx);
    return 0;
}

static int char_val_get(char *buf, const struct kernel_param *kp)
{
    return sprintf(buf, "%c\n", ch_val);
}

static const struct kernel_param_ops char_val_params = {
    .set = char_val_set,
    .get = char_val_get,
};

module_param_cb(char_val, &char_val_params, &ch_val, 0644);
MODULE_PARM_DESC(char_val, "ASCII character code to insert (32-126)");

static int my_str_get(char *buf, const struct kernel_param *kp)
{
    return sprintf(buf, "%s\n", my_str);
}

static const struct kernel_param_ops my_str_params = {
    .set = NULL,
    .get = my_str_get,
};

module_param_cb(my_str, &my_str_params, NULL, 0444);
MODULE_PARM_DESC(my_str, "Resulting string (read-only)");

static int __init hw02_init(void)
{
    pr_info("Module loaded. Ready to write 'Hello, World!'\n");
    return 0;
}

static void __exit hw02_exit(void)
{
    pr_info("String built: '%s'\n", my_str);
    pr_info("Module unloaded\n");
}

module_init(hw02_init);
module_exit(hw02_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zagovenko Vsevolod");
MODULE_DESCRIPTION("Kernel module that builds 'Hello, World!' via sysfs parameters");
MODULE_VERSION("1.0");