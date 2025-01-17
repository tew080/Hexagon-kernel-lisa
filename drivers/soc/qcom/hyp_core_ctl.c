// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2018-2020, The Linux Foundation. All rights reserved.
 */

#define pr_fmt(fmt)	"hyp_core_ctl: " fmt

#include <linux/init.h>
#include <linux/cpumask.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/sched/rt.h>
#include <linux/slab.h>
#include <linux/cpuhotplug.h>
#include <uapi/linux/sched/types.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/cpu_cooling.h>
#include <linux/mutex.h>
#include <linux/debugfs.h>
#include <linux/pm_qos.h>
#include <linux/cpufreq.h>
#include <linux/sched/sysctl.h>
#ifndef CONFIG_TRACEPOINTS
#include <linux/cpu.h>
#include <linux/interrupt.h>
#include <linux/irqreturn.h>
#endif

#include <linux/haven/hcall.h>
#include <linux/haven/hh_errno.h>
#include <linux/haven/hh_rm_drv.h>

#define MAX_RESERVE_CPUS (num_possible_cpus()/2)
#define SVM_STATE_RUNNING 1
#define SVM_STATE_CPUS_SUSPENDED 2
#define SVM_STATE_SYSTEM_SUSPENDED 3

static DEFINE_PER_CPU(struct freq_qos_request, qos_min_req);
static DEFINE_PER_CPU(unsigned int, qos_min_freq);

unsigned int sysctl_hh_suspend_timeout_ms = 1000;
/**
 * struct hyp_core_ctl_cpumap - vcpu to pcpu mapping for the other guest
 * @cap_id: System call id to be used while referring to this vcpu
 * @pcpu: The physical CPU number corresponding to this vcpu
 * @curr_pcpu: The current physical CPU number corresponding to this vcpu.
 *             The curr_pcu is set to another CPU when the original assigned
 *             CPU i.e pcpu can't be used due to thermal condition.
 *
 */
struct hyp_core_ctl_cpu_map {
	hh_capid_t cap_id;
	hh_label_t pcpu;
	hh_label_t curr_pcpu;
};

/**
 * struct hyp_core_ctl_data - The private data structure of this driver
 * @lock: spinlock to serialize task wakeup and enable/reserve_cpus
 * @task: task_struct pointer to the thread running the state machine
 * @pending: state machine work pending status
 * @reservation_enabled: status of the reservation
 * @reservation_mutex: synchronization between thermal handling and
 *                     reservation. The physical CPUs are re-assigned
 *                     during thermal conditions while reservation is
 *                     not enabled. So this synchronization is needed.
 * @reserve_cpus: The CPUs to be reserved. input.
 * @our_isolated_cpus: The CPUs isolated by hyp_core_ctl driver. output.
 * @final_reserved_cpus: The CPUs reserved for the Hypervisor. output.
 * @cpumap: The vcpu to pcpu mapping table
 */
struct hyp_core_ctl_data {
	spinlock_t lock;
	struct task_struct *task;
	bool pending;
	bool reservation_enabled;
	struct mutex reservation_mutex;
	cpumask_t reserve_cpus;
	cpumask_t our_isolated_cpus;
	cpumask_t final_reserved_cpus;
	struct hyp_core_ctl_cpu_map cpumap[NR_CPUS];
};

#define CREATE_TRACE_POINTS
#include "hyp_core_ctl_trace.h"

static struct hyp_core_ctl_data *the_hcd;
static struct hyp_core_ctl_cpu_map hh_cpumap[NR_CPUS];
static bool is_vcpu_info_populated;
static bool init_done;
static int nr_vcpus;
static bool freq_qos_init_done;
static u64 vpmg_cap_id;
static struct timer_list hh_suspend_timer;
static bool is_vpm_group_info_populated;

static inline void hyp_core_ctl_print_status(char *msg)
{
	trace_hyp_core_ctl_status(the_hcd, msg);

	pr_debug("%s: reserve=%*pbl reserved=%*pbl our_isolated=%*pbl online=%*pbl isolated=%*pbl thermal=%*pbl\n",
		msg, cpumask_pr_args(&the_hcd->reserve_cpus),
		cpumask_pr_args(&the_hcd->final_reserved_cpus),
		cpumask_pr_args(&the_hcd->our_isolated_cpus),
		cpumask_pr_args(cpu_online_mask),
		cpumask_pr_args(cpu_isolated_mask),
		cpumask_pr_args(cpu_cooling_get_max_level_cpumask()));
}

static void hyp_core_ctl_undo_reservation(struct hyp_core_ctl_data *hcd)
{
	int cpu, ret;
	struct freq_qos_request *qos_req;

	hyp_core_ctl_print_status("undo_reservation_start");

	for_each_cpu(cpu, &hcd->our_isolated_cpus) {
		ret = sched_unisolate_cpu(cpu);
		if (ret < 0) {
			pr_err("fail to un-isolate CPU%d. ret=%d\n", cpu, ret);
			continue;
		}

		cpumask_clear_cpu(cpu, &hcd->our_isolated_cpus);

		if (freq_qos_init_done) {
			qos_req = &per_cpu(qos_min_req, cpu);
			ret = freq_qos_update_request(qos_req,
					FREQ_QOS_MIN_DEFAULT_VALUE);
			if (ret < 0)
				pr_err("fail to update min freq for CPU%d ret=%d\n",
								cpu, ret);
		}
	}

	hyp_core_ctl_print_status("undo_reservation_end");
}

