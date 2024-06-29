# SPDX-License-Identifier: GPL-2.0
#
# Makefile for HSI
#
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-$(CONFIG_HSI)		+= hsi.o
hsi-objs			:= hsi_core.o
hsi-$(CONFIG_HSI_BOARDINFO)	+= hsi_boardinfo.o
obj-y				+= controllers/
obj-y				+= clients/
