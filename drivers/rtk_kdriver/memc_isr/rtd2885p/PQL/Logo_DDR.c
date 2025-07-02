#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
#include "memc_isr/PQL/PQLGlobalDefs.h"
#include "memc_isr/PQL/FRC_LogoDetect.h"
#include "memc_reg_def.h"
#include "memc_isr/PQL/LogoCtrl_extend.h"

//#define LOGO_EXPAND_DIST		(1)
#define LOGO_DDR_RIM_SHIFT	(2)
#define BLK_480_LOGO_SHIFT (3) //480x270->3840x2160
#define BLK_240_LOGO_SHIFT (4) //240x135->3840x2160
#define LOGO_DDR_DHLOGO_SHIFT	(1)
#define LOGO_DDR_REPLACEMV_SHIFT	(6)
#define LOGO_AREA_NUM				(10)
#define AREA_STABLE_CNT_MAX			(100)
#define AREA_STABLE_CNT_TH			(30)
#define SHOW_CENTER					(1)

const unsigned char LOGO_MIN_LENGTH = 96; //4k:12*8, block: 8x8(px)
unsigned int LOGO_MIN_LENGTH_480 = 11;//(96/8);
unsigned int MIN_LOGO_AREA_480 = 280;

//original logo
extern unsigned char logo_flag_real_480_map[LOGO_DDR_W * LOGO_DDR_H];
extern unsigned char LOGO_480_DDR_En;
extern unsigned int Get_Time_ms(void);
extern unsigned int Get_Time_us(void);

unsigned char logo_flag_array[LOGO_DDR_W][LOGO_DDR_H];
unsigned char logo_flag_480_raw[LOGO_DDR_W][LOGO_DDR_H];
unsigned char logo_iir_array[LOGO_DDR_W][LOGO_DDR_H] = {0};
unsigned char logo_same_array[LOGO_DDR_W][LOGO_DDR_H] = {0};
unsigned char logo_same_index = 1;

bool logo_invalid[LOGO_DDR_W][LOGO_DDR_H] = {0};
bool log_specified = false;//d9e8[14]
unsigned char g_frame_logo_id = 0; //LOGO_POSITION_NUM
bool g_Updat_DDR_Raw_Data = false;
bool g_Update_Logo_DDR = false;
bool g_logo_ddr_replace_mv_en = false;

MEMC_Logo_Boundary Whole_Search_Range = {0, 0, 0, 0};

MEMC_Logo_Boundary_Info SEARCH_BOUNDARY_LIMIT[LOGO_POSITION_NUM] = 
{
	{  0, 169,   0,  67}, //LOGO_LEFT_UP			{  0, 149,   0,  67},
	{310, 479,   0,  67}, //LOGO_RIGHT_UP			{330, 479,   0,  67},
	{  0, 179, 200, 269}, //LOGO_LEFT_LOW			{  0, 149, 200, 269},
	{300, 479, 200, 269}, //LOGO_RIGHT_LOW			{330, 479, 200, 269},
//	{ 35, 444, 200, 269}, //LOGO_CENTER_LOW			{ 35, 444, 200, 269},
};
	
MEMC_Logo_Boundary SEARCH_BOUNDARY[LOGO_POSITION_NUM] = 
{
	{  0, 169,   0,  67}, //LOGO_LEFT_UP			{  0, 149,   0,  67},
	{310, 479,   0,  67}, //LOGO_RIGHT_UP			{330, 479,   0,  67},
	{  0, 179, 200, 269}, //LOGO_LEFT_LOW			{  0, 149, 200, 269},
	{300, 479, 200, 269}, //LOGO_RIGHT_LOW			{330, 479, 200, 269},
//	{ 35, 444, 200, 269}, //LOGO_CENTER_LOW			{ 35, 444, 200, 269},
};

unsigned short UNCONF_LOGO_AREA[LOGO_POSITION_NUM] = 
{
	0, 0, 0, 0,// 0,
};