static void finalize_reservation(struct hyp_core_ctl_data *hcd, cpumask_t *temp)
{
	cpumask_t vcpu_adjust_mask;
	int i, orig_cpu, curr_cpu, replacement_cpu;
	int err;

	/*
	 * When thermal conditions are not present, we return
	 * from here.
	 */
	if (cpumask_equal(temp, &hcd->final_reserved_cpus))
		return;

	/*
	 * When we can't match with the original reserve CPUs request,
	 * don't change the existing scheme. We can't assign the
	 * same physical CPU to multiple virtual CPUs.
	 *
	 * This may only happen when thermal isolate more CPUs.
	 */
	if (cpumask_weight(temp) < cpumask_weight(&hcd->reserve_cpus)) {
		pr_debug("Fail to reserve some CPUs\n");
		return;
	}

	cpumask_copy(&hcd->final_reserved_cpus, temp);
	cpumask_clear(&vcpu_adjust_mask);

	/*
	 * In the first pass, we traverse all virtual CPUs and try
	 * to assign their original physical CPUs if they are
	 * reserved. if the original physical CPU is not reserved,
	 * then check the current physical CPU is reserved or not.
	 * so that we continue to use the current physical CPU.
	 *
	 * If both original CPU and the current CPU are not reserved,
	 * we have to find a replacement. These virtual CPUs are
	 * maintained in vcpu_adjust_mask and processed in the 2nd pass.
	 */
	for (i = 0; i < MAX_RESERVE_CPUS; i++) {
		if (hcd->cpumap[i].cap_id == 0)
			break;

		orig_cpu = hcd->cpumap[i].pcpu;
		curr_cpu = hcd->cpumap[i].curr_pcpu;

		if (cpumask_test_cpu(orig_cpu, &hcd->final_reserved_cpus)) {
			cpumask_clear_cpu(orig_cpu, temp);

			if (orig_cpu == curr_cpu)
				continue;

			/*
			 * The original pcpu corresponding to this vcpu i.e i
			 * is available in final_reserved_cpus. so restore
			 * the assignment.
			 */
			err = hh_hcall_vcpu_affinity_set(hcd->cpumap[i].cap_id,
								orig_cpu);
			if (err != HH_ERROR_OK) {
				pr_err("restore: fail to assign pcpu for vcpu#%d err=%d cap_id=%d cpu=%d\n",
					i, err, hcd->cpumap[i].cap_id, orig_cpu);
				continue;
			}

			hcd->cpumap[i].curr_pcpu = orig_cpu;
			pr_debug("err=%u vcpu=%d pcpu=%u curr_cpu=%u\n",
					err, i, hcd->cpumap[i].pcpu,
					hcd->cpumap[i].curr_pcpu);
			continue;
		}

		/*
		 * The original CPU is not available but the previously
		 * assigned CPU i.e curr_cpu is still available. so keep
		 * using it.
		 */
		if (cpumask_test_cpu(curr_cpu, &hcd->final_reserved_cpus)) {
			cpumask_clear_cpu(curr_cpu, temp);
			continue;
		}

		/*
		 * A replacement CPU is found in the 2nd pass below. Make
		 * a note of this virtual CPU for which both original and
		 * current physical CPUs are not available in the
		 * final_reserved_cpus.
		 */
		cpumask_set_cpu(i, &vcpu_adjust_mask);
	}

	/*
	 * The vcpu_adjust_mask contain the virtual CPUs that needs
	 * re-assignment. The temp CPU mask contains the remaining
	 * reserved CPUs. so we pick one by one from the remaining
	 * reserved CPUs and assign them to the pending virtual
	 * CPUs.
	 */
	for_each_cpu(i, &vcpu_adjust_mask) {
		replacement_cpu = cpumask_any(temp);
		cpumask_clear_cpu(replacement_cpu, temp);

		err = hh_hcall_vcpu_affinity_set(hcd->cpumap[i].cap_id,
							replacement_cpu);
		if (err != HH_ERROR_OK) {
			pr_err("adjust: fail to assign pcpu for vcpu#%d err=%d cap_id=%d cpu=%d\n",
				i, err, hcd->cpumap[i].cap_id, replacement_cpu);
			continue;
		}

		hcd->cpumap[i].curr_pcpu = replacement_cpu;
		pr_debug("adjust err=%u vcpu=%d pcpu=%u curr_cpu=%u\n",
				err, i, hcd->cpumap[i].pcpu,
				hcd->cpumap[i].curr_pcpu);

	}

	/* Did we reserve more CPUs than needed? */
	WARN_ON(!cpumask_empty(temp));
}

