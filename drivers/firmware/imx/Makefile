# SPDX-License-Identifier: GPL-2.0
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-$(CONFIG_IMX_DSP)		+= imx-dsp.o
obj-$(CONFIG_IMX_SCU)		+= imx-scu.o misc.o imx-scu-irq.o
obj-$(CONFIG_IMX_SCU_PD)	+= scu-pd.o
