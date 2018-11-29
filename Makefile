obj-m += chrdrv.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc polling.c -o polling.o

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm *.o