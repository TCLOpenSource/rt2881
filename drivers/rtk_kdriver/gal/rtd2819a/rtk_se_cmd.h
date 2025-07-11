/******************************************************************************
*	Copyright(c) 2014 Realtek Semiconductor Corp. All rights reserved.
*
*	@author baker.cheng@realtek.com
*
******************************************************************************/
#ifndef _SE_CMD_H_
#define _SE_CMD_H_
typedef struct _SECMD_WRITE_REGISTER
{
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} Word1;
	struct
	{
		uint32_t	RegisterValue;
	} Word2;

	struct
	{
		uint32_t	Reserved;
	} Word3;

	struct
	{
		uint32_t	Reserved;
	} Word4;
	struct
	{
		uint32_t	Reserved;
	} Word5;
	struct
	{
		uint32_t	Reserved;
	} Word6;
	struct
	{
		uint32_t	Reserved;
	} Word7;
	struct
	{
		uint32_t	Reserved;
	} Word8;
} SECMD_WRITE_REGISTER;

typedef struct _SECMD_WRITE_REGISTER2
{
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} Word1;
	struct
	{
		uint32_t	RegisterValue;
	} Word2;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} Word3;
	struct
	{
		uint32_t	RegisterValue;
	} Word4;

	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} Word5;
	struct
	{
		uint32_t	RegisterValue;
	} Word6;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} Word7;
	struct
	{
		uint32_t	RegisterValue;
	} Word8;
} SECMD_WRITE_REGISTER2;

typedef struct _SECMD_WRITE_CLUT
{
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} Word1;
	struct
	{
		uint32_t	RegisterValue;
	} Word2;

	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} Word3;

	struct
	{
		uint32_t	RegisterValue;
	} Word4;
} SECMD_WRITE_CLUT;

