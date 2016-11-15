KPATH := /usr/src/linux-headers-3.13.0-100-generic

obj-m := ./src/hello.o

all:
	make -C $(KPATH) M=$(PWD) modules

clean:
	make -C $(KPATH) M=$(PWD) clean
