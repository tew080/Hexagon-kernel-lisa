# SPDX-License-Identifier: GPL-2.0

# GCOV must not instrument itself in value profile, or kernel cannot be booted.
ifeq ($(CONFIG_PGO_GEN),y)
GCOV_PROFILE := n
endif

ccflags-y := -DSRCTREE='"$(srctree)"' -DOBJTREE='"$(objtree)"'

obj-y := base.o fs.o
obj-$(CONFIG_GCOV_FORMAT_3_4) += gcc_base.o gcc_3_4.o
obj-$(CONFIG_GCOV_FORMAT_4_7) += gcc_base.o gcc_4_7.o
obj-$(CONFIG_CC_IS_CLANG) += clang.o