typedef struct _SECMD_FETCH_CLUT
{
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	FetchEntry: 2;
		uint32_t	Force_load_clut: 1;
		uint32_t	Reserved: 25;
	} Word1;
	struct
	{
		uint32_t	StartingAddress: 31;
		uint32_t	Reserved: 1;
	} Word2;
	struct
	{
		uint32_t	Reserved;
	} Word3;
	struct
	{
		uint32_t	Reserved;
	} Word4;
	struct
	{
		uint32_t	Reserved;
	} Word5;
	struct
	{
		uint32_t	Reserved;
	} Word6;
	struct
	{
		uint32_t	Reserved;
	} Word7;
	struct
	{
		uint32_t	Reserved;
	} Word8;
} SECMD_FETCH_CLUT;
typedef struct _SECMD_LOCK_CLUT
{
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	Lock_clut: 1;
		uint32_t	Reserved: 27;
	} Word1;
	struct
	{
		uint32_t	Reserved;
	} Word2;
	struct
	{
		uint32_t	Reserved;
	} Word3;
	struct
	{
		uint32_t	Reserved;
	} Word4;
	struct
	{
		uint32_t	Reserved;
	} Word5;
	struct
	{
		uint32_t	Reserved;
	} Word6;
	struct
	{
		uint32_t	Reserved;
	} Word7;
	struct
	{
		uint32_t	Reserved;
	} Word8;
} SECMD_LOCK_CLUT;
typedef struct _SECMD_FORMAT_CONVERSION
{
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg1;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg2;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg3;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg3_1;

	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg4;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg4_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg5;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg5_1;

	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg6;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg6_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr0;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch0;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr1;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch1;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	Width: 12;
		uint32_t	Height: 12;
		uint32_t	SelectOutputAlpha: 1;
		uint32_t	ColorKey: 1;
		uint32_t	ConversionMode: 2;
	} Word1;
	struct
	{
		uint32_t	Reserved: 1;
		uint32_t	YUV422SeqEnable: 1;
		uint32_t	ARGB8888MatrixEnable: 1;
		uint32_t	directFB: 1;
		uint32_t	FormatConversionBitblit: 1;
		uint32_t	InputX: 12;
		uint32_t	InputY: 12;
		uint32_t	SourceFormat: 3;
	} Word2;
	struct
	{
		uint32_t	Reserved: 5;
		uint32_t	OutputX: 12;
		uint32_t	OutputY: 12;
		uint32_t	DestinationFormat: 3;
	} Word3;
	struct
	{
		uint32_t	Reserved: 9;
		uint32_t	ROPCode1: 2;
		uint32_t	ChromaAddressIndex: 7;
		uint32_t	DestAddressIndex: 7;
		uint32_t	SrcAddressIndex: 7;
	} Word4;
	struct
	{
		uint32_t	fb_sel_destclr_blend: 4;
		uint32_t	fb_sel_srcclr_blend: 4;
		uint32_t	fb_demultiply: 1;
		uint32_t	fb_blend_alphachannel: 1;
		uint32_t	fb_blend_coloralpha: 1;
		uint32_t	fb_colorize: 1;
		uint32_t	fb_xor: 1;
		uint32_t	fb_src_premultiply: 1;
		uint32_t	fb_src_premulcolor: 1;
		uint32_t	fb_dst_premultiply: 1;
		uint32_t	src_clr_transparency: 1;
		uint32_t	src_clr_transform: 2;
		uint32_t	rop_mode: 1;
		uint32_t	bitblit_color_key: 2;
		uint32_t	sel_src_alpha: 3;
		uint32_t	rop_code: 2;
		uint32_t	sel_output_alpha: 1;
		uint32_t	sync_vo: 1;
		uint32_t	sel_dest_alpha: 3;
	} Word5;
	struct
	{
		uint32_t	Reserved;
	} Word6;
	struct
	{
		uint32_t	Reserved;
	} Word7;
	struct
	{
		uint32_t	Reserved;
	} Word8;
} SECMD_FORMAT_CONVERSION;
typedef struct _SECMD_BITBLT
{
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg1;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg2;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg3;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg3_1;

	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg4;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg4_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg5;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg5_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg6;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg6_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg7;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg7_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg8;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg8_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg9;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg9_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg10;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg10_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr0;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch0;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr1;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch1;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	Width: 12;
		uint32_t	Height: 12;
		uint32_t	InputSelect: 1;
		uint32_t	SelectDestAlpha: 3;
	} Word1;
	struct
	{
		uint32_t	OutputX: 12;
		uint32_t	OutputY: 12;
		uint32_t	StartingCorner: 2;
		uint32_t	RopCode: 2;
		uint32_t	SelectOutputAlpha: 1;
		uint32_t	sync_vo: 1;
		uint32_t	DestAddressIndex: 2;
	} Word2;
	struct
	{
		uint32_t	InputX: 12;
		uint32_t	InputY: 12;
		uint32_t	RopMode: 1;
		uint32_t	ColorKey: 2;
		uint32_t	SelectSrcAlpha: 3;
		uint32_t	SrcAddressIndex: 2;
	} Word3;
	struct
	{
		uint32_t	Reserved: 9;
		uint32_t	alpha_stream_mode: 1;
		uint32_t	ROPCode1: 2;
		uint32_t	fb_sel_destclr_blend: 4;
		uint32_t	fb_sel_srcclr_blend: 4;
		uint32_t	fb_demultiply: 1;
		uint32_t	fb_blend_alphachannel: 1;
		uint32_t	fb_blend_coloralpha: 1;
		uint32_t	fb_colorize: 1;
		uint32_t	fb_xor: 1;
		uint32_t	fb_src_premultiply: 1;
		uint32_t	fb_src_premulcolor: 1;
		uint32_t	fb_dst_premultiply: 1;
		uint32_t	directFB: 1;
		uint32_t	src_clr_transparency: 1;
		uint32_t	src_clr_transform: 2;
	} Word4;
	struct
	{
		uint32_t	Reserved;
	} Word5;
	struct
	{
		uint32_t	Reserved;
	} Word6;
	struct
	{
		uint32_t	Reserved;
	} Word7;
	struct
	{
		uint32_t	Reserved;
	} Word8;
} SECMD_BITBLT;
typedef struct _SECMD_STRETCH
{
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg1;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg2;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg3;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg3_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg4;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg4_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg5;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg5_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg6;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg6_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg7;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg7_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg8;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg8_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg9;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg9_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg10;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg10_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr0;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch0;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr1;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch1;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	OutputX: 12;
		uint32_t	OutputY: 12;
		uint32_t	ColorKey: 1;
		uint32_t	HScalingEnable: 1;
		uint32_t	VScalingEnable: 1;
		uint32_t	SrcInterleaving: 1;
	} Word1;
	struct
	{
		uint32_t	OutputWidth: 12;
		uint32_t	OutputHeight: 12;
		uint32_t	StretchBitblit: 1;
		uint32_t	directFB: 1;
		uint32_t	Reserved: 4;
		uint32_t	ScalingAlgorithm: 2;
	} Word2;
	struct
	{
		uint32_t	InputX: 12;
		uint32_t	InputY: 12;
		uint32_t	DestInterleaving: 1;
		uint32_t	SrcAddressIndex: 7;
	} Word3;
	struct
	{
		uint32_t	InputWidth: 13;
		uint32_t	InputHeight: 12;
		uint32_t	DestAddressIndex: 7;
	} Word4;
	struct
	{
		uint32_t	fb_sel_destclr_blend: 4;
		uint32_t	fb_sel_srcclr_blend: 4;
		uint32_t	fb_demultiply: 1;
		uint32_t	fb_blend_alphachannel: 1;
		uint32_t	fb_blend_coloralpha: 1;
		uint32_t	fb_colorize: 1;
		uint32_t	fb_xor: 1;
		uint32_t	fb_src_premultiply: 1;
		uint32_t	fb_src_premulcolor: 1;
		uint32_t	fb_dst_premultiply: 1;
		uint32_t	src_clr_transparency: 1;
		uint32_t	src_clr_transform: 2;
		uint32_t	rop_mode: 1;
		uint32_t	bitblit_color_key: 2;
		uint32_t	sel_src_alpha: 3;
		uint32_t	rop_code: 2;
		uint32_t	sel_output_alpha: 1;
		uint32_t	sync_vo: 1;
		uint32_t	sel_dest_alpha: 3;
	} Word5;
	struct
	{
		uint32_t	Reserved;
	} Word6;
	struct
	{
		uint32_t	Reserved;
	} Word7;
	struct
	{
		uint32_t	Reserved;
	} Word8;
} SECMD_STRETCH;
typedef struct _SECMD_MIRROR
{
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg1;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr0;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch0;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr1;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch1;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	OutputX: 12;
		uint32_t	OutputY: 12;
		uint32_t	MirrorDirection: 2;
		uint32_t	DestAddressIndex: 2;
	} Word1;
	struct
	{
		uint32_t	Width: 12;
		uint32_t	Height: 12;
		uint32_t	Reserved: 8;
	} Word2;
	struct
	{
		uint32_t	Reserved1: 4;
		uint32_t	InputX: 12;
		uint32_t	InputY: 12;
		uint32_t	Reserved2: 2;
		uint32_t	SrcAddressIndex: 2;
	} Word3;
	struct
	{
		uint32_t	Reserved;
	} Word4;
	struct
	{
		uint32_t	Reserved;
	} Word5;
	struct
	{
		uint32_t	Reserved;
	} Word6;
} SECMD_MIRROR;
typedef struct _SECMD_ROTATE
{
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg1;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr0;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch0;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr1;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch1;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	Reserved1: 25;
		uint32_t	BlockModeOutput: 1;
		uint32_t	RotateMode: 2;
	} Word1;
	struct
	{
		uint32_t	InputX: 12;
		uint32_t	InputY: 12;
		uint32_t	Reserved: 1;
		uint32_t	SrcAddressIndex: 7;
	} Word2;
	struct
	{
		uint32_t	OutputX: 12;
		uint32_t	OutputY: 12;
		uint32_t	Reserved: 1;
		uint32_t	DestAddressIndex: 7;
	} Word3;
	struct
	{
		uint32_t	InputWidth: 12;
		uint32_t	Reserved1: 4;
		uint32_t	InputHeight: 12;
		uint32_t	Reserved2: 4;
	} Word4;
	struct
	{
		uint32_t	Reserved;
	} Word5;
	struct
	{
		uint32_t	Reserved;
	} Word6;
} SECMD_ROTATE;
typedef struct _SECMD_NOP
{
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	Reserved: 28;
	} Word1;
} SECMD_NOP;
typedef struct _SECMD_EXT_FMT_CONV_BLIT
{
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg1;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg2;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg3;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg3_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg4;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg4_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg5;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg5_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg6;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg6_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg7;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg7_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg8;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg8_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg9;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg9_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg10;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg10_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr0;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch0;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr1;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch1;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr2;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch2;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr3;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr3_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch3;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch3_1;
	struct
	{
		uint32_t	opcode: 4;
		uint32_t	rslt_fmt: 5;
		uint32_t	lay_ctl: 1;
		uint32_t	lay_num: 1;
		uint32_t	reserved0: 1;
		uint32_t	rslt_swp_chr: 2;
		uint32_t	src_swp_chr: 2;
		uint32_t	itu_en: 1;
		uint32_t	reserved1: 1;
		uint32_t	rslt_matx_en: 1;
		uint32_t	out_alpha: 1;
		uint32_t	rslt_argb_order: 5;
		uint32_t	reserved2: 1;
		uint32_t	src2_argb_order: 5;
		uint32_t	reserved3: 1;
	} Word1;
	struct
	{
		uint32_t	rslt_out_x: 13;
		uint32_t	rslt_out_y: 13;
		uint32_t	reserved0: 2;
		uint32_t  rslt_rotate: 2;
		uint32_t  rslt_mirror: 2;
	} Word2;
	struct
	{
		uint32_t	rslt_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	rslt_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	rslt_addr_index: 7;
		uint32_t	rslt_blk_mode_interleave: 1;
		uint32_t	reserved2: 8;
	} Word3;
	struct
	{
		uint32_t	src1_inpt_w: 13;
		uint32_t	src1_inpt_h: 13;
		uint32_t	src1_fmt: 5;
		uint32_t	src1_mat_en: 1;
	} Word4;
	struct
	{
		uint32_t	src1_inpt_x: 13;
		uint32_t	src1_inpt_y: 13;
		uint32_t	src1_argb_order: 5;
		uint32_t	src1_fcv_stch: 1;
	} Word5;
	struct
	{
		uint32_t	src1_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	src1_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	src1_addr_index: 7;
		uint32_t	reserved2: 1;
		uint32_t	src1_inpt_sel: 1;
		uint32_t	src1_linear_gradient: 1;
		uint32_t	reserved3: 2;
		uint32_t	src1_tfbdc_lossy: 2;
		uint32_t	reserved4: 2;
	} Word6;
	struct
	{
		uint32_t	src2_inpt_x: 13;
		uint32_t	src2_inpt_y: 13;
		uint32_t	src2_fmt: 5;
		uint32_t	src2_mat_en: 1;
	} Word7;
	struct
	{
		uint32_t	src2_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	src2_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	src2_addr_index: 7;
		uint32_t	reserved2: 1;
		uint32_t	src2_inpt_sel: 1;
		uint32_t	src2_swp_chr: 2;
		uint32_t	reserved3: 1;
		uint32_t	src2_linear_gradient: 1;
		uint32_t	src2_tfbdc_lossy: 2;
		uint32_t	reserved4: 1;
	} Word8;
	struct
	{
		uint32_t	sel_s2_alpha: 3;
		uint32_t	reserved0: 1;
		uint32_t	sel_s1_alpha: 3;
		uint32_t	reserved1: 1;
		uint32_t	lay_num_ext: 3;
		uint32_t	reserved2: 21;
	} Word9;
	struct
	{
		uint32_t	rop_code_1: 4;
		uint32_t	fb_sel_destclr_blend_1: 4;
		uint32_t	fb_sel_srcclr_blend_1: 4;
		uint32_t	fb_demultiply_1: 1;
		uint32_t	fb_blend_alphachannel_1: 1;
		uint32_t	fb_blend_coloralpha_1: 1;
		uint32_t	fb_colorize_1: 1;
		uint32_t	fb_xor_1: 1;
		uint32_t	fb_src_premultiply_1: 1;
		uint32_t	fb_src_premulcolor_1: 1;
		uint32_t	fb_dst_premultiply_1: 1;
		uint32_t	src_clr_transparency_1: 1;
		uint32_t	src_clr_transform_1: 2;
		uint32_t	reserved0: 2;
		uint32_t	color_key_1: 2;
		uint32_t	blend_sel_1: 4;
		uint32_t	reserved1: 1;
	} Word10;
	struct
	{
		uint32_t	gl_sel_destclr_1: 6;
		uint32_t	reserved0: 2;
		uint32_t	gl_sel_srctclr_1: 6;
		uint32_t	reserved1: 2;
		uint32_t	gl_sel_destalpha_1: 6;
		uint32_t	reserved2: 2;
		uint32_t	gl_sel_srctalpha_1: 6;
		uint32_t	reserved3: 2;
	} Word11;
	struct
	{
		uint32_t	Reserved: 32;
	} Word12;
	struct
	{
		uint32_t	Reserved: 32;
	} Word13;
	struct
	{
		uint32_t	Reserved: 32;
	} Word14;
	struct
	{
		uint32_t	Reserved: 32;
	} Word15;
	struct
	{
		uint32_t	Reserved: 32;
	} Word16;
} SECMD_EXT_FMT_CONV_BLIT;

