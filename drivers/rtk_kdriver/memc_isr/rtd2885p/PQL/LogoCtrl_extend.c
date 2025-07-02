#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
#include "memc_isr/PQL/PQLGlobalDefs.h"
#include "memc_isr/PQL/LogoCtrl_extend.h"
#include "memc_reg_def.h"


#define MAX_NUM 50

// global variable
MEMC_Logo_Boundary_Info_M8P bounding_box_whole_frame_list[MAX_NUM];
MEMC_Logo_Boundary_Info_M8P bounding_box_whole_frame_list_pre[MAX_NUM];
MEMC_Logo_Boundary_Info_M8P bounding_box_whole_frame_list_final[MAX_NUM];

int bounding_box_num = 0;
int bounding_box_num_pre = 0;
int bounding_box_num_final = 0;

bool logo_flag_array_M8P[540][960];
bool logo_invalid_array[540][960];

///////////////////////////////////////////////////////////////////////////////////////////
/*
PQL Improvement:
1. PQL run at PC use _WIN32. (need change to more clear define)
2. Param must be init in order, gen PQL_Glb_Param.c (it's not convenient, can same as output?)

*/
///////////////////////////////////////////////////////////////////////////////////////////
VOID LogoCtrl_Init_RTK2885pp(_OUTPUT_LOGO_CTRL *pOutput)
{
	// unsigned int u32_RB_val;
}

VOID Logo_SC_Detect(const _PARAM_LOGO_CTRL *pParam, _OUTPUT_LOGO_CTRL *pOutput)
{
	// const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned int print_in_func;

	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	if (pParam->u1_logo_sc_detect_en)
	{
	if (1)
	{
		pOutput->u1_logo_sc_status = 1;

	}
	else if (pOutput->u8_logo_sc_hold_frm > 0)
	{
		pOutput->u1_logo_sc_status = 1;
		pOutput->u8_logo_sc_hold_frm = pOutput->u8_logo_sc_hold_frm - 1;
	}
	else
	{
		pOutput->u1_logo_sc_status = 0;
	}
	}
	else
	{
		pOutput->u1_logo_sc_status = 0;

	}
}

VOID LogoCtrl_Sobel_Proc(const _PARAM_LOGO_CTRL *pParam, _OUTPUT_LOGO_CTRL *pOutput)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned int u32_i = 0, i_x = 0, i_y = 0, u12_logo_hor = 0, u12_logo_ver = 0, u13_logo_hor_ver_sum = 0;
	unsigned char u5_lft_rgn = 0, u5_rht_rgn = 0, u5_top_rgn = 0, u5_bot_rgn = 0, u1_enable = 0;
	unsigned char u1_legal_logo_rgn[32], u1_legal_logo_rgn_final[32];
	unsigned int u32_clear_flag = 0;
	unsigned int print_in_func;

	ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
	if (print_in_func == 1)
	{
		rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
	}
	// Step 1: check region itself logo legal condition
	for (u32_i = 0; u32_i < 32; u32_i ++)
	{
		u12_logo_hor = s_pContext->_output_read_comreg.u12_logo_rg_sobel_hor_rb[u32_i];
		u12_logo_ver = s_pContext->_output_read_comreg.u12_logo_rg_sobel_ver_rb[u32_i];
		u13_logo_hor_ver_sum = u12_logo_hor + u12_logo_ver;

		u1_legal_logo_rgn[u32_i] = 1;

		if (u12_logo_hor == 0 || u12_logo_ver == 0)
			u1_legal_logo_rgn[u32_i] = 0;
		else if (u12_logo_hor > 3 * u12_logo_ver)
			u1_legal_logo_rgn[u32_i] = 0;
		else if (u12_logo_ver > 3 * u12_logo_hor)
			u1_legal_logo_rgn[u32_i] = 0;
		else if (u13_logo_hor_ver_sum < 450 && s_pContext->_output_read_comreg.u14_logo_rg_cnt_rb[u32_i] < 800)
			u1_legal_logo_rgn[u32_i] = 0;
	}

	// Step 2: if neighbor is legal logo region, reduce logo count threshold
	for (u32_i = 0; u32_i < 32; u32_i ++)
	{
		u12_logo_hor = s_pContext->_output_read_comreg.u12_logo_rg_sobel_hor_rb[u32_i];
		u12_logo_ver = s_pContext->_output_read_comreg.u12_logo_rg_sobel_ver_rb[u32_i];
		u13_logo_hor_ver_sum = u12_logo_hor + u12_logo_ver;

		i_x = u32_i % 8;
		i_y = u32_i / 8;

		u5_lft_rgn = _CLIP_(i_y    , 0, 3) * 8 + _CLIP_(i_x - 1, 0, 7);
		u5_rht_rgn = _CLIP_(i_y    , 0, 3) * 8 + _CLIP_(i_x + 1, 0, 7);
		u5_top_rgn = _CLIP_(i_y - 1, 0, 3) * 8 + _CLIP_(i_x    , 0, 7);
		u5_bot_rgn = _CLIP_(i_y + 1, 0, 3) * 8 + _CLIP_(i_x    , 0, 7);

		u1_enable = u1_legal_logo_rgn[u32_i];

		if (u1_legal_logo_rgn[u5_lft_rgn] > 0 || u1_legal_logo_rgn[u5_rht_rgn] > 0 || u1_legal_logo_rgn[u5_top_rgn] > 0 || u1_legal_logo_rgn[u5_bot_rgn] > 0)
		{
			if (u12_logo_hor == 0 || u12_logo_ver == 0)
				u1_legal_logo_rgn[u32_i] = 0; // do nothing
			else if (u12_logo_hor < 40 * u12_logo_ver && u13_logo_hor_ver_sum < 220)
				u1_enable = 1;
			else if (u12_logo_ver < 40 * u12_logo_hor && u13_logo_hor_ver_sum < 220)
				u1_enable = 1;
			else if (u12_logo_hor > 5 * u12_logo_ver)
				u1_legal_logo_rgn[u32_i] = 0; // do nothing
			else if (u12_logo_ver > 5 * u12_logo_hor)
				u1_legal_logo_rgn[u32_i] = 0; // do nothing
			else if (u12_logo_hor < 4 * u12_logo_ver)
				u1_enable = 1;
			else if (u12_logo_ver < 4 * u12_logo_hor)
				u1_enable = 1;
			else if (u13_logo_hor_ver_sum > 60)
				u1_enable = 1;
		}
		u1_legal_logo_rgn_final[u32_i] = u1_enable;

		u32_clear_flag += ((u1_enable == 0?1:0) << u32_i);
	}

	pOutput->u32_sobel_logo_clear_flag = u32_clear_flag;

}

/*
bool inBoundary(MEMC_Logo_Boundary_Info_M8P Boundary_samll, MEMC_Logo_Boundary_Info_M8P Boundary_large)
{
	if(Boundary_samll.u16_start_x<Boundary_large.u16_start_x){
		return false;
	}
	if(Boundary_samll.u16_start_y<Boundary_large.u16_start_y){
		return false;
	}
	if(Boundary_samll.u16_end_x>Boundary_large.u16_end_x){
		return false;
	}
	if(Boundary_samll.u16_end_y>Boundary_large.u16_end_y){
		return false;
	}
	return true;
}
*/

void set_prv_match_bounding_box(MEMC_Logo_Boundary_Info_M8P box, int logo_cnt_diff, int logo_cnt, int logo_area)
{
	box.logo_cnt_diff = logo_cnt_diff;
	box.logo_cnt = logo_cnt;
	box.area = logo_area;
}

int get_area(MEMC_Logo_Boundary_Info_M8P box)
{
	box.area = _ABS_(box.end_x - box.start_x) * _ABS_(box.end_y - box.start_y);
	return box.area ;
}

int get_width_height_ration(MEMC_Logo_Boundary_Info_M8P box)
{
	int width = _ABS_(box.end_x - box.start_x);
	int height = _ABS_(box.end_y - box.start_y);
	box.width_height_ratio = _MAX_( (width/height), (height/width) );
	return box.width_height_ratio;
}

