# SPDX-License-Identifier: GPL-2.0-only
# drm/tegra depends on host1x, so if both drivers are built-in care must be
# taken to initialize them in the correct order. Link order is the only way
# to ensure this currently.
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod+sve2+fp16+aes+sha2+lse+simd -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 
ccflags-y += -Ofast -msve-vector-bits=128 -mfpu=neon-fp-armv8 -mfloat-abi=hard -fno-common
obj-$(CONFIG_TEGRA_HOST1X)	+= host1x/
obj-y			+= drm/ vga/
obj-$(CONFIG_IMX_IPUV3_CORE)	+= ipu-v3/
obj-$(CONFIG_QCOM_KGSL) += msm/
obj-$(CONFIG_TRACE_GPU_MEM)		+= trace/