typedef struct _SECMD_EXT_FMT_CONV_ROP
{
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg1;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg2;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg3;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg3_1;

	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg4;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg4_1;

	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr0;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch0;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr1;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch1;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr2;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch2;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr3;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr3_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch3;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch3_1;
	struct
	{
		uint32_t	opcode: 4;
		uint32_t	rslt_fmt: 5;
		uint32_t	lay_ctl: 1;
		uint32_t	lay_num: 1;
		uint32_t	reserved0: 1;
		uint32_t	rslt_swp_chr: 2;
		uint32_t	src_swp_chr: 2;
		uint32_t	itu_en: 1;
		uint32_t	reserved1: 1;
		uint32_t	rslt_matx_en: 1;
		uint32_t	out_alpha: 1;
		uint32_t	rslt_argb_order: 5;
		uint32_t	reserved2: 1;
		uint32_t	src2_argb_order: 5;
		uint32_t	reserved3: 1;
	} Word1;
	struct
	{
		uint32_t	rslt_out_x: 13;
		uint32_t	rslt_out_y: 13;
		uint32_t	reserved0: 2;
		uint32_t  rslt_rotate: 2;
		uint32_t  rslt_mirror: 2;
	} Word2;
	struct
	{
		uint32_t	rslt_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	rslt_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	rslt_addr_index: 7;
		uint32_t	rslt_blk_mode_interleave: 1;
		uint32_t	reserved2: 8;
	} Word3;
	struct
	{
		uint32_t	src1_inpt_w: 13;
		uint32_t	src1_inpt_h: 13;
		uint32_t	src1_fmt: 5;
		uint32_t	src1_mat_en: 1;
	} Word4;
	struct
	{
		uint32_t	src1_inpt_x: 13;
		uint32_t	src1_inpt_y: 13;
		uint32_t	src1_argb_order: 5;
		uint32_t	src1_fcv_stch: 1;
	} Word5;
	struct
	{
		uint32_t	src1_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	src1_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	src1_addr_index: 7;
		uint32_t	reserved2: 1;
		uint32_t	src1_inpt_sel: 1;
		uint32_t	src1_linear_gradient: 1;
		uint32_t	reserved3: 2;
		uint32_t	src1_tfbdc_lossy: 2;
		uint32_t	reserved4: 2;
	} Word6;
	struct
	{
		uint32_t	src2_inpt_x: 13;
		uint32_t	src2_inpt_y: 13;
		uint32_t	src2_fmt: 5;
		uint32_t	src2_mat_en: 1;
	} Word7;
	struct
	{
		uint32_t	src2_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	src2_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	src2_addr_index: 7;
		uint32_t	reserved2: 1;
		uint32_t	src2_inpt_sel: 1;
		uint32_t	src2_swp_chr: 2;
		uint32_t	reserved3: 1;
		uint32_t	src2_linear_gradient: 1;
		uint32_t	src2_tfbdc_lossy: 2;
		uint32_t	reserved4: 1;
	} Word8;
	struct
	{
		uint32_t	sel_s2_alpha: 3;
		uint32_t	reserved0: 1;
		uint32_t	sel_s1_alpha: 3;
		uint32_t	reserved1: 1;
		uint32_t	lay_num_ext: 3;
		uint32_t	reserved2: 21;
	} Word9;
	struct
	{
		uint32_t	rop_code_1: 4;
		uint32_t	fb_sel_destclr_blend_1: 4;
		uint32_t	fb_sel_srcclr_blend_1: 4;
		uint32_t	fb_demultiply_1: 1;
		uint32_t	fb_blend_alphachannel_1: 1;
		uint32_t	fb_blend_coloralpha_1: 1;
		uint32_t	fb_colorize_1: 1;
		uint32_t	fb_xor_1: 1;
		uint32_t	fb_src_premultiply_1: 1;
		uint32_t	fb_src_premulcolor_1: 1;
		uint32_t	fb_dst_premultiply_1: 1;
		uint32_t	src_clr_transparency_1: 1;
		uint32_t	src_clr_transform_1: 2;
		uint32_t	reserved0: 2;
		uint32_t	color_key_1: 2;
		uint32_t	blend_sel_1: 4;
		uint32_t	reserved1: 1;
	} Word10;
	struct
	{
		uint32_t	gl_sel_destclr_1: 6;
		uint32_t	reserved0: 2;
		uint32_t	gl_sel_srctclr_1: 6;
		uint32_t	reserved1: 2;
		uint32_t	gl_sel_destalpha_1: 6;
		uint32_t	reserved2: 2;
		uint32_t	gl_sel_srctalpha_1: 6;
		uint32_t	reserved3: 2;
	} Word11;
	struct
	{
		uint32_t	Reserved: 32;
	} Word12;
	struct
	{
		uint32_t	Reserved: 32;
	} Word13;
	struct
	{
		uint32_t	Reserved: 32;
	} Word14;
	struct
	{
		uint32_t	Reserved: 32;
	} Word15;
	struct
	{
		uint32_t	Reserved: 32;
	} Word16;
} SECMD_EXT_FMT_CONV_ROP;