int get_logo_cnt_area_ratio(MEMC_Logo_Boundary_Info_M8P box)
{
	box.logo_cnt_area_ratio = ( box.logo_cnt * 1024 / ( box.area + 1 ) );
	return box.logo_cnt_area_ratio;
}

int get_rgn_x(int x_pos)
{
	//MODIFY_BY_RESOLUTION
	int rgn_width = 3840 / 8;
	int i;
	
	for(i=0; i<8 ;i++)
	{
		if( (x_pos >= (rgn_width * i) ) && (x_pos < (rgn_width * (i+1)) ) )
			return i;		
	}
	return 0;
}

int get_rgn_y(int y_pos)
{
	//MODIFY_BY_RESOLUTION
	int rgn_height = 2160 / 4;
	int i;
	for(i=0; i<4 ;i++)
	{
		if( (y_pos >= (rgn_height * i) ) && (y_pos < (rgn_height * (i+1)) ) )
		{
			return i;
		}		
	}
	return 0;
}

bool in_boundary(MEMC_Logo_Boundary_Info_M8P big_box, MEMC_Logo_Boundary_Info_M8P small_box)
{
	if( small_box.start_x < big_box.start_x)
		return FALSE;
	if( small_box.start_y < big_box.start_y)
		return FALSE;	
	if( small_box.end_x > big_box.end_x)
		return FALSE;
	if( small_box.end_y > big_box.end_y)
		return FALSE;

	return TRUE;
}

int check_boundinging_box_match(MEMC_Logo_Boundary_Info_M8P box1, MEMC_Logo_Boundary_Info_M8P  box2, int tolerence)
{

	int LU_diff = _ABS_( box1.start_x - box2.start_x ) + _ABS_( box1.start_y - box2.start_y );
	int RU_diff = _ABS_( box1.end_x - box2.end_x ) + _ABS_( box1.start_y - box2.start_y );
	int LB_diff = _ABS_( box1.start_x - box2.start_x ) + _ABS_( box1.end_y - box2.end_y );
	int RB_diff = _ABS_( box1.end_x - box2.end_x ) + _ABS_( box1.end_y - box2.end_y );
	int end_point_match = 0;
	if( (LU_diff < tolerence) || (RU_diff < tolerence) || (LB_diff < tolerence) || (RB_diff < tolerence) )
	{
		end_point_match = ( end_point_match + 1);
	}
	return end_point_match;	
}

int calculate_bounding_box_dist(MEMC_Logo_Boundary_Info_M8P box1, MEMC_Logo_Boundary_Info_M8P box2)
{

	int dist[] = {0, 0, 0, 0, 0, 0, 0, 0};
	int i, add_distance;
	int min_dist = 0;

	// direction 0
	dist[0] = _ABS_( box1.start_x - box2.end_x );
	if( (box1.start_y <= box2.start_y) && (box1.start_y >= box2.end_y ) )
	{
		add_distance = 0;
	}
	else
	{
		add_distance = _MIN_( _ABS_(box1.start_y - box2.start_y), _ABS_(box1.start_y - box2.end_y) );
	}
	dist[0] = ( dist[0] + add_distance);

	// direction 1
	dist[1] = _ABS_( box1.start_y - box2.end_y );
	if( (box1.start_x <= box2.start_x) && (box1.start_x >= box2.end_x ) )
	{
		add_distance = 0;
	}
	else
	{
		add_distance = _MIN_( _ABS_(box1.start_x - box2.start_x), _ABS_(box1.start_x - box2.end_x) );
	}
	dist[1] = ( dist[1] + add_distance);

	// direction 2
	dist[2] = _ABS_( box1.end_x - box2.start_x );
	if( (box1.start_y <= box2.start_y) && (box1.start_y >= box2.end_y ) )
	{
		add_distance = 0;
	}
	else
	{
		add_distance = _MIN_( _ABS_(box1.start_y - box2.start_y), _ABS_(box1.start_y - box2.end_y) );
	}
	dist[2] = ( dist[2] + add_distance);

	// direction 3
	dist[3] = _ABS_( box1.start_y - box2.end_y );
	if( (box1.end_x <= box2.start_x) && (box1.end_x >= box2.end_x ) )
	{
		add_distance = 0;
	}
	else
	{
		add_distance = _MIN_( _ABS_(box1.end_x - box2.start_x), _ABS_(box1.end_x - box2.end_x) );
	}
	dist[3] = ( dist[3] + add_distance);

	// direction 4
	dist[4] = _ABS_( box1.start_x - box2.end_x );
	if( (box1.end_y <= box2.start_y) && (box1.end_y >= box2.end_y ) )
	{
		add_distance = 0;
	}
	else
	{
		add_distance = _MIN_( _ABS_(box1.end_y - box2.start_y), _ABS_(box1.end_y - box2.end_y) );
	}
	dist[4] = ( dist[4] + add_distance);	

	// direction 5
	dist[5] = _ABS_( box1.end_y - box2.start_y );
	if( (box1.start_x <= box2.start_x) && (box1.start_x >= box2.end_x ) )
	{
		add_distance = 0;
	}
	else
	{
		add_distance = _MIN_( _ABS_(box1.start_x - box2.start_x), _ABS_(box1.start_x - box2.end_x) );
	}
	dist[5] = ( dist[5] + add_distance);
	
	// direction 6
	dist[6] = _ABS_( box1.end_x - box2.start_x );
	if( (box1.end_y <= box2.start_y) && (box1.end_y >= box2.end_y ) )
	{
		add_distance = 0;
	}
	else
	{
		add_distance = _MIN_( _ABS_(box1.end_y - box2.start_y), _ABS_(box1.end_y - box2.end_y) );
	}
	dist[6] = ( dist[6] + add_distance);	

	// direction 7
	dist[7] = _ABS_( box1.end_y - box2.start_y );
	if( (box1.end_x <= box2.start_x) && (box1.end_x >= box2.end_x ) )
	{
		add_distance = 0;
	}
	else
	{
		add_distance = _MIN_( _ABS_(box1.end_x - box2.start_x), _ABS_(box1.end_x - box2.end_x) );
	}
	dist[7] = ( dist[7] + add_distance);

	// sort dist[0~7], return min dist
	for(i=0;i<8;i++)
	{
		if(dist[i] < min_dist)
		{
			min_dist = dist[i];
		}	
	}
	
	return min_dist;

}


int calculate_bounding_box_iou(MEMC_Logo_Boundary_Info_M8P box1, MEMC_Logo_Boundary_Info_M8P box2)
{
	int x_inter1, y_inter1, x_inter2, y_inter2;
	int width_inter, height_inter, area_inter;
	int width_box1, height_box1, width_box2, height_box2;
	int area_box1, area_box2, area_union, iou;


	x_inter1 = _MAX_( box1.start_x, box2.start_x );
	y_inter1 = _MAX_( box1.start_y, box2.start_y );
	x_inter2 = _MIN_( box1.end_x, box2.end_x );
	y_inter2 = _MIN_( box1.end_y, box2.end_y );
	if( (x_inter1 > x_inter2) || (y_inter1 > y_inter2))
		return 0;
	width_inter = _ABS_( x_inter1 - x_inter2 );
	height_inter = _ABS_( y_inter1 - y_inter2 );
	area_inter = width_inter * height_inter;
	width_box1 = _ABS_(box1.start_x - box1.end_x);
	height_box1 = _ABS_(box1.start_y - box1.end_y);
	width_box2 = _ABS_(box2.start_x - box2.end_x);
	height_box2 = _ABS_(box2.start_y - box2.end_y);
	area_box1 = width_box1 * height_box1;
	area_box2 = width_box2 * height_box2;
	area_union = area_box1 + area_box2 - area_inter;
	iou = area_inter / area_union;

	return iou;

}


bool bounding_box_width_height_Ratio(MEMC_Logo_Boundary_Info_M8P bounding_box)
{
	int ratio = 13;
	int width, height;
	width = _ABS_( bounding_box.end_x - bounding_box.start_x );
	height = _ABS_( bounding_box.end_y - bounding_box.start_y );
	if( (width*ratio) < height)
		return FALSE;
	if( (height*ratio) < width)
		return FALSE;
	return TRUE;
}


