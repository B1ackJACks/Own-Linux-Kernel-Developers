obj-m+=ex_bin_search.o
obj-m+=ex_bin_tree.o

KERNEL_DIR:=/lib/modules/$(shell uname -r)/build
CHECKPATCH := /usr/src/linux-headers-$(shell uname -r)/scripts/checkpatch.pl
.PHONY: all clean format check
all:
	make -C $(KERNEL_DIR) M=$(shell pwd) modules
clean:
	make -C $(KERNEL_DIR) M=$(shell pwd) clean
check:
	$(CHECKPATCH) --no-tree --file ex_bin_search.c ex_bin_tree.c
