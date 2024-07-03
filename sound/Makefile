# SPDX-License-Identifier: GPL-2.0
# Makefile for the Linux sound card driver
#
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod+sve2+fp16+aes+sha2+lse+simd -mcpu=cortex-a78 -mtune=cortex-a78  -funroll-loops -ftree-vectorize 
ccflags-y += -msve-vector-bits=128 -mfpu=neon-fp-armv8 -mfloat-abi=hard -fno-common

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
