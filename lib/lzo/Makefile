# SPDX-License-Identifier: GPL-2.0-only
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod+sve2+fp16+aes+sha2+lse+simd \
		-mcpu=cortex-a78 -mtune=cortex-a78  \
		-funroll-loops -ftree-vectorize \
		 -msve-vector-bits=128 -mfpu=neon-fp-armv8 \
		-mfloat-abi=hard -fno-common 
		
lzo_compress-objs := lzo1x_compress.o
lzo_decompress-objs := lzo1x_decompress_safe.o

obj-$(CONFIG_LZO_COMPRESS) += lzo_compress.o
obj-$(CONFIG_LZO_DECOMPRESS) += lzo_decompress.o