static void hyp_core_ctl_do_reservation(struct hyp_core_ctl_data *hcd)
{
	cpumask_t offline_cpus, iter_cpus, temp_reserved_cpus;
	int i, ret, iso_required, iso_done;
	const cpumask_t *thermal_cpus = cpu_cooling_get_max_level_cpumask();
	struct freq_qos_request *qos_req;
	unsigned int min_freq;

	cpumask_clear(&offline_cpus);
	cpumask_clear(&temp_reserved_cpus);

	hyp_core_ctl_print_status("reservation_start");

	/*
	 * Iterate all reserve CPUs and isolate them if not done already.
	 * The offline CPUs can't be isolated but they are considered
	 * reserved. When an offline and reserved CPU comes online, it
	 * will be isolated to honor the reservation.
	 */
	cpumask_andnot(&iter_cpus, &hcd->reserve_cpus, &hcd->our_isolated_cpus);
	cpumask_andnot(&iter_cpus, &iter_cpus, thermal_cpus);

	for_each_cpu(i, &iter_cpus) {
		if (!cpu_online(i)) {
			cpumask_set_cpu(i, &offline_cpus);
			continue;
		}

		ret = sched_isolate_cpu(i);
		if (ret < 0) {
			pr_debug("fail to isolate CPU%d. ret=%d\n", i, ret);
			continue;
		}

		cpumask_set_cpu(i, &hcd->our_isolated_cpus);

		min_freq = per_cpu(qos_min_freq, i);
		if (min_freq && freq_qos_init_done) {
			qos_req = &per_cpu(qos_min_req, i);
			ret = freq_qos_update_request(qos_req, min_freq);
			if (ret < 0)
				pr_err("fail to update min freq for CPU%d ret=%d\n",
								i, ret);
		}
	}

	cpumask_andnot(&iter_cpus, &hcd->reserve_cpus, &offline_cpus);
	iso_required = cpumask_weight(&iter_cpus);
	iso_done = cpumask_weight(&hcd->our_isolated_cpus);

	if (iso_done < iso_required) {
		int isolate_need;

		/*
		 * We have isolated fewer CPUs than required. This happens
		 * when some of the CPUs from the reserved_cpus mask
		 * are managed by thermal. Find the replacement CPUs and
		 * isolate them.
		 */
		isolate_need = iso_required - iso_done;

		/*
		 * Create a cpumask from which replacement CPUs can be
		 * picked. Exclude our isolated CPUs, thermal managed
		 * CPUs and offline CPUs, which are already considered
		 * as reserved.
		 */
		cpumask_andnot(&iter_cpus, cpu_possible_mask,
			       &hcd->our_isolated_cpus);
		cpumask_andnot(&iter_cpus, &iter_cpus, thermal_cpus);
		cpumask_andnot(&iter_cpus, &iter_cpus, &offline_cpus);

		/*
		 * Keep the replacement policy simple. The offline CPUs
		 * comes for free. so pick them first.
		 */
		for_each_cpu(i, &iter_cpus) {
			if (!cpu_online(i)) {
				cpumask_set_cpu(i, &offline_cpus);
				if (--isolate_need == 0)
					goto done;
			}
		}

		cpumask_andnot(&iter_cpus, &iter_cpus, &offline_cpus);

		for_each_cpu(i, &iter_cpus) {
			ret = sched_isolate_cpu(i);
			if (ret < 0) {
				pr_debug("fail to isolate CPU%d. ret=%d\n",
						i, ret);
				continue;
			}

			cpumask_set_cpu(i, &hcd->our_isolated_cpus);

			min_freq = per_cpu(qos_min_freq, i);
			if (min_freq && freq_qos_init_done) {
				qos_req = &per_cpu(qos_min_req, i);
				ret = freq_qos_update_request(qos_req,
								min_freq);
				if (ret < 0)
					pr_err("fail to update min freq for CPU%d ret=%d\n",
								i, ret);
			}

			if (--isolate_need == 0)
				break;
		}
	} else if (iso_done > iso_required) {
		int unisolate_need;

		/*
		 * We have isolated more CPUs than required. Un-isolate
		 * the additional CPUs which are not part of the
		 * reserve_cpus mask.
		 *
		 * This happens in the following scenario.
		 *
		 * - Lets say reserve CPUs are CPU4 and CPU5. They are
		 *   isolated.
		 * - CPU4 is isolated by thermal. We found CPU0 as the
		 *   replacement CPU. Now CPU0 and CPU5 are isolated by
		 *   us.
		 * - CPU4 is un-isolated by thermal. We first isolate CPU4
		 *   since it is part of our reserve CPUs. Now CPU0, CPU4
		 *   and CPU5 are isolated by us.
		 * - Since iso_done (3) > iso_required (2), un-isolate
		 *   a CPU which is not part of the reserve CPU. i.e CPU0.
		 */
		unisolate_need = iso_done - iso_required;
		cpumask_andnot(&iter_cpus, &hcd->our_isolated_cpus,
			       &hcd->reserve_cpus);
		for_each_cpu(i, &iter_cpus) {
			ret = sched_unisolate_cpu(i);
			if (ret < 0) {
				pr_err("fail to unisolate CPU%d. ret=%d\n",
				       i, ret);
				continue;
			}

			cpumask_clear_cpu(i, &hcd->our_isolated_cpus);

			if (freq_qos_init_done) {
				qos_req = &per_cpu(qos_min_req, i);
				ret = freq_qos_update_request(qos_req,
						FREQ_QOS_MIN_DEFAULT_VALUE);
				if (ret < 0)
					pr_err("fail to update min freq for CPU%d ret=%d\n",
								i, ret);
			}

			if (--unisolate_need == 0)
				break;
		}
	}

done:
	cpumask_or(&temp_reserved_cpus, &hcd->our_isolated_cpus, &offline_cpus);
	finalize_reservation(hcd, &temp_reserved_cpus);

	hyp_core_ctl_print_status("reservation_end");
}

