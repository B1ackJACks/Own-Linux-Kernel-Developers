obj-m += ex_softirq.o	ex_tasklets.o	ex_workqueue.o
KDIR ?= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

format:
	clang-format -i *.c || echo "установи clang-format"

check:
	$(KDIR)/scripts/checkpatch.pl --no-tree -f *.c