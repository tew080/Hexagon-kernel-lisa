# SPDX-License-Identifier: GPL-2.0-only
debugfs-objs	:= inode.o file.o

ifeq ($(CONFIG_DEBUG_KMEMLEAK),y)
ccflags-y	:= -DCONFIG_DEBUG_FS
endif

ifdef  CONFIG_DEBUG_FS
obj-y += debugfs.o
else ifdef CONFIG_PGO_GEN 
obj-y += debugfs.o
endif