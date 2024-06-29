# SPDX-License-Identifier: GPL-2.0
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

CFLAGS_core.o				:= -I$(src)
icc-core-objs				:= core.o
icc-test-objs				:= debugfs-test.o

obj-$(CONFIG_INTERCONNECT)		+= icc-core.o
obj-$(CONFIG_INTERCONNECT_TEST)		+= icc-test.o
obj-$(CONFIG_INTERCONNECT_QCOM)		+= qcom/
