# SPDX-License-Identifier: GPL-2.0
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-$(CONFIG_DAX) += dax.o
obj-$(CONFIG_DEV_DAX) += device_dax.o
obj-$(CONFIG_DEV_DAX_KMEM) += kmem.o

dax-y := super.o
dax-y += bus.o
device_dax-y := device.o

obj-y += pmem/