typedef struct _SECMD_EXT_STCH_BLIT
{
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg1;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg2;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg3;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg3_1;

	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg4;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg4_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg5;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg5_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg6;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg6_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg7;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg7_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg8;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg8_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg9;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg9_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg10;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg10_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg11;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg11_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg12;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg12_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr0;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch0;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr1;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch1;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr2;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch2;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr3;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr3_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch3;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch3_1;

	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordMulti0;
	struct
	{
		uint32_t	RegisterValue;
	} WordMulti0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordMulti1;
	struct
	{
		uint32_t	RegisterValue;
	} WordMulti1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordMulti2;
	struct
	{
		uint32_t	RegisterValue;
	} WordMulti2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordMulti3;
	struct
	{
		uint32_t	RegisterValue;
	} WordMulti3_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordMulti4;
	struct
	{
		uint32_t	RegisterValue;
	} WordMulti4_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordMulti5;
	struct
	{
		uint32_t	RegisterValue;
	} WordMulti5_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordMulti6;
	struct
	{
		uint32_t	RegisterValue;
	} WordMulti6_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordMulti7;
	struct
	{
		uint32_t	RegisterValue;
	} WordMulti7_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordMulti8;
	struct
	{
		uint32_t	RegisterValue;
	} WordMulti8_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordMulti9;
	struct
	{
		uint32_t	RegisterValue;
	} WordMulti9_1;
	
	struct
	{
		uint32_t	opcode: 4;
		uint32_t	rslt_fmt: 5;
		uint32_t	lay_ctl: 1;
		uint32_t	lay_num: 1;
		uint32_t	reserved0: 1;
		uint32_t	rslt_swp_chr: 2;
		uint32_t	src_swp_chr: 2;
		uint32_t	itu_en: 1;
		uint32_t	reserved1: 1;
		uint32_t	rslt_matx_en: 1;
		uint32_t	out_alpha: 1;
		uint32_t	rslt_argb_order: 5;
		uint32_t	reserved2: 1;
		uint32_t	src2_argb_order: 5;
		uint32_t	reserved3: 1;
	} Word1;
	struct
	{
		uint32_t	rslt_out_x: 13;
		uint32_t	rslt_out_y: 13;
		uint32_t	reserved0: 2;
		uint32_t  rslt_rotate: 2;
		uint32_t  rslt_mirror: 2;
	} Word2;
	struct
	{
		uint32_t	rslt_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	rslt_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	rslt_addr_index: 7;
		uint32_t	rslt_blk_mode_interleave: 1;
		uint32_t	reserved2: 8;
	} Word3;
	struct
	{
		uint32_t	src1_inpt_w: 13;
		uint32_t	src1_inpt_h: 13;
		uint32_t	src1_fmt: 5;
		uint32_t	src1_mat_en: 1;
	} Word4;
	struct
	{
		uint32_t	src1_inpt_x: 13;
		uint32_t	src1_inpt_y: 13;
		uint32_t	src1_argb_order: 5;
		uint32_t	src1_fcv_stch: 1;
	} Word5;
	struct
	{
		uint32_t	src1_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	src1_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	src1_addr_index: 7;
		uint32_t	reserved2: 1;
		uint32_t	src1_inpt_sel: 1;
		uint32_t	src1_linear_gradient: 1;
		uint32_t	reserved3: 2;
		uint32_t	src1_tfbdc_lossy: 2;
		uint32_t	reserved4: 2;
	} Word6;
	struct
	{
		uint32_t	src2_inpt_x: 13;
		uint32_t	src2_inpt_y: 13;
		uint32_t	src2_fmt: 5;
		uint32_t	src2_mat_en: 1;
	} Word7;
	struct
	{
		uint32_t	src2_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	src2_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	src2_addr_index: 7;
		uint32_t	reserved2: 1;
		uint32_t	src2_inpt_sel: 1;
		uint32_t	src2_swp_chr: 2;
		uint32_t	reserved3: 1;
		uint32_t	src2_linear_gradient: 1;
		uint32_t	src2_tfbdc_lossy: 2;
		uint32_t	reserved4: 1;
	} Word8;
	struct
	{
		uint32_t	sel_s2_alpha: 3;
		uint32_t	reserved0: 1;
		uint32_t	sel_s1_alpha: 3;
		uint32_t	reserved1: 1;
		uint32_t	lay_num_ext: 3;
		uint32_t	reserved2: 21;
	} Word9;
	struct
	{
		uint32_t	rop_code_1: 4;
		uint32_t	fb_sel_destclr_blend_1: 4;
		uint32_t	fb_sel_srcclr_blend_1: 4;
		uint32_t	fb_demultiply_1: 1;
		uint32_t	fb_blend_alphachannel_1: 1;
		uint32_t	fb_blend_coloralpha_1: 1;
		uint32_t	fb_colorize_1: 1;
		uint32_t	fb_xor_1: 1;
		uint32_t	fb_src_premultiply_1: 1;
		uint32_t	fb_src_premulcolor_1: 1;
		uint32_t	fb_dst_premultiply_1: 1;
		uint32_t	src_clr_transparency_1: 1;
		uint32_t	src_clr_transform_1: 2;
		uint32_t	reserved0: 2;
		uint32_t	color_key_1: 2;
		uint32_t	blend_sel_1: 4;
		uint32_t	reserved1: 1;
	} Word10;
	struct
	{
		uint32_t	gl_sel_destclr_1: 6;
		uint32_t	reserved0: 2;
		uint32_t	gl_sel_srctclr_1: 6;
		uint32_t	reserved1: 2;
		uint32_t	gl_sel_destalpha_1: 6;
		uint32_t	reserved2: 2;
		uint32_t	gl_sel_srctalpha_1: 6;
		uint32_t	reserved3: 2;
	} Word11;
	struct
	{
		uint32_t	stch1_out_w: 13;
		uint32_t	stch1_out_h: 13;
		uint32_t	stch1_src: 3;
		uint32_t	stch1_h_en: 1;
		uint32_t	stch1_v_en: 1;
		uint32_t	stch1_algo: 1;
	} Word12;
	struct
	{
		uint32_t	src2_inpt_w: 13;
		uint32_t	src2_inpt_h: 13;
		uint32_t	Reserved: 6;
	} Word13;
	struct
	{
		uint32_t	src2_out_x: 13;
		uint32_t	src2_out_y: 13;
		uint32_t	Reserved: 6;
	} Word14;
	struct
	{
		uint32_t	stch2_out_w: 13;
		uint32_t	stch2_out_h: 13;
		uint32_t	stch2_src: 3;
		uint32_t	stch2_h_en: 1;
		uint32_t	stch2_v_en: 1;
		uint32_t	stch2_algo: 1;
	} Word15;
	struct
	{
		uint32_t	Reserved: 32;
	} Word16;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordMulti10;
	struct
	{
		uint32_t	RegisterValue;
	} WordMulti10_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordMulti11;
	struct
	{
		uint32_t	RegisterValue;
	} WordMulti11_1;
#if SE_SE20
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} DReg1;
	struct
	{
		uint32_t	RegisterValue;
	} DReg1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} DReg2;
	struct
	{
		uint32_t	RegisterValue;
	} DReg2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} DReg3;
	struct
	{
		uint32_t	RegisterValue;
	} DReg3_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} DReg4;
	struct
	{
		uint32_t	RegisterValue;
	} DReg4_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} DReg5;
	struct
	{
		uint32_t	RegisterValue;
	} DReg5_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} DReg6;
	struct
	{
		uint32_t	RegisterValue;
	} DReg6_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} DAddr0;
	struct
	{
		uint32_t	RegisterValue;
	} DAddr0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} DPitch0;
	struct
	{
		uint32_t	RegisterValue;
	} DPitch0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} DAddr1;
	struct
	{
		uint32_t	RegisterValue;
	} DAddr1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} DPitch1;
	struct
	{
		uint32_t	RegisterValue;
	} DPitch1_1;
	struct
	{
		uint32_t	opcode: 4;
		uint32_t	rslt_fmt: 5;
		uint32_t	lay_ctl: 1;
		uint32_t	lay_num: 1;
		uint32_t	sync_vo: 1;
		uint32_t	rslt_swp_chr: 2;
		uint32_t	src_swp_chr: 2;
		uint32_t	itu_en: 1;
		uint32_t	clip_10b: 1;
		uint32_t	rslt_matx_en: 1;
		uint32_t	out_alpha: 1;
		uint32_t	rslt_argb_order: 5;
		uint32_t	reserved0: 1;
		uint32_t	src2_argb_order: 5;
		uint32_t	two_dc_mode: 1;
	} DWord1;
	struct
	{
		uint32_t	rslt_out_x: 13;
		uint32_t	rslt_out_y: 13;
		uint32_t	reserved0: 6;
	} DWord2;
	struct
	{
		uint32_t	rslt_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	rslt_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	rslt_addr_index: 7;
		uint32_t	rslt_blk_mode_interleave: 1;
		uint32_t	reserved2: 8;
	} DWord3;
	struct
	{
		uint32_t	src1_inpt_w: 13;
		uint32_t	src1_inpt_h: 13;
		uint32_t	src1_fmt: 5;
		uint32_t	src1_mat_en: 1;
	} DWord4;
	struct
	{
		uint32_t	src1_inpt_x: 13;
		uint32_t	src1_inpt_y: 13;
		uint32_t	src1_argb_order: 5;
		uint32_t	src1_fcv_stch: 1;
	} DWord5;
	struct
	{
		uint32_t	src1_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	src1_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	src1_addr_index: 7;
		uint32_t	src1_blk_mode_interleave: 1;
		uint32_t	src1_inpt_sel: 1;
		uint32_t	reserved2: 7;
	} DWord6;
	struct
	{
		uint32_t	src2_inpt_x: 13;
		uint32_t	src2_inpt_y: 13;
		uint32_t	src2_fmt: 5;
		uint32_t	src2_mat_en: 1;
	} DWord7;
	struct
	{
		uint32_t	src2_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	src2_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	src2_addr_index: 7;
		uint32_t	src2_blk_mode_interleave: 1;
		uint32_t	src2_inpt_sel: 1;
		uint32_t	src2_swp_chr: 2;
		uint32_t	src2_clip_10b: 1;
		uint32_t	reserved2: 4;
	} DWord8;
	struct
	{
		uint32_t	sel_s2_alpha: 3;
		uint32_t	reserved0: 1;
		uint32_t	sel_s1_alpha: 3;
		uint32_t	reserved1: 1;
		uint32_t	lay_num_ext: 3;
		uint32_t	reserved2: 21;
	} DWord9;
	struct
	{
		uint32_t	rop_code_1: 4;
		uint32_t	fb_sel_destclr_blend_1: 4;
		uint32_t	fb_sel_srcclr_blend_1: 4;
		uint32_t	fb_demultiply_1: 1;
		uint32_t	fb_blend_alphachannel_1: 1;
		uint32_t	fb_blend_coloralpha_1: 1;
		uint32_t	fb_colorize_1: 1;
		uint32_t	fb_xor_1: 1;
		uint32_t	fb_src_premultiply_1: 1;
		uint32_t	fb_src_premulcolor_1: 1;
		uint32_t	fb_dst_premultiply_1: 1;
		uint32_t	src_clr_transparency_1: 1;
		uint32_t	src_clr_transform_1: 2;
		uint32_t	reserved0: 2;
		uint32_t	color_key_1: 2;
		uint32_t	blend_sel_1: 4;
		uint32_t	reserved1: 1;
	} DWord10;
	struct
	{
		uint32_t	gl_sel_destclr_1: 6;
		uint32_t	reserved0: 2;
		uint32_t	gl_sel_srctclr_1: 6;
		uint32_t	reserved1: 2;
		uint32_t	gl_sel_destalpha_1: 6;
		uint32_t	reserved2: 2;
		uint32_t	gl_sel_srctalpha_1: 6;
		uint32_t	reserved3: 2;
	} DWord11;
	struct
	{
		uint32_t	stch1_out_w: 13;
		uint32_t	stch1_out_h: 13;
		uint32_t	stch1_src: 3;
		uint32_t	stch1_h_en: 1;
		uint32_t	stch1_v_en: 1;
		uint32_t	stch1_algo: 1;
	} DWord12;
	struct
	{
		uint32_t	src2_inpt_w: 13;
		uint32_t	src2_inpt_h: 13;
		uint32_t	Reserved: 6;
	} DWord13;
	struct
	{
		uint32_t	src2_out_x: 13;
		uint32_t	src2_out_y: 13;
		uint32_t	Reserved: 6;
	} DWord14;
	struct
	{
		uint32_t	stch2_out_w: 13;
		uint32_t	stch2_out_h: 13;
		uint32_t	stch2_src: 3;
		uint32_t	stch2_h_en: 1;
		uint32_t	stch2_v_en: 1;
		uint32_t	stch2_algo: 1;
	} DWord15;
	struct
	{
		uint32_t	Reserved: 32;
	} DWord16;
