# SPDX-License-Identifier: GPL-2.0-only
#
# Makefile for the kernel mmc device drivers.
#
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-$(CONFIG_MMC)		+= core/
obj-$(subst m,y,$(CONFIG_MMC))	+= host/