static int hyp_core_ctl_thread(void *data)
{
	struct hyp_core_ctl_data *hcd = data;
	unsigned long flags;

	while (1) {
		spin_lock_irqsave(&hcd->lock, flags);
		if (!hcd->pending) {
			set_current_state(TASK_INTERRUPTIBLE);
			spin_unlock_irqrestore(&hcd->lock, flags);

			schedule();

			spin_lock_irqsave(&hcd->lock, flags);
			set_current_state(TASK_RUNNING);
		}
		hcd->pending = false;
		spin_unlock_irqrestore(&hcd->lock, flags);

		if (kthread_should_stop())
			break;

		/*
		 * The reservation mutex synchronize the reservation
		 * happens in this thread against the thermal handling.
		 * The CPU re-assignment happens directly from the
		 * thermal callback context when the reservation is
		 * not enabled, since there is no need for isolating.
		 */
		mutex_lock(&hcd->reservation_mutex);
		if (hcd->reservation_enabled)
			hyp_core_ctl_do_reservation(hcd);
		else
			hyp_core_ctl_undo_reservation(hcd);
		mutex_unlock(&hcd->reservation_mutex);
	}

	return 0;
}

static void hyp_core_ctl_handle_thermal(struct hyp_core_ctl_data *hcd,
					int cpu, bool throttled)
{
	cpumask_t temp_mask, iter_cpus;
	const cpumask_t *thermal_cpus = cpu_cooling_get_max_level_cpumask();
	bool notify = false;
	int replacement_cpu;

	hyp_core_ctl_print_status("handle_thermal_start");

	/*
	 * Take a copy of the final_reserved_cpus and adjust the mask
	 * based on the notified CPU's thermal state.
	 */
	cpumask_copy(&temp_mask, &hcd->final_reserved_cpus);

	if (throttled) {
		/*
		 * Find a replacement CPU for this throttled CPU. Select
		 * any CPU that is not managed by thermal and not already
		 * part of the assigned CPUs.
		 */
		cpumask_andnot(&iter_cpus, cpu_possible_mask, thermal_cpus);
		cpumask_andnot(&iter_cpus, &iter_cpus,
			       &hcd->final_reserved_cpus);
		replacement_cpu = cpumask_any(&iter_cpus);

		if (replacement_cpu < nr_cpu_ids) {
			cpumask_clear_cpu(cpu, &temp_mask);
			cpumask_set_cpu(replacement_cpu, &temp_mask);
			notify = true;
		}
	} else {
		/*
		 * One of the original assigned CPU is unthrottled by thermal.
		 * Swap this CPU with any one of the replacement CPUs.
		 */
		cpumask_andnot(&iter_cpus, &hcd->final_reserved_cpus,
			       &hcd->reserve_cpus);
		replacement_cpu = cpumask_any(&iter_cpus);

		if (replacement_cpu < nr_cpu_ids) {
			cpumask_clear_cpu(replacement_cpu, &temp_mask);
			cpumask_set_cpu(cpu, &temp_mask);
			notify = true;
		}
	}

	if (notify)
		finalize_reservation(hcd, &temp_mask);

	hyp_core_ctl_print_status("handle_thermal_end");
}

