# SPDX-License-Identifier: GPL-2.0
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

ccflags-y := -DSRCTREE='"$(srctree)"' -DOBJTREE='"$(objtree)"'

obj-y := base.o fs.o
obj-$(CONFIG_GCOV_FORMAT_3_4) += gcc_base.o gcc_3_4.o
obj-$(CONFIG_GCOV_FORMAT_4_7) += gcc_base.o gcc_4_7.o
obj-$(CONFIG_CC_IS_CLANG) += clang.o