int bounding_box_logo_cnt(bool logo_flag_array[540][960], MEMC_Logo_Boundary_Info_M8P bounding_box)
{
	int logo_cnt = 0;
	int x, y;
	for(y=bounding_box.start_y; y<bounding_box.end_y; y++)
	{
		for(x=bounding_box.start_x; x<bounding_box.end_x; x++)
		{
			if(logo_flag_array[y][x] == TRUE)
				logo_cnt = logo_cnt + 1;
		}
	}
	return logo_cnt;
}


 // return -1 if out of search range or has_logo(already checked)
 // return 1 
int Search_Logo_Boundary_in_H_M8P(bool logo_flag_array[540][960], bool logo_invalid_array[540][960], MEMC_Logo_Boundary_M8P Search_Boundary, MEMC_Logo_Boundary_Info_M8P Boundary_Cur)
{
	int cur_pos_x, cur_pos_y;
	bool has_Logo;
	
	cur_pos_x = 0;
	cur_pos_y = Boundary_Cur.end_y;   

	has_Logo = FALSE;

	if(cur_pos_y > Search_Boundary.end_y)
	{
		Boundary_Cur.end_y = Search_Boundary.end_y;
		return -1;
	}

	// search logo in next line, if no logo return function
	for(cur_pos_x = Boundary_Cur.start_x; cur_pos_x < Boundary_Cur.end_x; cur_pos_x++)
	{
		if( (logo_flag_array[cur_pos_y][cur_pos_x] == TRUE) && (logo_invalid_array[cur_pos_y][cur_pos_x] == FALSE) )
		{
			logo_invalid_array[cur_pos_y][cur_pos_x] = TRUE;
			has_Logo = TRUE;
		}
	}

	if(has_Logo == FALSE)
	{
		return -1;
	}

	// try to find min left boundary by search logo in left side

	for(cur_pos_x = Boundary_Cur.start_x; cur_pos_x >= Search_Boundary.start_x; cur_pos_x--)	// for cur_pos_x in range(Boundary_Cur.start_x, Search_Boundary.start_x, -1):
	{
		if(logo_flag_array[cur_pos_y][cur_pos_x] == FALSE)
			break;
		else
			logo_invalid_array[cur_pos_y][cur_pos_x] = TRUE;
		// update the left boundary
		Boundary_Cur.start_x = cur_pos_x;
	}

	// try to find max right boundary by search logo in right side
	for(cur_pos_x = Boundary_Cur.end_x; cur_pos_x < Search_Boundary.end_x; cur_pos_x++)		// for cur_pos_x in range(Boundary_Cur.end_x, Search_Boundary.end_x, 1):
	{
		// complete finding right boundary when there is no logo
		if(logo_flag_array[cur_pos_y][cur_pos_x] == FALSE)
			break;
		else
			logo_invalid_array[cur_pos_y][cur_pos_x] = TRUE;
		// update the right boundary
		Boundary_Cur.end_x = cur_pos_x;
	}

	// update the bottom boundary
	Boundary_Cur.end_y = (Boundary_Cur.end_y + 1);

	// try to search next line
	Search_Logo_Boundary_in_H_M8P(logo_flag_array, logo_invalid_array, Search_Boundary, Boundary_Cur);

	return 1;
// HS test logo bounding box
}


void Search_MAX_area_boundary_M8P(bool logo_flag_array[540][960], bool logo_invalid_array[540][960], MEMC_Logo_Boundary_M8P search_range)
{
	int search_x, search_y;
	int logo_cnt, area;


	MEMC_Logo_Boundary_Info_M8P  search_position = {0};


	//range_logo_bounding_box_list = []

	for(search_y = search_range.start_y; search_y < search_range.end_y; search_y++)
	{
		for(search_x = search_range.start_x; search_x < search_range.end_x; search_x++)
		{
			if( (logo_flag_array[search_y][search_x] == TRUE) && (logo_invalid_array[search_y][search_x] == FALSE) )
			{
				search_position.start_x = search_x;
				search_position.end_x = search_x;
				search_position.start_y = search_y;
				search_position.end_y = search_y;        

				Search_Logo_Boundary_in_H_M8P(logo_flag_array, logo_invalid_array, search_range, search_position);

				search_position.valid = TRUE;

				logo_cnt = bounding_box_logo_cnt(logo_flag_array, search_position);
				search_position.logo_cnt = logo_cnt;
				area = _ABS_(search_position.end_x - search_position.start_x) * _ABS_(search_position.end_y - search_position.start_y);				

				if( area > 100 )
				{
					//search_position.get_width_height_ratio();
					search_position.width = _ABS_( search_position.end_x - search_position.start_x );
					search_position.height = _ABS_( search_position.end_y - search_position.start_y );
					
					search_position.width_height_ratio = _MAX_( (search_position.width/search_position.height), (search_position.height/search_position.width) );
					
					//search_position.get_logo_cnt_area_ratio();
					search_position.logo_cnt_area_ratio = ( search_position.logo_cnt / (search_position.area + 1) );

					
					//range_logo_bounding_box_list.append( deepcopy(search_position) );	// no list in C, only in python
					
					bounding_box_whole_frame_list[bounding_box_num] = search_position;
					bounding_box_num = bounding_box_num + 1;

				}

				search_x = ( search_position.end_x + 1 );
				
			}
		
		}
	}

	//return range_logo_bounding_box_list   // global variable don't return

}


