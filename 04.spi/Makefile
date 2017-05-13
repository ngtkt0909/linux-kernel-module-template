KPATH := $(HOME)/raspberrypi/linux
TPATH := $(HOME)/raspberrypi/tools
ARCH := arm
CROSS_COMPILE := $(TPATH)/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-
DOCS := ./doc

obj-m := ./src/spi_l3gd20.o

.PHONY: all clean doc

all:
	make -C $(KPATH) M=$(PWD) modules ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)

clean:
	make -C $(KPATH) M=$(PWD) clean
	$(RM) $(DOCS)

install:
	mkdir -p /lib/modules/`uname -r`/misc
	cp ./src/spi_l3gd20.ko /lib/modules/`uname -r`/misc
	depmod -a

uninstall:
	$(RM) /lib/modules/`uname -r`/misc/spi_l3gd20.ko
	depmod -a

connect:
	modprobe spi_l3gd20

disconnect:
	modprobe -r spi_l3gd20

read:
	cat /sys/bus/spi/drivers/spi_l3gd20/spi0.0/gyro

doc:
	doxygen ./Doxyfile
