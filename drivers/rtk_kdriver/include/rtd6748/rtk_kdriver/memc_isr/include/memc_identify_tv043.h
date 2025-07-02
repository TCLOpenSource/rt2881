#ifndef _MEMC_IDENTIFY_TV043_HEADER_INCLUDE_
#define _MEMC_IDENTIFY_TV043_HEADER_INCLUDE_

_IDENTIFY_INFO DynamicOptimizeMEMC_Info_TV043 = 
{
	0, //pattern_num //auto get pattern number by Update_Pattern_Num
	{
		//pattern 1 //MSPG PATTER 7 for MEMC
		{
			10, //array_len
			{
				//YH
				{ //#0	ntsc 0IRE BLV=HIGH,#1  ntsc 7.5IRE BLV=HIGH, #2 pal  ,#3 SD BLV=HIGH , #4 HD BLV=HIGH
				  //#5	ntsc 0IRE BLV=LOW ,#6  ntsc 7.5IRE BLV=LOW , #7 SD BLV=LOW , #8 HD BLV=LOW
					/*0*/ {0,0,657,6,12,6,0,19,0,12,25,0,0,0,0,6,0,0,0,0,0,19,0,12,0,0,0,13,6,198,0,0},    //  4k
					/*1*/ {0,31,658,7,16,0,0,16,0,11,5,0,5,0,0,21,0,0,5,0,0,16,0,16,0,0,5,11,41,99,28,0,},	//fhd, 20190726 rev.
					/*2*/ {0,0,637,17,39,0,7,13,0,11,5,0,2,0,0,22,1,0,5,0,0,17,0,14,0,13,7,32,47,98,0,0},  //fhd
					/*3*/ {0,0,655,2,17,0,6,13,19,0,0,0,26,0,0,0,0,0,6,0,0,13,0,13,6,0,13,6,14,185,0,0},//HDR
					/*4*/ {0,0,0,0,12,636,13,6,0,21,0,0,1,4,0,0,0,6,0,0,184,12,19,25,0,13, 19,6,6,8,0,0, },//HLG_1
					/*5*/ {0,56,110,0, 609,61,12,6,0,19,0,0,6,0,0,0, 6,0,0,0,6,12,24,24,0,6,18,6,6,7,0,0,},//HLG_2
					/*6*/ {0,0,0,0,12,636,4, 8,6,12,0,8,0,0,6,0,0,6, 0,0,6,12,6,26,12,6,19,6,190,8,0,0, },	
					/*7*/ {0,0,0,0,600,75,12,6,0,19,0,0,6,0,0,21,149,0,0,0,5,11,17,23,5,6,17,5,6,7,0,0, },	
					/*8*/ {0,0,0,0,12, 636,13,0,6,12,8,0,0,6,0,0,0,6,0,0,6,12,6,39,176,6,25,6,6,8,0,0, },
					/*9*/ {0,0,0,0,12, 636,7, 5,6,12,0,8,0,0,6,0,0,6,0,0,6,12,6,38,0,6,197,12,6,8,0,0, }
				},
				{ //#0	ntsc 0IRE BLV=HIGH,#1  ntsc 7.5IRE BLV=HIGH, #2 pal  ,#3 SD BLV=HIGH , #4 HD BLV=HIGH
				  //#5	ntsc 0IRE BLV=LOW ,#6  ntsc 7.5IRE BLV=LOW , #7 SD BLV=LOW , #8 HD BLV=LOW
					/*0*/ {0,4,161,0,0,1,166,0,0,163,2,0,1,5,159,0,0,164,3,0,0,1,163,0,0,0,0,0,0,0,0,0},  // 4k
					/*1*/ {0,0,193,2,2,1,2,187,9,1,0,1,6,191,0,2,2,1,193,2,0,2,1,191,0,0,0,0,0,0,0,0},	//fhd, 20190726 rev.
					/*2*/ {0,3,58,0,1,1,61,0,0,59,1,0,0,60,1,0,0,58,2,0,0,683,1,0,0,0,0,0,0,0,0,0}, //fhd
					/*3*/ {1,1,163,0,0,0,0,165,2,0,0,0,0,164,0,0,1,1,164,0,0,0,0,165,2,0,0,1,163,0,0,0},//HDR
					/*4*/ {1,1,163,0,0,0,0,163,4,0,0,0,163,1,0,0,1,1,163,0,0,0,0,166,1,0,0,0,1,163,0,0,},//HLG_1
					/*5*/ {0,0,165,1,0,0,0,162,4,0,0,0,165,0,0,0,0,1,165,0,0,0,0,164,0,0,0,0,2,163,0,0,},//HLG_2
					/*6*/ {0,0,165,0,0,0,0,162,4,0,0,0,165,0,0,0,0,1,165,0,0,0,0,165,0,0,0,0,2,163,0,0, },
					/*7*/ {0,0,166,0,0,0,0,162,4,0,0,0,164,0,0,0,0,0,165,0,0,0,0,164,0,0,0,0,2,163,0,0, },
					/*8*/ {0,0,165,1,0,0,0,161,4,0,0,0,165,0,0,0,0,1,165,0,0,0,0,164,0,0,0,0,2,163,0,0, },
					/*9*/ {0,0,165,1,0,0,0,161,4,0,0,0,165,0,0,0,0,1,165,0,0,0,0,164,0,0,0,0,2,163,0,0, },
				},
			},
			{
				mid_risk_Y_ratio_th,	//feature1_threshold
				high_risk_H_ratio_th,	//feature2_threshold
				0,	//special scene
				20,	//hold_cnt
				0,	//setting_index
				0,	//status_id
			},
			{}, //dummy
		},

		//DynamicOptimizeMEMC_TV043_1_y
		{},
		//DynamicOptimizeMEMC_TV043_2_y
		{},
		//DynamicOptimizeMEMC_TV043_3_y
		{},
		//DynamicOptimizeMEMC_TV043_4_y
		{},
		//DynamicOptimizeMEMC_TV043_5_y
		{},
		//DynamicOptimizeMEMC_TV043_6_y
		{},
		//DynamicOptimizeMEMC_TV043_7_y
		{},
		//DynamicOptimizeMEMC_TV043_8_y
		{},
		//DynamicOptimizeMEMC_TV043_9_y
		{},
		//DynamicOptimizeMEMC_TV043_10_y
		{},
		//DynamicOptimizeMEMC_TV043_11_y
		{},
		//DynamicOptimizeMEMC_TV043_12_y
		{},
		//DynamicOptimizeMEMC_TV043_13_y
		{},
		//DynamicOptimizeMEMC_TV043_14_y
		{},
		//DynamicOptimizeMEMC_TV043_15_y
		{},
		//DynamicOptimizeMEMC_TV043_16_y
		{},
		//DynamicOptimizeMEMC_TV043_17_y
		{},
		//DynamicOptimizeMEMC_TV043_18_y
		{},
		//DynamicOptimizeMEMC_TV043_19_y
		{},
		//DynamicOptimizeMEMC_TV043_20_y
		{},
		//DynamicOptimizeMEMC_TV043_21_y
		{},
		//DynamicOptimizeMEMC_TV043_22_y
		{},
		//DynamicOptimizeMEMC_TV043_23_y
		{},
		//DynamicOptimizeMEMC_TV043_24_y
		{},
		//DynamicOptimizeMEMC_TV043_25_y
		{},
		//DynamicOptimizeMEMC_TV043_26_y
		{},
		//DynamicOptimizeMEMC_TV043_28_y
		{},
		//DynamicOptimizeMEMC_TV043_29_y
		{},
		//DynamicOptimizeMEMC_TV043_30_y
		{},
		//DynamicOptimizeMEMC_TV043_31_y
		{},
		//DynamicOptimizeMEMC_TV043_32_y
		{},
		//DynamicOptimizeMEMC_TV043_33_y
		{},
		//DynamicOptimizeMEMC_TV043_34_y
		{},
		//DynamicOptimizeMEMC_TV043_35_y
		{},
		//DynamicOptimizeMEMC_TV043_36_y
		{},
		//DynamicOptimizeMEMC_TV043_37_y
		{},
		//DynamicOptimizeMEMC_TV043_38_y
		{},
		//DynamicOptimizeMEMC_TV043_39_y
		{},
		//DynamicOptimizeMEMC_TV043_41_y
		{},
		//DynamicOptimizeMEMC_TV043_42_y
		{},
		//DynamicOptimizeMEMC_TV043_43_y
		{},
		//DynamicOptimizeMEMC_TV043_44_y
		{},
		//DynamicOptimizeMEMC_TV043_45_y
		{},
		//DynamicOptimizeMEMC_TV043_46_y
		{},
		//DynamicOptimizeMEMC_TV043_47_y
		{},
		//DynamicOptimizeMEMC_TV043_48_y
		{},
		//DynamicOptimizeMEMC_TV043_50_y
		{},
		//DynamicOptimizeMEMC_TV043_51_y
		{},
		//DynamicOptimizeMEMC_TV043_52_y
		{},
		//DynamicOptimizeMEMC_TV043_54_y
		{},
		//DynamicOptimizeMEMC_TV043_55_y
		{},
		//DynamicOptimizeMEMC_TV043_56_y
		{},
		//DynamicOptimizeMEMC_TV043_58_y
		{},
		//DynamicOptimizeMEMC_TV043_59_y
		{},
		//DynamicOptimizeMEMC_TV043_60_y
		{},
		//DynamicOptimizeMEMC_TV043_61_y
		{},
		//DynamicOptimizeMEMC_TV043_62_y
		{},
		//DynamicOptimizeMEMC_TV043_63_y
		{},
		//DynamicOptimizeMEMC_TV043_64_y
		{},
		//DynamicOptimizeMEMC_TV043_65_y
		{},
		//DynamicOptimizeMEMC_TV043_66_y
		{},
		//DynamicOptimizeMEMC_TV043_67_y
		{},
		//DynamicOptimizeMEMC_TV043_69_y
		{},
		//DynamicOptimizeMEMC_TV043_70_y
		{},
		//DynamicOptimizeMEMC_TV043_71_y
		{},
		//DynamicOptimizeMEMC_TV043_72_y
		{},
		//DynamicOptimizeMEMC_TV043_73_y
		{},
		//DynamicOptimizeMEMC_TV043_74_y
		{},
		//DynamicOptimizeMEMC_TV043_76_y
		{},
		//DynamicOptimizeMEMC_TV043_77_y
		{},

		//END STRUCT
		{
			END_ARRAY_LEN, //array_len
			{
				//feature_1
				{
					{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				},
				//feature_2
				{
					{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				},
			},
			{
				END_THRESHOLD,	//feature1_threshold
				END_THRESHOLD,	//feature2_threshold
				0,	//special scene
				0,	//hold_cnt
				0,	//setting_index
				0,	//status_id
			},
			{}, //dummy
		},
	},
};
#endif

