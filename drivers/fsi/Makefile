# SPDX-License-Identifier: GPL-2.0-only
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-$(CONFIG_FSI) += fsi-core.o
obj-$(CONFIG_FSI_MASTER_HUB) += fsi-master-hub.o
obj-$(CONFIG_FSI_MASTER_GPIO) += fsi-master-gpio.o
obj-$(CONFIG_FSI_MASTER_AST_CF) += fsi-master-ast-cf.o
obj-$(CONFIG_FSI_SCOM) += fsi-scom.o
obj-$(CONFIG_FSI_SBEFIFO) += fsi-sbefifo.o
obj-$(CONFIG_FSI_OCC) += fsi-occ.o
