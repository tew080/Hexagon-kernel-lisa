# SPDX-License-Identifier: GPL-2.0
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

i3c-y				:= device.o master.o
obj-$(CONFIG_I3C)		+= i3c.o
obj-$(CONFIG_I3C)		+= master/
