KPATH := /usr/src/linux-headers-`uname -r`

obj-m := ./src/ktimer.o

.PHONY: all clean doc

all:
	make -C $(KPATH) M=$(CURDIR) modules

clean:
	make -C $(KPATH) M=$(CURDIR) clean
	$(RM) ./doc

doc:
	doxygen ./Doxyfile
