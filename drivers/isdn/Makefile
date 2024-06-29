# SPDX-License-Identifier: GPL-2.0
# Makefile for the kernel ISDN subsystem and device drivers.

# Object files in subdirectories
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-$(CONFIG_ISDN_CAPI)			+= capi/
obj-$(CONFIG_MISDN)			+= mISDN/
obj-$(CONFIG_ISDN)			+= hardware/
