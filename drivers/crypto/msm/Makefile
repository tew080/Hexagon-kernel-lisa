# SPDX-License-Identifier: GPL-2.0-only
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

obj-$(CONFIG_CRYPTO_DEV_QCOM_MSM_QCE) += qce50.o

obj-$(CONFIG_CRYPTO_DEV_QCEDEV) += qcedev-mod.o
qcedev-mod-y = qcedev.o qcedev_smmu.o
qcedev-mod-$(CONFIG_COMPAT) += compat_qcedev.o

obj-$(CONFIG_CRYPTO_DEV_QCRYPTO) += qcrypto-msm.o
qcrypto-msm-y = qcrypto.o
