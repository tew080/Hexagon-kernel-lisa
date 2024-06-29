# SPDX-License-Identifier: GPL-2.0-only
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-y	= printk.o
obj-$(CONFIG_PRINTK)	+= printk_safe.o
obj-$(CONFIG_A11Y_BRAILLE_CONSOLE)	+= braille.o
