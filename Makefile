KPATH := /usr/src/linux-headers-`uname -r`

obj-m := ./src/hello.o

.PHONY: all clean doc

all:
	make -C $(KPATH) M=$(PWD) modules

clean:
	make -C $(KPATH) M=$(PWD) clean
	rm -rf ./doc

doc:
	doxygen ./Doxyfile
