// SPDX-License-Identifier: GPL-2.0
#include <linux/cpufreq.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

__weak void arch_freq_prepare_all(void)
{
}

extern const struct seq_operations cpuinfo_op;
static int cpuinfo_open(struct inode *inode, struct file *file)
{
	arch_freq_prepare_all();
	return seq_open(file, &cpuinfo_op);
}

<<<<<<< HEAD
static const struct file_operations proc_cpuinfo_operations = {
	.open		= cpuinfo_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= seq_release,
=======
static const struct proc_ops cpuinfo_proc_ops = {
	.proc_flags	= PROC_ENTRY_PERMANENT,
	.proc_open	= cpuinfo_open,
	.proc_read	= seq_read,
	.proc_lseek	= seq_lseek,
	.proc_release	= seq_release,
>>>>>>> b0183a84bd3e (proc: faster open/read/close with "permanent" files)
};

static int __init proc_cpuinfo_init(void)
{
	proc_create("cpuinfo", 0, NULL, &proc_cpuinfo_operations);
	return 0;
}
fs_initcall(proc_cpuinfo_init);
