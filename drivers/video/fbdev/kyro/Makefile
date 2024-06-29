# SPDX-License-Identifier: GPL-2.0-only
#
# Makefile for the Kyro framebuffer driver
#
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-$(CONFIG_FB_KYRO)	+= kyrofb.o

kyrofb-objs	:= STG4000Ramdac.o STG4000VTG.o STG4000OverlayDevice.o \
		   STG4000InitDevice.o fbdev.o
