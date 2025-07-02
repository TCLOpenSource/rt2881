/*=============================================================
 * Copyright (c)      Realtek Semiconductor Corporation, 2016
 *
 * All rights reserved.
 *
 *============================================================*/

/*======================= Description ============================
 *
 * file: 		hdmi_main.c
 *
 * author: 
 * date:	2023/12/20
 * version: 	1.0
 *
 *============================================================*/

/*========================Header Files============================*/
#include <linux/kthread.h> //kthread_create()
#ifndef BUILD_QUICK_SHOW
#include <mach/platform.h>
#endif

#include "hdmi_common.h"
#include "hdmi_dsc.h"
#include "hdmi_phy.h"
#include "hdmi_vfe_config.h"
#include "hdmi_phy_dfe.h"
#include "hdmi_hdcp.h"
#include "hdmi_scdc.h"
#include "hdmi_power_saving.h"

extern wait_queue_head_t hdmi_thd_wait_qu;
/**********************************************************************************************
*
*	Marco or Definitions
*
**********************************************************************************************/


/**********************************************************************************************
*
*	Const Declarations
*
**********************************************************************************************/



/**********************************************************************************************
*
*	Variables
*
**********************************************************************************************/
unsigned int phy_isr_en[HDMI_PORT_TOTAL_NUM];


/**********************************************************************************************
*
*	Funtion Declarations
*
**********************************************************************************************/
void newbase_rxphy_isr_set(unsigned char port, unsigned char en)
{
	phy_isr_en[port] = en;
}


int newbase_rxphy_isr(void)
{
	unsigned char i;

	for (i=0; i<HDMI_PORT_TOTAL_NUM; i++)
	{
		if (!phy_isr_en[i])
			continue;

		if((GET_FLOW_CFG(HDMI_FLOW_CFG_HPD, HDMI_FLOW_CFG1_DISABLE_PORT_FAST_SWITCH) == HDMI_PCB_FORCE_ENABLE) && (i != newbase_hdmi_get_current_display_port()))
			continue;

		newbase_hdmi_rxphy_handler(i);

		newbase_hdcp_handler(i);

		newbase_hdmi_video_monitor(i);

		newbase_hdmi_error_handler(i);

		newbase_hdmi_check_connection_state(i);

		newbase_hdmi_check_sram_edid_state(i);

		newbase_hdmi_scdc_handler(i);  // handle SCDC event

		newbase_hdmi_hd21_dsc_handler(i);

		newbase_hdmi_power_saving_handler(i);  // handle Power Saving
	}

	return 1;

}

#ifndef BUILD_QUICK_SHOW
//------------------------------------------------------------------------------
// HDMI PHY ISR 
//------------------------------------------------------------------------------

static bool hdmiPhyEnable = false;
static struct task_struct *hdmi_task;


static int _hdmi_rxphy_thread(void* arg)
{
	unsigned int polling_cycle_us;
	while (!kthread_should_stop() && hdmiPhyEnable == true) 
	{
		polling_cycle_us = GET_FLOW_CFG(HDMI_FLOW_CFG_HPD, HDMI_FLOW_CFG1_DEF_PHY_THREAD_CYCLE_MS)*1000;
		if((polling_cycle_us> 0) && (polling_cycle_us <64000))
		{
			wait_event_interruptible_hrtimeout(hdmi_thd_wait_qu,(!hdmiPhyEnable),ktime_set(0,polling_cycle_us*NSEC_PER_USEC));
		}
		else
		{
			msleep(10);
		}
		newbase_rxphy_isr();
		
	}

	HDMI_WARN("[_hdmi_rxphy_thread] hdmi_task thread terminated, hdmiPhyEnable=%d\n", hdmiPhyEnable);
	#ifdef HDMI_LINUX_PLATFORM
	/* Wait until we are told to stop */
	for (;;) {
	    set_current_state(TASK_INTERRUPTIBLE);
	    if (kthread_should_stop())
	        break;
	    schedule();
	}
	__set_current_state(TASK_RUNNING);
	#else
 	hdmi_task = NULL;
	#endif

	return 0;
}

void newbase_rxphy_isr_enable(char enable)
{
	int err = 0;

	if (enable)
	{
		if (hdmiPhyEnable != true)
		{
			hdmi_task = kthread_create(_hdmi_rxphy_thread, NULL, "hdmi_task");

			if (IS_ERR(hdmi_task))
			{
				err = PTR_ERR(hdmi_task);
				hdmi_task = NULL;
				hdmiPhyEnable = false;
				HDMI_EMG("%s , Unable to start kernel thread (err_id = %d),HDMI_phy\n", __func__,err);
				return ;
			}

			hdmiPhyEnable = true;
			wake_up_process(hdmi_task);
			HDMI_PRINTF("%s hdmi phy thread started\n" , __func__ );
		}
	}
	else
	{
		if (hdmiPhyEnable == true)
		{
			hdmiPhyEnable = false;
			wake_up(&hdmi_thd_wait_qu);
			if (hdmi_task)
			{
				if (kthread_stop(hdmi_task)==0)
				{
					HDMI_PRINTF("hdmi rx_phy thread stopped\n");
					hdmi_task = NULL;
				}
				else
					HDMI_EMG("hdmi rx_phy thread stopped error\n");
			}
			else
				HDMI_EMG("hdmi rx_phy thread stopped already\n");
		}
	}
}
#endif

