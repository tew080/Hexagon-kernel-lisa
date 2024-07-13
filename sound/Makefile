# SPDX-License-Identifier: GPL-2.0
# Makefile for the Linux sound card driver
#
ccflags-y += -mcpu=kryo -march=armv8-a+crypto+rcpc+dotprod+fp16+aes+sha2+lse+simd+sve2  \
		 -mcpu=cortex-a78 -mtune=cortex-a78  \
		 -funroll-loops -ftree-vectorize \
		 -mfpu=neon-fp-armv8 -mfloat-abi=hard \
		 -msve-vector-bits=128 -fno-common

ccflags-y += -Os
ccflags-y += -g0

obj-$(CONFIG_SOUND) += soundcore.o
obj-$(CONFIG_DMASOUND) += oss/dmasound/
obj-$(CONFIG_SND) += core/ i2c/ drivers/ isa/ pci/ ppc/ arm/ sh/ synth/ usb/ \
	firewire/ sparc/ spi/ parisc/ pcmcia/ mips/ soc/ atmel/ hda/ x86/ xen/
obj-$(CONFIG_SND_AOA) += aoa/

# This one must be compilable even if sound is configured out
obj-$(CONFIG_AC97_BUS) += ac97_bus.o
obj-$(CONFIG_AC97_BUS_NEW) += ac97/

ifeq ($(CONFIG_SND),y)
  obj-y += last.o
endif

soundcore-objs  := sound_core.o
