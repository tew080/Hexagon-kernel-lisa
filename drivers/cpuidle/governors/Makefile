# SPDX-License-Identifier: GPL-2.0-only
#
# Makefile for cpuidle governors.
#
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mtune=cortex-a78 -mtune=cortex-a55 -flto=thin -funroll-loops -ftree-vectorize -Ofast 

obj-$(CONFIG_CPU_IDLE_GOV_LADDER) += ladder.o
obj-$(CONFIG_CPU_IDLE_GOV_MENU) += menu.o
obj-$(CONFIG_CPU_IDLE_GOV_TEO) += teo.o
obj-$(CONFIG_CPU_IDLE_GOV_HALTPOLL) += haltpoll.o
