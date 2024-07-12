# SPDX-License-Identifier: GPL-2.0-only
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod+sve2+fp16+aes+sha2+lse+simd \
		-mcpu=cortex-a78 -mtune=cortex-a78  \
		-funroll-loops -ftree-vectorize \
		 -msve-vector-bits=128 -mfpu=neon-fp-armv8 \
		-mfloat-abi=hard -fno-common 
		
obj-$(CONFIG_ZSTD_COMPRESS) += zstd_compress.o
obj-$(CONFIG_ZSTD_DECOMPRESS) += zstd_decompress.o


obj-$(CONFIG_LZ4_COMPRESS) += lz4_compress.o

zstd_compress-y := fse_compress.o huf_compress.o compress.o \
		   entropy_common.o fse_decompress.o zstd_common.o
zstd_decompress-y := huf_decompress.o decompress.o \
		     entropy_common.o fse_decompress.o zstd_common.o
