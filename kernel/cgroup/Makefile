# SPDX-License-Identifier: GPL-2.0
ccflags-y += -march=armv8.2-a+crypto+rcpc+dotprod -mtune=cortex-a78 -mtune=cortex-a55 -flto=thin -funroll-loops -ftree-vectorize -O3 

obj-y := cgroup.o rstat.o namespace.o cgroup-v1.o freezer.o

obj-$(CONFIG_CGROUP_FREEZER) += legacy_freezer.o
obj-$(CONFIG_CGROUP_PIDS) += pids.o
obj-$(CONFIG_CGROUP_RDMA) += rdma.o
obj-$(CONFIG_CPUSETS) += cpuset.o
obj-$(CONFIG_CGROUP_DEBUG) += debug.o