const bool LOGO_DDR_CAD_UPDATE_TABLE[_FRC_CADENCE_NUM_][25]=
{
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//_CAD_VIDEO,
	{1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//_CAD_22,	
	{1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//_CAD_32,	
	{1,0,0,1,0,1,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//_CAD_32322, 
	{1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//_CAD_334, 
	{1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//_CAD_22224, 
	{1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//_CAD_2224,	
	{1,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//_CAD_3223,	
	{1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//_CAD_55,	
	{1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//_CAD_66,	
	{1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//_CAD_44,	
	{1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//_CAD_1112,	
	{1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//_CAD_11112, 
	{1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//_CAD_122, 
	{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0},//_CAD_11i23,

};

const bool LOGO_DDR_CAD_UPDATE_TABLE_LENGTH[_FRC_CADENCE_NUM_]=
{
	1,	//_CAD_VIDEO,
	2,	//_CAD_22,	
	5,	//_CAD_32,	
	12,	//_CAD_32322, 
	10,	//_CAD_334,	
	12,	//_CAD_22224, 
	10,	//_CAD_2224, 
	10,	//_CAD_3223, 
	5,	//_CAD_55,	
	6,	//_CAD_66,	
	4,	//_CAD_44,	
	5,	//_CAD_1112,	
	6,	//_CAD_11112, 
	5,	//_CAD_122,	
	25,	//_CAD_11i23,
};

bool Logo_DDR_Get_update_raw_data_Flg_RTK2885p(void)
{
	return g_Updat_DDR_Raw_Data;
}

void Logo_DDR_Set_update_flag_RTK2885p(bool flag_en)
{
	g_Update_Logo_DDR = flag_en;
}

bool Logo_DDR_Get_update_flag_RTK2885p(void)
{
	return g_Update_Logo_DDR;
}

void Logo_DDR_update_RTK2885p(void)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	static unsigned char u8_id = 0, u8_cad_pre = _CAD_VIDEO;
	unsigned char u8_cad_cur = s_pContext->_output_filmDetectctrl.u8_cur_cadence_out[_FILM_ALL];

	if(u8_cad_cur == u8_cad_pre){
		u8_id = (u8_id+1>=LOGO_DDR_CAD_UPDATE_TABLE_LENGTH[u8_cad_cur]) ? 0 : u8_id+1;
	}else{
		u8_id = 0;
		u8_cad_pre = u8_cad_cur;
	}

	g_Updat_DDR_Raw_Data = LOGO_DDR_CAD_UPDATE_TABLE[u8_cad_cur][u8_id];
}

unsigned short Get_logo_cnt_sum_RTK2885p(MEMC_Logo_Boundary_Info Boundary)
{
	unsigned short u16_x, u16_y, u16_logo_cnt_sum = 0;

	for(u16_y=Boundary.u16_start_y; u16_y<=Boundary.u16_end_y; u16_y++){
		for(u16_x=Boundary.u16_start_x ; u16_x<=Boundary.u16_end_x; u16_x++){
			u16_logo_cnt_sum = (logo_flag_array[u16_x][u16_y]==1) ? u16_logo_cnt_sum+1 : u16_logo_cnt_sum;
		}
	}
	return u16_logo_cnt_sum;
}

bool Logo_DDR_Match_SearchBoundary_RTK2885p(MEMC_Logo_Boundary Search_Boundary, MEMC_Logo_Boundary_Info Boundary)
//check_rim
{
	if(Boundary.u16_start_x != Search_Boundary.u16_start_x){
		return false;
	}
	if(Boundary.u16_start_y != Search_Boundary.u16_start_y){
		return false;
	}
	if(Boundary.u16_end_x != Search_Boundary.u16_end_x){
		return false;
	}
	if(Boundary.u16_end_y != Search_Boundary.u16_end_y){
		return false;
	}
	return true;
}

bool Logo_DDR_WH_Ratio_Trust_RTK2885p(MEMC_Logo_Boundary_Info Boundary)
{
	unsigned short u12_width = Boundary.u16_end_x - Boundary.u16_start_x;
	unsigned short u12_height = Boundary.u16_end_y - Boundary.u16_start_y;

	if(u12_height > u12_width*2){
		return false;
	}
	if(u12_height > u12_height*10){
		return false;
	}
	return true;
}

unsigned char Logo_DDR_AreaMatchRatio_RTK2885p(MEMC_Logo_Boundary_Info new_Boundary, MEMC_Logo_Boundary_Info ori_Boundary)
{
	unsigned char u8_ratio = 0;
	unsigned int u17_area_new_boundary = (new_Boundary.u16_end_x-new_Boundary.u16_start_x)*(new_Boundary.u16_end_y-new_Boundary.u16_start_y);
	unsigned int u17_area_ori_boundary = (ori_Boundary.u16_end_x-ori_Boundary.u16_start_x)*(ori_Boundary.u16_end_y-ori_Boundary.u16_start_y);
	unsigned int u17_area_overlap = 0;
	unsigned short overlap[4] = {
		_MAX_(new_Boundary.u16_start_x, ori_Boundary.u16_start_x),
		_MAX_(new_Boundary.u16_start_y, ori_Boundary.u16_start_y),
		_MIN_(new_Boundary.u16_end_x, ori_Boundary.u16_end_x),
		_MIN_(new_Boundary.u16_end_y, ori_Boundary.u16_end_y),
	};

	if(u17_area_new_boundary==0 || u17_area_ori_boundary==0){
		u8_ratio = 0;
	}
	else{
		u17_area_overlap = (overlap[2]>overlap[0] && overlap[3]>overlap[1]) ? (overlap[2]-overlap[0])*(overlap[3]-overlap[1]) : 0;
		u8_ratio = u17_area_overlap*200/(u17_area_new_boundary+u17_area_ori_boundary);//unit:%
	}

	//if(log_specified){
	if(0){
		rtd_pr_memc_emerg("[%s] %d,%u,%u, overlap,%d,%d,%d,%d, n,%d,%d,%d,%d, o,%d,%d,%d,%d\n", __FUNCTION__, 
			u8_ratio, u17_area_overlap, (u17_area_new_boundary+u17_area_ori_boundary),
			overlap[0],overlap[1],overlap[2],overlap[3],
			new_Boundary.u16_start_x, new_Boundary.u16_start_y, new_Boundary.u16_end_x, new_Boundary.u16_end_y,
			ori_Boundary.u16_start_x, ori_Boundary.u16_start_y, ori_Boundary.u16_end_x, ori_Boundary.u16_end_y);
	}

	return u8_ratio;
}

bool Logo_DDR_tounch_inside_boundary_RTK2885p(MEMC_Logo_Boundary Search_Boundary, MEMC_Logo_Boundary_Info Boundary_cur, unsigned char boundary_id)
//check_rim
{
	unsigned int u1_check_touch;

	ReadRegister(SOFTWARE2_SOFTWARE2_33_reg, 5, 5, &u1_check_touch);

	if(!u1_check_touch){
		return false;
	}
	
	if(boundary_id==LOGO_LEFT_UP || boundary_id==LOGO_LEFT_LOW /*|| boundary_id==LOGO_CENTER_LOW*/){
		if(Boundary_cur.u16_end_x==Search_Boundary.u16_end_x){
			return true;
		}
	}
	else if(boundary_id==LOGO_RIGHT_UP || boundary_id==LOGO_RIGHT_LOW /*|| boundary_id==LOGO_CENTER_LOW*/){
		if(Boundary_cur.u16_start_x==Search_Boundary.u16_start_x){
			return true;
		}
	}

	if(boundary_id==LOGO_LEFT_UP || boundary_id==LOGO_RIGHT_UP){
		if(Boundary_cur.u16_end_y==Search_Boundary.u16_end_y){
			return true;
		}
	}
	else if(boundary_id==LOGO_LEFT_LOW || boundary_id==LOGO_RIGHT_LOW /*|| boundary_id==LOGO_CENTER_LOW*/){
		if(Boundary_cur.u16_start_y==Search_Boundary.u16_start_y){
			return true;
		}
	}

	return false;
}

bool Logo_DDR_Match_WH_Ratio_RTK2885p(MEMC_Logo_Boundary_Info Boundary_cur)
{
	const unsigned short ratio = 18; //#430: 18
	unsigned short width = Boundary_cur.u16_end_x - Boundary_cur.u16_start_x;
	unsigned short height = Boundary_cur.u16_end_y - Boundary_cur.u16_start_y;
	unsigned int u1_check_WH_Ratio;
	
	ReadRegister(SOFTWARE2_SOFTWARE2_33_reg, 2, 2, &u1_check_WH_Ratio);

	if(!u1_check_WH_Ratio){
		return true;
	}

	if(width*ratio < height){
		return false;
	}
	if(height*ratio < width){
		return false;
	}
	return true;
}

void Logo_DDR_expand_RTK2885p(MEMC_Logo_Boundary_Info *Boundary_Cur, unsigned char u8_expand_dist)
{
	Boundary_Cur->u16_start_x = (Boundary_Cur->u16_start_x > u8_expand_dist) ? Boundary_Cur->u16_start_x-u8_expand_dist : 0;
	Boundary_Cur->u16_start_y = (Boundary_Cur->u16_start_y > u8_expand_dist) ? Boundary_Cur->u16_start_y-u8_expand_dist : 0;
	
	Boundary_Cur->u16_end_x = (Boundary_Cur->u16_end_x < LOGO_DDR_W-u8_expand_dist) ? Boundary_Cur->u16_end_x+u8_expand_dist : LOGO_DDR_W;
	Boundary_Cur->u16_end_y = (Boundary_Cur->u16_end_y < LOGO_DDR_H-u8_expand_dist) ? Boundary_Cur->u16_end_y+u8_expand_dist : LOGO_DDR_H;
}

bool inBoundary_RTK2885p(MEMC_Logo_Boundary_Info Boundary_samll, MEMC_Logo_Boundary_Info Boundary_large)
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

void Logo_DDR_ShowLogoFlg_RTK2885p(void)
{
	unsigned short u16_i;
	unsigned char u8_j;
	unsigned int print_logo_start_y, print_logo_end_y;
	unsigned int show_en;
	
	ReadRegister(SOFTWARE2_SOFTWARE2_34_reg, 31, 31, &show_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_37_reg,  0,  8, &print_logo_start_y);
	ReadRegister(SOFTWARE2_SOFTWARE2_37_reg,  9, 17, &print_logo_end_y);

	if(show_en){
		// check print logo y range
		print_logo_start_y = _MIN_(print_logo_start_y, LOGO_DDR_H-1);
		print_logo_end_y = _MIN_(print_logo_end_y, LOGO_DDR_H-1);
		
		for(u16_i=print_logo_start_y; u16_i<print_logo_end_y; u16_i++ )
		{
			for(u8_j=0; u8_j<6; u8_j++ )
			{
				rtd_pr_memc_info("DDRL %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,re\n", 
					logo_flag_array[(0+u8_j*80)][u16_i],  logo_flag_array[(1+u8_j*80)][u16_i],  logo_flag_array[(2+u8_j*80)][u16_i],  logo_flag_array[(3+u8_j*80)][u16_i],  logo_flag_array[(4+u8_j*80)][u16_i], 
					logo_flag_array[(5+u8_j*80)][u16_i],  logo_flag_array[(6+u8_j*80)][u16_i],  logo_flag_array[(7+u8_j*80)][u16_i],  logo_flag_array[(8+u8_j*80)][u16_i],  logo_flag_array[(9+u8_j*80)][u16_i], 
					logo_flag_array[(10+u8_j*80)][u16_i], logo_flag_array[(11+u8_j*80)][u16_i], logo_flag_array[(12+u8_j*80)][u16_i], logo_flag_array[(13+u8_j*80)][u16_i], logo_flag_array[(14+u8_j*80)][u16_i], 
					logo_flag_array[(15+u8_j*80)][u16_i], logo_flag_array[(16+u8_j*80)][u16_i], logo_flag_array[(17+u8_j*80)][u16_i], logo_flag_array[(18+u8_j*80)][u16_i], logo_flag_array[(19+u8_j*80)][u16_i], 
					logo_flag_array[(20+u8_j*80)][u16_i], logo_flag_array[(21+u8_j*80)][u16_i], logo_flag_array[(22+u8_j*80)][u16_i], logo_flag_array[(23+u8_j*80)][u16_i], logo_flag_array[(24+u8_j*80)][u16_i], 
					logo_flag_array[(25+u8_j*80)][u16_i], logo_flag_array[(26+u8_j*80)][u16_i], logo_flag_array[(27+u8_j*80)][u16_i], logo_flag_array[(28+u8_j*80)][u16_i], logo_flag_array[(29+u8_j*80)][u16_i], 
					logo_flag_array[(30+u8_j*80)][u16_i], logo_flag_array[(31+u8_j*80)][u16_i], logo_flag_array[(32+u8_j*80)][u16_i], logo_flag_array[(33+u8_j*80)][u16_i], logo_flag_array[(34+u8_j*80)][u16_i], 
					logo_flag_array[(35+u8_j*80)][u16_i], logo_flag_array[(36+u8_j*80)][u16_i], logo_flag_array[(37+u8_j*80)][u16_i], logo_flag_array[(38+u8_j*80)][u16_i], logo_flag_array[(39+u8_j*80)][u16_i], 
					logo_flag_array[(40+u8_j*80)][u16_i], logo_flag_array[(41+u8_j*80)][u16_i], logo_flag_array[(42+u8_j*80)][u16_i], logo_flag_array[(43+u8_j*80)][u16_i], logo_flag_array[(44+u8_j*80)][u16_i], 
					logo_flag_array[(45+u8_j*80)][u16_i], logo_flag_array[(46+u8_j*80)][u16_i], logo_flag_array[(47+u8_j*80)][u16_i], logo_flag_array[(48+u8_j*80)][u16_i], logo_flag_array[(49+u8_j*80)][u16_i], 
					logo_flag_array[(50+u8_j*80)][u16_i], logo_flag_array[(51+u8_j*80)][u16_i], logo_flag_array[(52+u8_j*80)][u16_i], logo_flag_array[(53+u8_j*80)][u16_i], logo_flag_array[(54+u8_j*80)][u16_i], 
					logo_flag_array[(55+u8_j*80)][u16_i], logo_flag_array[(56+u8_j*80)][u16_i], logo_flag_array[(57+u8_j*80)][u16_i], logo_flag_array[(58+u8_j*80)][u16_i], logo_flag_array[(59+u8_j*80)][u16_i], 
					logo_flag_array[(60+u8_j*80)][u16_i], logo_flag_array[(61+u8_j*80)][u16_i], logo_flag_array[(62+u8_j*80)][u16_i], logo_flag_array[(63+u8_j*80)][u16_i], logo_flag_array[(64+u8_j*80)][u16_i], 
					logo_flag_array[(65+u8_j*80)][u16_i], logo_flag_array[(66+u8_j*80)][u16_i], logo_flag_array[(67+u8_j*80)][u16_i], logo_flag_array[(68+u8_j*80)][u16_i], logo_flag_array[(69+u8_j*80)][u16_i], 
					logo_flag_array[(70+u8_j*80)][u16_i], logo_flag_array[(71+u8_j*80)][u16_i], logo_flag_array[(72+u8_j*80)][u16_i], logo_flag_array[(73+u8_j*80)][u16_i], logo_flag_array[(74+u8_j*80)][u16_i], 
					logo_flag_array[(75+u8_j*80)][u16_i], logo_flag_array[(76+u8_j*80)][u16_i], logo_flag_array[(77+u8_j*80)][u16_i], logo_flag_array[(78+u8_j*80)][u16_i], logo_flag_array[(79+u8_j*80)][u16_i] );

			}

			rtd_pr_memc_info("DDRL \n");
		}
	
		WriteRegister(SOFTWARE2_SOFTWARE2_34_reg, 31, 31, 0);
	}
}

void Search_Logo_Boundary_in_H_RTK2885p(MEMC_Logo_Boundary Search_Boundary, MEMC_Logo_Boundary_Info *Boundary_Cur)
{
	unsigned short u16_x = 0, u16_y = Boundary_Cur->u16_end_y;
	short s16_x = 0;
	bool has_Logo = false;
	
	if(u16_y>Search_Boundary.u16_end_y){
		Boundary_Cur->u16_end_y = Search_Boundary.u16_end_y;
		return;
	}

	// search logo in next line, if no logo return function
	for(u16_x=Boundary_Cur->u16_start_x; u16_x<=Boundary_Cur->u16_end_x; u16_x++){
		if(logo_flag_array[u16_x][u16_y]==1 && logo_invalid[u16_x][u16_y]==0){
			logo_invalid[u16_x][u16_y] = 1;
			has_Logo = true;
		}
	}
	// no logo return function
	if(!has_Logo){
		return;
	}

	// try to find min left boundary by search logo in left side
	for(s16_x=Boundary_Cur->u16_start_x; s16_x>=Search_Boundary.u16_start_x; s16_x--){
		u16_x = (unsigned short)s16_x;
		// complete finding left boundary when there is no logo
		if(logo_flag_array[u16_x][u16_y]==0){
			break;
		}else{
			logo_invalid[u16_x][u16_y]=1;
		}
		// update the left boundary
		Boundary_Cur->u16_start_x = u16_x;
	}

	// try to find max right boundary by search logo in right side
	for(u16_x=Boundary_Cur->u16_end_x; u16_x<=Search_Boundary.u16_end_x; u16_x++){
		// complete finding right boundary when there is no logo
		if(logo_flag_array[u16_x][u16_y]==0){
			break;
		}else{
			logo_invalid[u16_x][u16_y]=1;
		}
		// update the right boundary
		Boundary_Cur->u16_end_x = u16_x;
	}	
	//rtd_pr_memc_crit("[ing] [%d] boudary,%d,%d,%d,%d\n", has_Logo, Logo_Boundary->u16_start_x, Logo_Boundary->u16_end_x, Logo_Boundary->u16_start_y, Logo_Boundary->u16_end_y);

	// update the bottom boundary
	Boundary_Cur->u16_end_y++;

	// try to search next line
	Search_Logo_Boundary_in_H_RTK2885p(Search_Boundary, Boundary_Cur);
}

MEMC_Logo_Boundary_Info Logo_DDR_FineSearchLogo_RTK2885p(MEMC_Logo_Boundary_Info Boundary_pre, MEMC_Logo_Boundary Search_Boundary, unsigned char boundary_id, unsigned char u8_stable_cnt)
{
	//const unsigned u8_stable_cnt_th = 20;
	MEMC_Logo_Boundary_Info Boundary_Before = {0};
	unsigned short u16_x, u16_y, u16_cnt_sum = 0, u16_logo_cnt_th_h_expand = 0, u16_logo_cnt_th_v_expand = 0, u16_logo_cnt_th_h_shrink = 0, u16_logo_cnt_th_v_shrink = 0, u16_logo_cnt_sum = 0;
	short s16_x, s16_y;
	bool u1_expaned = false;
	unsigned int logo_cnt_gain, u8_stable_cnt_th;
	
	ReadRegister(SOFTWARE2_SOFTWARE2_35_reg, 24, 30, &logo_cnt_gain);
	ReadRegister(SOFTWARE2_SOFTWARE2_33_reg, 24, 30, &u8_stable_cnt_th);

	if(u8_stable_cnt<u8_stable_cnt_th){
		return Boundary_pre;
	}

	// logo_cnt_gain will be smller when area more stable
	logo_cnt_gain = 100-((u8_stable_cnt-u8_stable_cnt_th)*(100-logo_cnt_gain)/(AREA_STABLE_CNT_MAX-u8_stable_cnt_th));
	
	u16_logo_cnt_sum = Get_logo_cnt_sum_RTK2885p(Boundary_pre);
	u16_logo_cnt_th_v_expand = (Boundary_pre.u16_end_x>Boundary_pre.u16_start_x) ? u16_logo_cnt_sum*logo_cnt_gain/(100*(Boundary_pre.u16_end_x-Boundary_pre.u16_start_x)) : u16_logo_cnt_sum;
	u16_logo_cnt_th_h_expand = (Boundary_pre.u16_end_y>Boundary_pre.u16_start_y) ? u16_logo_cnt_sum*logo_cnt_gain/(100*(Boundary_pre.u16_end_y-Boundary_pre.u16_start_y)) : u16_logo_cnt_sum;
	u16_logo_cnt_th_v_shrink = _MAX_(u16_logo_cnt_th_v_expand>>2, 1);
	u16_logo_cnt_th_h_shrink = _MAX_(u16_logo_cnt_th_h_expand>>2, 1);

	Boundary_Before.u16_start_x = Boundary_pre.u16_start_x;
	Boundary_Before.u16_start_y = Boundary_pre.u16_start_y;
	Boundary_Before.u16_end_x = Boundary_pre.u16_end_x;
	Boundary_Before.u16_end_y = Boundary_pre.u16_end_y;

	// check boundary whether is trusted or not when current boundary is wider than the previous boundary
	// ex: current left boundary(start_x)=15 and previous left boundary=20
	// set search line(x=19), and get logo_cnt in search line
	// if logo_cnt is small, system will set left boundary to 20 and finish.
	// if logo_cnt is large, system will try to search next line(x=18)

	// try to expand
	u1_expaned = false;
	for(s16_x=Boundary_Before.u16_start_x-1; s16_x>=Search_Boundary.u16_start_x; s16_x--){
		u16_x = (unsigned short)s16_x;
		u16_cnt_sum = 0;
		for(u16_y=Boundary_Before.u16_start_y; u16_y<=Boundary_Before.u16_end_y; u16_y++){
			u16_cnt_sum = (logo_flag_array[u16_x][u16_y]==1) ? u16_cnt_sum+1 : u16_cnt_sum;
		}
		if(u16_cnt_sum<u16_logo_cnt_th_v_expand){
			break;
		}
		Boundary_pre.u16_start_x = u16_x;
		u1_expaned = true;
	}
	// try to shrink
	if(u1_expaned==false){
		for(s16_x=Boundary_Before.u16_start_x+1; s16_x<=Search_Boundary.u16_end_x; s16_x++){
			u16_x = (unsigned short)s16_x;
			u16_cnt_sum = 0;
			for(u16_y=Boundary_Before.u16_start_y; u16_y<=Boundary_Before.u16_end_y; u16_y++){
				u16_cnt_sum = (logo_flag_array[u16_x][u16_y]==1) ? u16_cnt_sum+1 : u16_cnt_sum;
			}
			if(u16_cnt_sum>u16_logo_cnt_th_v_shrink){
				break;
			}
			Boundary_pre.u16_start_x = u16_x;
		}
	}

	u1_expaned = false;
	for(u16_x=Boundary_Before.u16_end_x+1; u16_x<=Search_Boundary.u16_end_x; u16_x++){
		u16_cnt_sum = 0;
		for(u16_y=Boundary_Before.u16_start_y; u16_y<=Boundary_Before.u16_end_y; u16_y++){
			u16_cnt_sum = (logo_flag_array[u16_x][u16_y]==1) ? u16_cnt_sum+1 : u16_cnt_sum;
		}
		if(u16_cnt_sum<u16_logo_cnt_th_v_expand){
			break;
		}
		Boundary_pre.u16_end_x = u16_x;
		u1_expaned = true;
	}
	if(u1_expaned==false){
		for(s16_x=Boundary_Before.u16_end_x-1; s16_x>=Search_Boundary.u16_start_x; s16_x--){
			u16_x = (unsigned short)s16_x;
			u16_cnt_sum = 0;
			for(u16_y=Boundary_Before.u16_start_y; u16_y<=Boundary_Before.u16_end_y; u16_y++){
				u16_cnt_sum = (logo_flag_array[u16_x][u16_y]==1) ? u16_cnt_sum+1 : u16_cnt_sum;
			}
			if(u16_cnt_sum>u16_logo_cnt_th_v_shrink){
				break;
			}
			Boundary_pre.u16_end_x = u16_x;
		}
	}

	u1_expaned = false;
	for(s16_y=Boundary_Before.u16_start_y-1; s16_y>=Search_Boundary.u16_start_y; s16_y--){
		u16_y = (unsigned short)s16_y;
		u16_cnt_sum = 0;
		for(u16_x=Boundary_Before.u16_start_x; u16_x<=Boundary_Before.u16_end_x; u16_x++){
			u16_cnt_sum = (logo_flag_array[u16_x][u16_y]==1) ? u16_cnt_sum+1 : u16_cnt_sum;
		}
		if(u16_cnt_sum<u16_logo_cnt_th_h_expand){
			break;
		}
		Boundary_pre.u16_start_y= u16_y;
		u1_expaned = true;
	}
	if(u1_expaned==false){
		for(s16_y=Boundary_Before.u16_start_y+1; s16_y<=Search_Boundary.u16_end_y; s16_y++){
			u16_y = (unsigned short)s16_y;
			u16_cnt_sum = 0;
			for(u16_x=Boundary_Before.u16_start_x; u16_x<=Boundary_Before.u16_end_x; u16_x++){
				u16_cnt_sum = (logo_flag_array[u16_x][u16_y]==1) ? u16_cnt_sum+1 : u16_cnt_sum;
			}
			if(u16_cnt_sum>u16_logo_cnt_th_h_shrink){
				break;
			}
			Boundary_pre.u16_start_y= u16_y;
		}
	}

	u1_expaned = false;
	for(u16_y=Boundary_Before.u16_end_y+1; u16_y<=Search_Boundary.u16_end_y; u16_y++){
		u16_cnt_sum = 0;
		for(u16_x=Boundary_Before.u16_start_x; u16_x<=Boundary_Before.u16_end_x; u16_x++){
			u16_cnt_sum = (logo_flag_array[u16_x][u16_y]==1) ? u16_cnt_sum+1 : u16_cnt_sum;
		}
		if(u16_cnt_sum<u16_logo_cnt_th_h_expand){
			break;
		}
		Boundary_pre.u16_end_y= u16_y;
		u1_expaned = true;
	}
	if(u1_expaned==false){
		for(s16_y=Boundary_Before.u16_end_y-1; s16_y>=Search_Boundary.u16_start_y; s16_y--){
			u16_y = (unsigned short)s16_y;
			u16_cnt_sum = 0;
			for(u16_x=Boundary_Before.u16_start_x; u16_x<=Boundary_Before.u16_end_x; u16_x++){
				u16_cnt_sum = (logo_flag_array[u16_x][u16_y]==1) ? u16_cnt_sum+1 : u16_cnt_sum;
			}
			if(u16_cnt_sum>u16_logo_cnt_th_h_shrink){
				break;
			}
			Boundary_pre.u16_end_y= u16_y;
		}
	}

#if 0
	u16_cnt_sum = 0;
	//u16_x = Boundary_Before.u16_start_x;
	u16_x = 418;
	//for(u16_y=Boundary_Before.u16_start_y; u16_y<=Boundary_Before.u16_end_y; u16_y++){
	for(u16_y=237; u16_y<=262; u16_y++){
		u16_cnt_sum = (logo_flag_array[u16_x][u16_y]==1) ? u16_cnt_sum+1 : u16_cnt_sum;
	}
#endif
	
	if(log_specified){
		rtd_pr_memc_crit("[Search_MAX_area_boundary_RTK2885p]  case,3,  before,%d,%d,%d,%d,  after,%d,%d,%d,%d, info,%d,%d,%d, x,%d,%d,%d\n",
			Boundary_Before.u16_start_x, Boundary_Before.u16_end_x, Boundary_Before.u16_start_y, Boundary_Before.u16_end_y, 
			Boundary_pre.u16_start_x, Boundary_pre.u16_end_x, Boundary_pre.u16_start_y, Boundary_pre.u16_end_y, 
			u1_expaned, u16_cnt_sum, u16_logo_cnt_th_v_expand, u16_x, Boundary_Before.u16_start_y, Boundary_Before.u16_end_y);
	}

	return Boundary_pre;
}

void Logo_DDR_Connect_RTK2885p(MEMC_Logo_Boundary_Info *MAX_Area_Logo_Boundary, MEMC_Logo_Boundary_Info *Boundary_list, unsigned char boundary_id)
{
	bool u1_get_min_dist = false;
	unsigned int algo_en, log_en, min_dist_th;
	unsigned char u8_i, u8_j, u8_k, min_dist_id = 0, min_dist_area_id = 0;
	unsigned short u16_min_dist = 0xffff, u16_dist[LOGO_AREA_NUM][8] = {0};

	ReadRegister(SOFTWARE2_SOFTWARE2_36_reg, 31, 31, &algo_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_36_reg, 30, 30, &log_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_36_reg, 24, 27, &min_dist_th);

	if(!algo_en){
		return;
	}

	if(log_en && boundary_id==g_frame_logo_id){//d984[30]
		//rtd_pr_memc_emerg("min_dist,%d,%d boundary,%d,%d,%d,%d\n", min_dist_id, u16_min_dist, 
		//	MAX_Area_Logo_Boundary->u16_start_x, MAX_Area_Logo_Boundary->u16_end_x, MAX_Area_Logo_Boundary->u16_start_y, MAX_Area_Logo_Boundary->u16_end_y);
		rtd_pr_memc_emerg("boundary,%d,%d,%d,%d,  %d,%d,%d,%d,  %d,%d,%d,%d,  %d,%d,%d,%d,  %d,%d,%d,%d,  %d,%d,%d,%d,  %d,%d,%d,%d,  %d,%d,%d,%d,  %d,%d,%d,%d,  %d,%d,%d,%d\n", 
			Boundary_list[0].u16_start_x, Boundary_list[0].u16_end_x, Boundary_list[0].u16_start_y, Boundary_list[0].u16_end_y, 
			Boundary_list[1].u16_start_x, Boundary_list[1].u16_end_x, Boundary_list[1].u16_start_y, Boundary_list[1].u16_end_y, 
			Boundary_list[2].u16_start_x, Boundary_list[2].u16_end_x, Boundary_list[2].u16_start_y, Boundary_list[2].u16_end_y, 
			Boundary_list[3].u16_start_x, Boundary_list[3].u16_end_x, Boundary_list[3].u16_start_y, Boundary_list[3].u16_end_y, 
			Boundary_list[4].u16_start_x, Boundary_list[4].u16_end_x, Boundary_list[4].u16_start_y, Boundary_list[4].u16_end_y, 
			Boundary_list[5].u16_start_x, Boundary_list[5].u16_end_x, Boundary_list[5].u16_start_y, Boundary_list[5].u16_end_y, 
			Boundary_list[6].u16_start_x, Boundary_list[6].u16_end_x, Boundary_list[6].u16_start_y, Boundary_list[6].u16_end_y, 
			Boundary_list[7].u16_start_x, Boundary_list[7].u16_end_x, Boundary_list[7].u16_start_y, Boundary_list[7].u16_end_y, 
			Boundary_list[8].u16_start_x, Boundary_list[8].u16_end_x, Boundary_list[8].u16_start_y, Boundary_list[8].u16_end_y, 
			Boundary_list[9].u16_start_x, Boundary_list[9].u16_end_x, Boundary_list[9].u16_start_y, Boundary_list[9].u16_end_y);
	}

	for(u8_k=0; u8_k<LOGO_AREA_NUM; u8_k++){
		u1_get_min_dist = false;
		u16_min_dist = 0xffff;
		// get the min dist of two logo area
		for(u8_i=1; u8_i<LOGO_AREA_NUM; u8_i++){
			if(Boundary_list[u8_i].u1_get_logo==true){
				
				u16_dist[u8_i][0] = _ABS_DIFF_(Boundary_list[0].u16_start_x, Boundary_list[u8_i].u16_end_x);
				u16_dist[u8_i][0] += (Boundary_list[0].u16_start_y <= Boundary_list[u8_i].u16_start_y && Boundary_list[0].u16_start_y >= Boundary_list[u8_i].u16_end_y) ? 0 : _MIN_(_ABS_DIFF_(Boundary_list[0].u16_start_y, Boundary_list[u8_i].u16_start_y), _ABS_DIFF_(Boundary_list[0].u16_start_y, Boundary_list[u8_i].u16_end_y)); //AB', AD'

				u16_dist[u8_i][1] = _ABS_DIFF_(Boundary_list[0].u16_start_y, Boundary_list[u8_i].u16_end_y);
				u16_dist[u8_i][1] += (Boundary_list[0].u16_start_x <= Boundary_list[u8_i].u16_start_x && Boundary_list[0].u16_start_x >= Boundary_list[u8_i].u16_end_x) ? 0 : _MIN_(_ABS_DIFF_(Boundary_list[0].u16_start_x, Boundary_list[u8_i].u16_start_x), _ABS_DIFF_(Boundary_list[0].u16_start_x, Boundary_list[u8_i].u16_end_x)); //AB', AD'

				u16_dist[u8_i][2] = _ABS_DIFF_(Boundary_list[0].u16_end_x, Boundary_list[u8_i].u16_start_x);
				u16_dist[u8_i][2] += (Boundary_list[0].u16_start_y <= Boundary_list[u8_i].u16_start_y && Boundary_list[0].u16_start_y >= Boundary_list[u8_i].u16_end_y) ? 0 : _MIN_(_ABS_DIFF_(Boundary_list[0].u16_start_y, Boundary_list[u8_i].u16_start_y), _ABS_DIFF_(Boundary_list[0].u16_start_y, Boundary_list[u8_i].u16_end_y)); //AB', AD'

				u16_dist[u8_i][3] = _ABS_DIFF_(Boundary_list[0].u16_start_y, Boundary_list[u8_i].u16_end_y);
				u16_dist[u8_i][3] += (Boundary_list[0].u16_end_x <= Boundary_list[u8_i].u16_start_x && Boundary_list[0].u16_end_x >= Boundary_list[u8_i].u16_end_x) ? 0 : _MIN_(_ABS_DIFF_(Boundary_list[0].u16_end_x, Boundary_list[u8_i].u16_start_x), _ABS_DIFF_(Boundary_list[0].u16_end_x, Boundary_list[u8_i].u16_end_x)); //AB', AD'

				u16_dist[u8_i][4] = _ABS_DIFF_(Boundary_list[0].u16_start_x, Boundary_list[u8_i].u16_end_x);
				u16_dist[u8_i][4] += (Boundary_list[0].u16_end_y <= Boundary_list[u8_i].u16_start_y && Boundary_list[0].u16_end_y >= Boundary_list[u8_i].u16_end_y) ? 0 : _MIN_(_ABS_DIFF_(Boundary_list[0].u16_end_y, Boundary_list[u8_i].u16_start_y), _ABS_DIFF_(Boundary_list[0].u16_end_y, Boundary_list[u8_i].u16_end_y)); //AB', AD'

				u16_dist[u8_i][5] = _ABS_DIFF_(Boundary_list[0].u16_end_y, Boundary_list[u8_i].u16_start_y);
				u16_dist[u8_i][5] += (Boundary_list[0].u16_start_x <= Boundary_list[u8_i].u16_start_x && Boundary_list[0].u16_start_x >= Boundary_list[u8_i].u16_end_x) ? 0 : _MIN_(_ABS_DIFF_(Boundary_list[0].u16_start_x, Boundary_list[u8_i].u16_start_x), _ABS_DIFF_(Boundary_list[0].u16_start_x, Boundary_list[u8_i].u16_end_x)); //AB', AD'

				u16_dist[u8_i][6] = _ABS_DIFF_(Boundary_list[0].u16_end_x, Boundary_list[u8_i].u16_start_x);
				u16_dist[u8_i][6] += (Boundary_list[0].u16_end_y <= Boundary_list[u8_i].u16_start_y && Boundary_list[0].u16_end_y >= Boundary_list[u8_i].u16_end_y) ? 0 : _MIN_(_ABS_DIFF_(Boundary_list[0].u16_end_y, Boundary_list[u8_i].u16_start_y), _ABS_DIFF_(Boundary_list[0].u16_end_y, Boundary_list[u8_i].u16_end_y)); //AB', AD'

				u16_dist[u8_i][7] = _ABS_DIFF_(Boundary_list[0].u16_end_y, Boundary_list[u8_i].u16_start_y);
				u16_dist[u8_i][7] += (Boundary_list[0].u16_end_x <= Boundary_list[u8_i].u16_start_x && Boundary_list[0].u16_end_x >= Boundary_list[u8_i].u16_end_x) ? 0 : _MIN_(_ABS_DIFF_(Boundary_list[0].u16_end_x, Boundary_list[u8_i].u16_start_x), _ABS_DIFF_(Boundary_list[0].u16_end_x, Boundary_list[u8_i].u16_end_x)); //AB', AD'

			//	u16_dist[u8_i][1] = _ABS_DIFF_(Boundary_list[0].u16_start_x, Boundary_list[u8_i].u16_start_x) + _ABS_DIFF_(Boundary_list[0].u16_start_y, Boundary_list[u8_i].u16_end_y);   //AC'
			//	u16_dist[u8_i][2] = _ABS_DIFF_(Boundary_list[0].u16_start_x, Boundary_list[u8_i].u16_end_x)   + _ABS_DIFF_(Boundary_list[0].u16_start_y, Boundary_list[u8_i].u16_end_y);   //AD'

			//	u16_dist[u8_i][3] = _ABS_DIFF_(Boundary_list[0].u16_end_x,   Boundary_list[u8_i].u16_start_x) + _ABS_DIFF_(Boundary_list[0].u16_start_y, Boundary_list[u8_i].u16_start_y); //BA'
			//	u16_dist[u8_i][4] = _ABS_DIFF_(Boundary_list[0].u16_end_x,   Boundary_list[u8_i].u16_start_x) + _ABS_DIFF_(Boundary_list[0].u16_start_y, Boundary_list[u8_i].u16_end_y);   //BC'
			//	u16_dist[u8_i][5] = _ABS_DIFF_(Boundary_list[0].u16_end_x,   Boundary_list[u8_i].u16_end_x)   + _ABS_DIFF_(Boundary_list[0].u16_start_y, Boundary_list[u8_i].u16_end_y);   //BD'

			//	u16_dist[u8_i][6] = _ABS_DIFF_(Boundary_list[0].u16_start_x, Boundary_list[u8_i].u16_start_x) + _ABS_DIFF_(Boundary_list[0].u16_end_y,   Boundary_list[u8_i].u16_start_y); //CA'
			//	u16_dist[u8_i][7] = _ABS_DIFF_(Boundary_list[0].u16_start_x, Boundary_list[u8_i].u16_end_x)   + _ABS_DIFF_(Boundary_list[0].u16_end_y,   Boundary_list[u8_i].u16_start_y); //CB'
			//	u16_dist[u8_i][8] = _ABS_DIFF_(Boundary_list[0].u16_start_x, Boundary_list[u8_i].u16_end_x)   + _ABS_DIFF_(Boundary_list[0].u16_end_y,   Boundary_list[u8_i].u16_end_y);   //CD'

			//	u16_dist[u8_i][9] = _ABS_DIFF_(Boundary_list[0].u16_end_x,   Boundary_list[u8_i].u16_start_x) + _ABS_DIFF_(Boundary_list[0].u16_end_y,   Boundary_list[u8_i].u16_start_y); //BA'
			//	u16_dist[u8_i][10]= _ABS_DIFF_(Boundary_list[0].u16_end_x,   Boundary_list[u8_i].u16_end_x)   + _ABS_DIFF_(Boundary_list[0].u16_end_y,   Boundary_list[u8_i].u16_start_y); //BC'
			//	u16_dist[u8_i][11]= _ABS_DIFF_(Boundary_list[0].u16_end_x,   Boundary_list[u8_i].u16_start_x) + _ABS_DIFF_(Boundary_list[0].u16_end_y,   Boundary_list[u8_i].u16_end_y);   //BD'

				for(u8_j=0; u8_j<8; u8_j++){
					if(u16_min_dist > u16_dist[u8_i][u8_j]){
						u16_min_dist = u16_dist[u8_i][u8_j];
						min_dist_id = u8_j;
						min_dist_area_id = u8_i;
						u1_get_min_dist = true;
					}
				}
			}
		}
		if(u1_get_min_dist==false){
			break;
		}

		if(u16_min_dist>min_dist_th){
			break;
		}

		// connct logo when distance between two logo is small
		#if 0
		if(min_dist_id==0){
			MAX_Area_Logo_Boundary->u16_end_x = MAX_Area_Logo_Boundary_list[min_dist_area_id].u16_end_x;
			MAX_Area_Logo_Boundary->u16_start_y = _MIN_(MAX_Area_Logo_Boundary->u16_start_y, MAX_Area_Logo_Boundary_list[min_dist_area_id].u16_start_y);
			MAX_Area_Logo_Boundary->u16_end_y = _MAX_(MAX_Area_Logo_Boundary->u16_end_y, MAX_Area_Logo_Boundary_list[min_dist_area_id].u16_end_y);
		}else if(min_dist_id==1){
			MAX_Area_Logo_Boundary->u16_start_x = MAX_Area_Logo_Boundary_list[min_dist_area_id].u16_start_x;
			MAX_Area_Logo_Boundary->u16_start_y = _MIN_(MAX_Area_Logo_Boundary->u16_start_y, MAX_Area_Logo_Boundary_list[min_dist_area_id].u16_start_y);
			MAX_Area_Logo_Boundary->u16_end_y = _MAX_(MAX_Area_Logo_Boundary->u16_end_y, MAX_Area_Logo_Boundary_list[min_dist_area_id].u16_end_y);
		}else if(min_dist_id==2){
			MAX_Area_Logo_Boundary->u16_end_y = MAX_Area_Logo_Boundary_list[min_dist_area_id].u16_end_y;
			MAX_Area_Logo_Boundary->u16_start_x = _MIN_(MAX_Area_Logo_Boundary->u16_start_x, MAX_Area_Logo_Boundary_list[min_dist_area_id].u16_start_x);
			MAX_Area_Logo_Boundary->u16_end_x = _MAX_(MAX_Area_Logo_Boundary->u16_end_x, MAX_Area_Logo_Boundary_list[min_dist_area_id].u16_end_x);
		}else if(min_dist_id==3){
			MAX_Area_Logo_Boundary->u16_start_y = MAX_Area_Logo_Boundary_list[min_dist_area_id].u16_start_y;
			MAX_Area_Logo_Boundary->u16_start_x = _MIN_(MAX_Area_Logo_Boundary->u16_start_x, MAX_Area_Logo_Boundary_list[min_dist_area_id].u16_start_x);
			MAX_Area_Logo_Boundary->u16_end_x = _MAX_(MAX_Area_Logo_Boundary->u16_end_x, MAX_Area_Logo_Boundary_list[min_dist_area_id].u16_end_x);
		}
		#endif
		
		Boundary_list[0].u16_start_x = _MIN_(Boundary_list[0].u16_start_x, Boundary_list[min_dist_area_id].u16_start_x);
		Boundary_list[0].u16_start_y = _MIN_(Boundary_list[0].u16_start_y, Boundary_list[min_dist_area_id].u16_start_y);
		Boundary_list[0].u16_end_x   = _MAX_(Boundary_list[0].u16_end_x,   Boundary_list[min_dist_area_id].u16_end_x);
		Boundary_list[0].u16_end_y   = _MAX_(Boundary_list[0].u16_end_y,   Boundary_list[min_dist_area_id].u16_end_y);
		
		Boundary_list[min_dist_area_id].u1_get_logo = false;
	}
	MAX_Area_Logo_Boundary->u16_start_x = Boundary_list[0].u16_start_x;
	MAX_Area_Logo_Boundary->u16_start_y = Boundary_list[0].u16_start_y;
	MAX_Area_Logo_Boundary->u16_end_x   = Boundary_list[0].u16_end_x;
	MAX_Area_Logo_Boundary->u16_end_y   = Boundary_list[0].u16_end_y;

	if(log_en && boundary_id==g_frame_logo_id){//d984[30]
		rtd_pr_memc_emerg("min_dist,%d,%d boundary,%d,%d,%d,%d\n", min_dist_id, u16_min_dist, 
			MAX_Area_Logo_Boundary->u16_start_x, MAX_Area_Logo_Boundary->u16_end_x, MAX_Area_Logo_Boundary->u16_start_y, MAX_Area_Logo_Boundary->u16_end_y);
	}
}

void HoldOn_Logo_Boundary_RTK2885p(MEMC_Logo_Boundary_Info Boundary_pre, MEMC_Logo_Boundary_Info *MAX_Area_Logo_Boundary_list, unsigned char boundary_id)
{
	//const _PQLCONTEXT *s_pContext = GetPQLContext();
	//unsigned char u8_hold_cnt  = s_pContext->_output_logo_ctrl.DDR_Logo_hold_cnt[boundary_id];
	//MEMC_Logo_Boundary_Info Boundary_pre = s_pContext->_output_logo_ctrl.DDR_Logo_hold_boundary[boundary_id];
	MEMC_Logo_Boundary_Info Boundary_tmp = {LOGO_DDR_W, 0, LOGO_DDR_H, 0};
	unsigned char u8_i = 0;
	unsigned int log_en, log_en2;

	ReadRegister(SOFTWARE2_SOFTWARE2_36_reg, 29, 29, &log_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_36_reg, 28, 28, &log_en2);
	Logo_DDR_expand_RTK2885p(&Boundary_pre, 2);

	for(u8_i=1; u8_i<LOGO_AREA_NUM; u8_i++){
		if(MAX_Area_Logo_Boundary_list[u8_i].u1_get_logo==false){
			continue;
		}
		Boundary_tmp.u16_start_x = _MIN_(MAX_Area_Logo_Boundary_list[0].u16_start_x, MAX_Area_Logo_Boundary_list[u8_i].u16_start_x);
		Boundary_tmp.u16_start_y = _MIN_(MAX_Area_Logo_Boundary_list[0].u16_start_y, MAX_Area_Logo_Boundary_list[u8_i].u16_start_y);
		Boundary_tmp.u16_end_x = _MAX_(MAX_Area_Logo_Boundary_list[0].u16_end_x, MAX_Area_Logo_Boundary_list[u8_i].u16_end_x);
		Boundary_tmp.u16_end_y = _MAX_(MAX_Area_Logo_Boundary_list[0].u16_end_y, MAX_Area_Logo_Boundary_list[u8_i].u16_end_y);

		if(inBoundary_RTK2885p(Boundary_tmp, Boundary_pre)==true){
			if(log_en && boundary_id==g_frame_logo_id){//d984[29]
				rtd_pr_memc_emerg("pre,%d,%d,%d,%d,  cur,%d,%d,%d,%d,  %d,%d,%d,%d\n",
					Boundary_pre.u16_start_x, Boundary_pre.u16_end_x, Boundary_pre.u16_start_y, Boundary_pre.u16_end_y,
					MAX_Area_Logo_Boundary_list[0].u16_start_x, MAX_Area_Logo_Boundary_list[0].u16_end_x, MAX_Area_Logo_Boundary_list[0].u16_start_y, MAX_Area_Logo_Boundary_list[0].u16_end_y,
					MAX_Area_Logo_Boundary_list[u8_i].u16_start_x, MAX_Area_Logo_Boundary_list[u8_i].u16_end_x, MAX_Area_Logo_Boundary_list[u8_i].u16_start_y, MAX_Area_Logo_Boundary_list[u8_i].u16_end_y);
			}
			MAX_Area_Logo_Boundary_list[0].u16_start_x = Boundary_tmp.u16_start_x;
			MAX_Area_Logo_Boundary_list[0].u16_start_y = Boundary_tmp.u16_start_y;
			MAX_Area_Logo_Boundary_list[0].u16_end_x = Boundary_tmp.u16_end_x;
			MAX_Area_Logo_Boundary_list[0].u16_end_y = Boundary_tmp.u16_end_y;

			MAX_Area_Logo_Boundary_list[u8_i].u1_get_logo = false;
		}
	}

	u8_i = 1;
	Boundary_tmp.u16_start_x = _MIN_(MAX_Area_Logo_Boundary_list[0].u16_start_x, MAX_Area_Logo_Boundary_list[u8_i].u16_start_x);
	Boundary_tmp.u16_start_y = _MIN_(MAX_Area_Logo_Boundary_list[0].u16_start_y, MAX_Area_Logo_Boundary_list[u8_i].u16_start_y);
	Boundary_tmp.u16_end_x = _MAX_(MAX_Area_Logo_Boundary_list[0].u16_end_x, MAX_Area_Logo_Boundary_list[u8_i].u16_end_x);
	Boundary_tmp.u16_end_y = _MAX_(MAX_Area_Logo_Boundary_list[0].u16_end_y, MAX_Area_Logo_Boundary_list[u8_i].u16_end_y);

	if(log_en2 && boundary_id==g_frame_logo_id){//d984[28]
		rtd_pr_memc_emerg("in,%d,%d  pre,%d,%d,%d,%d,  tmp,%d,%d,%d,%d,  max,%d,%d,%d,%d,  sec,%d,%d,%d,%d,\n", inBoundary_RTK2885p(Boundary_tmp, Boundary_pre), MAX_Area_Logo_Boundary_list[u8_i].u1_get_logo,
			Boundary_pre.u16_start_x, Boundary_pre.u16_end_x, Boundary_pre.u16_start_y, Boundary_pre.u16_end_y, 
			Boundary_tmp.u16_start_x, Boundary_tmp.u16_end_x, Boundary_tmp.u16_start_y, Boundary_tmp.u16_end_y,
			MAX_Area_Logo_Boundary_list[0].u16_start_x, MAX_Area_Logo_Boundary_list[0].u16_end_x, MAX_Area_Logo_Boundary_list[0].u16_start_y, MAX_Area_Logo_Boundary_list[0].u16_end_y,
			MAX_Area_Logo_Boundary_list[u8_i].u16_start_x, MAX_Area_Logo_Boundary_list[u8_i].u16_end_x, MAX_Area_Logo_Boundary_list[u8_i].u16_start_y, MAX_Area_Logo_Boundary_list[u8_i].u16_end_y);
	}
}

MEMC_Logo_Boundary_Info Search_MAX_area_boundary_RTK2885p(MEMC_Logo_Boundary Search_Boundary, MEMC_Logo_Boundary_Info Boundary_pre, unsigned char scale_gain_shift, unsigned char boundary_id)
{
	const unsigned char center_th = 8;
	unsigned char u8_i, u8_j, MatchRatio_Area = 0, MatchRatio_Area_new = 0, MatchRatio_LogoCnt = 0, MatchRatio_LogoCnt_new = 0, u8_case_id = 0;
	unsigned short u16_x, u16_y, /*u16_logo_cnt_avg_h = 0, u16_logo_cnt_avg_v = 0,*/ u16_logo_cnt_sum = 0, u16_logo_cnt_sum_pre = 0, u16_logo_cnt_sum_new = 0;
	unsigned int log_en, log_en_max, MAX_Area = 0, MAX_2nd_Area = 0, Area_cur = 0, MatchRatio_Area_th, MatchRatio_LogoCnt_th;
	unsigned int stable_en, u6_stable_cnt_th, MAX_Area_list[LOGO_AREA_NUM] = {0};
	unsigned int Logo_cur_cen_x, Logo_cur_cen_y, Logo_pre_cen_x, Logo_pre_cen_y;
	short Logo_Boundary_W, Logo_Boundary_H;
	bool u1_logo_center_stable = false, u1_new_logo_center_stable = false;
	MEMC_Logo_Boundary_Info Logo_Boundary = {LOGO_DDR_W, 0, LOGO_DDR_H, 0};
	MEMC_Logo_Boundary_Info MAX_Area_Logo_Boundary = {LOGO_DDR_W, 0, LOGO_DDR_H, 0};
	MEMC_Logo_Boundary_Info MAX_2nd_Area_Logo_Boundary = {LOGO_DDR_W, 0, LOGO_DDR_H, 0};
	MEMC_Logo_Boundary_Info MAX_Area_Logo_Boundary_list[LOGO_AREA_NUM] = {0};
	static MEMC_Logo_Boundary_Info new_logo_boundary[LOGO_AREA_NUM] = {0};
	static unsigned char logo_boundary_chg_cnt[LOGO_POSITION_NUM] = {0}, u8_stable_cnt[LOGO_POSITION_NUM] = {0};
	static bool u1_chg_boundary[LOGO_POSITION_NUM] = {0};
	ReadRegister(SOFTWARE2_SOFTWARE2_35_reg, 31, 31, &log_en_max);
	ReadRegister(SOFTWARE2_SOFTWARE2_35_reg, 15, 15, &stable_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_35_reg, 14, 14, &log_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_35_reg,  0,  7, &MatchRatio_Area_th);
	ReadRegister(SOFTWARE2_SOFTWARE2_35_reg, 16, 23, &MatchRatio_LogoCnt_th);
	ReadRegister(SOFTWARE2_SOFTWARE2_35_reg,  8, 13, &u6_stable_cnt_th);

	if(log_en && boundary_id==g_frame_logo_id){
		log_specified = true;
	}else{
		log_specified = false;
	}

	memset(MAX_Area_list, 0, sizeof(MAX_Area_list));
	for(u8_i=0; u8_i<LOGO_AREA_NUM; u8_i++){
		MAX_Area_Logo_Boundary_list[u8_i].u1_get_logo = false;
	}

	memset(logo_invalid, 0, sizeof(logo_invalid));
	memset(logo_same_array, 0, sizeof(logo_same_array));

	logo_same_index = 0;

	//first search pre area

	for(u16_y=Search_Boundary.u16_start_y; u16_y<=Search_Boundary.u16_end_y; u16_y++){
		for(u16_x=Search_Boundary.u16_start_x ; u16_x<=Search_Boundary.u16_end_x; u16_x++){
			
			logo_same_index++;
			
			// check whether the block has the logo flag which isn't used yet
			if(logo_flag_array[u16_x][u16_y]==1 && logo_invalid[u16_x][u16_y]==0){

				Logo_Boundary.u16_start_y = u16_y;
				Logo_Boundary.u16_start_x = u16_x;
				Logo_Boundary.u16_end_y = u16_y;
				Logo_Boundary.u16_end_x = u16_x;

				// search logo boundary
				Search_Logo_Boundary_in_H_RTK2885p(Search_Boundary, &Logo_Boundary);

				if(Logo_Boundary.u16_end_x<Logo_Boundary.u16_start_x){
					rtd_pr_memc_emerg("[%s] ERROR id,%d,  x,%d,%d\n", __FUNCTION__, boundary_id, Logo_Boundary.u16_start_x, Logo_Boundary.u16_end_x);
				}
				if(Logo_Boundary.u16_end_y<Logo_Boundary.u16_start_y){
					rtd_pr_memc_emerg("[%s] ERROR id,%d,  y,%d,%d\n", __FUNCTION__, boundary_id, Logo_Boundary.u16_start_y, Logo_Boundary.u16_end_y);
				}

				Logo_Boundary_W = Logo_Boundary.u16_end_x - Logo_Boundary.u16_start_x;
				Logo_Boundary_H = Logo_Boundary.u16_end_y - Logo_Boundary.u16_start_y;
				Area_cur = Logo_Boundary_W * Logo_Boundary_H;

				//if(boundary_id==2){
				//	rtd_pr_memc_crit("[%s]	area,%d,  info,%d, %d,  flg,%d,%d,  boundary,%d,%d,%d,%d\n", __FUNCTION__, Area_cur, Logo_Boundary_W, Logo_Boundary_H, 
				//		Logo_DDR_tounch_inside_boundary_RTK2885p(Search_Boundary, Logo_Boundary, boundary_id), Logo_DDR_Match_WH_Ratio_RTK2885p(Logo_Boundary),
				//		Logo_Boundary.u16_start_x,  Logo_Boundary.u16_end_x, Logo_Boundary.u16_start_y, Logo_Boundary.u16_end_y);	
				//}

				if(Logo_Boundary_W >= LOGO_MIN_LENGTH_480 && Logo_Boundary_H >= LOGO_MIN_LENGTH_480 && Area_cur>=MIN_LOGO_AREA_480 && 
					(Logo_DDR_tounch_inside_boundary_RTK2885p(Search_Boundary, Logo_Boundary, boundary_id)==false) &&
					Logo_DDR_Match_WH_Ratio_RTK2885p(Logo_Boundary)==true ){
					//rtd_pr_memc_emerg("[search f2] x,%d,%d,   y,%d,%d,   area,%d\n", Logo_Boundary.u16_start_x, Logo_Boundary.u16_end_x, Logo_Boundary.u16_start_y, Logo_Boundary.u16_end_y, Area_cur);

					// sort the logo bounary by area
					for(u8_i=0; u8_i<LOGO_AREA_NUM; u8_i++){
						if(Area_cur > MAX_Area_list[u8_i]){

							for(u8_j=LOGO_AREA_NUM-1; u8_j>=u8_i+1; u8_j--){
								MAX_Area_list[u8_j] = MAX_Area_list[u8_j-1];
								memcpy(&MAX_Area_Logo_Boundary_list[u8_j], &MAX_Area_Logo_Boundary_list[u8_j-1], sizeof(MEMC_Logo_Boundary_Info));
							}
							MAX_Area_list[u8_i] = Area_cur;
							memcpy(&MAX_Area_Logo_Boundary_list[u8_i], &Logo_Boundary, sizeof(Logo_Boundary));
							MAX_Area_Logo_Boundary_list[u8_i].u1_get_logo = true;
							break;
						}
					}

					if(Area_cur>MAX_Area){
						MAX_2nd_Area = MAX_Area;
						memcpy(&MAX_2nd_Area_Logo_Boundary, &MAX_Area_Logo_Boundary, sizeof(MAX_Area_Logo_Boundary));

						MAX_Area = Area_cur;
						memcpy(&MAX_Area_Logo_Boundary, &Logo_Boundary, sizeof(Logo_Boundary));
					}else if(Area_cur>MAX_2nd_Area){
						MAX_2nd_Area = Area_cur;
						memcpy(&MAX_2nd_Area_Logo_Boundary, &Logo_Boundary, sizeof(Logo_Boundary));
					}

				}
				u16_x = Logo_Boundary.u16_end_x+1;
			}
		}
	}

	if(log_en_max && boundary_id==g_frame_logo_id){ //d9e8[31]
		rtd_pr_memc_emerg("%d,%d,%d,%d,%d,  %d,%d,%d,%d,%d,  %d,%d,%d,%d,%d,  %d,%d,%d,%d,%d,  %d,%d,%d,%d,%d,  %d,%d,%d,%d,%d,  %d,%d,%d,%d,%d,  %d,%d,%d,%d,%d,\n",
			MAX_Area_Logo_Boundary_list[0].u1_get_logo, MAX_Area_Logo_Boundary_list[0].u16_start_x, MAX_Area_Logo_Boundary_list[0].u16_end_x, MAX_Area_Logo_Boundary_list[0].u16_start_y, MAX_Area_Logo_Boundary_list[0].u16_end_y,
			MAX_Area_Logo_Boundary_list[1].u1_get_logo, MAX_Area_Logo_Boundary_list[1].u16_start_x, MAX_Area_Logo_Boundary_list[1].u16_end_x, MAX_Area_Logo_Boundary_list[1].u16_start_y, MAX_Area_Logo_Boundary_list[1].u16_end_y,
			MAX_Area_Logo_Boundary_list[2].u1_get_logo, MAX_Area_Logo_Boundary_list[2].u16_start_x, MAX_Area_Logo_Boundary_list[2].u16_end_x, MAX_Area_Logo_Boundary_list[2].u16_start_y, MAX_Area_Logo_Boundary_list[2].u16_end_y,
			MAX_Area_Logo_Boundary_list[3].u1_get_logo, MAX_Area_Logo_Boundary_list[3].u16_start_x, MAX_Area_Logo_Boundary_list[3].u16_end_x, MAX_Area_Logo_Boundary_list[3].u16_start_y, MAX_Area_Logo_Boundary_list[3].u16_end_y,
			MAX_Area_Logo_Boundary_list[4].u1_get_logo, MAX_Area_Logo_Boundary_list[4].u16_start_x, MAX_Area_Logo_Boundary_list[4].u16_end_x, MAX_Area_Logo_Boundary_list[4].u16_start_y, MAX_Area_Logo_Boundary_list[4].u16_end_y,
			MAX_Area_Logo_Boundary_list[5].u1_get_logo, MAX_Area_Logo_Boundary_list[5].u16_start_x, MAX_Area_Logo_Boundary_list[5].u16_end_x, MAX_Area_Logo_Boundary_list[5].u16_start_y, MAX_Area_Logo_Boundary_list[5].u16_end_y,
			MAX_Area_Logo_Boundary_list[6].u1_get_logo, MAX_Area_Logo_Boundary_list[6].u16_start_x, MAX_Area_Logo_Boundary_list[6].u16_end_x, MAX_Area_Logo_Boundary_list[6].u16_start_y, MAX_Area_Logo_Boundary_list[6].u16_end_y,
			MAX_Area_Logo_Boundary_list[7].u1_get_logo, MAX_Area_Logo_Boundary_list[7].u16_start_x, MAX_Area_Logo_Boundary_list[7].u16_end_x, MAX_Area_Logo_Boundary_list[7].u16_start_y, MAX_Area_Logo_Boundary_list[7].u16_end_y );
	}

#if 0
	if(MAX_Area_Logo_Boundary_list[0].u1_get_logo && MAX_Area_Logo_Boundary_list[1].u1_get_logo){

		Logo_DDR_expand_RTK2885p(&Boundary_pre_expand, 2);
		if(inBoundary_RTK2885p(MAX_Area_Logo_Boundary_list[0], Boundary_pre_expand) && inBoundary_RTK2885p(MAX_Area_Logo_Boundary_list[1], Boundary_pre_expand)){
			MAX_Area_Logo_Boundary_list[0].u16_start_x = _MIN_(MAX_Area_Logo_Boundary_list[0].u16_start_x, MAX_Area_Logo_Boundary_list[1].u16_start_x);
			MAX_Area_Logo_Boundary_list[0].u16_start_y = _MIN_(MAX_Area_Logo_Boundary_list[0].u16_start_y, MAX_Area_Logo_Boundary_list[1].u16_start_y);
			MAX_Area_Logo_Boundary_list[0].u16_end_x = _MAX_(MAX_Area_Logo_Boundary_list[0].u16_end_x, MAX_Area_Logo_Boundary_list[1].u16_end_x);
			MAX_Area_Logo_Boundary_list[0].u16_end_y = _MAX_(MAX_Area_Logo_Boundary_list[0].u16_end_y, MAX_Area_Logo_Boundary_list[1].u16_end_y);
			MAX_Area_Logo_Boundary = MAX_Area_Logo_Boundary_list[0];
		}
		
	}
#endif
#if 0
	if(MAX_2nd_Area==0){
		if(MAX_Area!=MAX_Area_list[0]){
			rtd_pr_memc_emerg("ERROR, id,%d  area,%d,%d, boundary,%d,%d,%d,%d, %d,%d,%d,%d,\n", 
				boundary_id, MAX_Area, MAX_Area_list[0], 
				MAX_Area_Logo_Boundary.u16_start_x, MAX_Area_Logo_Boundary.u16_end_x, MAX_Area_Logo_Boundary.u16_start_y, MAX_Area_Logo_Boundary.u16_end_y,
				MAX_Area_Logo_Boundary_list[0].u16_start_x, MAX_Area_Logo_Boundary_list[0].u16_end_x, MAX_Area_Logo_Boundary_list[0].u16_start_y, MAX_Area_Logo_Boundary_list[0].u16_end_y);
		}
	}
	else{
		if(MAX_Area!=MAX_Area_list[0]){
			rtd_pr_memc_emerg("ERROR max_1st, id,%d	area,%d,%d, boundary,%d,%d,%d,%d, %d,%d,%d,%d,\n", boundary_id, MAX_Area, MAX_Area_list[0], 
				MAX_Area_Logo_Boundary.u16_start_x, MAX_Area_Logo_Boundary.u16_end_x, MAX_Area_Logo_Boundary.u16_start_y, MAX_Area_Logo_Boundary.u16_end_y,
				MAX_Area_Logo_Boundary_list[0].u16_start_x, MAX_Area_Logo_Boundary_list[0].u16_end_x, MAX_Area_Logo_Boundary_list[0].u16_start_y, MAX_Area_Logo_Boundary_list[0].u16_end_y);
		}

		if(MAX_2nd_Area!=MAX_Area_list[1]){
			rtd_pr_memc_emerg("ERROR max_2nd, id,%d	area,%d,%d, boundary,%d,%d,%d,%d, %d,%d,%d,%d,\n", boundary_id, MAX_2nd_Area, MAX_Area_list[1], 
				MAX_2nd_Area_Logo_Boundary.u16_start_x, MAX_2nd_Area_Logo_Boundary.u16_end_x, MAX_2nd_Area_Logo_Boundary.u16_start_y, MAX_2nd_Area_Logo_Boundary.u16_end_y,
				MAX_Area_Logo_Boundary_list[1].u16_start_x, MAX_Area_Logo_Boundary_list[1].u16_end_x, MAX_Area_Logo_Boundary_list[1].u16_start_y, MAX_Area_Logo_Boundary_list[1].u16_end_y);
		}
	}
#endif
#if 0
	if(boundary_id==3){
		rtd_pr_memc_emerg("boundary,%d,%d,%d,%d, %d,%d,%d,%d,   %d,%d,%d,%d, %d,%d,%d,%d,\n", 
			MAX_Area_Logo_Boundary.u16_start_x, MAX_Area_Logo_Boundary.u16_end_x, MAX_Area_Logo_Boundary.u16_start_y, MAX_Area_Logo_Boundary.u16_end_y,
			MAX_2nd_Area_Logo_Boundary.u16_start_x, MAX_2nd_Area_Logo_Boundary.u16_end_x, MAX_2nd_Area_Logo_Boundary.u16_start_y, MAX_2nd_Area_Logo_Boundary.u16_end_y,
			MAX_Area_Logo_Boundary_list[0].u16_start_x, MAX_Area_Logo_Boundary_list[0].u16_end_x, MAX_Area_Logo_Boundary_list[0].u16_start_y, MAX_Area_Logo_Boundary_list[0].u16_end_y,
			MAX_Area_Logo_Boundary_list[1].u16_start_x, MAX_Area_Logo_Boundary_list[1].u16_end_x, MAX_Area_Logo_Boundary_list[1].u16_start_y, MAX_Area_Logo_Boundary_list[1].u16_end_y);
	}
#endif

//	for(u16_x=0 ; u16_x<=LOGO_AREA_NUM; u16_x++){
//		if(MAX_Area_Logo_Boundary_list[u8_i].u1_get_logo == true){
//			Logo_DDR_FineSearchLogo_RTK2885p(MAX_Area_Logo_Boundary_list[u8_i], Search_Boundary, boundary_id);
//		}
//	}
	// connect neighborhood logo
	Logo_DDR_Connect_RTK2885p(&MAX_Area_Logo_Boundary, MAX_Area_Logo_Boundary_list, boundary_id);

	//save data by array
	#if 0
	if(boundary_id==g_frame_logo_id){
		rtd_pr_memc_emerg("Area, max,%d,%d,%d,%d,  2nd,%d,%d,%d,%d\n", 
			MAX_Area_Logo_Boundary.u16_start_x, MAX_Area_Logo_Boundary.u16_end_x, MAX_Area_Logo_Boundary.u16_start_y, MAX_Area_Logo_Boundary.u16_end_y, 
			MAX_2nd_Area_Logo_Boundary.u16_start_x, MAX_2nd_Area_Logo_Boundary.u16_end_x, MAX_2nd_Area_Logo_Boundary.u16_start_y, MAX_2nd_Area_Logo_Boundary.u16_end_y);
		memcpy(&MAX_Area_Logo_Boundary, &MAX_2nd_Area_Logo_Boundary, sizeof(MAX_2nd_Area_Logo_Boundary));
	}
	#endif

#if 0
	for(u16_y=MAX_Area_Logo_Boundary.u16_start_y; u16_y<=MAX_Area_Logo_Boundary.u16_end_y; u16_y++){
		for(u16_x=MAX_Area_Logo_Boundary.u16_start_x ; u16_x<=MAX_Area_Logo_Boundary.u16_end_x; u16_x++){
			u16_logo_cnt_sum = (logo_flag_array[u16_x][u16_y]==1) ? u16_logo_cnt_sum+1 ; u16_logo_cnt_sum;
		}
	}
#endif
	Boundary_pre.u16_start_x >>= scale_gain_shift;
	Boundary_pre.u16_start_y >>= scale_gain_shift;
	Boundary_pre.u16_end_x >>= scale_gain_shift;
	Boundary_pre.u16_end_y >>= scale_gain_shift;

	u16_logo_cnt_sum_pre = Get_logo_cnt_sum_RTK2885p(Boundary_pre);
	u16_logo_cnt_sum = Get_logo_cnt_sum_RTK2885p(MAX_Area_Logo_Boundary);

//	u16_logo_cnt_avg_v = (MAX_Area_Logo_Boundary.u16_end_x>MAX_Area_Logo_Boundary.u16_start_x) ? u16_logo_cnt_sum/(MAX_Area_Logo_Boundary.u16_end_x-MAX_Area_Logo_Boundary.u16_start_x) : u16_logo_cnt_sum;
//	u16_logo_cnt_avg_h = (MAX_Area_Logo_Boundary.u16_end_y>MAX_Area_Logo_Boundary.u16_start_y) ? u16_logo_cnt_sum/(MAX_Area_Logo_Boundary.u16_end_y-MAX_Area_Logo_Boundary.u16_start_y) : u16_logo_cnt_sum;

//	HoldOn_Logo_Boundary_RTK2885p(Boundary_pre, MAX_Area_Logo_Boundary_list, boundary_id);

	// area is untrusted
	if(MAX_Area<MIN_LOGO_AREA_480 || MAX_Area>=UNCONF_LOGO_AREA[boundary_id] || Logo_DDR_Match_SearchBoundary_RTK2885p(Search_Boundary, MAX_Area_Logo_Boundary)==true || Logo_DDR_WH_Ratio_Trust_RTK2885p(MAX_Area_Logo_Boundary)==false){
		MAX_Area_Logo_Boundary.u1_get_logo = false;
		if(log_specified){
			rtd_pr_memc_emerg("[%s]  case,0, MAX_Area,%d, th,%d, area,%d,%d,%d,%d\n", __FUNCTION__, MAX_Area, UNCONF_LOGO_AREA[boundary_id], MAX_Area_Logo_Boundary.u16_start_x, MAX_Area_Logo_Boundary.u16_end_x, MAX_Area_Logo_Boundary.u16_start_y, MAX_Area_Logo_Boundary.u16_end_y);
		}
		u8_stable_cnt[boundary_id]=0;
		MAX_Area_Logo_Boundary.u16_start_x <<= scale_gain_shift;
		MAX_Area_Logo_Boundary.u16_start_y <<= scale_gain_shift;
		MAX_Area_Logo_Boundary.u16_end_x <<= scale_gain_shift;
		MAX_Area_Logo_Boundary.u16_end_y <<= scale_gain_shift;
		return MAX_Area_Logo_Boundary;
	}else{
		MAX_Area_Logo_Boundary.u1_get_logo = true;
	}

#if 0//old
	if(stable_en){
		MatchRatio_Area = Logo_DDR_AreaMatchRatio_RTK2885p(Boundary_pre, MAX_Area_Logo_Boundary);
//		MatchRatio_LogoCnt = (Boundary_pre.u16_logo_cnt==0 && u16_logo_cnt_sum_pre==0) ? 0 : _MIN_(u16_logo_cnt_sum, u16_logo_cnt_sum_pre)*100/_MAX_(u16_logo_cnt_sum, u16_logo_cnt_sum_pre);
		MatchRatio_LogoCnt = (Boundary_pre.u16_logo_cnt==0 && u16_logo_cnt_sum_pre==0) ? 0 : _MIN_(Boundary_pre.u16_logo_cnt, u16_logo_cnt_sum_pre)*100/_MAX_(Boundary_pre.u16_logo_cnt, u16_logo_cnt_sum_pre);

		//logo_cnt_th = (Boundary_pre.u16_logo_cnt*u8_logo_cnt_th_gain)>>7;
		//if(_ABS_DIFF_(Boundary_pre.u16_logo_cnt, u16_logo_cnt_sum_pre) >= logo_cnt_th){
		if( (MatchRatio_Area <= MatchRatio_Area_th && MatchRatio_LogoCnt <= MatchRatio_LogoCnt_th) || 
			Boundary_pre.u1_get_logo == false){
			if(logo_boundary_chg_cnt[boundary_id]==0){
				logo_boundary_chg_cnt[boundary_id]++;
			//	new_Boundary = MAX_Area_Logo_Boundary;
				MAX_Area_Logo_Boundary.u1_get_logo = false;
			}else {
				if(MatchRatio_Area<25){
					logo_boundary_chg_cnt[boundary_id] = 0;
			//		new_Boundary = MAX_Area_Logo_Boundary;
					MAX_Area_Logo_Boundary.u1_get_logo = false;
				}else if(logo_boundary_chg_cnt[boundary_id]>=u6_stable_cnt_th){
					
					MAX_Area_Logo_Boundary.u1_get_logo = true;
				}else{
					logo_boundary_chg_cnt[boundary_id]++;
					MAX_Area_Logo_Boundary.u1_get_logo = false;
			//		new_Boundary.u16_start_x = (new_Boundary.u16_start_x+MAX_Area_Logo_Boundary.u16_start_x)>>1;
			//		new_Boundary.u16_start_y = (new_Boundary.u16_start_x+MAX_Area_Logo_Boundary.u16_start_y)>>1;
			//		new_Boundary.u16_end_x = (new_Boundary.u16_start_x+MAX_Area_Logo_Boundary.u16_end_x)>>1;
			//		new_Boundary.u16_end_y = (new_Boundary.u16_start_x+MAX_Area_Logo_Boundary.u16_end_y)>>1;
				}
			}
			if(log_specified){
				rtd_pr_memc_emerg("[%s]  case,12, Ratio,%d,%d, cnt,%d,  pre,%d\n", __FUNCTION__, 
					MatchRatio_Area, MatchRatio_LogoCnt, logo_boundary_chg_cnt[boundary_id], Boundary_pre.u1_get_logo);
			}
		}
		else{
			logo_boundary_chg_cnt[boundary_id] = 0;
			MAX_Area_Logo_Boundary = Logo_DDR_FineSearchLogo_RTK2885p(MAX_Area_Logo_Boundary_list[0], Search_Boundary, boundary_id);
		}
	}
#else
	if(stable_en){
		MatchRatio_Area = Logo_DDR_AreaMatchRatio_RTK2885p(Boundary_pre, MAX_Area_Logo_Boundary);
		MatchRatio_LogoCnt = (u16_logo_cnt_sum==0 && u16_logo_cnt_sum_pre==0) ? 0 : _MIN_(u16_logo_cnt_sum, u16_logo_cnt_sum_pre)*100/_MAX_(u16_logo_cnt_sum, u16_logo_cnt_sum_pre);

		Logo_cur_cen_x = (MAX_Area_Logo_Boundary.u16_end_x+MAX_Area_Logo_Boundary.u16_start_x)>>1;
		Logo_cur_cen_y = (MAX_Area_Logo_Boundary.u16_end_y+MAX_Area_Logo_Boundary.u16_start_y)>>1;
		Logo_pre_cen_x = (Boundary_pre.u16_end_x+Boundary_pre.u16_start_x)>>1;
		Logo_pre_cen_y = (Boundary_pre.u16_end_y+Boundary_pre.u16_start_y)>>1;
		u1_logo_center_stable = (_ABS_DIFF_(Logo_cur_cen_x, Logo_pre_cen_x)<=center_th && _ABS_DIFF_(Logo_cur_cen_y, Logo_pre_cen_y)<=center_th) ? true : false;

		//logo_cnt_th = (Boundary_pre.u16_logo_cnt*u8_logo_cnt_th_gain)>>7;
		//if(_ABS_DIFF_(Boundary_pre.u16_logo_cnt, u16_logo_cnt_sum_pre) >= logo_cnt_th){

		// area and logo cnt is stable
		if( (MatchRatio_Area >= MatchRatio_Area_th && MatchRatio_LogoCnt >= MatchRatio_LogoCnt_th) && u1_logo_center_stable == true && u1_chg_boundary[boundary_id] == false){
			u8_case_id = 1;
			logo_boundary_chg_cnt[boundary_id] = 0;
			u8_stable_cnt[boundary_id] = (u8_stable_cnt[boundary_id]<AREA_STABLE_CNT_MAX) ? u8_stable_cnt[boundary_id]+1 : AREA_STABLE_CNT_MAX;
			MAX_Area_Logo_Boundary.u1_get_logo = (u8_stable_cnt[boundary_id]>AREA_STABLE_CNT_TH) ? true : false;
			MAX_Area_Logo_Boundary = Logo_DDR_FineSearchLogo_RTK2885p(MAX_Area_Logo_Boundary_list[0], Search_Boundary, boundary_id, u8_stable_cnt[boundary_id]);
			if(log_specified){
				rtd_pr_memc_emerg("[%s]  case,10, flg,%d, id,%d, chg,%d, Ratio,%d,%d,%d,%d, cnt,%d,%d,  pre,%d,  center,%d,%d,  WHR,%d,  Y,%d,%d, X,%d,%d,%d,%d,%d,%d\n", __FUNCTION__, MAX_Area_Logo_Boundary.u1_get_logo, u8_case_id, u1_chg_boundary[boundary_id],
					MatchRatio_Area, MatchRatio_LogoCnt, MatchRatio_Area_new, MatchRatio_LogoCnt_new,  u8_stable_cnt[boundary_id], logo_boundary_chg_cnt[boundary_id], Boundary_pre.u1_get_logo, u1_logo_center_stable, u1_new_logo_center_stable,
					(MAX_Area_Logo_Boundary.u16_end_x-MAX_Area_Logo_Boundary.u16_start_x)/(MAX_Area_Logo_Boundary.u16_end_y-MAX_Area_Logo_Boundary.u16_start_y),MAX_Area_Logo_Boundary.u16_start_y, MAX_Area_Logo_Boundary.u16_end_y,
					MAX_Area_Logo_Boundary.u16_start_x, Boundary_pre.u16_start_x, new_logo_boundary[boundary_id].u16_start_x, MAX_Area_Logo_Boundary.u16_end_x, Boundary_pre.u16_end_x, new_logo_boundary[boundary_id].u16_end_x);
			}
		}else if(u8_stable_cnt[boundary_id] >= 10){
			u8_case_id = 2;
			u8_stable_cnt[boundary_id] -= 10;
			MAX_Area_Logo_Boundary.u1_get_logo = (u8_stable_cnt[boundary_id]>AREA_STABLE_CNT_TH) ? true : false;
			if(log_specified){
				rtd_pr_memc_emerg("[%s]  case,11, flg,%d, id,%d, chg,%d, Ratio,%d,%d,%d,%d, cnt,%d,%d,  pre,%d,  center,%d,%d,  MR,%d,%d,%d,%d, X,%d,%d,%d,%d,%d,%d\n", __FUNCTION__, MAX_Area_Logo_Boundary.u1_get_logo, u8_case_id, u1_chg_boundary[boundary_id],
					MatchRatio_Area, MatchRatio_LogoCnt, MatchRatio_Area_new, MatchRatio_LogoCnt_new,  u8_stable_cnt[boundary_id], logo_boundary_chg_cnt[boundary_id], Boundary_pre.u1_get_logo, u1_logo_center_stable, u1_new_logo_center_stable,
					new_logo_boundary[boundary_id].u16_start_x, new_logo_boundary[boundary_id].u16_end_x, MAX_Area_Logo_Boundary.u16_start_x, MAX_Area_Logo_Boundary.u16_end_x,
					MAX_Area_Logo_Boundary.u16_start_x, Boundary_pre.u16_start_x, new_logo_boundary[boundary_id].u16_start_x, MAX_Area_Logo_Boundary.u16_end_x, Boundary_pre.u16_end_x, new_logo_boundary[boundary_id].u16_end_x);
			}
		// area and logo cnt is unstable
		}else {
			u1_chg_boundary[boundary_id] = true;
			MAX_Area_Logo_Boundary.u1_get_logo = false;
			u8_stable_cnt[boundary_id] = 0;
			// set new logo boundary at first time change unstable status
			if(logo_boundary_chg_cnt[boundary_id]==0){
				u8_case_id = 3;
				logo_boundary_chg_cnt[boundary_id] = 1;
				new_logo_boundary[boundary_id] = MAX_Area_Logo_Boundary;
			}else{
				// check new logo boundary whether is stable or not
				u16_logo_cnt_sum_new = Get_logo_cnt_sum_RTK2885p(new_logo_boundary[boundary_id]);
				MatchRatio_Area_new = Logo_DDR_AreaMatchRatio_RTK2885p(new_logo_boundary[boundary_id], MAX_Area_Logo_Boundary);
				MatchRatio_LogoCnt_new = (u16_logo_cnt_sum_new==0 && u16_logo_cnt_sum==0) ? 0 : _MIN_(u16_logo_cnt_sum_new, u16_logo_cnt_sum)*100/_MAX_(u16_logo_cnt_sum_new, u16_logo_cnt_sum);
				Logo_pre_cen_x = (new_logo_boundary[boundary_id].u16_end_x+new_logo_boundary[boundary_id].u16_start_x)>>1;
				Logo_pre_cen_y = (new_logo_boundary[boundary_id].u16_end_y+new_logo_boundary[boundary_id].u16_start_y)>>1;
				u1_new_logo_center_stable = (_ABS_DIFF_(Logo_cur_cen_x, Logo_pre_cen_x)<=center_th && _ABS_DIFF_(Logo_cur_cen_y, Logo_pre_cen_y)<=center_th) ? true : false;
				if( (MatchRatio_Area_new >= MatchRatio_Area_th && MatchRatio_LogoCnt_new >= MatchRatio_LogoCnt_th) ){
					u8_case_id = 4;
					logo_boundary_chg_cnt[boundary_id]++;
				}else if(MatchRatio_Area_new>50){
					u8_case_id = 5;
					logo_boundary_chg_cnt[boundary_id]--;
				}else if(MatchRatio_Area_new<50){
					u8_case_id = 6;
					logo_boundary_chg_cnt[boundary_id] = 0;
					new_logo_boundary[boundary_id] = MAX_Area_Logo_Boundary;
				}

				if(logo_boundary_chg_cnt[boundary_id]>u6_stable_cnt_th){
					u8_case_id += 10;
					MAX_Area_Logo_Boundary.u1_get_logo = true;
					u1_chg_boundary[boundary_id] = false;
					MAX_Area_Logo_Boundary = Logo_DDR_FineSearchLogo_RTK2885p(new_logo_boundary[boundary_id], Search_Boundary, boundary_id, u8_stable_cnt[boundary_id]);
				}
			}
			if(log_specified){
				rtd_pr_memc_emerg("[%s]  case,12, flg,%d, id,%d, chg,%d, Ratio,%d,%d,%d,%d, cnt,%d,%d,  pre,%d,  center,%d,%d,  MR,%d,%d,%d,%d, X,%d,%d,%d,%d,%d,%d\n", __FUNCTION__, MAX_Area_Logo_Boundary.u1_get_logo, u8_case_id, u1_chg_boundary[boundary_id],
					MatchRatio_Area, MatchRatio_LogoCnt, MatchRatio_Area_new, MatchRatio_LogoCnt_new,  u8_stable_cnt[boundary_id], logo_boundary_chg_cnt[boundary_id], Boundary_pre.u1_get_logo, u1_logo_center_stable, u1_new_logo_center_stable,
					new_logo_boundary[boundary_id].u16_start_x, new_logo_boundary[boundary_id].u16_end_x, MAX_Area_Logo_Boundary.u16_start_x, MAX_Area_Logo_Boundary.u16_end_x,
					MAX_Area_Logo_Boundary.u16_start_x, Boundary_pre.u16_start_x, new_logo_boundary[boundary_id].u16_start_x, MAX_Area_Logo_Boundary.u16_end_x, Boundary_pre.u16_end_x, new_logo_boundary[boundary_id].u16_end_x);
			}
		}
	}
#endif
	MAX_Area_Logo_Boundary.u16_logo_cnt = Get_logo_cnt_sum_RTK2885p(MAX_Area_Logo_Boundary);

	if( MAX_Area_Logo_Boundary.u16_start_x<SEARCH_BOUNDARY[boundary_id].u16_start_x || MAX_Area_Logo_Boundary.u16_start_x>SEARCH_BOUNDARY[boundary_id].u16_end_x || 
		MAX_Area_Logo_Boundary.u16_end_x<SEARCH_BOUNDARY[boundary_id].u16_start_x   || MAX_Area_Logo_Boundary.u16_end_x>SEARCH_BOUNDARY[boundary_id].u16_end_x || 
		MAX_Area_Logo_Boundary.u16_start_y<SEARCH_BOUNDARY[boundary_id].u16_start_y || MAX_Area_Logo_Boundary.u16_start_y>SEARCH_BOUNDARY[boundary_id].u16_end_y || 
		MAX_Area_Logo_Boundary.u16_end_y<SEARCH_BOUNDARY[boundary_id].u16_start_y   || MAX_Area_Logo_Boundary.u16_end_y>SEARCH_BOUNDARY[boundary_id].u16_end_y){
		rtd_pr_memc_emerg("[%s] ,boundary_id,%d,  ERROR,%d,%d,%d,%d,  %d,%d,%d,%d\n", __FUNCTION__ , boundary_id,
			MAX_Area_Logo_Boundary.u16_start_x, MAX_Area_Logo_Boundary.u16_end_x, MAX_Area_Logo_Boundary.u16_start_y, MAX_Area_Logo_Boundary.u16_end_y,
			SEARCH_BOUNDARY[boundary_id].u16_start_x, SEARCH_BOUNDARY[boundary_id].u16_end_x, SEARCH_BOUNDARY[boundary_id].u16_start_y, SEARCH_BOUNDARY[boundary_id].u16_end_y);
	}

	//Logo_DDR_expand_RTK2885p(&MAX_Area_Logo_Boundary);

	MAX_Area_Logo_Boundary.u16_start_x <<= scale_gain_shift;
	MAX_Area_Logo_Boundary.u16_start_y <<= scale_gain_shift;
	MAX_Area_Logo_Boundary.u16_end_x <<= scale_gain_shift;
	MAX_Area_Logo_Boundary.u16_end_y <<= scale_gain_shift;

	if(0){
		rtd_pr_memc_emerg("[FINAL] x=%d,%d  y=%d,%d\n", MAX_Area_Logo_Boundary.u16_start_x, MAX_Area_Logo_Boundary.u16_end_x, MAX_Area_Logo_Boundary.u16_start_y, MAX_Area_Logo_Boundary.u16_end_y);
	}
	return MAX_Area_Logo_Boundary;
}

void Logo_DDR_set_dhlogo_boundary_RTK2885p(_OUTPUT_LOGO_CTRL *pOutput)
{
	MEMC_Logo_Boundary_Info Logo_Boundary;
	unsigned char u8_i;
	unsigned int logo_rim[_RIM_NUM] = {0};

	ReadRegister(KME_LOGO1_KME_LOGO1_04_reg, 0, 7, &logo_rim[_RIM_LFT]);
	ReadRegister(KME_LOGO1_KME_LOGO1_04_reg, 8,15, &logo_rim[_RIM_RHT]);
	ReadRegister(KME_LOGO1_KME_LOGO1_04_reg,16,23, &logo_rim[_RIM_TOP]);
	ReadRegister(KME_LOGO1_KME_LOGO1_04_reg,24,31, &logo_rim[_RIM_BOT]);

	for(u8_i=0; u8_i<LOGO_POSITION_NUM; u8_i++){
		Logo_Boundary = pOutput->DDR_Logo_raw[u8_i];
		Logo_Boundary.u16_start_x >>= BLK_240_LOGO_SHIFT;
		Logo_Boundary.u16_start_y >>= BLK_240_LOGO_SHIFT;
		Logo_Boundary.u16_end_x >>= BLK_240_LOGO_SHIFT;
		Logo_Boundary.u16_end_y >>= BLK_240_LOGO_SHIFT;

		Logo_Boundary.u16_start_x = (Logo_Boundary.u16_start_x<LOGO_DDR_DHLOGO_SHIFT) ? 0 : Logo_Boundary.u16_start_x-LOGO_DDR_DHLOGO_SHIFT;
		Logo_Boundary.u16_start_y = (Logo_Boundary.u16_start_y<LOGO_DDR_DHLOGO_SHIFT) ? 0 : Logo_Boundary.u16_start_y-LOGO_DDR_DHLOGO_SHIFT;
		Logo_Boundary.u16_end_x = (Logo_Boundary.u16_end_x>=LOGO_DDR_W-LOGO_DDR_DHLOGO_SHIFT) ? LOGO_DDR_W : Logo_Boundary.u16_end_x+LOGO_DDR_DHLOGO_SHIFT;
		Logo_Boundary.u16_end_y = (Logo_Boundary.u16_end_y>=LOGO_DDR_H-LOGO_DDR_DHLOGO_SHIFT) ? LOGO_DDR_H : Logo_Boundary.u16_end_y+LOGO_DDR_DHLOGO_SHIFT;

		Logo_Boundary.u16_start_x = _MAX_(Logo_Boundary.u16_start_x, logo_rim[_RIM_LFT]);
		Logo_Boundary.u16_start_y = _MAX_(Logo_Boundary.u16_start_y, logo_rim[_RIM_TOP]);
		Logo_Boundary.u16_end_x = _MIN_(Logo_Boundary.u16_end_x, logo_rim[_RIM_RHT]);
		Logo_Boundary.u16_end_y = _MIN_(Logo_Boundary.u16_end_y, logo_rim[_RIM_BOT]);
		pOutput->DDR_Logo_dhlogo[u8_i] = Logo_Boundary;
	}
}

void Logo_DDR_set_replave_mv_boundary_RTK2885p(_OUTPUT_LOGO_CTRL *pOutput)
{
	MEMC_Logo_Boundary_Info Logo_Boundary;
	unsigned char u8_i;
	unsigned int logo_rim[_RIM_NUM] = {0};

	ReadRegister(KME_LOGO1_KME_LOGO1_04_reg, 0, 7, &logo_rim[_RIM_LFT]);
	ReadRegister(KME_LOGO1_KME_LOGO1_04_reg, 8,15, &logo_rim[_RIM_RHT]);
	ReadRegister(KME_LOGO1_KME_LOGO1_04_reg,16,23, &logo_rim[_RIM_TOP]);
	ReadRegister(KME_LOGO1_KME_LOGO1_04_reg,24,31, &logo_rim[_RIM_BOT]);
	for(u8_i=0; u8_i<_RIM_NUM; u8_i++){
		logo_rim[u8_i] *= 2;
	}

	for(u8_i=0; u8_i<LOGO_POSITION_NUM; u8_i++){
	//	Logo_Boundary = pOutput->DDR_Logo_raw[u8_i];
		Logo_Boundary = pOutput->DDR_Logo_IIR[u8_i];
		Logo_Boundary.u16_start_x >>= BLK_480_LOGO_SHIFT;
		Logo_Boundary.u16_start_y >>= BLK_480_LOGO_SHIFT;
		Logo_Boundary.u16_end_x >>= BLK_480_LOGO_SHIFT;
		Logo_Boundary.u16_end_y >>= BLK_480_LOGO_SHIFT;

		Logo_Boundary.u16_start_x = (Logo_Boundary.u16_start_x<LOGO_DDR_REPLACEMV_SHIFT) ? 0 : Logo_Boundary.u16_start_x-LOGO_DDR_REPLACEMV_SHIFT;
		Logo_Boundary.u16_start_y = (Logo_Boundary.u16_start_y<LOGO_DDR_REPLACEMV_SHIFT) ? 0 : Logo_Boundary.u16_start_y-LOGO_DDR_REPLACEMV_SHIFT;
		Logo_Boundary.u16_end_x = (Logo_Boundary.u16_end_x>=LOGO_DDR_W-LOGO_DDR_REPLACEMV_SHIFT) ? LOGO_DDR_W : Logo_Boundary.u16_end_x+LOGO_DDR_REPLACEMV_SHIFT;
		Logo_Boundary.u16_end_y = (Logo_Boundary.u16_end_y>=LOGO_DDR_H-LOGO_DDR_REPLACEMV_SHIFT) ? LOGO_DDR_H : Logo_Boundary.u16_end_y+LOGO_DDR_REPLACEMV_SHIFT;

		Logo_Boundary.u16_start_x = _MAX_(Logo_Boundary.u16_start_x, logo_rim[_RIM_LFT]);
		Logo_Boundary.u16_start_y = _MAX_(Logo_Boundary.u16_start_y, logo_rim[_RIM_TOP]);
		Logo_Boundary.u16_end_x = _MIN_(Logo_Boundary.u16_end_x, logo_rim[_RIM_RHT]);
		Logo_Boundary.u16_end_y = _MIN_(Logo_Boundary.u16_end_y, logo_rim[_RIM_BOT]);
		pOutput->DDR_Logo_replacemv[u8_i] = Logo_Boundary;
	}
}

void Logo_DDR_GetLogoBoundary_RTK2885p(_OUTPUT_LOGO_CTRL *pOutput)
{
//	_PQLCONTEXT *s_pContext_w = GetPQLContext_m();
	const unsigned char center_th = 8;
	MEMC_Logo_Boundary_Info Logo_Boundary;
	MEMC_Logo_Boundary_Info DDR_Logo_pre[LOGO_POSITION_NUM];
	unsigned int Area = 0, stable_area_th = 0, stable_area_th_gain;
	unsigned char u8_i;
	unsigned int Logo_cur_cen_x, Logo_cur_cen_y, Logo_pre_cen_x, Logo_pre_cen_y, log_en, log_type, frame_pos;
	bool u1_logo_center_stable = false;
	static int Area_iir[LOGO_POSITION_NUM] = {0};
	static unsigned char u5_stable_cnt[LOGO_POSITION_NUM] = {0};
	
	ReadRegister(SOFTWARE2_SOFTWARE2_34_reg,  0,  7, &stable_area_th_gain);
	ReadRegister(SOFTWARE2_SOFTWARE2_34_reg, 16, 18, &log_type);
	ReadRegister(SOFTWARE2_SOFTWARE2_34_reg, 19, 19, &log_en);
//	ReadRegister(SOFTWARE2_SOFTWARE2_34_reg, 24, 26, &frame_pos);
	ReadRegister(SOFTWARE2_SOFTWARE2_34_reg, 24, 25, &frame_pos);
	g_frame_logo_id = _MIN_(frame_pos, LOGO_POSITION_NUM-1);

	memcpy(&DDR_Logo_pre[0], &pOutput->DDR_Logo_raw[0], sizeof(pOutput->DDR_Logo_raw));

	for(u8_i=0; u8_i<LOGO_POSITION_NUM; u8_i++){

		stable_area_th = (Area_iir[u8_i]*stable_area_th_gain/1000);

		Logo_Boundary = Search_MAX_area_boundary_RTK2885p(SEARCH_BOUNDARY[u8_i], DDR_Logo_pre[u8_i], BLK_480_LOGO_SHIFT, u8_i);

		Logo_cur_cen_x = (Logo_Boundary.u16_end_x-Logo_Boundary.u16_start_x)>>1;
		Logo_cur_cen_y = (Logo_Boundary.u16_end_y-Logo_Boundary.u16_start_y)>>1;
		Logo_pre_cen_x = (pOutput->DDR_Logo_raw[u8_i].u16_end_x-pOutput->DDR_Logo_raw[u8_i].u16_start_x)>>1;
		Logo_pre_cen_y = (pOutput->DDR_Logo_raw[u8_i].u16_end_y-pOutput->DDR_Logo_raw[u8_i].u16_start_y)>>1;
	
		Area = (Logo_Boundary.u16_end_x-Logo_Boundary.u16_start_x+1)*(Logo_Boundary.u16_end_y-Logo_Boundary.u16_start_y+1);
		Area_iir[u8_i] = (Area+Area_iir[u8_i]*3)>>2;

		// invaild when logo area too small
		Logo_Boundary.u1_vaild = (Area>MIN_LOGO_AREA_480) ? true : false;

		u1_logo_center_stable = (_ABS_DIFF_(Logo_cur_cen_x, Logo_pre_cen_x)<=center_th && _ABS_DIFF_(Logo_cur_cen_y, Logo_pre_cen_y)<=center_th);

		// check whether logo is stable or not
		//if(_ABS_DIFF_(Area_iir[u8_i], Area)<stable_area_th && Logo_Boundary.u1_vaild){
		if(u1_logo_center_stable && Logo_Boundary.u1_vaild){
			u5_stable_cnt[u8_i] = (u5_stable_cnt[u8_i]<15) ? u5_stable_cnt[u8_i]+1 : 15;
		}else{
			u5_stable_cnt[u8_i] = 0;
		}
		Logo_Boundary.u1_stable = (u5_stable_cnt[u8_i]>=15) ? true : false;

		if(log_en && u8_i==log_type){
			rtd_pr_memc_crit("stable,%d,%d,%d,%d,	x,%d,%d,	y,%d,%d,\n", 
				u5_stable_cnt[u8_i], u1_logo_center_stable, _ABS_DIFF_(Logo_cur_cen_x, Logo_pre_cen_x), _ABS_DIFF_(Logo_cur_cen_y, Logo_pre_cen_y),
				Logo_Boundary.u16_start_x, Logo_Boundary.u16_end_x, Logo_Boundary.u16_start_y, Logo_Boundary.u16_end_y);
		}

		pOutput->DDR_Logo_raw[u8_i] = Logo_Boundary;
		pOutput->DDR_Logo_cur[u8_i] = Logo_Boundary;

		if(Logo_Boundary.u1_stable){
			pOutput->DDR_Logo_IIR[u8_i].u16_start_x = (15*pOutput->DDR_Logo_IIR[u8_i].u16_start_x + Logo_Boundary.u16_start_x)>>4;
			pOutput->DDR_Logo_IIR[u8_i].u16_start_y = (15*pOutput->DDR_Logo_IIR[u8_i].u16_start_y + Logo_Boundary.u16_start_y)>>4;
			pOutput->DDR_Logo_IIR[u8_i].u16_end_x = (15*pOutput->DDR_Logo_IIR[u8_i].u16_end_x + Logo_Boundary.u16_end_x)>>4;
			pOutput->DDR_Logo_IIR[u8_i].u16_end_y = (15*pOutput->DDR_Logo_IIR[u8_i].u16_end_y + Logo_Boundary.u16_end_y)>>4;
		}else {
			pOutput->DDR_Logo_IIR[u8_i].u16_start_x = Logo_Boundary.u16_start_x;
			pOutput->DDR_Logo_IIR[u8_i].u16_start_y = Logo_Boundary.u16_start_y;
			pOutput->DDR_Logo_IIR[u8_i].u16_end_x = Logo_Boundary.u16_end_x;
			pOutput->DDR_Logo_IIR[u8_i].u16_end_y = Logo_Boundary.u16_end_y;
		}
		pOutput->DDR_Logo_IIR[u8_i].u1_stable = Logo_Boundary.u1_stable;
		pOutput->DDR_Logo_IIR[u8_i].u1_get_logo = Logo_Boundary.u1_get_logo;
		pOutput->DDR_Logo_IIR[u8_i].u1_vaild = Logo_Boundary.u1_vaild;
		
		Logo_DDR_set_replave_mv_boundary_RTK2885p(pOutput);
		Logo_DDR_set_dhlogo_boundary_RTK2885p(pOutput);
		//pOutput->DDR_Logo_replacemv[u8_i] = Logo_Boundary;
		Logo_DDR_expand_RTK2885p(&pOutput->DDR_Logo_cur[u8_i], 1);
		//Logo_DDR_expand_RTK2885p(&pOutput->DDR_Logo_replacemv[u8_i], 3);
	}
}

void Logo_DDR_FrameLogo_RTK2885p(_OUTPUT_LOGO_CTRL *pOutput)
{
//	const _PQLCONTEXT *s_pContext = GetPQLContext();
	MEMC_Logo_Boundary_Info Logo_Boundary;
	unsigned int plot_en, plot_replace_en, log_en, frame_pos;
	static bool plot_pre = false, plot_replace_pre = false;
	static unsigned char pos_id = 0, show_cnt = 0;
#if SHOW_CENTER//show center
	unsigned int center_x, center_y;
#endif

	ReadRegister(SOFTWARE2_SOFTWARE2_34_reg, 26, 26, &plot_replace_en);//show replace mv postion
	ReadRegister(SOFTWARE2_SOFTWARE2_34_reg, 27, 27, &plot_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_34_reg, 28, 28, &log_en);
//	ReadRegister(SOFTWARE2_SOFTWARE2_34_reg, 24, 26, &frame_pos);
	ReadRegister(SOFTWARE2_SOFTWARE2_34_reg, 24, 25, &frame_pos);

	// show single logo position
	if(frame_pos<LOGO_POSITION_NUM){
		pos_id = g_frame_logo_id;
//		Logo_Boundary = pOutput->DDR_Logo_raw[g_frame_logo_id];
	//Logo_Boundary = s_pContext->_output_logo_ctrl.DDR_Logo_cur[g_frame_logo_id];
	}
	// show all logo position
	else{
		show_cnt = (show_cnt<10) ? show_cnt+1 : 0;
		if(show_cnt==10){
			if(pos_id>=(LOGO_POSITION_NUM-1)){
				pos_id = 0;
			}else{
				pos_id++;
			}
		}
//		Logo_Boundary = pOutput->DDR_Logo_raw[pos_id];
	}
	Logo_Boundary = pOutput->DDR_Logo_IIR[pos_id];
#if SHOW_CENTER//show center
	center_x = (Logo_Boundary.u16_start_x+Logo_Boundary.u16_end_x)>>1;
	center_y = (Logo_Boundary.u16_start_y+Logo_Boundary.u16_end_y)>>1;
#endif

	if(plot_en && Logo_Boundary.u1_stable==true && Logo_Boundary.u1_vaild==true && Logo_Boundary.u1_get_logo==true){// area_stable_cnt>=5
#if SHOW_CENTER//show center
		WriteRegister(KPOST_TOP_KPOST_TOP_00_reg,  0, 12, center_x);
		WriteRegister(KPOST_TOP_KPOST_TOP_00_reg, 16, 28, center_y);
#else //show boundary
		WriteRegister(KPOST_TOP_KPOST_TOP_00_reg,  0, 12, Logo_Boundary.u16_start_x);
		WriteRegister(KPOST_TOP_KPOST_TOP_00_reg, 16, 28, Logo_Boundary.u16_start_y);
#endif
		WriteRegister(KPOST_TOP_KPOST_TOP_00_reg, 13, 13, 1);
		WriteRegister(KPOST_TOP_KPOST_TOP_00_reg, 15, 15, 1);
		
		//WriteRegister(MC_MC_CC_reg,  0, 23, 0xff004c);
#if !SHOW_CENTER //show boundary
		WriteRegister(MC_MC_C0_reg,  0, 11, Logo_Boundary.u16_end_x);
		WriteRegister(MC_MC_C0_reg, 16, 27, Logo_Boundary.u16_end_y);
		WriteRegister(MC_MC_CC_reg, 24, 24, 1);
		WriteRegister(MC_MC_CC_reg, 26, 26, 1);
#endif

		plot_pre = true;
	}
	else if(plot_pre){
		WriteRegister(KPOST_TOP_KPOST_TOP_00_reg, 13, 13, 0);
		WriteRegister(KPOST_TOP_KPOST_TOP_00_reg, 15, 15, 0);
#if !SHOW_CENTER //show boundary
		WriteRegister(MC_MC_CC_reg, 24, 24, 0);
		WriteRegister(MC_MC_CC_reg, 26, 26, 0);
#endif
		plot_pre = false;
	}

	if(plot_replace_en && pOutput->u1_apply_replacemv[pos_id]){
#if SHOW_CENTER
		WriteRegister(MC_MC_C0_reg,  0, 11, center_x);
		WriteRegister(MC_MC_C0_reg, 16, 27, center_y);
#endif
		WriteRegister(MC_MC_CC_reg, 24, 24, 1);
		WriteRegister(MC_MC_CC_reg, 26, 26, 1);
		plot_replace_pre = true;
	}else if(plot_replace_pre){
		WriteRegister(MC_MC_CC_reg, 24, 24, 0);
		WriteRegister(MC_MC_CC_reg, 26, 26, 0);
		plot_replace_pre = false;
	}

	if(log_en){
		rtd_pr_memc_emerg("[%s] %d,%d,%d, x,%d,%d,   y,%d,%d,\n", __FUNCTION__, Logo_Boundary.u1_vaild, Logo_Boundary.u1_stable, Logo_Boundary.u1_get_logo, Logo_Boundary.u16_start_x, Logo_Boundary.u16_end_x, Logo_Boundary.u16_start_y, Logo_Boundary.u16_end_y);
	}
}

#define MAX_LINE_NUM	(10)
MEMC_Logo_Boundary Line_Area[MAX_LINE_NUM] = {0};
unsigned char g_Line_num = 0;

void Logo_DDR_LineInit_RTK2885p(void)
{
	g_Line_num = 0;
	memset(Line_Area, 0, sizeof(Line_Area));
}

#if 0
void Logo_DDR_Rough_Clear_LineLogo(bool h_direction, unsigned short u16_pos)
{


	if(h_direction){
		if(logo_flag[0]==1 && logo_flag[1]==1 && logo_flag[2]==1 && logo_flag[3]==1){
			logo_flag_array[][u16_pos]
		}
	}
}
#endif

//it's for precise searching continue logo
void Logo_DDR_DetectContinueLogo_RTK2885p(bool h_direction, unsigned short u16_pos)
{
	unsigned char logo_num = 0, logo_num_th = 0, record_start_id = 0, record_end_id = 0;
	unsigned short u16_i = 0, u16_j = 0, u16_x = 0, u16_y = 0;
	unsigned int u4_continue_num = 0;
	static bool detect_logo = false;
	bool first_record = true, keep_search = false;
	//static MEMC_Logo_Boundary Logo_Area_pre[2][4]={0};
//	unsigned short new_record_id[MAX_LINE_NUM] = {0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff};
	
	ReadRegister(SOFTWARE2_SOFTWARE2_32_reg, 24, 27, &u4_continue_num);
	u4_continue_num = _MAX_(u4_continue_num, 3);
	logo_num_th = u4_continue_num-2;

	//memcpy(&Logo_Area_pre[2], &Logo_Area_pre[1], sizeof(MEMC_Logo_Boundary));
	//memcpy(&Logo_Area_pre[1], &Logo_Area_pre[0], sizeof(MEMC_Logo_Boundary));

	if(g_Line_num>=MAX_LINE_NUM-1){
		return;
	}

	if(h_direction==true && u16_pos>=LOGO_DDR_H){
		rtd_pr_memc_emerg("[%s] ERROR direction,%d,  pos,%d\n", __FUNCTION__, h_direction, u16_pos);
		return;
	}
	if(h_direction==false && u16_pos>=LOGO_DDR_W){
		rtd_pr_memc_emerg("[%s] ERROR direction,%d,  pos,%d\n", __FUNCTION__, h_direction, u16_pos);
		return;
	}

	detect_logo=false;

	if(h_direction){
		
		// search current line
		for(u16_i=0; u16_i<LOGO_DDR_W-u4_continue_num; u16_i++){

				if(logo_flag_array[u16_i][u16_pos]==0 && detect_logo==false){
					//detect_logo=false;
					continue;
				}

				// no search with already search area

				logo_num = 1;
				for(u16_j=1; u16_j<u4_continue_num; u16_j++){
					logo_num += logo_flag_array[u16_i+u16_j][u16_pos];
				}

				if(logo_num>=logo_num_th){
					if(detect_logo==false){
						g_Line_num = (first_record) ? g_Line_num : g_Line_num+1;
						g_Line_num = _MIN_(g_Line_num, MAX_LINE_NUM-1);
						Line_Area[g_Line_num].u16_start_x = u16_i;
						Line_Area[g_Line_num].u16_end_x = u16_i+u4_continue_num;
						Line_Area[g_Line_num].u16_start_y = u16_pos;
						Line_Area[g_Line_num].u16_end_y = u16_pos;

						record_start_id = (first_record) ? g_Line_num : record_start_id;
						record_end_id = g_Line_num;
						
						first_record=false;
						//u16_i = u16_i+u4_continue_num;
					}
					else{
						//g_Line_num = _MIN_(g_Line_num, MAX_LINE_NUM-1);
						Line_Area[g_Line_num].u16_end_x = u16_i+u4_continue_num;
					}
					detect_logo = true;
				}else{
					detect_logo = false;
				}
		}

		for(u16_i=record_start_id; u16_i<=record_end_id; u16_i++){
			// check whether y pos is in bottom
			if(Line_Area[u16_i].u16_end_y==LOGO_DDR_H-1){
				continue;
			}
			
			keep_search = true;
			for(u16_y=Line_Area[u16_i].u16_end_y+1; u16_y<LOGO_DDR_H && keep_search==true; u16_y++){
				
				logo_num = 0;
				for(u16_x=Line_Area[u16_i].u16_start_x; u16_x<=Line_Area[u16_i].u16_end_x; u16_x++){
					logo_num += logo_flag_array[u16_x][u16_y];
				}
				
				// check next line whether has enough logo
				if(logo_num < (Line_Area[u16_i].u16_end_x-Line_Area[u16_i].u16_start_x)>>1){
					keep_search = true;
					Line_Area[u16_i].u16_end_y = u16_y;
				}else{
					keep_search = false;
				}
			}
		}

	}
}

void Logo_DDR_DetectLine_RTK2885p(void)
{
	const unsigned short Hist_H_Limit = (LOGO_DDR_W*3)>>2;
	const unsigned short Hist_V_Limit = (LOGO_DDR_H*3)>>2;
	unsigned short Hist_H[LOGO_DDR_W]={0}, Hist_V[LOGO_DDR_H]={0};
	unsigned short u16_x = 0, u16_y = 0, max_hist_h = 0, max_hist_h_id = 0, max_hist_v = 0, max_hist_v_id = 0;
	unsigned int algo_en, log_en, plot_en, plot_direction;

	ReadRegister(SOFTWARE2_SOFTWARE2_32_reg, 31, 31, &algo_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_32_reg, 30, 30, &plot_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_32_reg, 29, 29, &plot_direction);
	ReadRegister(SOFTWARE2_SOFTWARE2_32_reg, 28, 28, &log_en);

	if(!algo_en){
		return;
	}

	Logo_DDR_LineInit_RTK2885p();

	for(u16_x=0; u16_x<LOGO_DDR_W; u16_x++){
		for(u16_y=0; u16_y<LOGO_DDR_H; u16_y++){
			if(logo_flag_array[u16_x][u16_y]==1){
				Hist_H[u16_x]++;
				Hist_V[u16_y]++;
			}
		}
	}

	//Logo_DDR_Rough_Clear_LineLogo();

	for(u16_x=0; u16_x<LOGO_DDR_W; u16_x++){
		if(max_hist_h < Hist_H[u16_x]){
			max_hist_h = Hist_H[u16_x];
			max_hist_h_id = u16_x;
		}
	}

	for(u16_y=0; u16_y<LOGO_DDR_H; u16_y++){
		if(max_hist_v < Hist_V[u16_y]){
			max_hist_v = Hist_V[u16_y];
			max_hist_v_id = u16_y;
		}
		//if(Hist_V[u16_y]>Hist_V_Limit){
		//	Logo_DDR_DetectContinueLogo_RTK2885p(1, u16_y);
		//}
	}

	if(plot_en){//d838[31]
		// horizont line
		if(plot_direction==0){
			if(max_hist_v>Hist_V_Limit){
				WriteRegister(MC_MC_CC_reg, 25, 25, 1);
				WriteRegister(MC_MC_CC_reg, 27, 27, 1);
				WriteRegister(MC_MC_C8_reg,  0, 11, 1920);
				WriteRegister(MC_MC_C8_reg, 16, 27, max_hist_v_id*8);
				WriteRegister(MC_MC_CC_reg,  0,  7, 0x95);
			}else{
				WriteRegister(MC_MC_CC_reg, 25, 25, 0);
				WriteRegister(MC_MC_CC_reg, 27, 27, 0);
			}
		}else{
			if(max_hist_h>Hist_H_Limit){
				WriteRegister(MC_MC_CC_reg, 25, 25, 1);
				WriteRegister(MC_MC_CC_reg, 27, 27, 1);
				WriteRegister(MC_MC_C8_reg,  0, 11, max_hist_h_id*8);
				WriteRegister(MC_MC_C8_reg, 16, 27, 1080);
				WriteRegister(MC_MC_CC_reg,  0,  7, 0x95);
			}else{
				WriteRegister(MC_MC_CC_reg, 25, 25, 0);
				WriteRegister(MC_MC_CC_reg, 27, 27, 0);
			}

		}
	}

	if(log_en){//d838[30]
		rtd_pr_memc_emerg("[%s] H,%d, V,%d\n", __FUNCTION__, Hist_H[LOGO_DDR_W-1], Hist_V[210]);
	}

#if 0
	if(log_en_line){//d838[29]
		rtd_pr_memc_emerg("[%s] num,%d,  %d,%d,%d,  %d,%d,%d,  %d,%d,%d,  %d,%d,%d,  %d,%d,%d,  %d,%d,%d,  %d,%d,%d,  %d,%d,%d,  %d,%d,%d,  %d,%d,%d,\n", __FUNCTION__, g_Line_num,
			Line_Area[0].u16_start_x, Line_Area[0].u16_end_x, Line_Area[0].u16_start_y, 
			Line_Area[1].u16_start_x, Line_Area[1].u16_end_x, Line_Area[1].u16_start_y, 
			Line_Area[2].u16_start_x, Line_Area[2].u16_end_x, Line_Area[2].u16_start_y, 
			Line_Area[3].u16_start_x, Line_Area[3].u16_end_x, Line_Area[3].u16_start_y, 
			Line_Area[4].u16_start_x, Line_Area[4].u16_end_x, Line_Area[4].u16_start_y, 
			Line_Area[5].u16_start_x, Line_Area[5].u16_end_x, Line_Area[5].u16_start_y, 
			Line_Area[6].u16_start_x, Line_Area[6].u16_end_x, Line_Area[6].u16_start_y, 
			Line_Area[7].u16_start_x, Line_Area[7].u16_end_x, Line_Area[7].u16_start_y, 
			Line_Area[8].u16_start_x, Line_Area[8].u16_end_x, Line_Area[8].u16_start_y, 
			Line_Area[9].u16_start_x, Line_Area[9].u16_end_x, Line_Area[9].u16_start_y);
	}
#endif
}

void Logo_DDR_Get_Logo_Rim_RTK2885p(unsigned short *u16_start_x, unsigned short *u16_end_x, unsigned short *u16_start_y, unsigned short *u16_end_y)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned short u16_rim_lft = (s_pContext->_output_rimctrl.u12_LogoRim_Pxl[_RIM_LFT])>>1;
	unsigned short u16_rim_top = (s_pContext->_output_rimctrl.u12_LogoRim_Pxl[_RIM_TOP])>>1;
	unsigned short u16_rim_rht = (s_pContext->_output_rimctrl.u12_LogoRim_Pxl[_RIM_RHT])>>1;
	unsigned short u16_rim_bot = (s_pContext->_output_rimctrl.u12_LogoRim_Pxl[_RIM_BOT])>>1;

	Whole_Search_Range.u16_start_x = (u16_rim_lft+LOGO_DDR_RIM_SHIFT < LOGO_DDR_W) ? u16_rim_lft+LOGO_DDR_RIM_SHIFT : LOGO_DDR_W-1;
	Whole_Search_Range.u16_start_y = (u16_rim_top+LOGO_DDR_RIM_SHIFT < LOGO_DDR_H) ? u16_rim_top+LOGO_DDR_RIM_SHIFT : LOGO_DDR_H-1;
	Whole_Search_Range.u16_end_x = (u16_rim_rht>LOGO_DDR_RIM_SHIFT) ? u16_rim_rht-LOGO_DDR_RIM_SHIFT : 0;
	Whole_Search_Range.u16_end_y = (u16_rim_bot>LOGO_DDR_RIM_SHIFT) ? u16_rim_bot-LOGO_DDR_RIM_SHIFT : 0;

	SEARCH_BOUNDARY[LOGO_LEFT_UP].u16_start_x = Whole_Search_Range.u16_start_x;
	SEARCH_BOUNDARY[LOGO_LEFT_UP].u16_start_y = Whole_Search_Range.u16_start_y;
	SEARCH_BOUNDARY[LOGO_RIGHT_UP].u16_end_x = Whole_Search_Range.u16_end_x;
	SEARCH_BOUNDARY[LOGO_RIGHT_UP].u16_start_y = Whole_Search_Range.u16_start_y;
	SEARCH_BOUNDARY[LOGO_LEFT_LOW].u16_start_x = Whole_Search_Range.u16_start_x;
	SEARCH_BOUNDARY[LOGO_LEFT_LOW].u16_end_y = Whole_Search_Range.u16_end_y ;
	SEARCH_BOUNDARY[LOGO_RIGHT_LOW].u16_end_x = Whole_Search_Range.u16_end_x ;
	SEARCH_BOUNDARY[LOGO_RIGHT_LOW].u16_end_y = Whole_Search_Range.u16_end_y ;
//	SEARCH_BOUNDARY[LOGO_CENTER_LOW].u16_end_y = Whole_Search_Range.u16_end_y ;

	*u16_start_x = Whole_Search_Range.u16_start_x;
	*u16_start_y = Whole_Search_Range.u16_start_y;
	*u16_end_x = Whole_Search_Range.u16_end_x;
	*u16_end_y = Whole_Search_Range.u16_end_y;
}

void Logo_DDR_Clear_LogoRim_RTK2885p(void)
{
	unsigned int u1_clear_en;
	unsigned short u16_x = 0, u16_y = 0;
	unsigned short u16_rim_lft = Whole_Search_Range.u16_start_x;
	unsigned short u16_rim_top = Whole_Search_Range.u16_start_y;
	unsigned short u16_rim_rht = Whole_Search_Range.u16_end_x;
	unsigned short u16_rim_bot = Whole_Search_Range.u16_end_y;

	ReadRegister(SOFTWARE2_SOFTWARE2_33_reg, 6, 6, &u1_clear_en);

	if(!u1_clear_en){//d948[6]
		return;
	}

	for(u16_x=0; u16_x<=u16_rim_lft; u16_x++){
		for(u16_y=0; u16_y<LOGO_DDR_H; u16_y++){
			logo_flag_array[u16_x][u16_y] = 0;
		}
	}
	for(u16_x=u16_rim_rht; u16_x<LOGO_DDR_W; u16_x++){
		for(u16_y=0; u16_y<LOGO_DDR_H; u16_y++){
			logo_flag_array[u16_x][u16_y] = 0;
		}
	}

	for(u16_x=u16_rim_lft+1; u16_x<u16_rim_rht; u16_x++){
		for(u16_y=0; u16_y<=u16_rim_top; u16_y++){
			logo_flag_array[u16_x][u16_y] = 0;
		}
	}
	for(u16_x=u16_rim_lft+1; u16_x<u16_rim_rht; u16_x++){
		for(u16_y=u16_rim_bot; u16_y<LOGO_DDR_H; u16_y++){
			logo_flag_array[u16_x][u16_y] = 0;
		}
	}

	//rtd_pr_memc_emerg("RIM,%d,%d,%d,%d", s_pContext->_output_rimctrl.u8_LogoRim_Blk[_RIM_LFT] ,s_pContext->_output_rimctrl.u8_LogoRim_Blk[_RIM_RHT] ,s_pContext->_output_rimctrl.u8_LogoRim_Blk[_RIM_TOP] ,s_pContext->_output_rimctrl.u8_LogoRim_Blk[_RIM_BOT]);
}

void Logo_DDR_IIR_RTK2885p(void)
{
	unsigned int u1_iir_enable, weight_1, weight_2, irr_th, type_sel;
	unsigned short u16_x, u16_y;
	ReadRegister(SOFTWARE2_SOFTWARE2_33_reg, 4,  4, &type_sel);
	ReadRegister(SOFTWARE2_SOFTWARE2_33_reg, 7,  7, &u1_iir_enable);
	ReadRegister(SOFTWARE2_SOFTWARE2_33_reg, 8, 15, &weight_1);
	ReadRegister(SOFTWARE2_SOFTWARE2_33_reg,16, 23, &irr_th);//50
	
	weight_1 = _MIN_(weight_1, 128);
	weight_2 = 128 - weight_1;

	if(!u1_iir_enable){//d948[7]
		memcpy(&logo_flag_480_raw, &logo_flag_array, sizeof(logo_flag_array));
		return;
	}

	if(type_sel==0){
		for(u16_y=0; u16_y<LOGO_DDR_H; u16_y++) {
			for(u16_x=0; u16_x<LOGO_DDR_W; u16_x++) {
				logo_iir_array[u16_x][u16_y] = (weight_1*logo_flag_480_raw[u16_x][u16_y]*255 + weight_2*logo_iir_array[u16_x][u16_y])>>7;
				logo_flag_array[u16_x][u16_y] = (logo_iir_array[u16_x][u16_y]>irr_th) ? 1 : 0;
			}
		}
	}else{
		memset(logo_flag_array, 0, sizeof(logo_flag_array));
		for(u16_y=0; u16_y<LOGO_DDR_H; u16_y++) {
			for(u16_x=0; u16_x<LOGO_DDR_W; u16_x++) {
				logo_iir_array[u16_x][u16_y] = (weight_1*logo_flag_480_raw[u16_x][u16_y]*255 + weight_2*logo_iir_array[u16_x][u16_y])>>7;
				if(logo_iir_array[u16_x][u16_y]>irr_th){
					logo_flag_array[u16_x][u16_y] = 1;
				}
			}
		}
	}
}

void Logo_DDR_close_DhLogo_RTK2885p(_OUTPUT_LOGO_CTRL *pOutput)
{
//	const _PQLCONTEXT *s_pContext = GetPQLContext();
	MEMC_Logo_Boundary_Info Logo_Boundary = pOutput->DDR_Logo_raw[g_frame_logo_id];
	kme_dehalo6_dh_logo_protect_win1_RBUS kme_dehalo6_dh_logo_protect_win1;
	kme_dehalo6_dh_logo_protect_win2_RBUS kme_dehalo6_dh_logo_protect_win2;
	kme_dehalo6_dh_logo_protect_win3_RBUS kme_dehalo6_dh_logo_protect_win3;
	kme_dehalo6_dh_logo_protect_win4_RBUS kme_dehalo6_dh_logo_protect_win4;
	kme_dehalo6_dh_logo_blkrg_cnt_idx30_31_win_en_RBUS kme_dehalo6_dh_logo_blkrg_cnt_idx30_31_win_en;
	unsigned int algo_en, log_en, select_mode;
	unsigned char u8_i;
	static bool last_repalce = false;

	kme_dehalo6_dh_logo_protect_win1.regValue = rtd_inl(KME_DEHALO6_dh_logo_protect_win1_reg);
	kme_dehalo6_dh_logo_protect_win2.regValue = rtd_inl(KME_DEHALO6_dh_logo_protect_win2_reg);
	kme_dehalo6_dh_logo_protect_win3.regValue = rtd_inl(KME_DEHALO6_dh_logo_protect_win3_reg);
	kme_dehalo6_dh_logo_protect_win4.regValue = rtd_inl(KME_DEHALO6_dh_logo_protect_win4_reg);
	kme_dehalo6_dh_logo_blkrg_cnt_idx30_31_win_en.regValue = rtd_inl(KME_DEHALO6_dh_logo_blkrg_cnt_idx30_31_win_en_reg);

	ReadRegister(SOFTWARE2_SOFTWARE2_32_reg,  0,  0, &algo_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_32_reg,  1,  1, &select_mode);
	ReadRegister(SOFTWARE2_SOFTWARE2_32_reg,  2,  2, &log_en);

	if(!algo_en){
		if(last_repalce){
			last_repalce = false;
			kme_dehalo6_dh_logo_blkrg_cnt_idx30_31_win_en.dh_logo_win1_en = 0;
			kme_dehalo6_dh_logo_blkrg_cnt_idx30_31_win_en.dh_logo_win2_en = 0;
			kme_dehalo6_dh_logo_blkrg_cnt_idx30_31_win_en.dh_logo_win3_en = 0;
			kme_dehalo6_dh_logo_blkrg_cnt_idx30_31_win_en.dh_logo_win4_en = 0;
			rtd_outl(KME_DEHALO6_dh_logo_blkrg_cnt_idx30_31_win_en_reg, kme_dehalo6_dh_logo_blkrg_cnt_idx30_31_win_en.regValue);
		}
		return;
	}

	if(select_mode){
		for(u8_i=0; u8_i<4; u8_i++){
			Logo_Boundary = pOutput->DDR_Logo_dhlogo[u8_i];
			if(Logo_Boundary.u1_stable==true && Logo_Boundary.u1_vaild==true && Logo_Boundary.u1_get_logo==true){
				if(u8_i==0){
					kme_dehalo6_dh_logo_protect_win1.dh_logo_win1_left = Logo_Boundary.u16_start_x;
					kme_dehalo6_dh_logo_protect_win1.dh_logo_win1_right = Logo_Boundary.u16_end_x;
					kme_dehalo6_dh_logo_protect_win1.dh_logo_win1_top = Logo_Boundary.u16_start_y;
					kme_dehalo6_dh_logo_protect_win1.dh_logo_win1_bottom = Logo_Boundary.u16_end_y;
					kme_dehalo6_dh_logo_blkrg_cnt_idx30_31_win_en.dh_logo_win1_en = 1;
				}else if(u8_i==1){
					kme_dehalo6_dh_logo_protect_win2.dh_logo_win2_left = Logo_Boundary.u16_start_x;
					kme_dehalo6_dh_logo_protect_win2.dh_logo_win2_right = Logo_Boundary.u16_end_x;
					kme_dehalo6_dh_logo_protect_win2.dh_logo_win2_top = Logo_Boundary.u16_start_y;
					kme_dehalo6_dh_logo_protect_win2.dh_logo_win2_bottom = Logo_Boundary.u16_end_y;
					kme_dehalo6_dh_logo_blkrg_cnt_idx30_31_win_en.dh_logo_win2_en = 1;
				}else if(u8_i==2){
					kme_dehalo6_dh_logo_protect_win3.dh_logo_win3_left = Logo_Boundary.u16_start_x;
					kme_dehalo6_dh_logo_protect_win3.dh_logo_win3_right = Logo_Boundary.u16_end_x;
					kme_dehalo6_dh_logo_protect_win3.dh_logo_win3_top = Logo_Boundary.u16_start_y;
					kme_dehalo6_dh_logo_protect_win3.dh_logo_win3_bottom = Logo_Boundary.u16_end_y;
					kme_dehalo6_dh_logo_blkrg_cnt_idx30_31_win_en.dh_logo_win3_en = 1;
				}else if(u8_i==3){
					kme_dehalo6_dh_logo_protect_win4.dh_logo_win4_left = Logo_Boundary.u16_start_x;
					kme_dehalo6_dh_logo_protect_win4.dh_logo_win4_right = Logo_Boundary.u16_end_x;
					kme_dehalo6_dh_logo_protect_win4.dh_logo_win4_top = Logo_Boundary.u16_start_y;
					kme_dehalo6_dh_logo_protect_win4.dh_logo_win4_bottom = Logo_Boundary.u16_end_y;
					kme_dehalo6_dh_logo_blkrg_cnt_idx30_31_win_en.dh_logo_win4_en = 1;
				}
			}else {
				if(u8_i==0){
					kme_dehalo6_dh_logo_blkrg_cnt_idx30_31_win_en.dh_logo_win1_en = 0;
				}else if(u8_i==1){
					kme_dehalo6_dh_logo_blkrg_cnt_idx30_31_win_en.dh_logo_win2_en = 0;
				}else if(u8_i==2){
					kme_dehalo6_dh_logo_blkrg_cnt_idx30_31_win_en.dh_logo_win3_en = 0;
				}else if(u8_i==3){
					kme_dehalo6_dh_logo_blkrg_cnt_idx30_31_win_en.dh_logo_win4_en = 0;
				}
			}
		}
	}else{
		Logo_Boundary = pOutput->DDR_Logo_dhlogo[g_frame_logo_id];
		if(Logo_Boundary.u1_stable==true && Logo_Boundary.u1_vaild==true && Logo_Boundary.u1_get_logo==true){
			kme_dehalo6_dh_logo_protect_win1.dh_logo_win1_left = Logo_Boundary.u16_start_x;
			kme_dehalo6_dh_logo_protect_win1.dh_logo_win1_right = Logo_Boundary.u16_end_x;
			kme_dehalo6_dh_logo_protect_win1.dh_logo_win1_top = Logo_Boundary.u16_start_y;
			kme_dehalo6_dh_logo_protect_win1.dh_logo_win1_bottom = Logo_Boundary.u16_end_y;
			kme_dehalo6_dh_logo_blkrg_cnt_idx30_31_win_en.dh_logo_win1_en = 1;
		}else {
			kme_dehalo6_dh_logo_blkrg_cnt_idx30_31_win_en.dh_logo_win1_en = 0;
		}

	}

	rtd_outl(KME_DEHALO6_dh_logo_protect_win1_reg, kme_dehalo6_dh_logo_protect_win1.regValue);
	rtd_outl(KME_DEHALO6_dh_logo_protect_win2_reg, kme_dehalo6_dh_logo_protect_win2.regValue);
	rtd_outl(KME_DEHALO6_dh_logo_protect_win3_reg, kme_dehalo6_dh_logo_protect_win3.regValue);
	rtd_outl(KME_DEHALO6_dh_logo_protect_win4_reg, kme_dehalo6_dh_logo_protect_win4.regValue);
	rtd_outl(KME_DEHALO6_dh_logo_blkrg_cnt_idx30_31_win_en_reg, kme_dehalo6_dh_logo_blkrg_cnt_idx30_31_win_en.regValue);

	last_repalce = true;
}

unsigned short Logo_DDR_LogoBoundary_MAX_rmv_RTK2885p(MEMC_Logo_Boundary_Info *Logo_Boundary)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	unsigned char u8_rgn_id = 0, u8_x = 0, u8_y =0, u8_x_start_id = 0, u8_x_end_id = 0, u8_y_start_id = 0,u8_y_end_id = 0;
	unsigned short Max_Rmv = 0;

	if(Logo_Boundary->u1_vaild == false){
		return 0;
	}

	u8_x_start_id = Logo_Boundary->u16_start_x/480;
	u8_y_start_id = Logo_Boundary->u16_start_y/540;
	u8_x_end_id = Logo_Boundary->u16_end_x/480;
	u8_y_end_id = Logo_Boundary->u16_end_y/540;

	for(u8_x=u8_x_start_id; u8_x<=u8_x_end_id; u8_x++){
		for(u8_y=u8_y_start_id; u8_y<=u8_y_end_id; u8_y++){
			u8_rgn_id = u8_y*8 + u8_x;
			Max_Rmv = _MAX_(_MAX_(Max_Rmv, _ABS_(s_pContext->_output_read_comreg.s11_me_rMV_1st_vx_rb[u8_rgn_id])), _ABS_(s_pContext->_output_read_comreg.s10_me_rMV_1st_vy_rb[u8_rgn_id]));
		}
	}

	return Max_Rmv;
}

void Logo_DDR_Apply_Replace_mv_RTK2885p(_OUTPUT_LOGO_CTRL *pOutput)
{
	mc3_logo_dh_window0_0_RBUS mc3_logo_dh_window0_0;
	mc3_logo_dh_window0_1_RBUS mc3_logo_dh_window0_1;
	mc3_logo_dh_window1_0_RBUS mc3_logo_dh_window1_0;
	mc3_logo_dh_window1_1_RBUS mc3_logo_dh_window1_1;
	mc3_logo_dh_window2_0_RBUS mc3_logo_dh_window2_0;
	mc3_logo_dh_window2_1_RBUS mc3_logo_dh_window2_1;
	mc3_logo_dh_window3_0_RBUS mc3_logo_dh_window3_0;
	mc3_logo_dh_window3_1_RBUS mc3_logo_dh_window3_1;
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	MEMC_Logo_Boundary_Info Logo_set_boundary;
	MEMC_Logo_Boundary_Info Logo_Boundary;
	unsigned short u16_max_mv[4] = {0};
	unsigned int algo_en, select_mode, rmv_th, u8_max_mv_diff_th = 0, max_drastic_chg_cnt = 0, u16_logo_th = 0, log_en;
	unsigned char u8_i;
	unsigned short Max_Rmv = 0;
	bool u1_logo[4] = {0};
	static bool last_repalce = false;
	static unsigned char u8_corner_mv_drastic_chg_cnt[4] = {0};
	static unsigned short u16_max_mv_pre[4] = {0};

	mc3_logo_dh_window0_0.regValue = rtd_inl(MC3_logo_dh_window0_0_reg);
	mc3_logo_dh_window0_1.regValue = rtd_inl(MC3_logo_dh_window0_1_reg);
	mc3_logo_dh_window1_0.regValue = rtd_inl(MC3_logo_dh_window1_0_reg);
	mc3_logo_dh_window1_1.regValue = rtd_inl(MC3_logo_dh_window1_1_reg);
	mc3_logo_dh_window2_0.regValue = rtd_inl(MC3_logo_dh_window2_0_reg);
	mc3_logo_dh_window2_1.regValue = rtd_inl(MC3_logo_dh_window2_1_reg);
	mc3_logo_dh_window3_0.regValue = rtd_inl(MC3_logo_dh_window3_0_reg);
	mc3_logo_dh_window3_1.regValue = rtd_inl(MC3_logo_dh_window3_1_reg);
	ReadRegister(SOFTWARE2_SOFTWARE2_32_reg,  4,  4, &algo_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_32_reg,  5,  5, &select_mode);
	ReadRegister(SOFTWARE2_SOFTWARE2_32_reg,  6,  6, &log_en);
	ReadRegister(SOFTWARE2_SOFTWARE2_32_reg,  8, 15, &rmv_th);
	ReadRegister(SOFTWARE3_SOFTWARE3_32_reg,  0,  7, &max_drastic_chg_cnt);
	ReadRegister(SOFTWARE3_SOFTWARE3_32_reg, 16, 23, &u8_max_mv_diff_th);
	ReadRegister(SOFTWARE3_SOFTWARE3_44_reg,  0,  9, &u16_logo_th);

	g_logo_ddr_replace_mv_en = (algo_en==1) ? true : false;

	// detect corner max rmv
	u16_max_mv[0] = _MAX_(s_pContext->_output_me_sceneAnalysis.u11_lt_1st_max_rmv, s_pContext->_output_me_sceneAnalysis.u11_lt_2nd_max_rmv);
	u16_max_mv[1] = _MAX_(s_pContext->_output_me_sceneAnalysis.u11_rt_1st_max_rmv, s_pContext->_output_me_sceneAnalysis.u11_rt_2nd_max_rmv);
	u16_max_mv[2] = _MAX_(s_pContext->_output_me_sceneAnalysis.u11_lb_1st_max_rmv, s_pContext->_output_me_sceneAnalysis.u11_lb_2nd_max_rmv);
	u16_max_mv[3] = _MAX_(s_pContext->_output_me_sceneAnalysis.u11_rb_1st_max_rmv, s_pContext->_output_me_sceneAnalysis.u11_rb_2nd_max_rmv);
//	u16_max_mv[4] = _MAX_(s_pContext->_output_me_sceneAnalysis.u11_bot_1st_max_rmv, s_pContext->_output_me_sceneAnalysis.u11_bot_2nd_max_rmv);
	// detect corner logo
	u1_logo[0] = (s_pContext->_output_FRC_LgDet.lt_logo_cnt_avg_IIR > u16_logo_th) ? true : false;
	u1_logo[1] = (s_pContext->_output_FRC_LgDet.rt_logo_cnt_avg_IIR > u16_logo_th) ? true : false;
	u1_logo[2] = (s_pContext->_output_FRC_LgDet.lb_logo_cnt_avg_IIR > u16_logo_th) ? true : false;
	u1_logo[3] = (s_pContext->_output_FRC_LgDet.rb_logo_cnt_avg_IIR > u16_logo_th) ? true : false;
//	u1_logo[4] = (s_pContext->_output_FRC_LgDet.bot_logo_cnt_avg_IIR > u16_logo_th) ? true : false;

	for(u8_i=0; u8_i<4; u8_i++){
		if(u1_logo[u8_i]==true && _ABS_DIFF_(u16_max_mv_pre[u8_i], u16_max_mv[u8_i])>u8_max_mv_diff_th){
			u8_corner_mv_drastic_chg_cnt[u8_i] = max_drastic_chg_cnt;
		}else if(u8_corner_mv_drastic_chg_cnt[u8_i]>0){
			u8_corner_mv_drastic_chg_cnt[u8_i]--;
		}
		u16_max_mv_pre[u8_i] = u16_max_mv[u8_i];
	}

	if(!g_logo_ddr_replace_mv_en){
		if(last_repalce){
			last_repalce = false;
			
			mc3_logo_dh_window0_0.mc_logo_dh_window0_l = 0;
			mc3_logo_dh_window0_0.mc_logo_dh_window0_r = 0;
			mc3_logo_dh_window0_1.mc_logo_dh_window0_t = 0;
			mc3_logo_dh_window0_1.mc_logo_dh_window0_b = 0;

			mc3_logo_dh_window1_0.mc_logo_dh_window1_l = 0;
			mc3_logo_dh_window1_0.mc_logo_dh_window1_r = 0;
			mc3_logo_dh_window1_1.mc_logo_dh_window1_t = 0;
			mc3_logo_dh_window1_1.mc_logo_dh_window1_b = 0;

			mc3_logo_dh_window2_0.mc_logo_dh_window2_l = 0;
			mc3_logo_dh_window2_0.mc_logo_dh_window2_r = 0;
			mc3_logo_dh_window2_1.mc_logo_dh_window2_t = 0;
			mc3_logo_dh_window2_1.mc_logo_dh_window2_b = 0;

			mc3_logo_dh_window3_0.mc_logo_dh_window3_l = 0;
			mc3_logo_dh_window3_0.mc_logo_dh_window3_r = 0;
			mc3_logo_dh_window3_1.mc_logo_dh_window3_t = 0;
			mc3_logo_dh_window3_1.mc_logo_dh_window3_b = 0;

			rtd_outl(MC3_logo_dh_window0_0_reg, mc3_logo_dh_window0_0.regValue);
			rtd_outl(MC3_logo_dh_window0_1_reg, mc3_logo_dh_window0_1.regValue);
			rtd_outl(MC3_logo_dh_window1_0_reg, mc3_logo_dh_window1_0.regValue);
			rtd_outl(MC3_logo_dh_window1_1_reg, mc3_logo_dh_window1_1.regValue);
			rtd_outl(MC3_logo_dh_window2_0_reg, mc3_logo_dh_window2_0.regValue);
			rtd_outl(MC3_logo_dh_window2_1_reg, mc3_logo_dh_window2_1.regValue);
			rtd_outl(MC3_logo_dh_window3_0_reg, mc3_logo_dh_window3_0.regValue);
			rtd_outl(MC3_logo_dh_window3_1_reg, mc3_logo_dh_window3_1.regValue);
		}
		return;
	}
	
	if(select_mode){
		if(s_pContext->_output_wrt_comreg.u1_logo_replace_mv_wholeframe){
			mc3_logo_dh_window0_0.mc_logo_dh_window0_l = 0;
			mc3_logo_dh_window0_0.mc_logo_dh_window0_r = LOGO_DDR_W-1;
			mc3_logo_dh_window0_1.mc_logo_dh_window0_t = 0;
			mc3_logo_dh_window0_1.mc_logo_dh_window0_b = LOGO_DDR_H-1;

			mc3_logo_dh_window1_0.mc_logo_dh_window1_l = 0;
			mc3_logo_dh_window1_0.mc_logo_dh_window1_r = 0;
			mc3_logo_dh_window1_1.mc_logo_dh_window1_t = 0;
			mc3_logo_dh_window1_1.mc_logo_dh_window1_b = 0;
			
			mc3_logo_dh_window2_0.mc_logo_dh_window2_l = 0;
			mc3_logo_dh_window2_0.mc_logo_dh_window2_r = 0;
			mc3_logo_dh_window2_1.mc_logo_dh_window2_t = 0;
			mc3_logo_dh_window2_1.mc_logo_dh_window2_b = 0;
	
			mc3_logo_dh_window3_0.mc_logo_dh_window3_l = 0;
			mc3_logo_dh_window3_0.mc_logo_dh_window3_r = 0;
			mc3_logo_dh_window3_1.mc_logo_dh_window3_t = 0;
			mc3_logo_dh_window3_1.mc_logo_dh_window3_b = 0;
			for(u8_i=0; u8_i<CORNER_LOGO_POSITION_NUM; u8_i++){
				pOutput->u1_apply_replacemv[u8_i]=1;
			}
		}else {
			for(u8_i=0; u8_i<CORNER_LOGO_POSITION_NUM; u8_i++){
				Logo_Boundary = pOutput->DDR_Logo_replacemv[u8_i];
				Max_Rmv = Logo_DDR_LogoBoundary_MAX_rmv_RTK2885p(&Logo_Boundary);
				if(Logo_Boundary.u1_stable==true && Logo_Boundary.u1_vaild==true && Logo_Boundary.u1_get_logo==true && Max_Rmv<=rmv_th && u8_corner_mv_drastic_chg_cnt[u8_i]==0){
					Logo_set_boundary = Logo_Boundary;
				//	u1_set_boundary[u8_i] = true;
					pOutput->u1_apply_replacemv[u8_i]=true;
				}else{
					memset(&Logo_set_boundary, 0, sizeof(Logo_set_boundary));
				//	u1_set_boundary[u8_i] = false;
					pOutput->u1_apply_replacemv[u8_i]=false;
				}

				if(u8_i==CORNER_LOGO_LEFT_UP){
					mc3_logo_dh_window0_0.mc_logo_dh_window0_l = Logo_set_boundary.u16_start_x;
					mc3_logo_dh_window0_0.mc_logo_dh_window0_r = Logo_set_boundary.u16_end_x;
					mc3_logo_dh_window0_1.mc_logo_dh_window0_t = Logo_set_boundary.u16_start_y;
					mc3_logo_dh_window0_1.mc_logo_dh_window0_b = Logo_set_boundary.u16_end_y;
				}else if(u8_i==CORNER_LOGO_RIGHT_UP){
					mc3_logo_dh_window1_0.mc_logo_dh_window1_l = Logo_set_boundary.u16_start_x;
					mc3_logo_dh_window1_0.mc_logo_dh_window1_r = Logo_set_boundary.u16_end_x;
					mc3_logo_dh_window1_1.mc_logo_dh_window1_t = Logo_set_boundary.u16_start_y;
					mc3_logo_dh_window1_1.mc_logo_dh_window1_b = Logo_set_boundary.u16_end_y;
				}else if(u8_i==CORNER_LOGO_LEFT_LOW){
					mc3_logo_dh_window2_0.mc_logo_dh_window2_l = Logo_set_boundary.u16_start_x;
					mc3_logo_dh_window2_0.mc_logo_dh_window2_r = Logo_set_boundary.u16_end_x;
					mc3_logo_dh_window2_1.mc_logo_dh_window2_t = Logo_set_boundary.u16_start_y;
					mc3_logo_dh_window2_1.mc_logo_dh_window2_b = Logo_set_boundary.u16_end_y;
				}else if(u8_i==CORNER_LOGO_RIGHT_LOW){
					mc3_logo_dh_window3_0.mc_logo_dh_window3_l = Logo_set_boundary.u16_start_x;
					mc3_logo_dh_window3_0.mc_logo_dh_window3_r = Logo_set_boundary.u16_end_x;
					mc3_logo_dh_window3_1.mc_logo_dh_window3_t = Logo_set_boundary.u16_start_y;
					mc3_logo_dh_window3_1.mc_logo_dh_window3_b = Logo_set_boundary.u16_end_y;
				}

				if(log_en && u8_i==g_frame_logo_id){
					rtd_pr_memc_emerg("flg,%d,  Max_Rmv,%d,  cnt,%d,  info,%d,%d,%d\n", 
						pOutput->u1_apply_replacemv[u8_i], Max_Rmv, u8_corner_mv_drastic_chg_cnt[u8_i],
						Logo_Boundary.u1_stable, Logo_Boundary.u1_vaild, Logo_Boundary.u1_get_logo);
				}
			}

#if 0 //LOGO_CENTER_LOW
			// detect lower left logo and lower right logo
			if(pOutput->u1_apply_replacemv[CORNER_LOGO_RIGHT_LOW]==true && pOutput->u1_apply_replacemv[CORNER_LOGO_RIGHT_LOW]==true){
				mc3_logo_dh_window3_0.mc_logo_dh_window3_l = mc3_logo_dh_window2_0.mc_logo_dh_window2_l;
				mc3_logo_dh_window3_1.mc_logo_dh_window3_t = SEARCH_BOUNDARY_LIMIT[LOGO_CENTER_LOW].u16_start_y;
				mc3_logo_dh_window3_1.mc_logo_dh_window3_b = SEARCH_BOUNDARY_LIMIT[LOGO_CENTER_LOW].u16_end_y;
			}
			// only detect lower right logo
			else if(pOutput->u1_apply_replacemv[CORNER_LOGO_RIGHT_LOW]==true){
				mc3_logo_dh_window3_0.mc_logo_dh_window3_l = SEARCH_BOUNDARY_LIMIT[LOGO_CENTER_LOW].u16_start_x;
				mc3_logo_dh_window3_1.mc_logo_dh_window3_t = SEARCH_BOUNDARY_LIMIT[LOGO_CENTER_LOW].u16_start_y;
				mc3_logo_dh_window3_1.mc_logo_dh_window3_b = SEARCH_BOUNDARY_LIMIT[LOGO_CENTER_LOW].u16_end_y;
			}
			// only detect lower left logo
			else if(pOutput->u1_apply_replacemv[CORNER_LOGO_RIGHT_LOW]==true){
				mc3_logo_dh_window2_0.mc_logo_dh_window2_r = SEARCH_BOUNDARY_LIMIT[LOGO_CENTER_LOW].u16_end_x;
				mc3_logo_dh_window2_1.mc_logo_dh_window2_t = SEARCH_BOUNDARY_LIMIT[LOGO_CENTER_LOW].u16_start_y;
				mc3_logo_dh_window2_1.mc_logo_dh_window2_b = SEARCH_BOUNDARY_LIMIT[LOGO_CENTER_LOW].u16_end_y;
			}
			// can't detect lower left logo and lower right logo
			else{
				mc3_logo_dh_window3_0.mc_logo_dh_window3_l = SEARCH_BOUNDARY_LIMIT[LOGO_LEFT_LOW].u16_start_x;
				mc3_logo_dh_window3_0.mc_logo_dh_window3_r = SEARCH_BOUNDARY_LIMIT[LOGO_RIGHT_LOW].u16_end_x;
				mc3_logo_dh_window3_1.mc_logo_dh_window3_t = SEARCH_BOUNDARY_LIMIT[LOGO_CENTER_LOW].u16_start_y;
				mc3_logo_dh_window3_1.mc_logo_dh_window3_b = SEARCH_BOUNDARY_LIMIT[LOGO_CENTER_LOW].u16_end_y;
			}
#endif
		}
	}else{
		Logo_Boundary = pOutput->DDR_Logo_replacemv[g_frame_logo_id];
		if(Logo_Boundary.u1_stable==true && Logo_Boundary.u1_vaild==true && Logo_Boundary.u1_get_logo==true){
			//apply setting
			//mv threshold: reg_mc_logo_dh_mv_thr
			//mv select: reg_mc_logo_dh_mode
			mc3_logo_dh_window0_0.mc_logo_dh_window0_l = Logo_Boundary.u16_start_x;
			mc3_logo_dh_window0_0.mc_logo_dh_window0_r = Logo_Boundary.u16_end_x;
			mc3_logo_dh_window0_1.mc_logo_dh_window0_t = Logo_Boundary.u16_start_y;
			mc3_logo_dh_window0_1.mc_logo_dh_window0_b = Logo_Boundary.u16_end_y;
		}else{
			//close setting
			mc3_logo_dh_window0_0.mc_logo_dh_window0_l = 0;
			mc3_logo_dh_window0_0.mc_logo_dh_window0_r = 0;
			mc3_logo_dh_window0_1.mc_logo_dh_window0_t = 0;
			mc3_logo_dh_window0_1.mc_logo_dh_window0_b = 0;
		}

		mc3_logo_dh_window1_0.mc_logo_dh_window1_l = 0;
		mc3_logo_dh_window1_0.mc_logo_dh_window1_r = 0;
		mc3_logo_dh_window1_1.mc_logo_dh_window1_t = 0;
		mc3_logo_dh_window1_1.mc_logo_dh_window1_b = 0;
		
		mc3_logo_dh_window2_0.mc_logo_dh_window2_l = 0;
		mc3_logo_dh_window2_0.mc_logo_dh_window2_r = 0;
		mc3_logo_dh_window2_1.mc_logo_dh_window2_t = 0;
		mc3_logo_dh_window2_1.mc_logo_dh_window2_b = 0;

		mc3_logo_dh_window3_0.mc_logo_dh_window3_l = 0;
		mc3_logo_dh_window3_0.mc_logo_dh_window3_r = 0;
		mc3_logo_dh_window3_1.mc_logo_dh_window3_t = 0;
		mc3_logo_dh_window3_1.mc_logo_dh_window3_b = 0;
	}

	last_repalce = true;

	rtd_outl(MC3_logo_dh_window0_0_reg, mc3_logo_dh_window0_0.regValue);
	rtd_outl(MC3_logo_dh_window0_1_reg, mc3_logo_dh_window0_1.regValue);
	rtd_outl(MC3_logo_dh_window1_0_reg, mc3_logo_dh_window1_0.regValue);
	rtd_outl(MC3_logo_dh_window1_1_reg, mc3_logo_dh_window1_1.regValue);
	rtd_outl(MC3_logo_dh_window2_0_reg, mc3_logo_dh_window2_0.regValue);
	rtd_outl(MC3_logo_dh_window2_1_reg, mc3_logo_dh_window2_1.regValue);
	rtd_outl(MC3_logo_dh_window3_0_reg, mc3_logo_dh_window3_0.regValue);
	rtd_outl(MC3_logo_dh_window3_1_reg, mc3_logo_dh_window3_1.regValue);
}

void Logo_DDR_init_RTK2885p(void)
{
#ifdef BUILD_QUICK_SHOW
	return
#else
	unsigned char u8_i = 0;

	for(u8_i=0; u8_i<LOGO_POSITION_NUM; u8_i++){
		UNCONF_LOGO_AREA[u8_i] = (SEARCH_BOUNDARY_LIMIT[u8_i].u16_end_x-SEARCH_BOUNDARY_LIMIT[u8_i].u16_start_x)*(SEARCH_BOUNDARY_LIMIT[u8_i].u16_end_y-SEARCH_BOUNDARY_LIMIT[u8_i].u16_start_y)*3/4;
	}
#endif
}

void Logo_DDR_Proc_RTK2885p(_OUTPUT_LOGO_CTRL *pOutput)
{
#ifdef BUILD_QUICK_SHOW
	return
#else
	unsigned int show_time_info, time_1, time_2, time_3, time_4, time_5;
	
	if(LOGO_480_DDR_En==false){
		return;
	}
	ReadRegister(SOFTWARE2_SOFTWARE2_37_reg, 18, 21, &LOGO_MIN_LENGTH_480);
	ReadRegister(SOFTWARE2_SOFTWARE2_37_reg, 22, 31, &MIN_LOGO_AREA_480);
	ReadRegister(SOFTWARE2_SOFTWARE2_34_reg, 23, 23, &show_time_info);
	time_1 = Get_Time_us();

	//clear logo out of rim
	Logo_DDR_Clear_LogoRim_RTK2885p();
	time_2 = Get_Time_us();

	//iir
	Logo_DDR_IIR_RTK2885p();
	time_3 = Get_Time_us();

	Logo_DDR_DetectLine_RTK2885p();
	time_4 = Get_Time_us();

	Logo_DDR_GetLogoBoundary_RTK2885p(pOutput);
	time_5 = Get_Time_us();

	Logo_DDR_Apply_Replace_mv_RTK2885p(pOutput);
	Logo_DDR_close_DhLogo_RTK2885p(pOutput);
	Logo_DDR_FrameLogo_RTK2885p(pOutput);

	Logo_DDR_ShowLogoFlg_RTK2885p();

	if(show_time_info){	
		rtd_pr_memc_emerg("[%s] total,%u, init,%u, iir,%u, detect_line,%u, get_boundary,%u\n", __FUNCTION__, time_5-time_1, time_2-time_1, time_3-time_2, time_4-time_3, time_5-time_4);
	}
#endif
}



