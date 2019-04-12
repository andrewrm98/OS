ccflags-y=-std=gnu99
obj-m := ki1.o
KDIR := /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	gcc test1.c -o test1

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm test1

