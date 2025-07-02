#ifndef _FRACTION_H_
#define _FRACTION_H_

// if need support more, please change this config
#define REMOVE_MULTIPLE_MAX 7

struct remove_multiple_info
{
	unsigned int panel_refresh;
	unsigned int dtg_master_v_freq;	// depend on modestate_decide_dtg_m_mode() or modestate_decide_dtg_m_modeByDisp()
	unsigned int remove;
	unsigned int multiple;
};

struct fraction_info
{
	unsigned int inputFrameRateStart;
	unsigned int inputFrameRateEnd;
	struct remove_multiple_info rm_multiple[REMOVE_MULTIPLE_MAX];
};

void get_scaler_remove_multiple(unsigned int inputFrameRate, unsigned int dtg_master_v_freq, unsigned int panel_refresh, unsigned int *remove, unsigned int *multiple);

#endif
