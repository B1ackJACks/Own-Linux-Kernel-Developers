all: mod_kthread.ko user_threads

mod_kthread.ko:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

user_threads: user_threads.c
	gcc -o user_threads user_threads.c -pthread

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f user_threads

load:
	insmod mod_kthread.ko

unload:
	rmmod mod_kthread.ko

check:
	scripts/checkpatch.pl --terse --file mod_kthread.c
	scripts/checkpatch.pl --terse --file user_threads.c

format:
	astyle --style=linux mod_kthread.c user_threads.c

.PHONY: all clean load unload check format