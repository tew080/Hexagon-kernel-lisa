# SPDX-License-Identifier: GPL-2.0-only
#
# Makefile for Counter devices
#
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-$(CONFIG_COUNTER) += counter.o

obj-$(CONFIG_104_QUAD_8)	+= 104-quad-8.o
obj-$(CONFIG_STM32_TIMER_CNT)	+= stm32-timer-cnt.o
obj-$(CONFIG_STM32_LPTIMER_CNT)	+= stm32-lptimer-cnt.o
obj-$(CONFIG_FTM_QUADDEC)	+= ftm-quaddec.o