static int hyp_core_ctl_cpu_cooling_cb(struct notifier_block *nb,
				       unsigned long val, void *data)
{
	int cpu = (long) data;
	const cpumask_t *thermal_cpus = cpu_cooling_get_max_level_cpumask();
	struct freq_qos_request *qos_req;
	int ret;
	unsigned long flags;

	if (!the_hcd)
		return NOTIFY_DONE;

	mutex_lock(&the_hcd->reservation_mutex);

	pr_debug("CPU%d is %s by thermal\n", cpu,
		 val ? "throttled" : "unthrottled");

	if (val) {
		/*
		 * The thermal mitigated CPU is not part of our reserved
		 * CPUs. So nothing to do.
		 */
		if (!cpumask_test_cpu(cpu, &the_hcd->final_reserved_cpus))
			goto out;

		/*
		 * The thermal mitigated CPU is part of our reserved CPUs.
		 *
		 * If it is isolated by us, unisolate it. If it is not
		 * isolated, probably it is offline. In both cases, kick
		 * the state machine to find a replacement CPU.
		 */
		if (cpumask_test_cpu(cpu, &the_hcd->our_isolated_cpus)) {
			sched_unisolate_cpu(cpu);
			cpumask_clear_cpu(cpu, &the_hcd->our_isolated_cpus);
			if (freq_qos_init_done) {
				qos_req = &per_cpu(qos_min_req, cpu);
				ret = freq_qos_update_request(qos_req,
						FREQ_QOS_MIN_DEFAULT_VALUE);
				if (ret < 0)
					pr_err("fail to update min freq for CPU%d ret=%d\n",
								cpu, ret);
			}
		}
	} else {
		/*
		 * A CPU is unblocked by thermal. We are interested if
		 *
		 * (1) This CPU is part of the original reservation request
		 *     In this case, this CPU should be swapped with one of
		 *     the replacement CPU that is currently reserved.
		 * (2) When some of the thermal mitigated CPUs are currently
		 *     reserved due to unavailability of CPUs. Now that
		 *     thermal unblocked a CPU, swap this with one of the
		 *     thermal mitigated CPU that is currently reserved.
		 */
		if (!cpumask_test_cpu(cpu, &the_hcd->reserve_cpus) &&
		    !cpumask_intersects(&the_hcd->final_reserved_cpus,
		    thermal_cpus))
			goto out;
	}

	if (the_hcd->reservation_enabled) {
		spin_lock_irqsave(&the_hcd->lock, flags);
		the_hcd->pending = true;
		wake_up_process(the_hcd->task);
		spin_unlock_irqrestore(&the_hcd->lock, flags);
	} else {
		/*
		 * When the reservation is enabled, the state machine
		 * takes care of finding the new replacement CPU or
		 * isolating the unthrottled CPU. However when the
		 * reservation is not enabled, we still want to
		 * re-assign another CPU for a throttled CPU.
		 */
		hyp_core_ctl_handle_thermal(the_hcd, cpu, val);
	}
out:
	mutex_unlock(&the_hcd->reservation_mutex);
	return NOTIFY_OK;
}

static struct notifier_block hyp_core_ctl_nb = {
	.notifier_call = hyp_core_ctl_cpu_cooling_cb,
};

static int hyp_core_ctl_hp_offline(unsigned int cpu)
{
	struct freq_qos_request *qos_req;
	int ret;

	if (!the_hcd || !the_hcd->reservation_enabled)
		return 0;

	/*
	 * A CPU can't be left in isolated state while it is
	 * going offline. So unisolate the CPU if it is
	 * isolated by us. An offline CPU is considered
	 * as reserved. So no further action is needed.
	 */
	if (cpumask_test_and_clear_cpu(cpu, &the_hcd->our_isolated_cpus)) {
		sched_unisolate_cpu_unlocked(cpu);
		if (freq_qos_init_done) {
			qos_req = &per_cpu(qos_min_req, cpu);
			ret = freq_qos_update_request(qos_req,
					FREQ_QOS_MIN_DEFAULT_VALUE);
			if (ret < 0)
				pr_err("fail to update min freq for CPU%d ret=%d\n",
								cpu, ret);
		}
	}

	return 0;
}

static int hyp_core_ctl_hp_online(unsigned int cpu)
{
	unsigned long flags;

	if (!the_hcd || !the_hcd->reservation_enabled)
		return 0;

	/*
	 * A reserved CPU is coming online. It should be isolated
	 * to honor the reservation. So kick the state machine.
	 */
	spin_lock_irqsave(&the_hcd->lock, flags);
	if (cpumask_test_cpu(cpu, &the_hcd->final_reserved_cpus)) {
		the_hcd->pending = true;
		wake_up_process(the_hcd->task);
	}
	spin_unlock_irqrestore(&the_hcd->lock, flags);

	return 0;
}

static void hyp_core_ctl_init_reserve_cpus(struct hyp_core_ctl_data *hcd)
{
	int i;
	unsigned long flags;

	spin_lock_irqsave(&hcd->lock, flags);
	cpumask_clear(&hcd->reserve_cpus);

	for (i = 0; i < MAX_RESERVE_CPUS; i++) {
		if (hh_cpumap[i].cap_id == 0)
			break;

		hcd->cpumap[i].cap_id = hh_cpumap[i].cap_id;
		hcd->cpumap[i].pcpu = hh_cpumap[i].pcpu;
		hcd->cpumap[i].curr_pcpu = hh_cpumap[i].curr_pcpu;
		cpumask_set_cpu(hcd->cpumap[i].pcpu, &hcd->reserve_cpus);
		pr_debug("vcpu%u map to pcpu%u\n", i, hcd->cpumap[i].pcpu);
	}

	cpumask_copy(&hcd->final_reserved_cpus, &hcd->reserve_cpus);
	spin_unlock_irqrestore(&hcd->lock, flags);
	pr_info("reserve_cpus=%*pbl\n", cpumask_pr_args(&hcd->reserve_cpus));
}

/*
 * Called when vm_status is STATUS_READY, multiple times before status
 * moves to STATUS_RUNNING
 */
int hh_vcpu_populate_affinity_info(u32 cpu_idx, u64 cap_id)
{
	if (!init_done) {
		pr_err("Driver probe failed\n");
		return -ENXIO;
	}

	if (!is_vcpu_info_populated) {
		hh_cpumap[nr_vcpus].cap_id = cap_id;
		hh_cpumap[nr_vcpus].pcpu = cpu_idx;
		hh_cpumap[nr_vcpus].curr_pcpu = cpu_idx;

		nr_vcpus++;
		pr_debug("cpu_index:%u vcpu_cap_id:%llu nr_vcpus:%d\n",
					cpu_idx, cap_id, nr_vcpus);
	}

	return 0;
}

