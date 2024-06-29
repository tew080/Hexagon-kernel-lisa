# SPDX-License-Identifier: GPL-2.0-only
#
# Makefile for the nubus specific drivers.
#
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-y := nubus.o bus.o

obj-$(CONFIG_PROC_FS) += proc.o
