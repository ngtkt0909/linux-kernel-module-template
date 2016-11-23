# linux-kernel-module-template

## Overview
**Template for Linux kernel module**.
Print a message to kernel's message buffer when a module is loaded/unloaded.
In order to see the message, run command: `dmesg`.

## Install
### Self Compiling
Install development environment:
```shell
$ sudo apt-get install build-essential
```

Download **linux-headers** (change version according to the latest version):
```shell
$ sudo apt-get install linux-headers-3.13.0-100-generic
```

Change first line of Makefile (**KPATH**) according to installed **linux-headers** version and path.

Make a kernel module (./src/hello.ko):
```shell
$ make
```

### Cross Compiling (for Raspberry Pi)
Install development environment:
```shell
$ sudo apt-get install build-essential git
```

Build cross compilation environment (For details, refer to [Raspberry Pi](https://www.raspberrypi.org/documentation/linux/kernel/building.md "link to KERNEL BUILDING")):
```shell
$ git clone --depth=1 https://github.com/raspberrypi/linux
$ git clone --depth=1 https://github.com/raspberrypi/tools
$ cd linux
$ KERNEL=kernel7
$ make ARCH=arm CROSS_COMPILE=../tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf- bcm2709_defconfig
$ make ARCH=arm CROSS_COMPILE=../tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf- zImage modules dtbs
```

Change first and second lines of Makefile.raspberrypi (**KPATH**, **TPATH**) according to download path on previous step.

Make a kernel module (./src/hello.ko):
```shell
$ make -f Makefile.raspberrypi
```

## Usage
Load the kernel module.
```shell
$ sudo insmod hello.ko
```

Can see the loaded kernel module:
```shell
$ lsmod | grep hello
```

Can see exported valiable:
```shell
$ cat /sys/module/hello/parameters/hello_value
```

If want to remove, Unload the kernel module.
```shell
$ sudo rmmod hello
```

## License
[MIT](https://github.com/ngtkt0909/linux-kernel-module-template/blob/master/LICENSE-MIT) and
[GPL](https://github.com/ngtkt0909/linux-kernel-module-template/blob/master/LICENSE-GPL)

## Author
[T. Ngtk](https://github.com/ngtkt0909)