#endif
} SECMD_EXT_STCH_BLIT;

typedef struct _SECMD_EXT_FMT_CONV
{
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg1;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg2;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg3;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg3_1;

	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg4;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg4_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr0;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch0;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr1;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch1;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr2;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch2;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr3;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr3_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch3;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch3_1;
	struct
	{
		uint32_t	opcode: 4;
		uint32_t	rslt_fmt: 5;
		uint32_t	lay_ctl: 1;
		uint32_t	lay_num: 1;
		uint32_t	reserved0: 1;
		uint32_t	rslt_swp_chr: 2;
		uint32_t	src_swp_chr: 2;
		uint32_t	itu_en: 1;
		uint32_t	reserved1: 1;
		uint32_t	rslt_matx_en: 1;
		uint32_t	out_alpha: 1;
		uint32_t	rslt_argb_order: 5;
		uint32_t	reserved2: 1;
		uint32_t	src2_argb_order: 5;
		uint32_t	reserved3: 1;
	} Word1;
	struct
	{
		uint32_t	rslt_out_x: 13;
		uint32_t	rslt_out_y: 13;
		uint32_t	reserved0: 2;
		uint32_t  rslt_rotate: 2;
		uint32_t  rslt_mirror: 2;
	} Word2;
	struct
	{
		uint32_t	rslt_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	rslt_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	rslt_addr_index: 7;
		uint32_t	rslt_blk_mode_interleave: 1;
		uint32_t	reserved2: 8;
	} Word3;
	struct
	{
		uint32_t	src1_inpt_w: 13;
		uint32_t	src1_inpt_h: 13;
		uint32_t	src1_fmt: 5;
		uint32_t	src1_mat_en: 1;
	} Word4;
	struct
	{
		uint32_t	src1_inpt_x: 13;
		uint32_t	src1_inpt_y: 13;
		uint32_t	src1_argb_order: 5;
		uint32_t	src1_fcv_stch: 1;
	} Word5;
	struct
	{
		uint32_t	src1_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	src1_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	src1_addr_index: 7;
		uint32_t	reserved2: 1;
		uint32_t	src1_inpt_sel: 1;
		uint32_t	src1_linear_gradient: 1;
		uint32_t	reserved3: 2;
		uint32_t	src1_tfbdc_lossy: 2;
		uint32_t	reserved4: 2;
	} Word6;
	struct
	{
		uint32_t	Reserved: 32;
	} Word7;
	struct
	{
		uint32_t	Reserved: 32;
	} Word8;
} SECMD_EXT_FMT_CONV;