VOID logo_bounding_box_fw_sim(const _PARAM_LOGO_CTRL *pParam, _OUTPUT_LOGO_CTRL *pOutput)
{

	const _PQLCONTEXT *s_pContext = GetPQLContext();

	int i,j,m,n;
	int min_dist_th = 20;
	
	int logo_cnt_min = 900, logo_cnt_max = 47500, logo_area_min = 1300, logo_area_max = 110000, width_height_ratio_min = 1, width_height_ratio_max = 40, stable_count_th = 2;
	int logo_cnt_area_ratio_min = 0.2 *1024;
	int logo_cnt_area_ratio_max = 1 * 1024;
	int logo_cnt, bounding_box_dist, iou, logo_cnt_diff;
	int rgn_start_x, rgn_end_x, rgn_start_y, rgn_end_y, rgn_sum, rmv_x_sum, rmv_y_sum, rmv_x, rmv_y;
	bool is_contain, logo_cnt_cond, logo_area_cond, width_height_ratio_cond, logo_cnt_area_ratio_cond, temporal_stable_cond;

	int end_point_match_num;
	int end_point_match_th = 2;
	int check_stable_dist_th = 20;

	MEMC_Logo_Boundary_M8P search_range_whole_frame;

	// get 32rgn rmv & long-term logo score
	//int logo_long_term_32rgn[32];
	//logo_long_term_32rgn = torch.zeros(32)

	//rmv_mv = me1_output_module.pfv.rmv_info.mv.reshape(2, 4, 8, 3).clone()

	//for i in range(32):
	//	logo_long_term_32rgn[i] = self.internal_module.long_term_logo_counter[i]

	//logo_long_term_8x4 = logo_long_term_32rgn.reshape(4, 8)

	// get logo flag from ddr
	//logo_flag_array = logo_output_module.logo_flag > 0

	for(j=0;j<540;j++)
	{
		for(i=0;i<960;i++)
		{
			logo_invalid_array[j][i] = FALSE;
		}
	}

// modify search range to whole frame

	// 1. scan logo_flag_array to get logo bounding box

	//search_range_whole_frame = {0, 960, 0, 539};
	//MEMC_Logo_Boundary_M8P search_range_whole_frame;
	search_range_whole_frame.start_x = 0;
	search_range_whole_frame.end_x = 960;
	search_range_whole_frame.start_y = 0;
	search_range_whole_frame.end_y = 539;

	Search_MAX_area_boundary_M8P(logo_flag_array_M8P, logo_invalid_array, search_range_whole_frame);

	// sort bounding_box_whole_frame_list
	// sort array
	//bounding_box_whole_frame_list.sort(key=get_area, reverse = TRUE);

	// 2. merge logo bounding box
	min_dist_th = 20;

//MEMC_Logo_Boundary_Info_M8P bounding_box_whole_frame_list[MAX_NUM];

//int bounding_box_num;

		if( bounding_box_num > 1 )
		{
			//for(j=0;j<len(bounding_box_whole_frame_list);j++)
			for(j=0;j<bounding_box_num;j++)
			{
			//for j in range(0, len(bounding_box_whole_frame_list), 1):
				//for(i=0;i<len(bounding_box_whole_frame_list);i++)
				for(i=0; i<bounding_box_num;i++)
				{
				//for i in range(0, len(bounding_box_whole_frame_list), 1):
					bounding_box_dist = calculate_bounding_box_dist(bounding_box_whole_frame_list[i], bounding_box_whole_frame_list[j]);
				
					if(bounding_box_whole_frame_list[i].area > bounding_box_whole_frame_list[j].area)
					{
						is_contain = in_boundary(bounding_box_whole_frame_list[i], bounding_box_whole_frame_list[j]);
					}
					else
					{
						is_contain = in_boundary(bounding_box_whole_frame_list[j], bounding_box_whole_frame_list[i]);
					}

					if( (i != j) && (bounding_box_whole_frame_list[i].valid == TRUE) && ((bounding_box_whole_frame_list[j].valid == TRUE)) && ( (bounding_box_dist < min_dist_th) || (is_contain == TRUE) ) )
					{
						bounding_box_whole_frame_list[j].start_x = _MIN_(bounding_box_whole_frame_list[j].start_x, bounding_box_whole_frame_list[i].start_x);
						bounding_box_whole_frame_list[j].start_y = _MIN_(bounding_box_whole_frame_list[j].start_y, bounding_box_whole_frame_list[i].start_y);
						bounding_box_whole_frame_list[j].end_x = _MAX_(bounding_box_whole_frame_list[j].end_x, bounding_box_whole_frame_list[i].end_x);
						bounding_box_whole_frame_list[j].end_y = _MAX_(bounding_box_whole_frame_list[j].end_y, bounding_box_whole_frame_list[i].end_y);
						//bounding_box_whole_frame_list[j].calculate_area();
						bounding_box_whole_frame_list[j].area = _ABS_(bounding_box_whole_frame_list[j].end_x - bounding_box_whole_frame_list[j].start_x ) * _ABS_( bounding_box_whole_frame_list[j].end_y - bounding_box_whole_frame_list[j].start_y );
						logo_cnt = bounding_box_logo_cnt(logo_flag_array_M8P, bounding_box_whole_frame_list[j]);
						bounding_box_whole_frame_list[j].logo_cnt = logo_cnt;                        
						//bounding_box_whole_frame_list[j].get_width_height_ratio();
						get_width_height_ration( bounding_box_whole_frame_list[j] );                       
						//bounding_box_whole_frame_list[j].get_logo_cnt_area_ratio();
						get_logo_cnt_area_ratio( bounding_box_whole_frame_list[j] );
						bounding_box_whole_frame_list[i].valid = FALSE;
					}
				}
			}
		}


		// 2.1 merge overlap bounding box
		//if( len(bounding_box_whole_frame_list) > 1)
		if( bounding_box_num > 1 )
		{
			//for(j=0; j<len(bounding_box_whole_frame_list); j++)
			for(j=0; j<bounding_box_num; j++ )
			{
			//for j in range(0, len(bounding_box_whole_frame_list), 1):
				//for(i=0; i<len(bounding_box_whole_frame_list); i++)
				for(i=0; i<bounding_box_num; i++)
				{
				//for i in range(0, len(bounding_box_whole_frame_list), 1):
					if( (i != j) && (bounding_box_whole_frame_list[i].valid == TRUE) && ((bounding_box_whole_frame_list[j].valid == TRUE)) )
					{
						iou = calculate_bounding_box_iou(bounding_box_whole_frame_list[i], bounding_box_whole_frame_list[j]);

						if(iou > 0)
						{
							bounding_box_whole_frame_list[j].start_x = _MIN_(bounding_box_whole_frame_list[j].start_x, bounding_box_whole_frame_list[i].start_x);
							bounding_box_whole_frame_list[j].start_y = _MIN_(bounding_box_whole_frame_list[j].start_y, bounding_box_whole_frame_list[i].start_y);
							bounding_box_whole_frame_list[j].end_x = _MAX_(bounding_box_whole_frame_list[j].end_x, bounding_box_whole_frame_list[i].end_x);
							bounding_box_whole_frame_list[j].end_y = _MAX_(bounding_box_whole_frame_list[j].end_y, bounding_box_whole_frame_list[i].end_y);
							//bounding_box_whole_frame_list[j].calculate_area();
							get_area(bounding_box_whole_frame_list[j]);
							// recalculate logo_cnt
							logo_cnt = bounding_box_logo_cnt(logo_flag_array_M8P, bounding_box_whole_frame_list[j]);
							bounding_box_whole_frame_list[j].logo_cnt = logo_cnt;                        
							//bounding_box_whole_frame_list[j].get_width_height_ratio();
							get_width_height_ration(bounding_box_whole_frame_list[j]);
							//bounding_box_whole_frame_list[j].get_logo_cnt_area_ratio();
							get_logo_cnt_area_ratio(bounding_box_whole_frame_list[j]);
							bounding_box_whole_frame_list[i].valid = FALSE;
						}
					}
				}

			}          
		}

		// 3. check logo bounding box valid condition

        //self.dbg_module.logo_bounding_box_list = []
        //if len(bounding_box_whole_frame_list) > 0:
        if( bounding_box_num > 0 )
        {
            //for i in range(0, len(bounding_box_whole_frame_list), 1):
            for(i=0; i<bounding_box_num; i++)
            {

                rgn_start_x = get_rgn_x(bounding_box_whole_frame_list[i].start_x);
                rgn_end_x = get_rgn_x(bounding_box_whole_frame_list[i].end_x);
                rgn_start_y = get_rgn_y(bounding_box_whole_frame_list[i].start_y);
                rgn_end_y = get_rgn_y(bounding_box_whole_frame_list[i].end_y);

                rgn_sum = 0;
                rmv_x_sum = 0;
                rmv_y_sum = 0;
                //for m in range(rgn_start_x, (rgn_end_x+1), 1):
                for(m = rgn_start_x; m <= rgn_end_x; m++)
                {
                    //for n in range(rgn_start_y, (rgn_end_y+1), 1)
                    for(n = rgn_start_y; n <= rgn_end_y; n++)
                    {
                        rgn_sum = rgn_sum + 1;
                        rmv_x_sum = rmv_x_sum + s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[8*n+m];
                        rmv_y_sum = rmv_y_sum + s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[8*n+m];
                        //rmv_x_sum = rmv_x_sum + rmv_mv[1, n, m, 0];
                        //rmv_y_sum = rmv_y_sum + rmv_mv[0, n, m, 0];
                    }
                }
                rmv_x = (rmv_x_sum / rgn_sum);
                rmv_y = (rmv_y_sum / rgn_sum);
                bounding_box_whole_frame_list[i].rmv_x = rmv_x;
                bounding_box_whole_frame_list[i].rmv_y = rmv_y;

                // update bounding box temporal count
                if( bounding_box_num_pre > 0 )                  
                //if( len(self.internal_module.logo_bounding_box_pre_frame_list) ) > 0:
                {
                    //for j in range(0, len(self.internal_module.logo_bounding_box_pre_frame_list), 1):
                    for(j=0; j<bounding_box_num_pre; j++)
                    {
                        end_point_match_num = check_boundinging_box_match(bounding_box_whole_frame_list[i], bounding_box_whole_frame_list_pre[j], check_stable_dist_th);
                        if( end_point_match_num >= end_point_match_th )
                        {
                            bounding_box_whole_frame_list[i].stable_cnt = (bounding_box_whole_frame_list_pre[j].stable_cnt + 1);

                            logo_cnt_diff = _ABS_( bounding_box_whole_frame_list[i].logo_cnt - bounding_box_whole_frame_list_pre[j].logo_cnt );
                            //bounding_box_whole_frame_list[i].set_prv_match_bounding_box( logo_cnt_diff, bounding_box_whole_frame_list_pre[j].logo_cnt, bounding_box_whole_frame_list_pre[j].area );
                            set_prv_match_bounding_box( bounding_box_whole_frame_list[i], logo_cnt_diff, bounding_box_whole_frame_list_pre[j].logo_cnt, bounding_box_whole_frame_list_pre[j].area );
                        }
                    }
                }



                if( bounding_box_whole_frame_list[i].logo_cnt < 4000 )
                {
                    logo_cnt_area_ratio_min = 0.2* 1024;
                    logo_cnt_area_ratio_max = 1* 1024;
                }
                else if( (bounding_box_whole_frame_list[i].logo_cnt >= 4000) && (bounding_box_whole_frame_list[i].logo_cnt < 5000) )
                {
                    logo_cnt_area_ratio_min = 0.35* 1024;
                    logo_cnt_area_ratio_max = 0.9* 1024;
                }
                else if( (bounding_box_whole_frame_list[i].logo_cnt >= 5000) && (bounding_box_whole_frame_list[i].logo_cnt < 10000) )
                {
                    logo_cnt_area_ratio_min = 0.55* 1024;
                    logo_cnt_area_ratio_max = 0.9* 1024;
                }
                else if( (bounding_box_whole_frame_list[i].logo_cnt >= 10000) && (bounding_box_whole_frame_list[i].logo_cnt < 17500) )
                {
                    logo_cnt_area_ratio_min = 0.5* 1024;
                    logo_cnt_area_ratio_max = 0.95 * 1024;
                }
                else if( bounding_box_whole_frame_list[i].logo_cnt >= 17500 )
                {
                    logo_cnt_area_ratio_min = 0.35 * 1024;
                    logo_cnt_area_ratio_max = 0.5 * 1024;
                }


                logo_cnt_cond = ( (bounding_box_whole_frame_list[i].logo_cnt >= logo_cnt_min) && (bounding_box_whole_frame_list[i].logo_cnt <= logo_cnt_max) );
                logo_area_cond = ( (bounding_box_whole_frame_list[i].area >= logo_area_min) && (bounding_box_whole_frame_list[i].area <= logo_area_max) );
                width_height_ratio_cond = ( (bounding_box_whole_frame_list[i].width_height_ratio >= width_height_ratio_min) && ( bounding_box_whole_frame_list[i].width_height_ratio <= width_height_ratio_max ) );
                logo_cnt_area_ratio_cond = ( ( bounding_box_whole_frame_list[i].logo_cnt_area_ratio >= logo_cnt_area_ratio_min ) && ( bounding_box_whole_frame_list[i].logo_cnt_area_ratio <= logo_cnt_area_ratio_max ) );
                temporal_stable_cond = (bounding_box_whole_frame_list[i].stable_cnt >= stable_count_th);
                if( logo_cnt_cond && logo_area_cond && width_height_ratio_cond && logo_cnt_area_ratio_cond && temporal_stable_cond )
                    bounding_box_whole_frame_list[i].final_valid = TRUE;

                if( (bounding_box_whole_frame_list[i].valid == TRUE) && (bounding_box_whole_frame_list[i].area> 100) )
                {
                    //self.dbg_module.logo_bounding_box_list.append( deepcopy(bounding_box_whole_frame_list[i]) );
                    bounding_box_whole_frame_list_final[bounding_box_num_final] = bounding_box_whole_frame_list[i];
                    bounding_box_num_final = bounding_box_num_final + 1;
                }


            }
        }


        // 4. copy bounding_box_whole_frame_list to logo_bounding_box_pre_frame_list
        // copy list
        //self.internal_module.logo_bounding_box_pre_frame_list = deepcopy( bounding_box_whole_frame_list );
        for(i=0; i<bounding_box_num; i++)            
        {
            bounding_box_whole_frame_list_pre[i] = bounding_box_whole_frame_list[i];    
        }
        bounding_box_num_pre = bounding_box_num;
    }


