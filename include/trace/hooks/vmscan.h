/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM vmscan

#define TRACE_INCLUDE_PATH trace/hooks

#if !defined(_TRACE_HOOK_VMSCAN_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_HOOK_VMSCAN_H

#include <trace/hooks/vendor_hooks.h>

DECLARE_RESTRICTED_HOOK(android_rvh_set_balance_anon_file_reclaim,
			TP_PROTO(bool *balance_anon_file_reclaim),
			TP_ARGS(balance_anon_file_reclaim), 1);
DECLARE_HOOK(android_vh_kswapd_per_node,
	TP_PROTO(int nid, bool *skip, bool run),
	TP_ARGS(nid, skip, run));
DECLARE_HOOK(android_vh_page_referenced_check_bypass,
	TP_PROTO(struct page *page, unsigned long nr_to_scan, int lru, bool *bypass),
	TP_ARGS(page, nr_to_scan, lru, bypass));

DECLARE_HOOK(android_vh_shrink_node_memcgs,
	TP_PROTO(struct mem_cgroup *memcg, bool *skip),
	TP_ARGS(memcg, skip));
DECLARE_HOOK(android_vh_shrink_slab_bypass,
	TP_PROTO(gfp_t gfp_mask, int nid, struct mem_cgroup *memcg, int priority, bool *bypass),
	TP_ARGS(gfp_mask, nid, memcg, priority, bypass));
DECLARE_HOOK(android_vh_do_shrink_slab,
	TP_PROTO(struct shrinker *shrinker, struct shrink_control *shrinkctl, int priority),
	TP_ARGS(shrinker, shrinkctl, priority));
#ifdef CONFIG_RTK_KDRV_KILLER
DECLARE_HOOK(android_vh_wakeup_kswapd,
	TP_PROTO(void *unused),
	TP_ARGS(unused));
DECLARE_HOOK(android_vh_kswapd_sleep,
	TP_PROTO(void *unused),
	TP_ARGS(unused));
#endif /* CONFIG_RTK_KDRV_KILLER */
#endif /* _TRACE_HOOK_VMSCAN_H */
/* This part must be outside protection */
#include <trace/define_trace.h>
