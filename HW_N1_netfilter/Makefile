obj-m:=netf.o
KERNEL_DIR:=/lib/modules/$(shell uname -r)/build
.PHONY: all clean format check
CHECKPATCH := /usr/src/linux-headers-$(shell uname -r)/scripts/checkpatch.pl
all:
	make -C $(KERNEL_DIR) M=$(shell pwd) modules
clean:
	make -C $(KERNEL_DIR) M=$(shell pwd) clean
format:
	clang-format -i netf.c
check:
	$(CHECKPATCH) --no-tree --max-line-length=100 -f nfctrl.c
