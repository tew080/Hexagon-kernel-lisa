# SPDX-License-Identifier: GPL-2.0-only
# drm/tegra depends on host1x, so if both drivers are built-in care must be
# taken to initialize them in the correct order. Link order is the only way
# to ensure this currently.

# Inlin optimization
KBUILD_CFLAGS += -mllvm -inline-threshold=5000
KBUILD_CFLAGS += -mllvm -inlinehint-threshold=4000
KBUILD_CFLAGS += -mllvm -unroll-count=10
KBUILD_CFLAGS += -mllvm -unroll-threshold=3000
KBUILD_CFLAGS += -mllvm -unroll-partial-threshold=3000

obj-$(CONFIG_TEGRA_HOST1X)	+= host1x/
obj-y			+= drm/ vga/
obj-$(CONFIG_IMX_IPUV3_CORE)	+= ipu-v3/
obj-$(CONFIG_QCOM_KGSL) += msm/
obj-$(CONFIG_TRACE_GPU_MEM)		+= trace/
