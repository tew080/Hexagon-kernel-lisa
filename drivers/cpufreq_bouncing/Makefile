ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod+sve2+fp16+aes+sha2+lse+simd -mcpu=cortex-a78 -mtune=cortex-a78 -mcpu=cortex-a55 -mtune=cortex-a55 -funroll-loops -ftree-vectorize 
ccflags-y += -Ofast -msve-vector-bits=128 -mfpu=neon-fp-armv8 -mfloat-abi=hard -fno-common
obj-$(CONFIG_OPLUS_FEATURE_CPUFREQ_BOUNCING) += cpufreq_bouncing.o