VOID long_term_logo_fw(const _PARAM_LOGO_CTRL *pParam, _OUTPUT_LOGO_CTRL *pOutput)
{

    const _PQLCONTEXT *s_pContext = GetPQLContext();

    // global variable
    static int logo_detect_rgcnt_pre[32];
    static int zmv_rg_cnt_pre[32];
    static int logo_detect_rgcnt_avg[32];
    static int zmv_rg_cnt_avg[32];

    int avg_select = 2;
    int logo_detect_rgcnt[32];
    int zmv_rg_cnt[32];
    int zmv_deviation[32];
    int still_logo_step_rgn[32];
    int logo_case_rgn[32];
    int i;
    int zmv_cnt_diff, zmv_diff_avg, logo_cnt_num, zmv_cnt_num, logo_cnt_diff, logo_cnt_veriation, zmv_cnt_veriation, logo_case, still_logo_step;
    int temporal_counter;
    unsigned int print_in_func;
    ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
    if (print_in_func == 1)
    {
        rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
    }
    for(i=0;i<32;i++)
    {
        zmv_deviation[i] = 0;
        still_logo_step_rgn[i] = 0;
        logo_case_rgn[i] = 0;        
    }

    for(i=0;i<32;i++)
    {
        logo_detect_rgcnt[i] = (s_pContext->_output_read_comreg.u14_logo_rg_cnt_rb[i] >> 4);        // read logo detection 8x4 statistics register
        zmv_rg_cnt[i] = s_pContext->_output_read_comreg.u10_me_bv_stats_rb[i];                    // read zmv 8x4 region cnt register
    }

    for(i=0;i<32;i++)
    {
        if(avg_select == 0)
        {
            logo_detect_rgcnt_avg[i] = ( ( (logo_detect_rgcnt_avg[i] * 7) + logo_detect_rgcnt_pre[i]) >> 3 );
            zmv_rg_cnt_avg[i] = ( ( (zmv_rg_cnt_avg[i] * 7 ) + zmv_rg_cnt_pre[i] ) >> 3 );
            //self.internal_module.logo_detect_rgcnt_avg[i] = ( ( (self.internal_module.logo_detect_rgcnt_avg[i].int() * 7) + self.internal_module.logo_detect_rgcnt_pre[i].int() ) >> 3 )
            //self.internal_module.zmv_rg_cnt_avg[i] = ( ( (self.internal_module.zmv_rg_cnt_avg[i].int() * 7) + self.internal_module.zmv_rg_cnt_pre[i].int() ) >> 3 )  
        }
        else if(avg_select == 1)
        {
            logo_detect_rgcnt_avg[i] = ( ( (logo_detect_rgcnt_avg[i] * 3) + logo_detect_rgcnt_pre[i] ) >> 2 );
            zmv_rg_cnt_avg[i] = ( ( ( zmv_rg_cnt_avg[i] * 3 ) + zmv_rg_cnt_pre[i] ) >> 2 );
            //self.internal_module.logo_detect_rgcnt_avg[i] = ( ( (self.internal_module.logo_detect_rgcnt_avg[i].int() * 3) + self.internal_module.logo_detect_rgcnt_pre[i].int() ) >> 2 )
            //self.internal_module.zmv_rg_cnt_avg[i] = ( ( (self.internal_module.zmv_rg_cnt_avg[i].int() * 3) + self.internal_module.zmv_rg_cnt_pre[i].int() ) >> 2 ) 
        }
        else if(avg_select == 2)
        {
            logo_detect_rgcnt_avg[i] = logo_detect_rgcnt_pre[i];
            zmv_rg_cnt_avg[i] = zmv_rg_cnt_pre[i];
            //self.internal_module.logo_detect_rgcnt_avg[i] = self.internal_module.logo_detect_rgcnt_pre[i]
            //self.internal_module.zmv_rg_cnt_avg[i] = self.internal_module.zmv_rg_cnt_pre[i] 
        }    
    }

    for(i=0;i<32;i++)
    {
        
        zmv_cnt_diff = 0;
        zmv_diff_avg = 0;
        temporal_counter = pOutput->long_term_logo_counter[i];
        if( i < 24 )
        {
            logo_cnt_num = (34*30);
            zmv_cnt_num = (34*30);
        }
        else
        {
            logo_cnt_num = (33*30);
            zmv_cnt_num = (33*30);        
        }

        // calculate logo_cnt_diff, logo_cnt_variation
        if( logo_detect_rgcnt[i] >= logo_detect_rgcnt_avg[i] )
        {
            logo_cnt_diff = logo_detect_rgcnt[i] - logo_detect_rgcnt_avg[i];
            logo_cnt_veriation = logo_cnt_diff * 1000 / logo_cnt_num;            
        }
        else
        {
            logo_cnt_diff = logo_detect_rgcnt_avg[i] - logo_detect_rgcnt[i];
            logo_cnt_veriation = -1*(logo_cnt_diff * 1000 / logo_cnt_num);        
        }

        // calculate zmv_cnt_diff, zmv_cnt_variation
        if(zmv_rg_cnt[i] >= zmv_rg_cnt_avg[i])
        {
            zmv_cnt_diff = zmv_rg_cnt[i] - zmv_rg_cnt_avg[i];
            zmv_cnt_veriation = zmv_cnt_diff * 1000 / zmv_cnt_num;
        }
        else
        {
            zmv_cnt_diff = zmv_rg_cnt_avg[i] - zmv_rg_cnt[i];
            zmv_cnt_veriation = -1*(zmv_cnt_diff * 1000 / zmv_cnt_num);
        }

        // calculate zmv_deviation
        if(zmv_rg_cnt[i] >= logo_detect_rgcnt[i])
            zmv_deviation[i] = zmv_rg_cnt[i] - logo_detect_rgcnt[i];
        else
            zmv_deviation[i] = -1*(logo_detect_rgcnt[i] - zmv_rg_cnt[i]);


        // calculate logo_case
        if( zmv_rg_cnt[i] > (zmv_cnt_num * 80 / 100 ) )
        {
            // logo_case_1 : no motion, do nothing
            logo_case = 1;
        }
        else if( zmv_rg_cnt[i] <= ( zmv_cnt_num * 1 / 100 ) )
        {
            // logo_case_2 : big motion, decrease
            logo_case = 2;
        }
        else
        {
            //other case
            if(logo_detect_rgcnt[i] < (logo_cnt_num * 4 / 100) )
            {
                // logo_case_3 : no logo, decrease
                logo_case = 3;
            }
            else if(logo_detect_rgcnt[i] > (logo_cnt_num * 60 / 100) )
            {
                // logo_case_4 : too much logo, do nothing
                logo_case = 4;
            }
            else
            {
                if( _ABS_(logo_cnt_veriation) <= 5 )
                {
                    // increase high
                    if( _ABS_(zmv_cnt_veriation) <= 5 )
                        logo_case = 5; // logo_cnt & zmv_cnt is very stable, increase highest
                    else if( ( _ABS_(zmv_cnt_veriation) > 5 ) && ( _ABS_(zmv_cnt_veriation) <= 10 ) )
                        logo_case = 6;
                    else if( ( _ABS_(zmv_cnt_veriation) > 10 ) && ( _ABS_(zmv_cnt_veriation) <= 30 ) )
                        logo_case = 7;
                    else   // if( abs(zmv_cnt_veriation) > 30 )
                        logo_case = 8;

                }
                else if( ( _ABS_(logo_cnt_veriation) > 5 ) && ( _ABS_(logo_cnt_veriation) <= 10 ) )
                {
                    // increase low
                    if( _ABS_(zmv_cnt_veriation) <= 5 )
                        logo_case = 9;
                    else if( ( _ABS_(zmv_cnt_veriation) > 5 ) && ( _ABS_(zmv_cnt_veriation) <= 10 ) )
                        logo_case = 10;
                    else if( ( _ABS_(zmv_cnt_veriation) > 10 ) && ( _ABS_(zmv_cnt_veriation) <= 30 ) )
                        logo_case = 11;
                    else   // if( abs(zmv_cnt_veriation) > 30 )
                        logo_case = 12;
                }
                else if( ( _ABS_(logo_cnt_veriation) > 10 ) && ( _ABS_(logo_cnt_veriation) <= 30 ) )
                {
                    //increase low
                    if( _ABS_(zmv_cnt_veriation) <= 5 )
                        logo_case = 13;
                    else if( ( _ABS_(zmv_cnt_veriation) > 5 ) && ( _ABS_(zmv_cnt_veriation) <= 10 ) )
                        logo_case = 14;
                    else if( ( _ABS_(zmv_cnt_veriation) > 10 ) && ( _ABS_(zmv_cnt_veriation) <= 30 ) )
                        logo_case = 15;
                    else //if( abs(zmv_cnt_veriation) > 30 )
                        logo_case = 16;    
                }            
                else    // if( abs(logo_cnt_veriation) > 30 )
                {
                    //decrease
                    if( _ABS_(zmv_cnt_veriation) <= 5)
                        logo_case = 17;
                    else if( ( _ABS_(zmv_cnt_veriation) > 5 ) && ( _ABS_(zmv_cnt_veriation) <= 10 ) )
                        logo_case = 18;
                    else if( ( _ABS_(zmv_cnt_veriation) > 10 ) && ( _ABS_(zmv_cnt_veriation) <= 30 ) )
                        logo_case = 19;
                    else // if( abs(zmv_cnt_veriation) > 30 )
                        logo_case = 20;
                }
            }
        }

        if( logo_case == 1 )
            still_logo_step = -2;
        else if( logo_case == 2 )
            still_logo_step = -9;
        else if( logo_case == 3 )
            still_logo_step = -4;
        else if( logo_case == 4 )
            still_logo_step = -0;
        else if( logo_case == 5 )
            still_logo_step = 14;
        else if( logo_case == 6 )
            still_logo_step = 13;
        else if( logo_case == 7 )
            still_logo_step = 12;
        else if( logo_case == 8 )
            still_logo_step = 11;
        else if( logo_case == 9 )
            still_logo_step = 9;
        else if( logo_case == 10 )
            still_logo_step = 7;
        else if( logo_case == 11 )
            still_logo_step = 5;
        else if( logo_case == 12 )
            still_logo_step = 3;
        else if( logo_case == 13 )
            still_logo_step = 2;
        else if( logo_case == 14 )
            still_logo_step = 1;
        else if( logo_case == 15 )
            still_logo_step = 0;
        else if( logo_case == 16 )
            still_logo_step = -1;
        else if( logo_case == 17 )
            still_logo_step = -3;
        else if( logo_case == 18 )
            still_logo_step = -5;
        else if( logo_case == 19 )
            still_logo_step = -7;
        else if( logo_case == 20 )
            still_logo_step = -9;
        else
            still_logo_step = 0;


        if( ( ( ( i >= 8 ) && ( i <= 17 ) ) || i == 22 || i == 23 ) && (still_logo_step > 0) )
        {
            still_logo_step = ( still_logo_step / 4 );
        }

        still_logo_step_rgn[i] = still_logo_step;
        logo_case_rgn[i] = logo_case;
        pOutput->long_term_logo_counter[i] = _CLIP_( ( temporal_counter + still_logo_step ), 0, 1023 );
        
    }


    for(i=0;i<32;i++)
    {
        logo_detect_rgcnt_pre[i] = logo_detect_rgcnt[i];
        zmv_rg_cnt_pre[i] = zmv_rg_cnt[i];
    }

    if (print_in_func == 1)
    {
        for(i=0;i<32;i++)
        {            
            rtd_pr_memc_emerg("//********** logo_cnt[%d] = %d **********//\n", i, logo_detect_rgcnt[i]);
            rtd_pr_memc_emerg("//********** zmv_cnt[%d] = %d **********//\n", i, zmv_rg_cnt[i]);
            rtd_pr_memc_emerg("//********** logo_term[%d] = %d **********//\n", i, pOutput->long_term_logo_counter[i]);
        }
    }
}


