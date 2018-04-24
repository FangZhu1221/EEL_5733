obj-m := assignment6_mod1.o

KERNEL_DIR = /usr/src/linux-headers-$(shell uname -r)

all:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(shell pwd)

userapp: userapp.o
	cc userapp.o -pthread -o userapp

userapp.o: userapp.c
	cc -c userapp.c -lpthread

clean-userapp:
	rm -rf userapp

clean:
	rm -rf *.o *.ko .*.mod.o.cmd .*.o.cmd .*.ko.cmd *.symvers *.mod.c .tmp_versions *.order *~
