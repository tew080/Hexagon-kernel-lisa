# SPDX-License-Identifier: GPL-2.0
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

host1x-y = \
	bus.o \
	syncpt.o \
	dev.o \
	intr.o \
	cdma.o \
	channel.o \
	job.o \
	debug.o \
	mipi.o \
	hw/host1x01.o \
	hw/host1x02.o \
	hw/host1x04.o \
	hw/host1x05.o \
	hw/host1x06.o \
	hw/host1x07.o

obj-$(CONFIG_TEGRA_HOST1X) += host1x.o
