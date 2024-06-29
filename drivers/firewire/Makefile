# SPDX-License-Identifier: GPL-2.0
#
# Makefile for the Linux IEEE 1394 implementation
#
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

firewire-core-y += core-card.o core-cdev.o core-device.o \
                   core-iso.o core-topology.o core-transaction.o
firewire-ohci-y += ohci.o
firewire-sbp2-y += sbp2.o
firewire-net-y  += net.o

obj-$(CONFIG_FIREWIRE)      += firewire-core.o
obj-$(CONFIG_FIREWIRE_OHCI) += firewire-ohci.o
obj-$(CONFIG_FIREWIRE_SBP2) += firewire-sbp2.o
obj-$(CONFIG_FIREWIRE_NET)  += firewire-net.o
obj-$(CONFIG_FIREWIRE_NOSY) += nosy.o
obj-$(CONFIG_PROVIDE_OHCI1394_DMA_INIT) += init_ohci1394_dma.o
