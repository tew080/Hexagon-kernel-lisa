# SPDX-License-Identifier: GPL-2.0-only
# Exynos DEVFREQ Event Drivers
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-$(CONFIG_DEVFREQ_EVENT_EXYNOS_NOCP) += exynos-nocp.o
obj-$(CONFIG_DEVFREQ_EVENT_EXYNOS_PPMU) += exynos-ppmu.o
obj-$(CONFIG_DEVFREQ_EVENT_ROCKCHIP_DFI) += rockchip-dfi.o