typedef struct _SECMD_EXT_2LAYER_BITBLIT
{
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg1;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg1_1;

	// plane address and pitch
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg2;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg3;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg3_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg4;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg4_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg5;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg5_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg6;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg6_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg7;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg7_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg8;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg8_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg9;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg9_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg10;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg10_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg11;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg11_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg12;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg12_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg13;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg13_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg14;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg14_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg15;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg15_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg16;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg16_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg17;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg17_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg18;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg18_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg19;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg19_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg20;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg20_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg21;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg21_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg22;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg22_1;

	// se combo command
	struct
	{
		uint32_t	opcode: 4;
		uint32_t	rslt_fmt: 5;
		uint32_t	lay_ctl: 1;
		uint32_t	lay_num: 1;
		uint32_t	reserved0: 1;
		uint32_t	rslt_swp_chr: 2;
		uint32_t	src_swp_chr: 2;
		uint32_t	itu_en: 1;
		uint32_t	reserved1: 1;
		uint32_t	rslt_matx_en: 1;
		uint32_t	out_alpha: 1;
		uint32_t	rslt_argb_order: 5;
		uint32_t	reserved2: 1;
		uint32_t	src2_argb_order: 5;
		uint32_t	reserved3: 1;
	} Word1;
	struct
	{
		uint32_t	rslt_out_x: 13;
		uint32_t	rslt_out_y: 13;
		uint32_t	reserved0: 2;
		uint32_t  rslt_rotate: 2;
		uint32_t  rslt_mirror: 2;
	} Word2;
	struct
	{
		uint32_t	rslt_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	rslt_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	rslt_addr_index: 7;
		uint32_t	rslt_blk_mode_interleave: 1;
		uint32_t	reserved2: 8;
	} Word3;
	struct
	{
		uint32_t	src1_inpt_w: 13;
		uint32_t	src1_inpt_h: 13;
		uint32_t	src1_fmt: 5;
		uint32_t	src1_mat_en: 1;
	} Word4;
	struct
	{
		uint32_t	src1_inpt_x: 13;
		uint32_t	src1_inpt_y: 13;
		uint32_t	src1_argb_order: 5;
		uint32_t	src1_fcv_stch: 1;
	} Word5;
	struct
	{
		uint32_t	src1_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	src1_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	src1_addr_index: 7;
		uint32_t	reserved2: 1;
		uint32_t	src1_inpt_sel: 1;
		uint32_t	src1_linear_gradient: 1;
		uint32_t	reserved3: 2;
		uint32_t	src1_tfbdc_lossy: 2;
		uint32_t	reserved4: 2;
	} Word6;
	struct
	{
		uint32_t	src2_inpt_x: 13;
		uint32_t	src2_inpt_y: 13;
		uint32_t	src2_fmt: 5;
		uint32_t	src2_mat_en: 1;
	} Word7;
	struct
	{
		uint32_t	src2_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	src2_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	src2_addr_index: 7;
		uint32_t	reserved2: 1;
		uint32_t	src2_inpt_sel: 1;
		uint32_t	src2_swp_chr: 2;
		uint32_t	reserved3: 1;
		uint32_t	src2_linear_gradient: 1;
		uint32_t	src2_tfbdc_lossy: 2;
		uint32_t	reserved4: 1;
	} Word8;
	struct
	{
		uint32_t	sel_s2_alpha: 3;
		uint32_t	reserved0: 1;
		uint32_t	sel_s1_alpha: 3;
		uint32_t	reserved1: 1;
		uint32_t	lay_num_ext: 3;
		uint32_t	reserved2: 21;
	} Word9;
	struct
	{
		uint32_t	rop_code_1: 4;
		uint32_t	fb_sel_destclr_blend_1: 4;
		uint32_t	fb_sel_srcclr_blend_1: 4;
		uint32_t	fb_demultiply_1: 1;
		uint32_t	fb_blend_alphachannel_1: 1;
		uint32_t	fb_blend_coloralpha_1: 1;
		uint32_t	fb_colorize_1: 1;
		uint32_t	fb_xor_1: 1;
		uint32_t	fb_src_premultiply_1: 1;
		uint32_t	fb_src_premulcolor_1: 1;
		uint32_t	fb_dst_premultiply_1: 1;
		uint32_t	src_clr_transparency_1: 1;
		uint32_t	src_clr_transform_1: 2;
		uint32_t	reserved0: 2;
		uint32_t	color_key_1: 2;
		uint32_t	blend_sel_1: 4;
		uint32_t	reserved1: 1;
	} Word10;
	struct
	{
		uint32_t	gl_sel_destclr_1: 6;
		uint32_t	reserved0: 2;
		uint32_t	gl_sel_srctclr_1: 6;
		uint32_t	reserved1: 2;
		uint32_t	gl_sel_destalpha_1: 6;
		uint32_t	reserved2: 2;
		uint32_t	gl_sel_srctalpha_1: 6;
		uint32_t	reserved3: 2;
	} Word11;
	struct
	{
		uint32_t	stch1_out_w: 13;
		uint32_t	stch1_out_h: 13;
		uint32_t	stch1_src: 3;
		uint32_t	stch1_h_en: 1;
		uint32_t	stch1_v_en: 1;
		uint32_t	stch1_algo: 1;
	} Word12;
	struct
	{
		uint32_t	src2_inpt_w: 13;
		uint32_t	src2_inpt_h: 13;
		uint32_t	Reserved: 6;
	} Word13;
	struct
	{
		uint32_t	src2_out_x: 13;
		uint32_t	src2_out_y: 13;
		uint32_t	Reserved: 6;
	} Word14;
	struct
	{
		uint32_t	stch2_out_w: 13;
		uint32_t	stch2_out_h: 13;
		uint32_t	stch2_src: 3;
		uint32_t	stch2_h_en: 1;
		uint32_t	stch2_v_en: 1;
		uint32_t	stch2_algo: 1;
	} Word15;
	struct
	{
		uint32_t	Reserved: 32;
	} Word16;
} SECMD_EXT_2LAYER_BITBLIT;

