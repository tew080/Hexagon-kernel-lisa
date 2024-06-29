# SPDX-License-Identifier: GPL-2.0
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-$(CONFIG_ASYNC_CORE) += async_tx.o
obj-$(CONFIG_ASYNC_MEMCPY) += async_memcpy.o
obj-$(CONFIG_ASYNC_XOR) += async_xor.o
obj-$(CONFIG_ASYNC_PQ) += async_pq.o
obj-$(CONFIG_ASYNC_RAID6_RECOV) += async_raid6_recov.o
obj-$(CONFIG_ASYNC_RAID6_TEST) += raid6test.o