static int hh_vcpu_done_populate_affinity_info(struct notifier_block *nb,
						unsigned long cmd, void *data)
{
	struct hh_rm_notif_vm_status_payload *vm_status_payload = data;
	u8 vm_status = vm_status_payload->vm_status;

	if (cmd == HH_RM_NOTIF_VM_STATUS &&
			vm_status == HH_RM_VM_STATUS_RUNNING &&
			!is_vcpu_info_populated) {
		mutex_lock(&the_hcd->reservation_mutex);
		hyp_core_ctl_init_reserve_cpus(the_hcd);
		is_vcpu_info_populated = true;
		mutex_unlock(&the_hcd->reservation_mutex);
	}

	return NOTIFY_DONE;
}

static struct notifier_block hh_vcpu_nb = {
	.notifier_call = hh_vcpu_done_populate_affinity_info,
};

static void hh_suspend_timer_callback(struct timer_list *t)
{
	pr_err("Warning:%ums timeout occurred while waiting for SVM suspend\n",
							sysctl_hh_suspend_timeout_ms);
}

static inline void hh_del_suspend_timer(void)
{
	del_timer(&hh_suspend_timer);
}

static inline void hh_start_suspend_timer(void)
{
	mod_timer(&hh_suspend_timer, jiffies +
			msecs_to_jiffies(sysctl_hh_suspend_timeout_ms));
}

static irqreturn_t hh_susp_res_irq_handler(int irq, void *data)
{
	int err;
	uint64_t vpmg_state;
	unsigned long flags;

	err = hh_hcall_vpm_group_get_state(vpmg_cap_id, &vpmg_state);

	if (err != HH_ERROR_OK) {
		pr_err("Failed to get VPM Group state for cap_id=%llu err=%d\n",
			vpmg_cap_id, err);
		return IRQ_HANDLED;
	}

	spin_lock_irqsave(&the_hcd->lock, flags);
	if (vpmg_state == SVM_STATE_RUNNING) {
		if (!the_hcd->reservation_enabled)
			pr_err_ratelimited("Reservation not enabled,unexpected SVM wake up\n");
	} else if (vpmg_state == SVM_STATE_SYSTEM_SUSPENDED) {
		hh_del_suspend_timer();
	} else {
		pr_err("VPM Group state invalid/non-existent\n");
	}
	spin_unlock_irqrestore(&the_hcd->lock, flags);
	return IRQ_HANDLED;
}

int hh_vpm_grp_populate_info(u64 cap_id, int virq_num)
{
	int ret = 0;

	if (!init_done) {
		pr_err("%s: Driver probe failed\n", __func__);
		return -ENXIO;
	}

	if (virq_num < 0) {
		pr_err("%s: Invalid IRQ number\n", __func__);
		return -EINVAL;
	}

	vpmg_cap_id = cap_id;
	ret = request_irq(virq_num, hh_susp_res_irq_handler, 0,
			"hh_susp_res_irq", NULL);
	if (ret < 0) {
		pr_err("%s: IRQ registration failed ret=%d\n", __func__, ret);
		return ret;
	}

	timer_setup(&hh_suspend_timer, hh_suspend_timer_callback, 0);
	is_vpm_group_info_populated = true;

	return ret;
}

static void hyp_core_ctl_enable(bool enable)
{
	unsigned long flags;

	mutex_lock(&the_hcd->reservation_mutex);
	if (!is_vcpu_info_populated) {
		pr_err("VCPU info isn't populated\n");
		goto err_out;
	}

	spin_lock_irqsave(&the_hcd->lock, flags);
	if (enable == the_hcd->reservation_enabled)
		goto out;

	if (is_vpm_group_info_populated) {
		if (enable)
			hh_del_suspend_timer();
		else
			hh_start_suspend_timer();
	}

	trace_hyp_core_ctl_enable(enable);
	pr_debug("reservation %s\n", enable ? "enabled" : "disabled");

	the_hcd->reservation_enabled = enable;
	the_hcd->pending = true;
	wake_up_process(the_hcd->task);
out:
	spin_unlock_irqrestore(&the_hcd->lock, flags);
err_out:
	mutex_unlock(&the_hcd->reservation_mutex);
}