VOID logo_counter_fw(const _PARAM_LOGO_CTRL *pParam, _OUTPUT_LOGO_CTRL *pOutput)
{
    const _PQLCONTEXT *s_pContext = GetPQLContext();    

    int gmv_abs;
    static int  input_frame_idx = 0;


    int zmv_rg_cnt_sum;

    int gmv_th = 4;
    int i;
    int long_term_score_bottom_sum = 0, long_term_score_bottom_avg = 0;
    int dtl_bottom_sum = 0, dtl_bottom_avg = 0;
    int prd_bottom_sum = 0, prd_bottom_avg = 0;

    int hammock_cond_longterm_score_th = 100, hammock_cond_dtl_th = 700, hammock_cond_prd_th = 100;
    bool hammock_cond_longterm = FALSE, hammock_cond_dtl = FALSE, hammock_cond_prd = FALSE;
    unsigned int print_in_func;
    
    ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
    if (print_in_func == 1)
    {
        rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
    }
    gmv_abs = _ABS_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb) + _ABS_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb);


    zmv_rg_cnt_sum = 0;
    for(i=0;i<32;i++)
    {
        zmv_rg_cnt_sum = zmv_rg_cnt_sum + s_pContext->_output_read_comreg.u10_me_bv_stats_rb[i];
    }
    if( zmv_rg_cnt_sum >= (240*135*8*4*80/100) )
        pOutput->zmv_cnt_sum_cond = TRUE;
    else
        pOutput->zmv_cnt_sum_cond = FALSE;

    if(gmv_abs <= gmv_th)
        pOutput->small_gmv_cond = TRUE;
        else
        pOutput->small_gmv_cond = FALSE;

    for(i=0;i<32;i++)
        {

        if( i >= 16 )
    {
            long_term_score_bottom_sum = long_term_score_bottom_sum + pOutput->long_term_logo_counter[i];
            dtl_bottom_sum = dtl_bottom_sum + s_pContext->_output_read_comreg.u20_me_rgn_dtl_rb[i];
            prd_bottom_sum = prd_bottom_sum + s_pContext->_output_read_comreg.u13_me_rPRD_rb[i];
    }

    }
    long_term_score_bottom_avg = ( long_term_score_bottom_sum / 16 );
    dtl_bottom_avg = ( dtl_bottom_sum / 16 );
    prd_bottom_avg = ( prd_bottom_sum / 16 );

    hammock_cond_longterm = ( long_term_score_bottom_avg < hammock_cond_longterm_score_th ) ? TRUE : FALSE;
    hammock_cond_dtl = ( dtl_bottom_avg > hammock_cond_dtl_th ) ? TRUE : FALSE;
    hammock_cond_prd = ( prd_bottom_avg > hammock_cond_prd_th ) ? TRUE : FALSE;

    pOutput->hammock_condition = ( hammock_cond_longterm && hammock_cond_dtl && hammock_cond_prd );
    
    if(input_frame_idx % 3 == 0)
        pOutput->input_frame_cond = TRUE;
    else
        pOutput->input_frame_cond = FALSE;
    
    input_frame_idx = (input_frame_idx + 1);


}


