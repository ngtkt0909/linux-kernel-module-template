# Linux-Kernel-Module-Template

## Overview
**Template for Linux kernel module**.
Print a message to kernel's message buffer (see to run command: `dmesg`)
when a module is loaded/unloaded.

## Install
Install development environment:
```shell
	$ sudo apt-get install build-essential
```

Download **linux-headers** (change version according to the latest version):
```shell
	$ sudo apt-get install linux-headers-3.13.0-100-generic
```

Change first line of Makefile (**KPATHP**) according to installed **linux-headers** version and path.

Make a kernel module (hello.ko):
```shell
	$ make
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
[MIT](https://github.com/ngtkt0909/Linux-Kernel-Module-Template/blob/develop/LICENSE)

## Author
[ngtkt0909](https://github.com/ngtkt0909)
