KPATH := /usr/src/linux-headers-3.13.0-100-generic
DOCS := ./doc

obj-m := ./src/hello.o

.PHONY: all clean doc

all:
	make -C $(KPATH) M=$(PWD) modules

clean:
	make -C $(KPATH) M=$(PWD) clean
	$(RM) $(DOCS)

doc:
	doxygen ./Doxyfile
