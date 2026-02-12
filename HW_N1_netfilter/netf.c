// SPDX-License-Identifier: GPL-2.0
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>

#define TARGET_IP_BE 0x33FA7892

static unsigned int simple_hook(void *priv, struct sk_buff *skb,
				const struct nf_hook_state *state)
{
	struct iphdr *iph = ip_hdr(skb);

	if (!skb)
		return NF_ACCEPT;

	if (!iph || iph->version != 4)
		return NF_ACCEPT;

	if (be32_to_cpu(iph->saddr) == TARGET_IP_BE) {
		pr_info("DROPPED packet from itmo.ru\n");
		return NF_DROP;
	}

	return NF_ACCEPT;
}

static struct nf_hook_ops nfho = {
	.hook = simple_hook,
	.hooknum = NF_INET_PRE_ROUTING,
	.pf = NFPROTO_IPV4,
	.priority = NF_IP_PRI_FIRST,
};

static int __init simple_nf_init(void)
{
	int ret = nf_register_net_hook(&init_net, &nfho);

	if (ret)
		pr_err("Hook registration failed\n");
	else
		pr_info("simple_nf loaded: blocking itmo.ru\n");
	return ret;
}

static void __exit simple_nf_exit(void)
{
	nf_unregister_net_hook(&init_net, &nfho);
	pr_info("simple_nf unloaded\n");
}

module_init(simple_nf_init);
module_exit(simple_nf_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vsevolod Zag");
MODULE_DESCRIPTION("Minimal netfilter IP blocker");