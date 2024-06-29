# SPDX-License-Identifier: GPL-2.0-only
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

ccflags-y += -I$(src)			# needed for trace events

obj-$(CONFIG_ANDROID_BINDERFS)		+= binderfs.o
obj-$(CONFIG_ANDROID_BINDER_IPC)	+= binder.o binder_alloc.o
obj-$(CONFIG_ANDROID_BINDER_IPC_SELFTEST) += binder_alloc_selftest.o
obj-$(CONFIG_ANDROID_VENDOR_HOOKS) += vendor_hooks.o
obj-$(CONFIG_ANDROID_SIMPLE_LMK)	+= simple_lmk.o psi.o
