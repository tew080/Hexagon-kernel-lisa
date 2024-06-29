# SPDX-License-Identifier: GPL-2.0-only
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-$(CONFIG_VIDEO_GS1662) += gs1662.o
obj-$(CONFIG_CXD2880_SPI_DRV) += cxd2880-spi.o

ccflags-y += -I $(srctree)/drivers/media/dvb-frontends/cxd2880