VOID dehalo_logo_8x4_region_setting(const _PARAM_LOGO_CTRL *pParam, _OUTPUT_LOGO_CTRL *pOutput)
{
    int long_term_logo_counter[32];

    int long_term_th = 500;
    int i;
    unsigned int print_in_func;
    
    ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
    if (print_in_func == 1)
    {
        rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
    }
    for(i=0;i<32;i++)
    {
        long_term_logo_counter[i] = pOutput->long_term_logo_counter[i];
    }

    // default value for dehalo logo boundary
    for(i=0; i<7; i++)
    {
        pOutput->dh_logo_boundary_x[i] = 30 * (i+1);
    }

    // pattern 389
    if( (long_term_logo_counter[0] > long_term_th) && (long_term_logo_counter[5] > long_term_th) && (long_term_logo_counter[6] > long_term_th) && (long_term_logo_counter[7] > long_term_th) &&
        (long_term_logo_counter[26] > long_term_th) && (long_term_logo_counter[27] > long_term_th) && (long_term_logo_counter[28] > long_term_th) && (long_term_logo_counter[29] > long_term_th))
    {
        pOutput->dh_logo_boundary_x[0] = 40;
    }
    // pattern 198
    else if( (long_term_logo_counter[0] > long_term_th) && (long_term_logo_counter[1] < long_term_th) && (long_term_logo_counter[6] < long_term_th) && (long_term_logo_counter[7] > long_term_th) &&
            (long_term_logo_counter[24] < long_term_th) && (long_term_logo_counter[25] < long_term_th) && (long_term_logo_counter[30] > long_term_th) && (long_term_logo_counter[31] > long_term_th))
    {
        pOutput->dh_logo_boundary_x[0] = 40;
    }
    else
    {
        pOutput->dh_logo_boundary_x[0] = 30;
    }

    // pattern 315 and 181
    if( (long_term_logo_counter[0] > long_term_th) && (long_term_logo_counter[1] > long_term_th) && (long_term_logo_counter[6] > long_term_th) && (long_term_logo_counter[7] > long_term_th) &&
        (long_term_logo_counter[26] > long_term_th) )
    {
        pOutput->dh_logo_boundary_x[1] = 67;
    }
    // pattern 166
    else if( (long_term_logo_counter[24] > long_term_th) && (long_term_logo_counter[25] > long_term_th) && (long_term_logo_counter[30] > long_term_th) && (long_term_logo_counter[31] > long_term_th) )
    {
        pOutput->dh_logo_boundary_x[1] = 65;
    }
    else
    {
        pOutput->dh_logo_boundary_x[1] = 60;
    }
    
    // pattern 165
    if( (long_term_logo_counter[0] < long_term_th) && (long_term_logo_counter[1] < long_term_th) && (long_term_logo_counter[6] < long_term_th) && (long_term_logo_counter[7] < long_term_th) &&
        (long_term_logo_counter[24] < long_term_th) && (long_term_logo_counter[25] < long_term_th) && (long_term_logo_counter[30] < long_term_th) && (long_term_logo_counter[31] > long_term_th))
    {
        pOutput->dh_logo_boundary_x[6] = 206;
    }
    // pattern 186
    else if( (long_term_logo_counter[0] > long_term_th) && (long_term_logo_counter[1] > long_term_th) && (long_term_logo_counter[6] < long_term_th) && (long_term_logo_counter[7] < long_term_th) &&
        (long_term_logo_counter[24] > long_term_th) && (long_term_logo_counter[25] < long_term_th) && (long_term_logo_counter[30] < long_term_th) && (long_term_logo_counter[31] > long_term_th))
    {
        pOutput->dh_logo_boundary_x[6] = 202;
    }
    else
    {
        pOutput->dh_logo_boundary_x[6] = 210;
    }
    
    // pattern 474
    if( (long_term_logo_counter[0] > long_term_th) && (long_term_logo_counter[1] > long_term_th) && (long_term_logo_counter[6] > long_term_th) && (long_term_logo_counter[7] > long_term_th) &&
        (long_term_logo_counter[24] < long_term_th) && (long_term_logo_counter[25] < long_term_th) && (long_term_logo_counter[30] < long_term_th) && (long_term_logo_counter[31] < long_term_th))
    {
        pOutput->dh_logo_boundary_x[5] = 175;
    }
    else
    {
        pOutput->dh_logo_boundary_x[5] = 180;
    }


}

