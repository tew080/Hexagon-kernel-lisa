# SPDX-License-Identifier: GPL-2.0-only
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 

hdlcd-y := hdlcd_drv.o hdlcd_crtc.o
obj-$(CONFIG_DRM_HDLCD)	+= hdlcd.o
mali-dp-y := malidp_drv.o malidp_hw.o malidp_planes.o malidp_crtc.o
mali-dp-y += malidp_mw.o
obj-$(CONFIG_DRM_MALI_DISPLAY)	+= mali-dp.o
obj-$(CONFIG_DRM_KOMEDA) += display/
