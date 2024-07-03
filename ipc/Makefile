# SPDX-License-Identifier: GPL-2.0
#
# Makefile for the linux ipc.
#

ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod+sve2+fp16+aes+sha2+lse+simd -mcpu=cortex-a78 -mtune=cortex-a78  -funroll-loops -ftree-vectorize 
ccflags-y += -msve-vector-bits=128 -mfpu=neon-fp-armv8 -mfloat-abi=hard -fno-common

obj-$(CONFIG_SYSVIPC_COMPAT) += compat.o
obj-$(CONFIG_SYSVIPC) += util.o msgutil.o msg.o sem.o shm.o syscall.o
obj-$(CONFIG_SYSVIPC_SYSCTL) += ipc_sysctl.o
obj-$(CONFIG_POSIX_MQUEUE) += mqueue.o msgutil.o
obj-$(CONFIG_IPC_NS) += namespace.o
obj-$(CONFIG_POSIX_MQUEUE_SYSCTL) += mq_sysctl.o

