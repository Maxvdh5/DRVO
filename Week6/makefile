KDIR = ./5.0.13-yocto-standard
PWD  = $(shell pwd)
MAKE = /usr/bin/make

%.ko : %.c
	make $(*).ko obj-m=$(*).o -C ${KDIR} M=${PWD} ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-modules