# SPDX-License-Identifier: GPL-2.0
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mtune=cortex-a78 -mtune=cortex-a55 -flto=thin -funroll-loops -ftree-vectorize

obj-$(CONFIG_VGASTATE)            += vgastate.o
obj-$(CONFIG_HDMI)                += hdmi.o

obj-$(CONFIG_VT)		  += console/
obj-$(CONFIG_FB_STI)		  += console/
obj-$(CONFIG_LOGO)		  += logo/
obj-y				  += backlight/

obj-y				  += fbdev/

obj-$(CONFIG_VIDEOMODE_HELPERS) += display_timing.o videomode.o
ifeq ($(CONFIG_OF),y)
obj-$(CONFIG_VIDEOMODE_HELPERS) += of_display_timing.o of_videomode.o
endif
