# SPDX-License-Identifier: GPL-2.0-only
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-$(CONFIG_FS_ENCRYPTION)	+= fscrypto.o

fscrypto-y := crypto.o \
	      fname.o \
	      hkdf.o \
	      hooks.o \
	      keyring.o \
	      keysetup.o \
	      keysetup_v1.o \
	      policy.o

fscrypto-$(CONFIG_BLOCK) += bio.o
fscrypto-$(CONFIG_FS_ENCRYPTION_INLINE_CRYPT) += inline_crypt.o
