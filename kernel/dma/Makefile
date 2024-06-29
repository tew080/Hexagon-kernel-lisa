# SPDX-License-Identifier: GPL-2.0
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-$(CONFIG_HAS_DMA)			+= mapping.o direct.o dummy.o
obj-$(CONFIG_DMA_CMA)			+= contiguous.o
obj-$(CONFIG_DMA_DECLARE_COHERENT)	+= coherent.o
obj-$(CONFIG_DMA_VIRT_OPS)		+= virt.o
obj-$(CONFIG_DMA_API_DEBUG)		+= debug.o
obj-$(CONFIG_SWIOTLB)			+= swiotlb.o
obj-$(CONFIG_DMA_REMAP)			+= remap.o
