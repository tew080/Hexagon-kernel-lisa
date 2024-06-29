# SPDX-License-Identifier: GPL-2.0
#
# Makefile for Open-Channel SSDs.
#
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-$(CONFIG_NVM)		:= core.o
obj-$(CONFIG_NVM_PBLK)		+= pblk.o
pblk-y				:= pblk-init.o pblk-core.o pblk-rb.o \
				   pblk-write.o pblk-cache.o pblk-read.o \
				   pblk-gc.o pblk-recovery.o pblk-map.o \
				   pblk-rl.o pblk-sysfs.o
