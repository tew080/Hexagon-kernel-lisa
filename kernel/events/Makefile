# SPDX-License-Identifier: GPL-2.0
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

ifdef CONFIG_FUNCTION_TRACER
CFLAGS_REMOVE_core.o = $(CC_FLAGS_FTRACE)
endif

obj-y := core.o ring_buffer.o callchain.o

obj-$(CONFIG_HAVE_HW_BREAKPOINT) += hw_breakpoint.o
obj-$(CONFIG_UPROBES) += uprobes.o

