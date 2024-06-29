# SPDX-License-Identifier: GPL-2.0-only
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-$(CONFIG_CONNECTOR)		+= cn.o
obj-$(CONFIG_PROC_EVENTS)	+= cn_proc.o

cn-y				+= cn_queue.o connector.o