static ssize_t enable_store(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
{
	bool enable;
	int ret;

	ret = kstrtobool(buf, &enable);
	if (ret < 0)
		return -EINVAL;

	hyp_core_ctl_enable(enable);

	return count;
}

static ssize_t enable_show(struct device *dev, struct device_attribute *attr,
			   char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%u\n", the_hcd->reservation_enabled);
}

static DEVICE_ATTR_RW(enable);

static ssize_t status_show(struct device *dev, struct device_attribute *attr,
			   char *buf)
{
	struct hyp_core_ctl_data *hcd = the_hcd;
	ssize_t count;
	int i;

	mutex_lock(&hcd->reservation_mutex);

	count = scnprintf(buf, PAGE_SIZE, "enabled=%d\n",
			  hcd->reservation_enabled);

	count += scnprintf(buf + count, PAGE_SIZE - count,
			   "reserve_cpus=%*pbl\n",
			   cpumask_pr_args(&hcd->reserve_cpus));

	count += scnprintf(buf + count, PAGE_SIZE - count,
			   "reserved_cpus=%*pbl\n",
			   cpumask_pr_args(&hcd->final_reserved_cpus));

	count += scnprintf(buf + count, PAGE_SIZE - count,
			   "our_isolated_cpus=%*pbl\n",
			   cpumask_pr_args(&hcd->our_isolated_cpus));

	count += scnprintf(buf + count, PAGE_SIZE - count,
			   "online_cpus=%*pbl\n",
			   cpumask_pr_args(cpu_online_mask));

	count += scnprintf(buf + count, PAGE_SIZE - count,
			   "isolated_cpus=%*pbl\n",
			   cpumask_pr_args(cpu_isolated_mask));

	count += scnprintf(buf + count, PAGE_SIZE - count,
		   "thermal_cpus=%*pbl\n",
		   cpumask_pr_args(cpu_cooling_get_max_level_cpumask()));

	count += scnprintf(buf + count, PAGE_SIZE - count,
			   "Vcpu to Pcpu mappings:\n");

	for (i = 0; i < MAX_RESERVE_CPUS; i++) {
		if (hcd->cpumap[i].cap_id == 0)
			break;

		count += scnprintf(buf + count, PAGE_SIZE - count,
			 "vcpu=%d pcpu=%u curr_pcpu=%u\n",
			 i, hcd->cpumap[i].pcpu, hcd->cpumap[i].curr_pcpu);

	}

	mutex_unlock(&hcd->reservation_mutex);

	return count;
}

static DEVICE_ATTR_RO(status);

static int init_freq_qos_req(void)
{
	int cpu, ret;
	struct cpufreq_policy *policy;
	struct freq_qos_request *qos_req;

	for_each_possible_cpu(cpu) {
		policy = cpufreq_cpu_get(cpu);
		if (!policy) {
			pr_err("cpufreq policy not found for cpu%d\n", cpu);
			ret = -ESRCH;
			goto remove_qos_req;
		}

		qos_req = &per_cpu(qos_min_req, cpu);
		ret = freq_qos_add_request(&policy->constraints, qos_req,
				FREQ_QOS_MIN, FREQ_QOS_MIN_DEFAULT_VALUE);
		if (ret < 0) {
			pr_err("Failed to add min freq constraint (%d)\n", ret);
			cpufreq_cpu_put(policy);
			goto remove_qos_req;
		}
		cpufreq_cpu_put(policy);
	}

	return 0;

remove_qos_req:
	for_each_possible_cpu(cpu) {
		qos_req = &per_cpu(qos_min_req, cpu);
		if (freq_qos_request_active(qos_req))
			freq_qos_remove_request(qos_req);
	}

	return ret;
}

static ssize_t hcc_min_freq_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t count)
{
	int i, ret, ntokens = 0;
	unsigned int val, cpu;
	const char *cp = buf;

	mutex_lock(&the_hcd->reservation_mutex);
	if (!is_vcpu_info_populated) {
		pr_err("VCPU info isn't populated\n");
		goto err_out;
	}

	if (!freq_qos_init_done) {
		if (init_freq_qos_req())
			goto err_out;
		freq_qos_init_done = true;
	}

	while ((cp = strpbrk(cp + 1, " :")))
		ntokens++;

	/* CPU:value pair */
	if (!(ntokens % 2))
		goto err_out;

	cp = buf;
	for (i = 0; i < ntokens; i += 2) {
		if (sscanf(cp, "%u:%u", &cpu, &val) != 2)
			goto err_out;
		if (cpu >= num_possible_cpus())
			goto err_out;

		per_cpu(qos_min_freq, cpu) = val;
		cp = strnchr(cp, strlen(cp), ' ');
		cp++;
	}

	mutex_unlock(&the_hcd->reservation_mutex);
	return count;

err_out:
	ret = -EINVAL;
	mutex_unlock(&the_hcd->reservation_mutex);
	return ret;
}

static ssize_t hcc_min_freq_show(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	int cnt = 0, cpu;

	for_each_possible_cpu(cpu) {
		cnt += scnprintf(buf + cnt, PAGE_SIZE - cnt,
				"%d:%u ", cpu,
				per_cpu(qos_min_freq, cpu));
	}
	cnt += scnprintf(buf + cnt, PAGE_SIZE - cnt, "\n");
	return cnt;
}

static DEVICE_ATTR_RW(hcc_min_freq);

static struct attribute *hyp_core_ctl_attrs[] = {
	&dev_attr_enable.attr,
	&dev_attr_status.attr,
	&dev_attr_hcc_min_freq.attr,
	NULL
};

static struct attribute_group hyp_core_ctl_attr_group = {
	.attrs = hyp_core_ctl_attrs,
	.name = "hyp_core_ctl",
};

