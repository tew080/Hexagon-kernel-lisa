# SPDX-License-Identifier: GPL-2.0
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mtune=cortex-a78 -mtune=cortex-a55 -flto=thin -funroll-loops -ftree-vectorize 

obj-$(CONFIG_SCHED_WALT) += walt.o boost.o sched_avg.o qc_vas.o core_ctl.o trace.o
obj-$(CONFIG_CPU_FREQ) += cpu-boost.o