typedef struct
{
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg1;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg2;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg3;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg3_1;

	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg4;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg4_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg5;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg5_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg6;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg6_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg7;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg7_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg8;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg8_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg9;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg9_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg10;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg10_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg11;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg11_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg12;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg12_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr0;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch0;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr1;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch1;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch1_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr2;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch2;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordAddr3;
	struct
	{
		uint32_t	RegisterValue;
	} WordAddr3_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordPitch3;
	struct
	{
		uint32_t	RegisterValue;
	} WordPitch3_1;
	struct
	{
		uint32_t	opcode: 4;
		uint32_t	rslt_fmt: 5;
		uint32_t	lay_ctl: 1;
		uint32_t	lay_num: 1;
		uint32_t	reserved0: 1;
		uint32_t	rslt_swp_chr: 2;
		uint32_t	src_swp_chr: 2;
		uint32_t	itu_en: 1;
		uint32_t	reserved1: 1;
		uint32_t	rslt_matx_en: 1;
		uint32_t	out_alpha: 1;
		uint32_t	rslt_argb_order: 5;
		uint32_t	reserved2: 1;
		uint32_t	src2_argb_order: 5;
		uint32_t	reserved3: 1;
	} Word1;
	struct
	{
		uint32_t	rslt_out_x: 13;
		uint32_t	rslt_out_y: 13;
		uint32_t	reserved0: 2;
		uint32_t  rslt_rotate: 2;
		uint32_t  rslt_mirror: 2;
	} Word2;
	struct
	{
		uint32_t	rslt_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	rslt_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	rslt_addr_index: 7;
		uint32_t	rslt_blk_mode_interleave: 1;
		uint32_t	reserved2: 8;
	} Word3;
	struct
	{
		uint32_t	src1_inpt_w: 13;
		uint32_t	src1_inpt_h: 13;
		uint32_t	src1_fmt: 5;
		uint32_t	src1_mat_en: 1;
	} Word4;
	struct
	{
		uint32_t	src1_inpt_x: 13;
		uint32_t	src1_inpt_y: 13;
		uint32_t	src1_argb_order: 5;
		uint32_t	src1_fcv_stch: 1;
	} Word5;
	struct
	{
		uint32_t	src1_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	src1_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	src1_addr_index: 7;
		uint32_t	reserved2: 1;
		uint32_t	src1_inpt_sel: 1;
		uint32_t	src1_linear_gradient: 1;
		uint32_t	reserved3: 2;
		uint32_t	src1_tfbdc_lossy: 2;
		uint32_t	reserved4: 2;
	} Word6;
	struct
	{
		uint32_t	src2_inpt_x: 13;
		uint32_t	src2_inpt_y: 13;
		uint32_t	src2_fmt: 5;
		uint32_t	src2_mat_en: 1;
	} Word7;
	struct
	{
		uint32_t	src2_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	src2_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	src2_addr_index: 7;
		uint32_t	reserved2: 1;
		uint32_t	src2_inpt_sel: 1;
		uint32_t	src2_swp_chr: 2;
		uint32_t	reserved3: 1;
		uint32_t	src2_linear_gradient: 1;
		uint32_t	src2_tfbdc_lossy: 2;
		uint32_t	reserved4: 1;
	} Word8;
	struct
	{
		uint32_t	sel_s2_alpha: 3;
		uint32_t	reserved0: 1;
		uint32_t	sel_s1_alpha: 3;
		uint32_t	reserved1: 1;
		uint32_t	lay_num_ext: 3;
		uint32_t	reserved2: 21;
	} Word9;
	struct
	{
		uint32_t	rop_code_1: 4;
		uint32_t	fb_sel_destclr_blend_1: 4;
		uint32_t	fb_sel_srcclr_blend_1: 4;
		uint32_t	fb_demultiply_1: 1;
		uint32_t	fb_blend_alphachannel_1: 1;
		uint32_t	fb_blend_coloralpha_1: 1;
		uint32_t	fb_colorize_1: 1;
		uint32_t	fb_xor_1: 1;
		uint32_t	fb_src_premultiply_1: 1;
		uint32_t	fb_src_premulcolor_1: 1;
		uint32_t	fb_dst_premultiply_1: 1;
		uint32_t	src_clr_transparency_1: 1;
		uint32_t	src_clr_transform_1: 2;
		uint32_t	reserved0: 2;
		uint32_t	color_key_1: 2;
		uint32_t	blend_sel_1: 4;
		uint32_t	reserved1: 1;
	} Word10;
	struct
	{
		uint32_t	gl_sel_destclr_1: 6;
		uint32_t	reserved0: 2;
		uint32_t	gl_sel_srctclr_1: 6;
		uint32_t	reserved1: 2;
		uint32_t	gl_sel_destalpha_1: 6;
		uint32_t	reserved2: 2;
		uint32_t	gl_sel_srctalpha_1: 6;
		uint32_t	reserved3: 2;
	} Word11;
	struct
	{
		uint32_t	stch1_out_w: 13;
		uint32_t	stch1_out_h: 13;
		uint32_t	stch1_src: 3;
		uint32_t	stch1_h_en: 1;
		uint32_t	stch1_v_en: 1;
		uint32_t	stch1_algo: 1;
	} Word12;
} SECMD_PQMASK_EXT_STCH_BLIT;

