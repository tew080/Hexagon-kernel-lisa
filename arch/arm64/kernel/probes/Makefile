# SPDX-License-Identifier: GPL-2.0
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-$(CONFIG_KPROBES)		+= kprobes.o decode-insn.o	\
				   kprobes_trampoline.o		\
				   simulate-insn.o
obj-$(CONFIG_UPROBES)		+= uprobes.o decode-insn.o	\
				   simulate-insn.o
