# SPDX-License-Identifier: GPL-2.0
#
# Makefile for the memory technology device drivers.
#
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

# Core functionality.
obj-$(CONFIG_MTD)		+= mtd.o
mtd-y				:= mtdcore.o mtdsuper.o mtdconcat.o mtdpart.o mtdchar.o

obj-y				+= parsers/

# 'Users' - code which presents functionality to userspace.
obj-$(CONFIG_MTD_BLKDEVS)	+= mtd_blkdevs.o
obj-$(CONFIG_MTD_BLOCK)		+= mtdblock.o
obj-$(CONFIG_MTD_BLOCK_RO)	+= mtdblock_ro.o
obj-$(CONFIG_FTL)		+= ftl.o
obj-$(CONFIG_NFTL)		+= nftl.o
obj-$(CONFIG_INFTL)		+= inftl.o
obj-$(CONFIG_RFD_FTL)		+= rfd_ftl.o
obj-$(CONFIG_SSFDC)		+= ssfdc.o
obj-$(CONFIG_SM_FTL)		+= sm_ftl.o
obj-$(CONFIG_MTD_OOPS)		+= mtdoops.o
obj-$(CONFIG_MTD_SWAP)		+= mtdswap.o

nftl-objs		:= nftlcore.o nftlmount.o
inftl-objs		:= inftlcore.o inftlmount.o

obj-y		+= chips/ lpddr/ maps/ devices/ nand/ tests/

obj-$(CONFIG_MTD_SPI_NOR)	+= spi-nor/
obj-$(CONFIG_MTD_UBI)		+= ubi/
obj-$(CONFIG_MTD_HYPERBUS)	+= hyperbus/
