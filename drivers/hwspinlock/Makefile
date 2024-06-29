# SPDX-License-Identifier: GPL-2.0
#
# Generic Hardware Spinlock framework
#
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-$(CONFIG_HWSPINLOCK)		+= hwspinlock_core.o
obj-$(CONFIG_HWSPINLOCK_OMAP)		+= omap_hwspinlock.o
obj-$(CONFIG_HWSPINLOCK_QCOM)		+= qcom_hwspinlock.o
obj-$(CONFIG_HWSPINLOCK_SIRF)		+= sirf_hwspinlock.o
obj-$(CONFIG_HWSPINLOCK_SPRD)		+= sprd_hwspinlock.o
obj-$(CONFIG_HWSPINLOCK_STM32)		+= stm32_hwspinlock.o
obj-$(CONFIG_HSEM_U8500)		+= u8500_hsem.o
