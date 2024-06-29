# SPDX-License-Identifier: GPL-2.0
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-$(CONFIG_QCOM_BAM_DMA) += bam_dma.o
obj-$(CONFIG_QCOM_HIDMA_MGMT) += hdma_mgmt.o
hdma_mgmt-objs	 := hidma_mgmt.o hidma_mgmt_sys.o
obj-$(CONFIG_QCOM_HIDMA) +=  hdma.o
hdma-objs        := hidma_ll.o hidma.o hidma_dbg.o
obj-$(CONFIG_MSM_GPI_DMA) += gpi.o
obj-$(CONFIG_QCOM_PCI_EDMA) += pci-edma.o
