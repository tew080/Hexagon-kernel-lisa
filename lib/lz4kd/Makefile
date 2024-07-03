ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod+sve2+fp16+aes+sha2+lse+simd -mcpu=cortex-a78 -mtune=cortex-a78  -funroll-loops -ftree-vectorize 
ccflags-y += -msve-vector-bits=128 -mfpu=neon-fp-armv8 -mfloat-abi=hard -fno-common
obj-$(CONFIG_LZ4KD_COMPRESS) += lz4kd_encode.o
obj-$(CONFIG_LZ4KD_DECOMPRESS) += lz4kd_decode.o
