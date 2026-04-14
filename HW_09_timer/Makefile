obj-m += ex_timer.o
KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(CURDIR)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

format:
	clang-format -i *.c || echo "установи clang-format"

check:
	$(KDIR)/scripts/checkpatch.pl --no-tree -f *.c

load: all
	sudo insmod ex_timer.ko

unload:
	sudo rmmod ex_timer