VOID dehalo_logo_fw(const _PARAM_LOGO_CTRL *pParam, _OUTPUT_LOGO_CTRL *pOutput)
{
    const _PQLCONTEXT *s_pContext = GetPQLContext();

    static int scene_change_frame_count = 60;

    int long_term_logo_counter[32];
    int long_term_logo_sum = 0;
    int long_term_logo_sum_th = 1024;

    // cond1 and cond2
    int dh_logo_mv_th = 4, dh_logo_mv_th_weak = 8, dh_logo_mv_th_strong = 3;
    int dh_logo_sad_mvd_th = 255, dh_logo_sad_mvd_th_weak = 200, dh_logo_sad_mvd_th_strong = 512;
    int dh_logo_sad_ofs = 80, dh_logo_sad_ofs_weak = 60, dh_logo_sad_ofs_strong = 160;

    // cond3 and cond4
    int zmv_cnt_th, mvd_3x3_cnt_th, mvd_3x3_th, large_mv_cnt_thr, zmv_cnt_diff_th, var_cnt_th;
    int gmv_grp0, max_rmv_thr;
    int rmv_thr[32];
    
    int i;
    unsigned int print_in_func;
    
    ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
    if (print_in_func == 1)
    {
        rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
    }
    for(i=0;i<32;i++)
    {
        long_term_logo_counter[i] = pOutput->long_term_logo_counter[i];
        long_term_logo_sum = long_term_logo_sum + pOutput->long_term_logo_counter[i];

        if (print_in_func == 1)
        {
            rtd_pr_memc_emerg("//********** dehalo_logo_logoterm[%d] = %d %d **********//\n", i, long_term_logo_counter[i], pOutput->long_term_logo_counter[i]);
        }        
    }

    // scene change count
    if( (s_pContext->_output_read_comreg.u32_me_sc_pattern_rb == 1) && (long_term_logo_sum < long_term_logo_sum_th) )
    {
        scene_change_frame_count = 60;
    }
    else
    {
        scene_change_frame_count = _CLIP_( (scene_change_frame_count - 1), 0, 255);
    }

    // calculate gmv_grp0 and rmv_thr[32]
    gmv_grp0 = _ABS_(s_pContext->_output_read_comreg.s11_me_GMV_1st_vx_rb) + _ABS_(s_pContext->_output_read_comreg.s10_me_GMV_1st_vy_rb);
    for(i=0;i<32;i++)
    { 
        rmv_thr[i] = ( _ABS_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[i]) + _ABS_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[i]) );
    }

    // set reg_dh_logo_max_mv_th
    pOutput->dh_logo_max_mv_th = _CLIP_(gmv_grp0 / 5, 10, 16);

    for(i=0;i<32;i++)
    {
        // calculate max_rmv_thr
        max_rmv_thr = rmv_thr[i];    // rmv_thr[j, i]    

        if( scene_change_frame_count > 0 )
        {
            dh_logo_mv_th = 4;
            dh_logo_sad_mvd_th = 255;
            dh_logo_sad_ofs = 80;
        }
        else
        {
            // dehalo logo cond1 and cond2
            if( long_term_logo_counter[i] >= 700 )
            {
                dh_logo_mv_th = dh_logo_mv_th_weak;
                dh_logo_sad_mvd_th = dh_logo_sad_mvd_th_weak;
                dh_logo_sad_ofs = dh_logo_sad_ofs_weak;                
            }
            else if( long_term_logo_counter[i] <= 300 )
            {
                dh_logo_mv_th = dh_logo_mv_th_strong;
                dh_logo_sad_mvd_th = dh_logo_sad_mvd_th_strong;
                dh_logo_sad_ofs = dh_logo_sad_ofs_strong;                
            }
            else
            {
                dh_logo_mv_th = ( long_term_logo_counter[i] * dh_logo_mv_th_weak ) + ( (1024 - long_term_logo_counter[i])* dh_logo_mv_th_strong ) / 1024;
                dh_logo_sad_mvd_th = ( long_term_logo_counter[i] * dh_logo_sad_mvd_th_weak ) + ( (1024 - long_term_logo_counter[i])* dh_logo_sad_mvd_th_strong ) / 1024;
                dh_logo_sad_ofs = ( long_term_logo_counter[i] * dh_logo_sad_ofs_weak ) + ( (1024 - long_term_logo_counter[i])* dh_logo_sad_ofs_strong ) / 1024;
            }
            
        }
            
        // dehalo logo cond3 and cond4        
        if( (scene_change_frame_count > 0) || (long_term_logo_counter[i] > 300) )
        {
            zmv_cnt_th = _CLIP_UBOUND_( ( 8 - (max_rmv_thr / 8) ), 4 );
            mvd_3x3_cnt_th = 8;
            mvd_3x3_th = ( _CLIP_UBOUND_( (max_rmv_thr / 2), 8) / 4 );
            large_mv_cnt_thr = (35/8);
            zmv_cnt_diff_th = _CLIP_( ( (max_rmv_thr * 2) / (32*16) ), 8, 31) ;
            var_cnt_th = 5;
        }
        else
        {
            zmv_cnt_th = _CLIP_UBOUND_( ( 16 - (max_rmv_thr / 8) ), 8 );
            mvd_3x3_cnt_th = 2;
            mvd_3x3_th = ( _CLIP_UBOUND_( (max_rmv_thr / 4), 2) / 4 );
            large_mv_cnt_thr = (35/4);
            zmv_cnt_diff_th = _CLIP_( ( (max_rmv_thr * 2) / (32*32) ), 1, 31);
            var_cnt_th = 9;        
        }


        if(pParam->u1_logo_dehalo_logo_mode == 1) // dehalo logo force strong
        {
            dh_logo_mv_th = dh_logo_mv_th_strong;
            dh_logo_sad_mvd_th = dh_logo_sad_mvd_th_strong;
            dh_logo_sad_ofs = dh_logo_sad_ofs_strong;         
        }
        else if(pParam->u1_logo_dehalo_logo_mode == 2) // dehalo logo force weak
        {
            dh_logo_mv_th = dh_logo_mv_th_weak;
            dh_logo_sad_mvd_th = dh_logo_sad_mvd_th_weak;
            dh_logo_sad_ofs = dh_logo_sad_ofs_weak;
        }

        pOutput->dh_logo_sad_ofs[i] = dh_logo_sad_ofs;
        pOutput->dh_logo_sad_mvd_th[i] = dh_logo_sad_mvd_th;
        pOutput->dh_logo_mv_th[i] = dh_logo_mv_th;
        
        pOutput->dh_logo_zmv_cnt_th[i] = zmv_cnt_th;
        pOutput->dh_logo_mvd_3x3_cnt_th[i] = mvd_3x3_cnt_th;
        pOutput->dh_logo_mvd_3x3_th[i] = mvd_3x3_th;
        pOutput->dh_logo_large_mv_cnt_th[i] = large_mv_cnt_thr;
        pOutput->dh_logo_zmv_cnt_diff_th[i] = zmv_cnt_diff_th;
        pOutput->dh_logo_var_cnt_th[i] = var_cnt_th;


    }


}


VOID logo_bounding_box_fw(const _PARAM_LOGO_CTRL *pParam, _OUTPUT_LOGO_CTRL *pOutput)
{

	//static int long_term_logo_counter[32];

	//int min_dist_th = 20;

	// get logo flag from ddr
	//bool logo_flag_array[960][540];
	//bool logo_invalid_array[960][540];

	//search_range_whole_frame = 

	//bounding_box_whole_frame

	
}


VOID LogoCtrl_Proc_RTK2885pp(const _PARAM_LOGO_CTRL *pParam, _OUTPUT_LOGO_CTRL *pOutput)
{
    unsigned int print_in_func;
    ReadRegister(SOFTWARE3_SOFTWARE3_04_reg, 0, 0, &print_in_func);
    if (print_in_func == 1)
    {
        rtd_pr_memc_emerg("//********** [MEMC Check in func][%s][%d] **********//\n", __FUNCTION__, __LINE__);
    }
    if (pParam->u1_logo_pql_en == 1)
    {
        //const _PQLCONTEXT *s_pContext        = GetPQLContext();
        pOutput->u32_blklogo_clr_idx        = 0;

        Logo_SC_Detect(pParam, pOutput);
        LogoCtrl_Sobel_Proc(pParam, pOutput);

        if(pParam->u1_logo_longterm_logo_en == 1)
        {
            long_term_logo_fw(pParam, pOutput);
        }
        if(pParam->u1_logo_dehalo_logo_en == 1)
        {
            dehalo_logo_fw(pParam, pOutput);
        }
        if(pParam->u1_logo_dehalo_logo_boundary_en == 1)
        {
            dehalo_logo_8x4_region_setting(pParam, pOutput);      
        }
        if(pParam->u1_logo_counter_en == 1)
        {
            logo_counter_fw(pParam, pOutput);
        }
       
    }
    else
    {
        // pOutput->u1_sw_clr_en_out = 1;
    }

}


