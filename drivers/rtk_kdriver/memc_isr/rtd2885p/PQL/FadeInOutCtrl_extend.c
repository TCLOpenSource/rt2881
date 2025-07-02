#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
#include "memc_isr/PQL/PQLGlobalDefs.h"
#include "memc_reg_def.h"

VOID FadeInOutCtrl_Init_RTK2885pp(_OUTPUT_FADEINOUT_CTRL *pOutput)
{
	pOutput->u1_detect = 0;
}

VOID FadeInOutCtrl_Proc_RTK2885pp(const _PARAM_FADEINOUT_CTRL *pParam, _OUTPUT_FADEINOUT_CTRL *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	int rgn_apli[32]={0};
	int rgn_aplp[32]={0};
	//int rgn_mot[32]={0};
	int rgn_dtl[32]={0};
	int rgn_apld[32]={0};
	int gmv0_x=0,gmv0_y=0,gmv0_cnt=0, gmv1_x=0,gmv1_y=0,gmv1_cnt=0;
	int gmv0_max,gmv1_max,mot;
	int cnt_32_big=0,cnt_32_little=0,rgn_avg_cnt=0,sum_apli=0,sum_aplp=0,rgn_apld_max=0,rgn_apld_min=0,variance_avg=0,variance=0,dark_difflittle=0,cnt_32_most=0,rgn_max_min_diff=0;
	int rgnIdx = 0;
	bool bg_cond,fg_cond,gmv_cond,rgn_max_min_flag,region_flag,rgn_avg_flag,var_flag,mot_flag,dark_flag,apld_flag,fifo_flag;
	unsigned int print_in_func;
	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (pParam->u1_fadeinout_calc_en == 0)
		return;

	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}

	for(rgnIdx=0;rgnIdx<32;rgnIdx++)
	{
		rgn_apli[rgnIdx] = s_pContext->_output_read_comreg.u20_me_rAPLi_rb[rgnIdx];
		rgn_aplp[rgnIdx] = s_pContext->_output_read_comreg.u20_me_rAPLp_rb[rgnIdx];
		rgn_dtl[rgnIdx] =  s_pContext->_output_read_comreg.u20_me_rgn_dtl_rb[rgnIdx];
		//rgn_mot[rgnIdx] = 0;
		rgn_apld[rgnIdx] = _ABS_DIFF_(rgn_apli[rgnIdx],rgn_aplp[rgnIdx]);
	}

	mot = s_pContext->_output_read_comreg.u30_me_glb_mot_rb;
	gmv0_x = s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb;
	gmv0_y = s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb;
	gmv1_x = s_pContext->_output_read_comreg.s11_me_GMV_2nd_vx_rb;
	gmv1_y = s_pContext->_output_read_comreg.s10_me_GMV_2nd_vy_rb;
	gmv0_cnt = s_pContext->_output_read_comreg.u17_me_GMV_1st_cnt_rb;
	gmv1_cnt = s_pContext->_output_read_comreg.u17_me_GMV_2nd_cnt_rb;
	gmv0_max = _MAX_(gmv0_x,gmv0_y);
	bg_cond = (gmv0_max>65 && gmv0_cnt>11000);
	gmv1_max = _MAX_(gmv1_x,gmv1_y);
	fg_cond = (gmv1_max>90 && gmv1_cnt>11000);
	//fg_cond = (gmv1_max>200 && gmv1_cnt>200);
	gmv_cond = bg_cond || fg_cond ;
	variance_avg = _MAX_(sum_apli,sum_aplp)/32;
	rgn_apld_max = rgn_apld[0];
	rgn_apld_min = rgn_apld[0];

	for(rgnIdx=0;rgnIdx<32;rgnIdx++)
	{
		if(rgn_apli[rgnIdx]<=rgn_aplp[rgnIdx])
			cnt_32_big++;
		if(rgn_apli[rgnIdx]>=rgn_aplp[rgnIdx])
			cnt_32_little++;
		if(rgn_apld[rgnIdx]>variance_avg)
			rgn_avg_cnt++;
		if(rgn_apld[rgnIdx]>rgn_apld_max)
			rgn_apld_max = rgn_apld[rgnIdx];
		if(rgn_apld[rgnIdx]<rgn_apld_max)
			rgn_apld_min = rgn_apld[rgnIdx];
		variance = variance + (rgn_apld[rgnIdx] - variance_avg)*(rgn_apld[rgnIdx] - variance_avg);
		if(rgn_apli[rgnIdx]<7000 && rgn_apli[rgnIdx]<7000 && rgn_apld[rgnIdx]<500)
			dark_difflittle++;

	}

	cnt_32_most = _MAX_(cnt_32_big, cnt_32_little);
	rgn_max_min_diff = rgn_apld_max - rgn_apld_min;
	rgn_max_min_flag = (rgn_max_min_diff>41000 &&  (cnt_32_most<29))|| (rgn_max_min_diff>50000);
	region_flag = (cnt_32_most>30) && _ABS_DIFF_(sum_apli,sum_aplp)>45000 && sum_apli<4400000 && sum_aplp<4400000 ;
	if(cnt_32_most==31)
		region_flag = region_flag && (1 - gmv_cond);
	rgn_avg_flag = (rgn_avg_cnt<10) && (cnt_32_most<20);
	variance = variance/320000;
	var_flag = (variance>10000 &&  rgn_avg_cnt<15) || variance>17000 ; 
	mot_flag = ((mot<290000 || mot>650000) &&  (cnt_32_most<30)) || ((mot-_ABS_DIFF_(sum_apli,sum_aplp))>100000);
	dark_flag = cnt_32_most>27 && (dark_difflittle>(47 - cnt_32_most)) && mot<41000 && mot>0 ;
	    apld_flag = _ABS_DIFF_(sum_apli,sum_aplp)>120000;

	if ((((apld_flag && (1 - gmv_cond)) || region_flag) && (1 - rgn_max_min_flag) && (1 - mot_flag)) || (dark_flag))
		fifo_flag = 1;
	else
		fifo_flag = 0;

	pOutput->u1_detect = fifo_flag;
}
