# SPDX-License-Identifier: GPL-2.0-only
#
# Makefile for the IPACK bridge device drivers.
#
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-$(CONFIG_IPACK_BUS)		+= ipack.o
obj-y				+= devices/
obj-y				+= carriers/