/**
 * @brief 
 * wordReg 52 + 1 dummy + se combo command 15 = 68 = 4*17 commands
 */
typedef struct _SECMD_PQMASK_EXT_2LAYER_STRETCH_BITBLIT
{
	// wordReg00~wordReg25
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg0;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg0_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg1;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg1_1;

	// plane address and pitch
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg2;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg2_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg3;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg3_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg4;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg4_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg5;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg5_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg6;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg6_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg7;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg7_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg8;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg8_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg9;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg9_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg10;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg10_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg11;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg11_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg12;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg12_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg13;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg13_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg14;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg14_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg15;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg15_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg16;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg16_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg17;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg17_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg18;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg18_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg19;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg19_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg20;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg20_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg21;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg21_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg22;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg22_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg23;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg23_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg24;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg24_1;
	struct
	{
		uint32_t	OpCode: 4;
		uint32_t	RegisterAddress: 12;
		uint32_t	Clut: 1;
		uint32_t	Reserved: 15;
	} WordReg25;
	struct
	{
		uint32_t	RegisterValue;
	} WordReg25_1;
	struct
	{
		uint32_t	Value;
	} Dummy1;

	// se combo command
	struct
	{
		uint32_t	opcode: 4;
		uint32_t	rslt_fmt: 5;
		uint32_t	lay_ctl: 1;
		uint32_t	lay_num: 1;
		uint32_t	reserved0: 1;
		uint32_t	rslt_swp_chr: 2;
		uint32_t	src_swp_chr: 2;
		uint32_t	itu_en: 1;
		uint32_t	reserved1: 1;
		uint32_t	rslt_matx_en: 1;
		uint32_t	out_alpha: 1;
		uint32_t	rslt_argb_order: 5;
		uint32_t	reserved2: 1;
		uint32_t	src2_argb_order: 5;
		uint32_t	reserved3: 1;
	} Word1;
	struct
	{
		uint32_t	rslt_out_x: 13;
		uint32_t	rslt_out_y: 13;
		uint32_t	reserved0: 2;
		uint32_t  rslt_rotate: 2;
		uint32_t  rslt_mirror: 2;
	} Word2;
	struct
	{
		uint32_t	rslt_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	rslt_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	rslt_addr_index: 7;
		uint32_t	rslt_blk_mode_interleave: 1;
		uint32_t	reserved2: 8;
	} Word3;
	struct
	{
		uint32_t	src1_inpt_w: 13;
		uint32_t	src1_inpt_h: 13;
		uint32_t	src1_fmt: 5;
		uint32_t	src1_mat_en: 1;
	} Word4;
	struct
	{
		uint32_t	src1_inpt_x: 13;
		uint32_t	src1_inpt_y: 13;
		uint32_t	src1_argb_order: 5;
		uint32_t	src1_fcv_stch: 1;
	} Word5;
	struct
	{
		uint32_t	src1_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	src1_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	src1_addr_index: 7;
		uint32_t	reserved2: 1;
		uint32_t	src1_inpt_sel: 1;
		uint32_t	src1_linear_gradient: 1;
		uint32_t	reserved3: 2;
		uint32_t	src1_tfbdc_lossy: 2;
		uint32_t	reserved4: 2;
	} Word6;
	struct
	{
		uint32_t	src2_inpt_x: 13;
		uint32_t	src2_inpt_y: 13;
		uint32_t	src2_fmt: 5;
		uint32_t	src2_mat_en: 1;
	} Word7;
	struct
	{
		uint32_t	src2_chr2_addr_index: 7;
		uint32_t	reserved0: 1;
		uint32_t	src2_chr1_addr_index: 7;
		uint32_t	reserved1: 1;
		uint32_t	src2_addr_index: 7;
		uint32_t	reserved2: 1;
		uint32_t	src2_inpt_sel: 1;
		uint32_t	src2_swp_chr: 2;
		uint32_t	reserved3: 1;
		uint32_t	src2_linear_gradient: 1;
		uint32_t	src2_tfbdc_lossy: 2;
		uint32_t	reserved4: 1;
	} Word8;
	struct
	{
		uint32_t	sel_s2_alpha: 3;
		uint32_t	reserved0: 1;
		uint32_t	sel_s1_alpha: 3;
		uint32_t	reserved1: 1;
		uint32_t	lay_num_ext: 3;
		uint32_t	reserved2: 21;
	} Word9;
	struct
	{
		uint32_t	rop_code_1: 4;
		uint32_t	fb_sel_destclr_blend_1: 4;
		uint32_t	fb_sel_srcclr_blend_1: 4;
		uint32_t	fb_demultiply_1: 1;
		uint32_t	fb_blend_alphachannel_1: 1;
		uint32_t	fb_blend_coloralpha_1: 1;
		uint32_t	fb_colorize_1: 1;
		uint32_t	fb_xor_1: 1;
		uint32_t	fb_src_premultiply_1: 1;
		uint32_t	fb_src_premulcolor_1: 1;
		uint32_t	fb_dst_premultiply_1: 1;
		uint32_t	src_clr_transparency_1: 1;
		uint32_t	src_clr_transform_1: 2;
		uint32_t	reserved0: 2;
		uint32_t	color_key_1: 2;
		uint32_t	blend_sel_1: 4;
		uint32_t	reserved1: 1;
	} Word10;
	struct
	{
		uint32_t	gl_sel_destclr_1: 6;
		uint32_t	reserved0: 2;
		uint32_t	gl_sel_srctclr_1: 6;
		uint32_t	reserved1: 2;
		uint32_t	gl_sel_destalpha_1: 6;
		uint32_t	reserved2: 2;
		uint32_t	gl_sel_srctalpha_1: 6;
		uint32_t	reserved3: 2;
	} Word11;
	struct
	{
		uint32_t	stch1_out_w: 13;
		uint32_t	stch1_out_h: 13;
		uint32_t	stch1_src: 3;
		uint32_t	stch1_h_en: 1;
		uint32_t	stch1_v_en: 1;
		uint32_t	stch1_algo: 1;
	} Word12;
	struct
	{
		uint32_t	src2_inpt_w: 13;
		uint32_t	src2_inpt_h: 13;
		uint32_t	Reserved: 6;
	} Word13;
	struct
	{
		uint32_t	src2_out_x: 13;
		uint32_t	src2_out_y: 13;
		uint32_t	Reserved: 6;
	} Word14;
	struct
	{
		uint32_t	stch2_out_w: 13;
		uint32_t	stch2_out_h: 13;
		uint32_t	stch2_src: 3;
		uint32_t	stch2_h_en: 1;
		uint32_t	stch2_v_en: 1;
		uint32_t	stch2_algo: 1;
	} Word15;
	
} SECMD_PQMASK_EXT_2LAYER_STRETCH_BITBLIT;

#endif
