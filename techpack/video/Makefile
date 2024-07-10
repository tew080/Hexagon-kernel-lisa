# SPDX-License-Identifier: GPL-2.0-only

# auto-detect subdirs
ccflags-y += -mcpu=kryo -march=armv8.2-a+crypto+rcpc+dotprod+fp16+aes+sha2+lse+simd+sve2  \
		 -mcpu=cortex-a78 -mtune=cortex-a78  \
		 -funroll-loops -ftree-vectorize \
		 -mfpu=neon-fp-armv8 -mfloat-abi=hard \
		 -msve-vector-bits=128 -fno-common

ifneq ($(CONFIG_ARCH_QTI_VM), y)
ifeq ($(CONFIG_ARCH_LAHAINA), y)
include $(srctree)/techpack/video/config/konavid.conf
LINUXINCLUDE    += -include $(srctree)/techpack/video/config/konavidconf.h
endif

# auto-detect subdirs
ifeq ($(CONFIG_ARCH_HOLI), y)
include $(srctree)/techpack/video/config/holivid.conf
endif

ifeq ($(CONFIG_ARCH_HOLI), y)
LINUXINCLUDE    += -include $(srctree)/techpack/video/config/holividconf.h
endif

# auto-detect subdirs
ifeq ($(CONFIG_ARCH_LITO), y)
include $(srctree)/techpack/video/config/litovid.conf
endif

ifeq ($(CONFIG_ARCH_LITO), y)
LINUXINCLUDE    += -include $(srctree)/techpack/video/config/litovidconf.h
endif
endif

# auto-detect subdirs
ifeq ($(CONFIG_ARCH_SCUBA), y)
include $(srctree)/techpack/video/config/scubavid.conf
endif

ifeq ($(CONFIG_ARCH_SCUBA), y)
LINUXINCLUDE    += -include $(srctree)/techpack/video/config/scubavidconf.h
endif

LINUXINCLUDE    += -I$(srctree)/techpack/video/include \
                   -I$(srctree)/techpack/video/include/uapi

USERINCLUDE     += -I$(srctree)/techpack/video/include/uapi

obj-y +=msm/
