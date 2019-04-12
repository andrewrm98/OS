obj-m := ki2.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	gcc test2.c -o test2
	gcc procAncestry.c -o procAncestry

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm test2 procAncestry
