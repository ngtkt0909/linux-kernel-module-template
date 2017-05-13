# linux-kernel-module-template

* master:  
[![Build Status](https://travis-ci.org/ngtkt0909/linux-kernel-module-template.svg?branch=master)](https://travis-ci.org/ngtkt0909/linux-kernel-module-template)

* develop:  
[![Build Status](https://travis-ci.org/ngtkt0909/linux-kernel-module-template.svg?branch=develop)](https://travis-ci.org/ngtkt0909/linux-kernel-module-template)

## Overview
### 00.hello
**Hello World of linux kernel module**.
Print a message to kernel's message buffer
on the module loaded/unloaded, open()/close(), write()/read().

### 01.ktimer
**Kernel Timer Template**.
Timer handler using kernel timer is periodically executed, and
it increments counter every 100ms.

### 02.hrtimer
**High-resolution Timer Template**.
Timer handler using high-resolution timer is periodically executed, and
it increments counter every 100ms.

## Install
### Self Compiling (for Ubuntu, etc)
Download **linux-headers**:
```shell
$ sudo apt-get install linux-headers-`uname -r`
```

Make a kernel module:
```shell
$ make
```

### Cross Compiling (for Raspberry Pi)
Build cross compilation environment
(refer to the [official documentation](https://www.raspberrypi.org/documentation/linux/kernel/building.md "link to KERNEL BUILDING") for details).

On 32bit Linux:
```shell
$ git clone --depth=1 https://github.com/raspberrypi/linux
$ git clone --depth=1 https://github.com/raspberrypi/tools
$ cd linux
$ KERNEL=kernel7
$ make ARCH=arm CROSS_COMPILE=../tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf- bcm2709_defconfig
$ make ARCH=arm CROSS_COMPILE=../tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf- zImage modules dtbs
```

On 64bit Linux:
```shell
$ git clone --depth=1 https://github.com/raspberrypi/linux
$ git clone --depth=1 https://github.com/raspberrypi/tools
$ cd linux
$ KERNEL=kernel7
$ make ARCH=arm CROSS_COMPILE=../tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf- bcm2709_defconfig
$ make ARCH=arm CROSS_COMPILE=../tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf- zImage modules dtbs
```

Change first and second lines of Makefile.raspberrypi (**KPATH**, **TPATH**) according to download path on the previous step.

Make a kernel module:
```shell
$ make -f Makefile.raspberrypi
```

### Cross Compiling (for Digilent Board)
Download and Install [Vivado Design Suite HLx Edition](https://japan.xilinx.com/products/design-tools/vivado.html "link to Xilinx").

Add bellow line to your `.bashrc` in order to add cross-compiler path (change version according to your Vivado):
``` shell
export PATH=$PATH:/opt/Xilinx/SDK/2017.1/gnu/arm/lin/bin
```

Build cross compilation environment.
```shell
$ git clone -b master-next https://github.com/DigilentInc/Linux-Digilent-Dev.git
$ cd Linux-Digilent-Dev
$ make ARCH=arm CROSS_COMPILE=arm-xilinx-linux-gnueabi- xilinx_zynq_defconfig
$ make ARCH=arm CROSS_COMPILE=arm-xilinx-linux-gnueabi-
```

Change first line of Makefile.digilent (**KPATH**) according to download path on the previous step.

Make a kernel module:
```shell
$ make -f Makefile.digilent
```

## Usage
### 00.hello
Load the kernel module:
```shell
$ sudo insmod hello.ko
```

Can see the loaded kernel module:
```shell
$ lsmod | grep hello
```

Can see exported valiable:
```shell
$ cat /sys/module/hello/parameters/g_dev_major
$ cat /sys/module/hello/parameters/g_dev_minor
```

Unload the kernel module:
```shell
$ sudo rmmod hello
```

### 01.ktimer
Load the kernel module:
```shell
$ sudo insmod ktimer.ko
```

Can see the loaded kernel module:
```shell
$ lsmod | grep ktimer
```

Can see exported valiable:
```shell
$ cat /sys/module/ktimer/parameters/g_counter
```

Unload the kernel module:
```shell
$ sudo rmmod ktimer
```

### 02.hrtimer
Load the kernel module:
```shell
$ sudo insmod hrtimer.ko
```

Can see the loaded kernel module:
```shell
$ lsmod | grep hrtimer
```

Can see exported valiable:
```shell
$ cat /sys/module/hrtimer/parameters/g_counter
```

Unload the kernel module:
```shell
$ sudo rmmod hrtimer
```

## License
[MIT](https://github.com/ngtkt0909/linux-kernel-module-template/blob/master/LICENSE-MIT) and
[GPL](https://github.com/ngtkt0909/linux-kernel-module-template/blob/master/LICENSE-GPL)

## Author
[T. Ngtk](https://github.com/ngtkt0909)