#define CPULIST_SZ 32
static ssize_t read_reserve_cpus(struct file *file, char __user *ubuf,
				 size_t count, loff_t *ppos)
{
	char kbuf[CPULIST_SZ];
	int ret;

	ret = scnprintf(kbuf, CPULIST_SZ, "%*pbl\n",
			cpumask_pr_args(&the_hcd->reserve_cpus));

	return simple_read_from_buffer(ubuf, count, ppos, kbuf, ret);
}

static ssize_t write_reserve_cpus(struct file *file, const char __user *ubuf,
				  size_t count, loff_t *ppos)
{
	char kbuf[CPULIST_SZ];
	int ret;
	cpumask_t temp_mask;
	unsigned long flags;

	mutex_lock(&the_hcd->reservation_mutex);
	if (!is_vcpu_info_populated) {
		pr_err("VCPU info isn't populated\n");
		ret = -EPERM;
		goto err_out;
	}

	ret = simple_write_to_buffer(kbuf, CPULIST_SZ - 1, ppos, ubuf, count);
	if (ret < 0)
		goto err_out;

	kbuf[ret] = '\0';
	ret = cpulist_parse(kbuf, &temp_mask);
	if (ret < 0)
		goto err_out;

	if (cpumask_weight(&temp_mask) !=
			cpumask_weight(&the_hcd->reserve_cpus)) {
		pr_err("incorrect reserve CPU count. expected=%u\n",
				cpumask_weight(&the_hcd->reserve_cpus));
		ret = -EINVAL;
		goto err_out;
	}

	spin_lock_irqsave(&the_hcd->lock, flags);
	if (the_hcd->reservation_enabled) {
		count = -EPERM;
		pr_err("reservation is enabled, can't change reserve_cpus\n");
	} else {
		cpumask_copy(&the_hcd->reserve_cpus, &temp_mask);
	}
	spin_unlock_irqrestore(&the_hcd->lock, flags);
	mutex_unlock(&the_hcd->reservation_mutex);

	return count;
err_out:
	mutex_unlock(&the_hcd->reservation_mutex);
	return ret;
}

static const struct file_operations debugfs_reserve_cpus_ops = {
	.read = read_reserve_cpus,
	.write = write_reserve_cpus,
};

static void hyp_core_ctl_debugfs_init(void)
{
	struct dentry *dir, *file;

	dir = debugfs_create_dir("hyp_core_ctl", NULL);
	if (IS_ERR_OR_NULL(dir))
		return;

	file = debugfs_create_file("reserve_cpus", 0644, dir, NULL,
				   &debugfs_reserve_cpus_ops);
	if (!file)
		debugfs_remove(dir);
}

static int hyp_core_ctl_probe(struct platform_device *pdev)
{
	int ret;
	struct hyp_core_ctl_data *hcd;
	struct sched_param param = { .sched_priority = MAX_RT_PRIO - 1 };

	ret = hh_rm_register_notifier(&hh_vcpu_nb);
	if (ret)
		return ret;

	hcd = kzalloc(sizeof(*hcd), GFP_KERNEL);
	if (!hcd) {
		ret = -ENOMEM;
		goto unregister_rm_notifier;
	}

	spin_lock_init(&hcd->lock);
	mutex_init(&hcd->reservation_mutex);
	hcd->task = kthread_run(hyp_core_ctl_thread, (void *) hcd,
				"hyp_core_ctl");

	if (IS_ERR(hcd->task)) {
		ret = PTR_ERR(hcd->task);
		goto free_hcd;
	}

	sched_setscheduler_nocheck(hcd->task, SCHED_FIFO, &param);

	ret = sysfs_create_group(&cpu_subsys.dev_root->kobj,
				 &hyp_core_ctl_attr_group);
	if (ret < 0) {
		pr_err("Fail to create sysfs files. ret=%d\n", ret);
		goto stop_task;
	}

	cpuhp_setup_state_nocalls(CPUHP_AP_ONLINE_DYN,
				  "qcom/hyp_core_ctl:online",
				  hyp_core_ctl_hp_online, NULL);

	cpuhp_setup_state_nocalls(CPUHP_HYP_CORE_CTL_ISOLATION_DEAD,
				  "qcom/hyp_core_ctl:dead",
				  NULL, hyp_core_ctl_hp_offline);

	cpu_cooling_max_level_notifier_register(&hyp_core_ctl_nb);
	hyp_core_ctl_debugfs_init();

	the_hcd = hcd;
	init_done = true;
	return 0;

stop_task:
	kthread_stop(hcd->task);
free_hcd:
	kfree(hcd);
unregister_rm_notifier:
	hh_rm_unregister_notifier(&hh_vcpu_nb);

	return ret;
}

static const struct of_device_id hyp_core_ctl_match_table[] = {
	{ .compatible = "qcom,hyp-core-ctl" },
	{},
};

static struct platform_driver hyp_core_ctl_driver = {
	.probe = hyp_core_ctl_probe,
	.driver = {
		.name = "hyp_core_ctl",
		.owner = THIS_MODULE,
		.of_match_table = hyp_core_ctl_match_table,
	 },
};

builtin_platform_driver(hyp_core_ctl_driver);
MODULE_DESCRIPTION("Core Control for Hypervisor");
MODULE_LICENSE("GPL v2");
