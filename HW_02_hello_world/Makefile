
KERNEL_DIR:=/lib/modules/$(shell uname -r)/build
all:
	make -C $(KERNEL_DIR) M=$(shell pwd) modules
clean:
	make -C $(KERNEL_DIR) M=$(shell pwd) clean
format:
	clang-format -i hello_module.c
checker:
	sudo python3 checker.py
.PHONY: all clean format checker

