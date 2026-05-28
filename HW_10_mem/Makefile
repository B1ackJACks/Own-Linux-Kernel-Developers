obj-m := ex_kmalloc.o ex_vmalloc.o ex_kmem_cache.o ex_mempool.o ex_get_page.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
format:
	indent -linux -i4 -nut *.c

CHECKPATCH ?= /lib/modules/$(shell uname -r)/build/scripts/checkpatch.pl

check:
	@if [ -x $(CHECKPATCH) ]; then \
		for f in *.c; do \
			echo "Checking $$f ..."; \
			$(CHECKPATCH) --no-tree --file $$f; \
		done; \
	else \
		echo "checkpatch.pl not found, skipping"; \
	fi