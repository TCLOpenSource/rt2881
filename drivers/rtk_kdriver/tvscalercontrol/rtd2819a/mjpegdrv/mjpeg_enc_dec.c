//#include <arch.h>
//#include <mach.h>
//#include <rtk_io.h>

#include "mjpeg_enc_dec.h"

#include <rbus/mjpeg_enc_reg.h>
#include <rbus/uvc_vgip_reg.h>
#include <rtd_log/rtd_module_log.h>
#include <generated/autoconf.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/uaccess.h>
#include <asm/cacheflush.h>
#include <linux/pageremap.h>
#include <linux/auth.h>
#include <linux/version.h>
#include <rtk_kdriver/RPCDriver.h>
#include <uapi/linux/const.h>
#include <linux/mm.h>
#include <scaler/scalerCommon.h>
#include <tvscalercontrol/scaler/scalerstruct.h>

//#define FPGA_TEST

//#define UT_flag

unsigned char yc_separate=0;
unsigned char output_fmt=1;
unsigned char demo_mode=0;
unsigned char no_signal_mode=0;
unsigned char no_support_mode=0;
unsigned char hdcp_mode=0;
unsigned int demo_mode_jpeg;
//unsigned int no_signal_jpeg;
//unsigned int no_support_jpeg;
unsigned int val=0;
unsigned short pic_width;
unsigned short pic_height;

extern StructUVCInfo uvc_info;

#if 0
const int quant_tbl[16][64] = {
{8, 5, 6, 7, 9, 9, 10, 11, 5, 5, 7, 8, 9, 10, 11, 12, 6, 7, 9, 9, 10, 11, 11, 13, 7, 7, 9, 9, 10, 11, 12, 13, 7, 9, 9, 10, 11, 12, 13, 16, 9, 9, 10, 11, 12, 13, 16, 20, 9, 9, 10, 11, 13, 16, 19, 24, 9, 10, 12, 13, 16, 19, 24, 29,},
{8, 6, 7, 8, 10, 10, 11, 13, 6, 6, 8, 9, 10, 11, 13, 14, 7, 8, 10, 10, 11, 13, 13, 15, 8, 8, 10, 10, 11, 13, 14, 16, 8, 10, 10, 11, 12, 14, 16, 19, 10, 10, 11, 12, 14, 16, 19, 23, 10, 10, 11, 13, 15, 18, 22, 27, 10, 11, 14, 15, 18, 22, 27, 33,},
{8, 7, 8, 9, 11, 12, 13, 15, 7, 7, 9, 10, 12, 13, 15, 16, 8, 9, 11, 12, 13, 15, 15, 17, 9, 9, 11, 12, 13, 15, 16, 18, 9, 11, 12, 13, 14, 15, 18, 21, 11, 12, 13, 14, 15, 18, 21, 26, 11, 12, 13, 15, 17, 20, 25, 31, 12, 13, 15, 17, 20, 25, 31, 37,},
{8, 8, 9, 11, 13, 13, 14, 17, 8, 8, 11, 12, 13, 14, 17, 18, 9, 11, 13, 13, 14, 17, 17, 19, 11, 11, 13, 13, 14, 17, 18, 20, 11, 13, 13, 14, 16, 17, 20, 24, 13, 13, 14, 16, 17, 20, 24, 29, 13, 13, 14, 17, 19, 23, 28, 34, 13, 14, 17, 19, 23, 28, 34, 41,},
{8, 8, 10, 12, 14, 14, 15, 18, 8, 8, 12, 13, 14, 15, 18, 20, 10, 12, 14, 14, 15, 18, 18, 20, 12, 12, 14, 14, 15, 18, 20, 21, 12, 14, 14, 15, 17, 19, 21, 26, 14, 14, 15, 17, 19, 21, 26, 31, 14, 14, 15, 18, 20, 25, 30, 37, 14, 15, 19, 20, 25, 30, 37, 45,},
{8, 9, 11, 13, 15, 16, 17, 20, 9, 9, 13, 14, 16, 17, 20, 22, 11, 13, 15, 16, 17, 20, 20, 22, 13, 13, 15, 16, 17, 20, 22, 23, 13, 15, 16, 17, 19, 20, 23, 28, 15, 16, 17, 19, 20, 23, 28, 34, 15, 16, 17, 20, 22, 27, 33, 41, 16, 17, 20, 22, 27, 33, 41, 49,},
{8, 10, 11, 13, 16, 16, 18, 21, 10, 10, 13, 15, 16, 18, 21, 23, 11, 13, 16, 16, 18, 21, 21, 23, 13, 13, 16, 16, 18, 21, 23, 25, 13, 16, 16, 18, 20, 21, 25, 30, 16, 16, 18, 20, 21, 25, 30, 36, 16, 16, 18, 21, 23, 28, 35, 43, 16, 18, 21, 23, 28, 35, 43, 51,},
{8, 10, 12, 14, 16, 17, 18, 22, 10, 10, 14, 15, 17, 18, 22, 24, 12, 14, 16, 17, 18, 22, 22, 24, 14, 14, 16, 17, 18, 22, 24, 26, 14, 16, 17, 18, 20, 22, 26, 31, 16, 17, 18, 20, 22, 26, 31, 37, 16, 17, 18, 22, 24, 29, 36, 44, 17, 18, 22, 24, 29, 36, 44, 53,},
{8, 10, 12, 14, 17, 18, 19, 22, 10, 10, 14, 16, 18, 19, 22, 24, 12, 14, 17, 18, 19, 22, 22, 25, 14, 14, 17, 18, 19, 22, 24, 26, 14, 17, 18, 19, 21, 23, 26, 32, 17, 18, 19, 21, 23, 26, 32, 39, 17, 18, 19, 22, 25, 31, 37, 46, 18, 19, 23, 25, 31, 37, 46, 56,},
{8, 11, 13, 15, 18, 18, 20, 23, 11, 11, 15, 16, 18, 20, 23, 25, 13, 15, 18, 18, 20, 23, 23, 26, 15, 15, 18, 18, 20, 23, 25, 28, 15, 18, 18, 20, 22, 24, 28, 33, 18, 18, 20, 22, 24, 28, 33, 40, 18, 18, 20, 23, 26, 32, 39, 48, 18, 20, 24, 26, 32, 39, 48, 58,},
{8, 11, 13, 15, 18, 19, 21, 24, 11, 11, 15, 17, 19, 21, 24, 26, 13, 15, 18, 19, 21, 24, 24, 27, 15, 15, 18, 19, 21, 24, 26, 28, 15, 18, 19, 21, 23, 25, 28, 34, 18, 19, 21, 23, 25, 28, 34, 42, 18, 19, 21, 24, 27, 33, 40, 49, 19, 21, 25, 27, 33, 40, 49, 60,},
{8, 12, 14, 17, 20, 20, 22, 26, 12, 12, 17, 18, 20, 22, 26, 28, 14, 17, 20, 20, 22, 26, 26, 29, 17, 17, 20, 20, 22, 26, 28, 31, 17, 20, 20, 22, 24, 27, 31, 37, 20, 20, 22, 24, 27, 31, 37, 44, 20, 20, 22, 26, 29, 35, 43, 53, 20, 22, 27, 29, 35, 43, 53, 64,},
{8, 13, 15, 18, 21, 22, 23, 28, 13, 13, 18, 19, 22, 23, 28, 30, 15, 18, 21, 22, 23, 28, 28, 31, 18, 18, 21, 22, 23, 28, 30, 33, 18, 21, 22, 23, 26, 28, 33, 39, 21, 22, 23, 26, 28, 33, 39, 47, 21, 22, 23, 28, 31, 37, 46, 56, 22, 23, 28, 31, 37, 46, 56, 68,},
{8, 14, 16, 19, 22, 23, 25, 29, 14, 14, 19, 21, 23, 25, 29, 32, 16, 19, 22, 23, 25, 29, 29, 33, 19, 19, 22, 23, 25, 29, 32, 35, 19, 22, 23, 25, 28, 30, 35, 42, 22, 23, 25, 28, 30, 35, 42, 50, 22, 23, 25, 29, 33, 40, 49, 60, 23, 25, 30, 33, 40, 49, 60, 72,},
{8, 14, 17, 20, 24, 24, 26, 31, 14, 14, 20, 22, 24, 26, 31, 34, 17, 20, 24, 24, 26, 31, 31, 35, 20, 20, 24, 24, 26, 31, 34, 36, 20, 24, 24, 26, 29, 32, 36, 44, 24, 24, 26, 29, 32, 36, 44, 53, 24, 24, 26, 31, 35, 42, 51, 63, 24, 26, 32, 35, 42, 51, 63, 76,},
{8, 15, 18, 21, 25, 26, 28, 33, 15, 15, 21, 23, 26, 28, 33, 36, 18, 21, 25, 26, 28, 33, 33, 37, 21, 21, 25, 26, 28, 33, 36, 38, 21, 25, 26, 28, 31, 34, 38, 46, 25, 26, 28, 31, 34, 38, 46, 56, 25, 26, 28, 33, 37, 44, 54, 67, 26, 28, 34, 37, 44, 54, 67, 80,},
};
#else
const int quant_tbl[16][64] = {
{  8,   5,   5,   6,   5,   6,   7,   7,   7,   7,   7,   7,   9,   8,   9,   9,   9,   9,   9,   9,   9,   9,   9,   9,   9,  10,  10,  10,  11,  11,  11,  10,  10,  10,   9,   9,  10,  10,  11,  11,  11,  11,  12,  13,  12,  12,  12,  11,  12,  13,  13,  13,  13,  13,  16,  16,  16,  16,  19,  19,  20,  24,  24,  29,},
{  8,   6,   6,   7,   6,   7,   8,   8,   8,   8,   8,   8,  10,   9,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  11,  11,  11,  13,  13,  13,  11,  11,  11,  10,  10,  11,  11,  12,  12,  13,  13,  14,  15,  14,  14,  14,  13,  14,  15,  15,  16,  16,  16,  19,  19,  18,  18,  22,  22,  23,  27,  27,  33,},
{  8,   7,   7,   8,   7,   8,   9,   9,   9,   9,   9,   9,  11,  10,  11,  12,  12,  12,  11,  11,  11,  11,  12,  12,  12,  13,  13,  13,  15,  15,  15,  13,  13,  13,  12,  12,  13,  13,  14,  14,  15,  15,  16,  17,  16,  15,  15,  15,  15,  17,  17,  18,  18,  18,  21,  21,  20,  20,  25,  25,  26,  31,  31,  37,},
{  8,   8,   8,   9,   8,   9,  11,  11,  11,  11,  11,  11,  13,  12,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  14,  14,  14,  17,  17,  17,  14,  14,  14,  13,  13,  14,  14,  16,  16,  17,  17,  18,  19,  18,  17,  17,  17,  17,  19,  19,  20,  20,  20,  24,  24,  23,  23,  28,  28,  29,  34,  34,  41,},
{  8,   8,   8,  10,   8,  10,  12,  12,  12,  12,  12,  12,  14,  13,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  15,  15,  15,  18,  18,  18,  15,  15,  15,  14,  14,  15,  15,  17,  17,  18,  18,  20,  20,  20,  19,  19,  18,  19,  20,  20,  21,  21,  21,  26,  26,  25,  25,  30,  30,  31,  37,  37,  45,},
{  8,   9,   9,  11,   9,  11,  13,  13,  13,  13,  13,  13,  15,  14,  15,  16,  16,  16,  15,  15,  15,  15,  16,  16,  16,  17,  17,  17,  20,  20,  20,  17,  17,  17,  16,  16,  17,  17,  19,  19,  20,  20,  22,  22,  22,  20,  20,  20,  20,  22,  22,  23,  23,  23,  28,  28,  27,  27,  33,  33,  34,  41,  41,  49,},
{  8,  10,  10,  11,  10,  11,  13,  13,  13,  13,  13,  13,  16,  15,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  16,  18,  18,  18,  21,  21,  21,  18,  18,  18,  16,  16,  18,  18,  20,  20,  21,  21,  23,  23,  23,  21,  21,  21,  21,  23,  23,  25,  25,  25,  30,  30,  28,  28,  35,  35,  36,  43,  43,  51,},
{  8,  10,  10,  12,  10,  12,  14,  14,  14,  14,  14,  14,  16,  15,  16,  17,  17,  17,  16,  16,  16,  16,  17,  17,  17,  18,  18,  18,  22,  22,  22,  18,  18,  18,  17,  17,  18,  18,  20,  20,  22,  22,  24,  24,  24,  22,  22,  22,  22,  24,  24,  26,  26,  26,  31,  31,  29,  29,  36,  36,  37,  44,  44,  53,},
{  8,  10,  10,  12,  10,  12,  14,  14,  14,  14,  14,  14,  17,  16,  17,  18,  18,  18,  17,  17,  17,  17,  18,  18,  18,  19,  19,  19,  22,  22,  22,  19,  19,  19,  18,  18,  19,  19,  21,  21,  22,  22,  24,  25,  24,  23,  23,  22,  23,  25,  25,  26,  26,  26,  32,  32,  31,  31,  37,  37,  39,  46,  46,  56,},
{  8,  11,  11,  13,  11,  13,  15,  15,  15,  15,  15,  15,  18,  16,  18,  18,  18,  18,  18,  18,  18,  18,  18,  18,  18,  20,  20,  20,  23,  23,  23,  20,  20,  20,  18,  18,  20,  20,  22,  22,  23,  23,  25,  26,  25,  24,  24,  23,  24,  26,  26,  28,  28,  28,  33,  33,  32,  32,  39,  39,  40,  48,  48,  58,},
{  8,  11,  11,  13,  11,  13,  15,  15,  15,  15,  15,  15,  18,  17,  18,  19,  19,  19,  18,  18,  18,  18,  19,  19,  19,  21,  21,  21,  24,  24,  24,  21,  21,  21,  19,  19,  21,  21,  23,  23,  24,  24,  26,  27,  26,  25,  25,  24,  25,  27,  27,  28,  28,  28,  34,  34,  33,  33,  40,  40,  42,  49,  49,  60,},
{  8,  12,  12,  14,  12,  14,  17,  17,  17,  17,  17,  17,  20,  18,  20,  20,  20,  20,  20,  20,  20,  20,  20,  20,  20,  22,  22,  22,  26,  26,  26,  22,  22,  22,  20,  20,  22,  22,  24,  24,  26,  26,  28,  29,  28,  27,  27,  26,  27,  29,  29,  31,  31,  31,  37,  37,  35,  35,  43,  43,  44,  53,  53,  64,},
{  8,  13,  13,  15,  13,  15,  18,  18,  18,  18,  18,  18,  21,  19,  21,  22,  22,  22,  21,  21,  21,  21,  22,  22,  22,  23,  23,  23,  28,  28,  28,  23,  23,  23,  22,  22,  23,  23,  26,  26,  28,  28,  30,  31,  30,  28,  28,  28,  28,  31,  31,  33,  33,  33,  39,  39,  37,  37,  46,  46,  47,  56,  56,  68,},
{  8,  14,  14,  16,  14,  16,  19,  19,  19,  19,  19,  19,  22,  21,  22,  23,  23,  23,  22,  22,  22,  22,  23,  23,  23,  25,  25,  25,  29,  29,  29,  25,  25,  25,  23,  23,  25,  25,  28,  28,  29,  29,  32,  33,  32,  30,  30,  29,  30,  33,  33,  35,  35,  35,  42,  42,  40,  40,  49,  49,  50,  60,  60,  72,},
{  8,  14,  14,  17,  14,  17,  20,  20,  20,  20,  20,  20,  24,  22,  24,  24,  24,  24,  24,  24,  24,  24,  24,  24,  24,  26,  26,  26,  31,  31,  31,  26,  26,  26,  24,  24,  26,  26,  29,  29,  31,  31,  34,  35,  34,  32,  32,  31,  32,  35,  35,  36,  36,  36,  44,  44,  42,  42,  51,  51,  53,  63,  63,  76,},
{  8,  15,  15,  18,  15,  18,  21,  21,  21,  21,  21,  21,  25,  23,  25,  26,  26,  26,  25,  25,  25,  25,  26,  26,  26,  28,  28,  28,  33,  33,  33,  28,  28,  28,  26,  26,  28,  28,  31,  31,  33,  33,  36,  37,  36,  34,  34,  33,  34,  37,  37,  38,  38,  38,  46,  46,  44,  44,  54,  54,  56,  67,  67,  80,},
};
#endif

const int qmem_tbl[16][64] = {
{4096, 6553, 5461, 4681, 3640, 3640, 3276, 2978, 6553, 6553, 4681, 4096, 3640, 3276, 2978, 2730, 5461, 4681, 3640, 3640, 3276, 2978, 2978, 2520, 4681, 4681, 3640, 3640, 3276, 2978, 2730, 2520, 4681, 3640, 3640, 3276, 2978, 2730, 2520, 2048, 3640, 3640, 3276, 2978, 2730, 2520, 2048, 1638, 3640, 3640, 3276, 2978, 2520, 2048, 1724, 1365, 3640, 3276, 2730, 2520, 2048, 1724, 1365, 1129,},
{4096, 5461, 4681, 4096, 3276, 3276, 2978, 2520, 5461, 5461, 4096, 3640, 3276, 2978, 2520, 2340, 4681, 4096, 3276, 3276, 2978, 2520, 2520, 2184, 4096, 4096, 3276, 3276, 2978, 2520, 2340, 2048, 4096, 3276, 3276, 2978, 2730, 2340, 2048, 1724, 3276, 3276, 2978, 2730, 2340, 2048, 1724, 1424, 3276, 3276, 2978, 2520, 2184, 1820, 1489, 1213, 3276, 2978, 2340, 2184, 1820, 1489, 1213, 992,},
{4096, 4681, 4096, 3640, 2978, 2730, 2520, 2184, 4681, 4681, 3640, 3276, 2730, 2520, 2184, 2048, 4096, 3640, 2978, 2730, 2520, 2184, 2184, 1927, 3640, 3640, 2978, 2730, 2520, 2184, 2048, 1820, 3640, 2978, 2730, 2520, 2340, 2184, 1820, 1560, 2978, 2730, 2520, 2340, 2184, 1820, 1560, 1260, 2978, 2730, 2520, 2184, 1927, 1638, 1310, 1057, 2730, 2520, 2184, 1927, 1638, 1310, 1057, 885,},
{4096, 4096, 3640, 2978, 2520, 2520, 2340, 1927, 4096, 4096, 2978, 2730, 2520, 2340, 1927, 1820, 3640, 2978, 2520, 2520, 2340, 1927, 1927, 1724, 2978, 2978, 2520, 2520, 2340, 1927, 1820, 1638, 2978, 2520, 2520, 2340, 2048, 1927, 1638, 1365, 2520, 2520, 2340, 2048, 1927, 1638, 1365, 1129, 2520, 2520, 2340, 1927, 1724, 1424, 1170, 963, 2520, 2340, 1927, 1724, 1424, 1170, 963, 799,},
{4096, 4096, 3276, 2730, 2340, 2340, 2184, 1820, 4096, 4096, 2730, 2520, 2340, 2184, 1820, 1638, 3276, 2730, 2340, 2340, 2184, 1820, 1820, 1638, 2730, 2730, 2340, 2340, 2184, 1820, 1638, 1560, 2730, 2340, 2340, 2184, 1927, 1724, 1560, 1260, 2340, 2340, 2184, 1927, 1724, 1560, 1260, 1057, 2340, 2340, 2184, 1820, 1638, 1310, 1092, 885, 2340, 2184, 1724, 1638, 1310, 1092, 885, 728,},
{4096, 3640, 2978, 2520, 2184, 2048, 1927, 1638, 3640, 3640, 2520, 2340, 2048, 1927, 1638, 1489, 2978, 2520, 2184, 2048, 1927, 1638, 1638, 1489, 2520, 2520, 2184, 2048, 1927, 1638, 1489, 1424, 2520, 2184, 2048, 1927, 1724, 1638, 1424, 1170, 2184, 2048, 1927, 1724, 1638, 1424, 1170, 963, 2184, 2048, 1927, 1638, 1489, 1213, 992, 799, 2048, 1927, 1638, 1489, 1213, 992, 799, 668,},
{4096, 3276, 2978, 2520, 2048, 2048, 1820, 1560, 3276, 3276, 2520, 2184, 2048, 1820, 1560, 1424, 2978, 2520, 2048, 2048, 1820, 1560, 1560, 1424, 2520, 2520, 2048, 2048, 1820, 1560, 1424, 1310, 2520, 2048, 2048, 1820, 1638, 1560, 1310, 1092, 2048, 2048, 1820, 1638, 1560, 1310, 1092, 910, 2048, 2048, 1820, 1560, 1424, 1170, 936, 762, 2048, 1820, 1560, 1424, 1170, 936, 762, 642,},
{4096, 3276, 2730, 2340, 2048, 1927, 1820, 1489, 3276, 3276, 2340, 2184, 1927, 1820, 1489, 1365, 2730, 2340, 2048, 1927, 1820, 1489, 1489, 1365, 2340, 2340, 2048, 1927, 1820, 1489, 1365, 1260, 2340, 2048, 1927, 1820, 1638, 1489, 1260, 1057, 2048, 1927, 1820, 1638, 1489, 1260, 1057, 885, 2048, 1927, 1820, 1489, 1365, 1129, 910, 744, 1927, 1820, 1489, 1365, 1129, 910, 744, 618,},
{4096, 3276, 2730, 2340, 1927, 1820, 1724, 1489, 3276, 3276, 2340, 2048, 1820, 1724, 1489, 1365, 2730, 2340, 1927, 1820, 1724, 1489, 1489, 1310, 2340, 2340, 1927, 1820, 1724, 1489, 1365, 1260, 2340, 1927, 1820, 1724, 1560, 1424, 1260, 1024, 1927, 1820, 1724, 1560, 1424, 1260, 1024, 840, 1927, 1820, 1724, 1489, 1310, 1057, 885, 712, 1820, 1724, 1424, 1310, 1057, 885, 712, 585,},
{4096, 2978, 2520, 2184, 1820, 1820, 1638, 1424, 2978, 2978, 2184, 2048, 1820, 1638, 1424, 1310, 2520, 2184, 1820, 1820, 1638, 1424, 1424, 1260, 2184, 2184, 1820, 1820, 1638, 1424, 1310, 1170, 2184, 1820, 1820, 1638, 1489, 1365, 1170, 992, 1820, 1820, 1638, 1489, 1365, 1170, 992, 819, 1820, 1820, 1638, 1424, 1260, 1024, 840, 682, 1820, 1638, 1365, 1260, 1024, 840, 682, 564,},
{4096, 2978, 2520, 2184, 1820, 1724, 1560, 1365, 2978, 2978, 2184, 1927, 1724, 1560, 1365, 1260, 2520, 2184, 1820, 1724, 1560, 1365, 1365, 1213, 2184, 2184, 1820, 1724, 1560, 1365, 1260, 1170, 2184, 1820, 1724, 1560, 1424, 1310, 1170, 963, 1820, 1724, 1560, 1424, 1310, 1170, 963, 780, 1820, 1724, 1560, 1365, 1213, 992, 819, 668, 1724, 1560, 1310, 1213, 992, 819, 668, 546,},
{4096, 2730, 2340, 1927, 1638, 1638, 1489, 1260, 2730, 2730, 1927, 1820, 1638, 1489, 1260, 1170, 2340, 1927, 1638, 1638, 1489, 1260, 1260, 1129, 1927, 1927, 1638, 1638, 1489, 1260, 1170, 1057, 1927, 1638, 1638, 1489, 1365, 1213, 1057, 885, 1638, 1638, 1489, 1365, 1213, 1057, 885, 744, 1638, 1638, 1489, 1260, 1129, 936, 762, 618, 1638, 1489, 1213, 1129, 936, 762, 618, 512,},
{4096, 2520, 2184, 1820, 1560, 1489, 1424, 1170, 2520, 2520, 1820, 1724, 1489, 1424, 1170, 1092, 2184, 1820, 1560, 1489, 1424, 1170, 1170, 1057, 1820, 1820, 1560, 1489, 1424, 1170, 1092, 992, 1820, 1560, 1489, 1424, 1260, 1170, 992, 840, 1560, 1489, 1424, 1260, 1170, 992, 840, 697, 1560, 1489, 1424, 1170, 1057, 885, 712, 585, 1489, 1424, 1170, 1057, 885, 712, 585, 481,},
{4096, 2340, 2048, 1724, 1489, 1424, 1310, 1129, 2340, 2340, 1724, 1560, 1424, 1310, 1129, 1024, 2048, 1724, 1489, 1424, 1310, 1129, 1129, 992, 1724, 1724, 1489, 1424, 1310, 1129, 1024, 936, 1724, 1489, 1424, 1310, 1170, 1092, 936, 780, 1489, 1424, 1310, 1170, 1092, 936, 780, 655, 1489, 1424, 1310, 1129, 992, 819, 668, 546, 1424, 1310, 1092, 992, 819, 668, 546, 455,},
{4096, 2340, 1927, 1638, 1365, 1365, 1260, 1057, 2340, 2340, 1638, 1489, 1365, 1260, 1057, 963, 1927, 1638, 1365, 1365, 1260, 1057, 1057, 936, 1638, 1638, 1365, 1365, 1260, 1057, 963, 910, 1638, 1365, 1365, 1260, 1129, 1024, 910, 744, 1365, 1365, 1260, 1129, 1024, 910, 744, 618, 1365, 1365, 1260, 1057, 936, 780, 642, 520, 1365, 1260, 1024, 936, 780, 642, 520, 431,},
{4096, 2184, 1820, 1560, 1310, 1260, 1170, 992, 2184, 2184, 1560, 1424, 1260, 1170, 992, 910, 1820, 1560, 1310, 1260, 1170, 992, 992, 885, 1560, 1560, 1310, 1260, 1170, 992, 910, 862, 1560, 1310, 1260, 1170, 1057, 963, 862, 712, 1310, 1260, 1170, 1057, 963, 862, 712, 585, 1310, 1260, 1170, 992, 885, 744, 606, 489, 1260, 1170, 963, 885, 744, 606, 489, 409,},
};

const char black_jpg[627]={
0xFF,0xD8,0xFF,0xC0,0x00,0x11,0x08,0x04,0x40,0x07,0x80,0x03,0x01,0x22,0x00,0x02,
0x11,0x01,0x03,0x11,0x01,0xFF,0xC4,0x00,0x1F,0x00,0x00,0x01,0x05,0x01,0x01,0x01,
0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x03,0x04,0x05,
0x06,0x07,0x08,0x09,0x0A,0x0B,0xFF,0xC4,0x00,0xB5,0x10,0x00,0x02,0x01,0x03,0x03,
0x02,0x04,0x03,0x05,0x05,0x04,0x04,0x00,0x00,0x01,0x7D,0x01,0x02,0x03,0x00,0x04,
0x11,0x05,0x12,0x21,0x31,0x41,0x06,0x13,0x51,0x61,0x07,0x22,0x71,0x14,0x32,0x81,
0x91,0xA1,0x08,0x23,0x42,0xB1,0xC1,0x15,0x52,0xD1,0xF0,0x24,0x33,0x62,0x72,0x82,
0x09,0x0A,0x16,0x17,0x18,0x19,0x1A,0x25,0x26,0x27,0x28,0x29,0x2A,0x34,0x35,0x36,
0x37,0x38,0x39,0x3A,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x53,0x54,0x55,0x56,
0x57,0x58,0x59,0x5A,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x73,0x74,0x75,0x76,
0x77,0x78,0x79,0x7A,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x92,0x93,0x94,0x95,
0x96,0x97,0x98,0x99,0x9A,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xB2,0xB3,
0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,
0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,
0xE8,0xE9,0xEA,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFF,0xC4,0x00,
0x1F,0x01,0x00,0x03,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0xFF,0xC4,
0x00,0xB5,0x11,0x00,0x02,0x01,0x02,0x04,0x04,0x03,0x04,0x07,0x05,0x04,0x04,0x00,
0x01,0x02,0x77,0x00,0x01,0x02,0x03,0x11,0x04,0x05,0x21,0x31,0x06,0x12,0x41,0x51,
0x07,0x61,0x71,0x13,0x22,0x32,0x81,0x08,0x14,0x42,0x91,0xA1,0xB1,0xC1,0x09,0x23,
0x33,0x52,0xF0,0x15,0x62,0x72,0xD1,0x0A,0x16,0x24,0x34,0xE1,0x25,0xF1,0x17,0x18,
0x19,0x1A,0x26,0x27,0x28,0x29,0x2A,0x35,0x36,0x37,0x38,0x39,0x3A,0x43,0x44,0x45,
0x46,0x47,0x48,0x49,0x4A,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x63,0x64,0x65,
0x66,0x67,0x68,0x69,0x6A,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x82,0x83,0x84,
0x85,0x86,0x87,0x88,0x89,0x8A,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0xA2,
0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,
0xBA,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
0xD8,0xD9,0xDA,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xF2,0xF3,0xF4,0xF5,
0xF6,0xF7,0xF8,0xF9,0xFA,0xFF,0xE0,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xDB,0x00,0x43,0x00,0x08,0x07,0x07,0x08,
0x07,0x08,0x09,0x09,0x09,0x09,0x09,0x09,0x0B,0x0A,0x0B,0x0C,0x0C,0x0C,0x0B,0x0B,
0x0B,0x0B,0x0C,0x0C,0x0C,0x0D,0x0D,0x0D,0x0F,0x0F,0x0F,0x0D,0x0D,0x0D,0x0C,0x0C,
0x0D,0x0D,0x0E,0x0E,0x0F,0x0F,0x10,0x11,0x10,0x0F,0x0F,0x0F,0x0F,0x11,0x11,0x12,
0x12,0x12,0x15,0x15,0x14,0x14,0x19,0x19,0x1A,0x1F,0x1F,0x25,0xFF,0xDB,0x00,0x43,
0x01,0x08,0x07,0x07,0x08,0x07,0x08,0x09,0x09,0x09,0x09,0x09,0x09,0x0B,0x0A,0x0B,
0x0C,0x0C,0x0C,0x0B,0x0B,0x0B,0x0B,0x0C,0x0C,0x0C,0x0D,0x0D,0x0D,0x0F,0x0F,0x0F,
0x0D,0x0D,0x0D,0x0C,0x0C,0x0D,0x0D,0x0E,0x0E,0x0F,0x0F,0x10,0x11,0x10,0x0F,0x0F,
0x0F,0x0F,0x11,0x11,0x12,0x12,0x12,0x15,0x15,0x14,0x14,0x19,0x19,0x1A,0x1F,0x1F,
0x25,0xFF,0xDA,0x00,0x0C,0x03,0x01,0x00,0x02,0x11,0x03,0x11,0x00,0x3F,0x00,0xF9,
0xFE,0x8A,0x28};

void mjpeg_enc_black_jpg(unsigned int width, unsigned int height, unsigned int *addr)
{
unsigned int offset=0;
unsigned int wh=0;
int coef_s=0xF9FE8A28;
int coef_n=0xA0028A28;
int coef_e=0xA00FFFD9;
int i=0,repeat=0;
int *target_addr;

  // copy header to memory
  memcpy((void*)addr,&black_jpg,sizeof(black_jpg));

  offset=7;

  target_addr = (int *)(addr + offset);

  wh=(height<<16)|(width&0xFFFF);
  memcpy(target_addr , &wh, sizeof(int));
  
  //[F9 FE 8A 28] + [A0 02 8A 28]* repeat N  + [A0 0F FF D9] 
  //N=w/16 * h/16 -1

  offset=623;
  target_addr=(int *)(addr + offset);
  memset(target_addr,coef_s,sizeof(int));

  target_addr+=sizeof(int);

  repeat=width/16+height/16 -1;
  
  for (i = 0; i < repeat; ++i) 
  memcpy(target_addr + (i * sizeof(int)), &coef_n, sizeof(int));
  
  target_addr+=sizeof(int);
  memset(target_addr,coef_e,sizeof(int));
  
}
	
void mjpeg_enc_qmem_table (void)
{
int table_idx,row_idx;
int temp,temp0,temp1;

  for(table_idx=0; table_idx<16; table_idx+=1)
  {
    for(row_idx=0;row_idx<8;row_idx+=1)
    {
  	  temp0=qmem_tbl[table_idx][row_idx+6*8];
  	  temp1=qmem_tbl[table_idx][row_idx+4*8];
  	  temp=(temp0+(temp1<<16))&0xffffffff;
  	  rtd_outl(MJPEG_ENC_mjpg_enc_ld_qtable_reg,temp);
  
      temp0=qmem_tbl[table_idx][row_idx+2*8];
      temp1=qmem_tbl[table_idx][row_idx+0*8];
      temp=(temp0+(temp1<<16))&0xffffffff;
      rtd_outl(MJPEG_ENC_mjpg_enc_ld_qtable_reg,temp);
  	
      temp0=qmem_tbl[table_idx][row_idx+7*8];
      temp1=qmem_tbl[table_idx][row_idx+5*8];
      temp=(temp0+(temp1<<16))&0xffffffff;
      rtd_outl(MJPEG_ENC_mjpg_enc_ld_qtable_reg,temp);
  
      temp0=qmem_tbl[table_idx][row_idx+3*8];
      temp1=qmem_tbl[table_idx][row_idx+1*8];
      temp=(temp0+(temp1<<16))&0xffffffff;
      rtd_outl(MJPEG_ENC_mjpg_enc_ld_qtable_reg,temp); 
    }
  }
}

void mjpeg_enc_quant_table(void)
{
int j;

  for (j=0;j<1024;j+=4)
  {
    int vlc_temp0,vlc_temp1,vlc_temp2,vlc_temp3,vlc_temp;  
    vlc_temp0=quant_tbl[(j/64)][(j%64)];
    vlc_temp1=quant_tbl[((j+1)/64)][((j+1)%64)];
    vlc_temp2=quant_tbl[((j+2)/64)][((j+2)%64)];
	vlc_temp3=quant_tbl[((j+3)/64)][((j+3)%64)];
	vlc_temp=((vlc_temp3<<24)+(vlc_temp2<<16)+(vlc_temp1<<8)+vlc_temp0)&0xffffffff;
    rtd_outl(MJPEG_ENC_mjpg_enc_ld_qtable_reg,vlc_temp);   
	
  }

}


#ifdef FPGA_TEST

void IMD_bypass_setting(void)
{
return;
	//pm_printk(LOGGER_ERROR, "\n IMD_bypass_setting\n");
#if 0
	//vby1 display 4K default);
	//dw 0xb802d904 , 01b008c9 (iv2dv dvtotal);
	//dw 0xb802d908 , 112f112f (dhtotal dh last line);
	//dw 0xb802d90c , 00001f03 (hswidth 15~8 vswidth 7~0);
	//dw 0xb802d910 , 01181018 (dh_den_st dh_den_end);
	//dw 0xb802d914 , 002d089d (dv_den_st dv_den_end);

	//dh total , 897);
	//dh denst , 8c end+780);
	//dv debst , 16 end+438);

	// 2kto4k timing);
	rtd_outl(0xb808d970,0x0000031f);
	rtd_outl(0xb808d974,0x08c91130);
	rtd_outl(0xb808d978,0x002c089c);
	rtd_outl(0xb808d97c,0x01181018);

	// vby1 timing adjust);
	rtd_outl(0xb808d914,0x002c089c);

	//vby1 pq bypass);
	rtd_outl(0xb808d900,0x003b0001);

	//========================================
	// IMD START
	//========================================
	// vgip
	rtd_outl(0xb8020210, 0x80000003);
	rtd_outl(0xb8020218, 0x00000780);
	rtd_outl(0xb802021c, 0x00000438);
	rtd_outl(0xb8020224, 0x20002900);
	//rtd_outl(0xb8020224, 0x20002904); // add channel swap

	// ds444
	rtd_outl(0xb8023b1c, 0x00000003);

	// multi band
	rtd_outl(0xb8023ce8, 0x00000001);

	// srnn prepost
	rtd_outl(0xb8025600, 0x00000002);
	rtd_outl(0xb8025604, 0x07800438);
	rtd_outl(0xb8025804, 0x07800438);

	// two step uzu
	rtd_outl(0xb8029604, 0x07800438);
	rtd_outl(0xb8029b2c, 0x07800438);

	// dctl
	rtd_outl(0xb8027004, 0x0000001f);
	rtd_outl(0xb8027008, 0x08970000);
	rtd_outl(0xb802700c, 0x00000002);
	rtd_outl(0xb8027010, 0x00000464);
	rtd_outl(0xb8027014, 0x008c080c);
	rtd_outl(0xb8027018, 0x0016044e);
	rtd_outl(0xb8027400, 0x00000006); //
	rtd_outl(0xb8027420, 0x00580000); //
	rtd_outl(0xb8027440, 0x00d30000); //
	rtd_outl(0xb8027000, 0x80000010);
	rtd_outl(0xb8027000, 0x80000011);

	// dctl bypass
	rtd_outl(0xb8027804, 0x0000001f);
	rtd_outl(0xb8027808, 0x08970000);
	rtd_outl(0xb802780c, 0x00000002);
	rtd_outl(0xb8027810, 0x00000464);
	rtd_outl(0xb8027814, 0x008c080c);
	rtd_outl(0xb8027818, 0x0016044e);
	rtd_outl(0xb8027840, 0x008c080c);
	rtd_outl(0xb8027844, 0x0016044e);
	rtd_outl(0xb8027800, 0x80000010); // no need fix_last_line
	rtd_outl(0xb8027800, 0x80000011);
#endif
}

void frc_2kto1k_uzd_enc_dec_uzu(void)
{
	// uzd enc
	//rtd_outl(0xb8085204, 0x00100100); // [9] out bit : 0-8   / 1-10
									  // [8] sort fmt: 0-422 / 1-444
	//rtd_outl(0xb808520c, 0x00200000);
	//rtd_outl(0xb8085210, 0x00200000);
	//rtd_outl(0xb8085214, 0x00008080);
	//rtd_outl(0xb8085240, 0x04380780);

	// uzd enc
	rtd_outl(0xb8085204, 0x00100103); // [9] out bit : 0-8	 / 1-10
									  // [8] sort fmt: 0-422 / 1-444
	rtd_outl(0xb808520c, 0x00200000);
	rtd_outl(0xb8085210, 0x00200000);
	rtd_outl(0xb8085214, 0x00008080);
	rtd_outl(0xb8085240, 0x021c03c0);


	// hdr2sdr
	rtd_outl(0xb8082c00, 0x00000000); // in_sel[31]
	// rgb2yuv
	rtd_outl(0xb8082c00, 0x00000020);
	// yuv2rgb
	rtd_outl(0xb8082804, 0x00000001);

	// 1920x1080 422 8bit (10bit use only 8)
	// 128bit based
	// 1920*16(8/4/4) / 128 = line_burst_num
	// line_step = line_burst_num (because unit=128)
	// *** frc cap ***
	// *** test ***
	rtd_outl(0xb8083750, 0x00000898); // htotal
	rtd_outl(0xb8083684, 0x00100438);
	//rtd_outl(0xb8083684, 0x04380010); // buf_ok end, sta
	// *** test ***
	rtd_outl(0xb8083688, 0x000a0000);
	rtd_outl(0xb8083640, 0x40000000); // waddr0
	rtd_outl(0xb8083644, 0x50000000); // waddr1
	rtd_outl(0xb8083648, 0x60000000); // waddr2
	rtd_outl(0xb808364c, 0x70000000); // waddr3
	rtd_outl(0xb8083650, 0x00000000); // line step = 0 (frame mode)
	///rtd_outl(0xb8083654, 0x043803c0); // line num / burst_len
	rtd_outl(0xb8083654, 0x04380040); // line num / burst_len
	rtd_outl(0xb8083658, 0x0000ffff); // line_burst_num [23:0]
	rtd_outl(0xb8083608, 0x00000381);
	rtd_outl(0xb808368c, 0x00000000); // index distance = 1
	// *** down-sample ***
	rtd_outl(0xb8083770, 0x00003c07); // ds, h_szie
	rtd_outl(0xb8083774, 0x05000a00); // ds
	rtd_outl(0xb8083778, 0x00000100); // ds
	// *** path contrl ***
	rtd_outl(0xb8083764, 0x821c03c0); // wrap, vact, hact
	rtd_outl(0xb8083760, 0x00000006); // sram insel, clock, mux
#if 1
	// *** enc ***
	rtd_outl(0xb8084e14, 0x000000ff); // q table
	rtd_outl(0xb8084e00, 0x1d438780); // height, width
	rtd_outl(0xb8084e68, 0x00094000);
	rtd_outl(0xb8084e70, 0x000016b6);
	rtd_outl(0xb8084e44, 0x00000800);
	rtd_outl(0xb8084e48, 0x260f0003);
	rtd_outl(0xb8084e4c, 0x01220002);
	rtd_outl(0xb8084e50, 0x11010311);
	rtd_outl(0xb8084e54, 0x00000001);
	rtd_outl(0xb8084e04, 0x00000003);

    
	rtd_outl(0xb8004e00, 0xb9438780); // load qmem,height, width
	mjpeg_enc_qmem_table();

	rtd_outl(0xb8004e00, 0xd9438780); // load quant,height, width
	mjpeg_enc_quant_table();

	rtd_outl(0xb8004e14, 0x000000ff); // q table
	
	rtd_outl(0xb8004e00, 0x99438780); // height, width
	
	rtd_outl(0xb8004e68, 0x00094000);
	rtd_outl(0xb8004e70, 0x000016b6);
	rtd_outl(0xb8004e44, 0x00110804);				  
	rtd_outl(0xb8004e48, 0x38078003);	 
	rtd_outl(0xb8004e4c, 0x01220002);
	rtd_outl(0xb8004e50, 0x11010311);
	rtd_outl(0xb8004e54, 0x00000001);
	rtd_outl(0xb8004e04, 0x00000003);

	
#else
    set_MJPEG_ENC();
#endif
	// *** disp ***
	rtd_outl(0xb8083920, 0x40000260); // raddr0
	rtd_outl(0xb8083924, 0x50000260); // raddr1
	rtd_outl(0xb8083928, 0x60000260); // raddr2
	rtd_outl(0xb808392c, 0x70000260); // raddr3
	rtd_outl(0xb8083930, 0x00000000); // line step = 0 (frame mode)
	///rtd_outl(0xb8083934, 0x043803c0); // line num / burst_len [10:4]
	rtd_outl(0xb8083934, 0x04380040); // line num / burst_len [10:4]
	rtd_outl(0xb8083938, 0x0000ffff); // line_burst_num [23:0]
	rtd_outl(0xb8083908, 0x00000381);
	// *** path contrl ***
	rtd_outl(0xb8083a80, 0x00000102);
	rtd_outl(0xb8083a98, 0x00400001);
	rtd_outl(0xb8083a88, 0x021c03c0);
	rtd_outl(0xb8083a70, 0x00000010);
#if 0
	// *** dec ***
	rtd_outl(0xb8085f00, 0x3921c3c0);
	rtd_outl(0xb8085f3c, 0x0f000000);
	rtd_outl(0xb8085f04, 0x00000003);
#else
    set_MJPEG_DEC();
#endif
	//
	rtd_outl(0xb8083680, 0x00000020);

	// su
	//rtd_outl(0xb8089004, 0x10000010);
	//rtd_outl(0xb8089008, 0x0007ffff);
	//rtd_outl(0xb808900c, 0x0007ffff);
	//rtd_outl(0xb8089028, 0x07800438);
	//rtd_outl(0xb80890c8, 0x07800438);
	//rtd_outl(0xb8089128, 0x07800438);
	//rtd_outl(0xb80891c8, 0x07800438);

	// su
	rtd_outl(0xb8089004, 0x10000013);
	rtd_outl(0xb8089008, 0x0007ffff);
	rtd_outl(0xb808900c, 0x0007ffff);
	rtd_outl(0xb8089028, 0x03c0021c);
	rtd_outl(0xb80890c8, 0x03c0021c);
	rtd_outl(0xb8089128, 0x03c0021c);
	rtd_outl(0xb80891c8, 0x03c0021c);

	// dc2hlk tg
	rtd_outl(0xb8088100, 0x00000897);
	rtd_outl(0xb8088104, 0x00000464);
	rtd_outl(0xb8088108, 0x008c080c);
	rtd_outl(0xb808810c, 0x0016044e);

	rtd_outl(0xb8088008, 0x00000005);
	rtd_outl(0xb8088000, 0x71000700);

	// dc2h path
	rtd_outl(0xb80408fc, 0x80000000);
	rtd_outl(0xb80408f4, 0x00000000); // line dly
	rtd_outl(0xb80408f8, 0x000003cf); // pxl dly


}


#endif



unsigned short UZD_GetPicWidth(void)
{
unsigned short w;
 w=1920;
return w;
}

unsigned short UZD_GetPicHeight(void)
{
unsigned short h;
 h=1080;
return h;
}

unsigned char Get_Val_outputfmt(void)
{
#ifdef FPGA_TEST
 output_fmt=0;
#else
 output_fmt=0;
#endif
 return output_fmt;
}

void Set_Val_output_fmt(unsigned char value)
{
 output_fmt=value;
}


unsigned char Get_Val_demo_mode(void)
{
 demo_mode=0;
 return demo_mode;
}

void Set_Val_demo_mode(unsigned char value)
{
 demo_mode=value;
}

void Set_Val_demo_mode_jpeg_addr(unsigned int addr)
{
 //pm_printk(LOGGER_ERROR, "Set_Val_demo_mode_jpeg_addr :%x\n", addr);
 demo_mode_jpeg=addr;
}



unsigned int Get_Val_demo_mode_jpeg_addr(void)
{
 return demo_mode_jpeg;
}

#if 0
unsigned int Get_Val_nosignal_jpeg_addr(void)
{
 return (unsigned long int)&no_signal_jpeg;
}

unsigned int Get_Val_nosupport_jpeg_addr(void)
{
 return (unsigned long int)&no_support_jpeg;
}
#endif

unsigned char Get_Val_nosignal_mode(void)
{
 no_signal_mode = 0;

 return no_signal_mode;
}

unsigned char Get_Val_nosupport_mode(void)
{
 no_support_mode = 0;

 return no_support_mode;
}

unsigned char Get_Val_HDCP_mode(void)
{
 no_support_mode = 0;

 return hdcp_mode;
}

void Set_Val_nosupport_mode(unsigned char value)
{
 no_support_mode=value;
}

void Set_Val_nosignal_mode(unsigned char value)
{
 no_signal_mode=value;
}

void Set_Val_yc_spearate(unsigned char val)
{
yc_separate=val;
}

unsigned int y_buf_size;
unsigned int c_buf_size;

unsigned int mjpeg_get_YBufSize(void)
{
	return y_buf_size;
}

unsigned int mjpeg_get_CBufSize(void)
{
	return c_buf_size;
}
void mjpeg_enc_init(unsigned short pic_width,unsigned short pic_height)
{
//unsigned int y_buf_size;
//unsigned int c_buf_size;
unsigned short avg_mb_size_y;
//unsigned short mb_num;

// mb_num=(pic_width>>4) * (pic_height>>4);
// avg_mb_size_y=(y_buf_size<<3)/mb_num;
// avg_mb_size_c=(c_buf_size<<3)/mb_num;

// mb_num=32414

 if(1)//DDR Model
 {
	 if(pic_width==3840 && pic_height==2160)//4K
	 {
		y_buf_size=0x1FFFFF;
		avg_mb_size_y=0x205;
	 }
	 else if(pic_width< 3840 && pic_height< 2160 && pic_width>= 2560 && pic_height >= 1440) // 2.5K(2560x1440)
	 {
		y_buf_size=0x1FFFFF;
		avg_mb_size_y=0x400;
	 }
	 else // 2K
	 {
		y_buf_size=0x1FFFFF;
		avg_mb_size_y=0x808;
	 }
 }
 
  rtd_outl(MJPEG_ENC_enc_budget0_reg ,y_buf_size);
  rtd_outl(MJPEG_ENC_avg_mb_size_reg ,avg_mb_size_y);

  rtd_outl(MJPEG_ENC_mjpg_enc_ctrl_reg ,MJPEG_ENC_mjpg_enc_ctrl_write_enable4(1)|MJPEG_ENC_mjpg_enc_ctrl_load_qmem(1)) ;
  mjpeg_enc_qmem_table();

  rtd_outl(MJPEG_ENC_mjpg_enc_ctrl_reg ,MJPEG_ENC_mjpg_enc_ctrl_write_enable4(1)|MJPEG_ENC_mjpg_enc_ctrl_load_qmem_head(1)) ;
  mjpeg_enc_quant_table();

  rtd_outl(MJPEG_ENC_mjpg_enc_ctrl_reg ,MJPEG_ENC_mjpg_enc_ctrl_write_enable4(1)|MJPEG_ENC_mjpg_enc_ctrl_load_qmem_head(0)|MJPEG_ENC_mjpg_enc_ctrl_load_qmem(0)) ;


rtd_outl(MJPEG_ENC_mjpg_enc_quant_reg ,0x00);//init qp index to 15.

}

unsigned char dqt_last_value[16]=
{
  29,33,37,41,45,49,51,53,56,58,60,64,68,72,76,80
};

unsigned char Get_Val_dqt_last_value(unsigned char id)
{
	return dqt_last_value[id];
}

unsigned char Get_Val_dqt_id(unsigned char dqt_value)
{
unsigned char i=0;
	for(i=0;i<16;i++)
	{
	 if(dqt_value==Get_Val_dqt_last_value(i))
	  break;
	}
return i;
}

unsigned int mjpeg_enc_write_jpeg_SOF(unsigned short pic_width,unsigned short pic_height)
{
 unsigned char  enc_enable;

 //yc_separate=1;

 //if(pic_width>3840 && pic_height>2160)
 //return 0xFFFF;

 enc_enable=1;
 //val= MJPEG_ENC_mjpg_enc_ctrl_write_enable3(1)|((unsigned int)enc_enable<<27)|MJPEG_ENC_mjpg_enc_ctrl_write_enable2(1)|((unsigned int)yc_separate<<25)|MJPEG_ENC_mjpg_enc_ctrl_write_enable1(1)|((unsigned int)pic_height<<12) | (unsigned int)pic_width;
 *((unsigned char*)&val + 3) = 0x18 | (0x04 | ((yc_separate & 1) << 1)) | 0x01; 	// MJPEG_ENC_mjpg_enc_ctrl_write_enable3(1)|((unsigned int)enc_enable<<27)|MJPEG_ENC_mjpg_enc_ctrl_write_enable2(1)|((unsigned int)yc_separate<<25) |MJPEG_ENC_mjpg_enc_ctrl_write_enable1(1)
 *((unsigned char*)&val + 2) = (*((unsigned short*)&pic_height) & 0xff0) >> 4;				//(pic_height<<12) | pic_width
 *((unsigned short*)&val) = (*((unsigned short*)&pic_width) & 0x0fff) | ((*((unsigned short*)&pic_height) & 0xf) << 12); // pic_width;
 rtd_outl(MJPEG_ENC_mjpg_enc_ctrl_reg ,val) ;

 val=_SOI_SOF_;
 rtd_outl(MJPEG_ENC_jpg_head_soi_sof_reg,val);

 val=((unsigned int)SOF_Lf<<16)|((unsigned int)SOF_P <<8) |((unsigned int)pic_height) >>8;//SOF DATA[135:104]
 rtd_outl(MJPEG_ENC_jpg_head_sof_data1_reg,val);

 val=((unsigned int)pic_height<<24)| ((unsigned int)pic_width<<8) | (unsigned int)SOF_Nf ;   //SOF DATA[103:72]
 rtd_outl(MJPEG_ENC_jpg_head_sof_data2_reg,val);


 val= ((unsigned int)SOF_C1<<24)|((unsigned int)SOF_H1<<20)|((unsigned int)SOF_V1<<16)|((unsigned int)SOF_Tq1<<8) |((unsigned int)SOF_C2); //SOF DATA[103:72]
 rtd_outl(MJPEG_ENC_jpg_head_sof_data3_reg,val); //SOF DATA[71:40]


 val =((unsigned int)SOF_H2<<28)|((unsigned int)SOF_V2<<24)|((unsigned int)SOF_Tq2<<16)|((unsigned int)SOF_C3<<8)|((unsigned int)SOF_H3<<4)|((unsigned int)SOF_V3); //SOF DATA[39:8]
 rtd_outl(MJPEG_ENC_jpg_head_sof_data4_reg,val);

 rtd_outl(MJPEG_ENC_jpg_head_sof_data5_reg,SOF_Tq3);//SOF DATA[7:0]


 return MJPEG_RTN_OK;
}


unsigned int mjpeg_enc_write_jpeg_SOS(void)
{

  val = ((unsigned int)SOS_Td3<<28)|((unsigned int)SOS_Ta3<<24)|((unsigned int)SOS_Ss<<16)|((unsigned int)SOS_Se<<8)|((unsigned int)SOS_Ah<<4)|SOS_Al;//SOS[95:64]
  rtd_outl(MJPEG_ENC_jpg_head0_reg, val) ;


  val=((unsigned int)SOS_Td1<<28)|((unsigned int)SOS_Ta1<<24)|((unsigned int)SOS_Cs2<<16)|((unsigned int)SOS_Td2<<12)|((unsigned int)SOS_Ta2<<8)|SOS_Cs3;//SOS[63:32]
  rtd_outl(MJPEG_ENC_jpg_head1_reg, val);


  val=((unsigned int)SOS_Ls<<16)|((unsigned int)SOS_Ns<<8)|SOS_Cs1;//SOS[31:0]
  rtd_outl(MJPEG_ENC_jpg_head2_reg, val);

  val =((unsigned int)APP0_Length<<24|_SOS_);//SOS[111:96]
  rtd_outl(MJPEG_ENC_jpg_head3_reg, val);


  return MJPEG_RTN_OK;


}


unsigned int Get_Val_bitcnt(void)
{
return rtd_inl(MJPEG_ENC_vlc_bitcnt_reg);
}

unsigned int Get_Val_enc_crc(void)
{
return rtd_inl(MJPEG_ENC_enc_crc_reg);
}


int mjpeg_enc_done_isr(void)
{
	unsigned int int_status = rtd_inl(MJPEG_ENC_mjpg_enc_int_status_reg) ;

	if(int_status&0x2)
	{
     //enc done.print log for check here.
	}

return 1;
}

void set_MJPEG_ENC(void)
{
 unsigned char enc_enable=0;
 unsigned int  uvc_reg=0,tmp_reg=0;

 pic_width=uvc_info.Cap_Wid;
 pic_height=uvc_info.Cap_Len;

 rtd_pr_vdec_info("set_MJPEG_ENC pic_width=%x\n",(unsigned int)pic_width);
 rtd_pr_vdec_info("set_MJPEG_ENC pic_height=%x\n",(unsigned int)pic_height);

 uvc_reg=rtd_inl(UVC_VGIP_UVC_VGIP_CTRL_reg);//get_UVC_VGIP_UVC_VGIP_CTRL_reg; 

 if(UVC_VGIP_UVC_VGIP_CTRL_uvc_in_clk_en(uvc_reg)==0)
  rtd_outl( UVC_VGIP_UVC_VGIP_CTRL_reg ,uvc_reg|UVC_VGIP_UVC_VGIP_CTRL_uvc_in_clk_en_mask);// [0]: reg_uvc_in_clk_en

 rtd_outl(MJPEG_ENC_mjpg_enc_int_en_reg,MJPEG_ENC_mjpg_enc_int_en_enc_done_ie(1)|MJPEG_ENC_mjpg_enc_int_en_write_data(1));
 rtd_outl(MJPEG_ENC_mjpg_enc_dbg_sel_reg,MJPEG_ENC_mjpg_enc_dbg_sel_crc_rst_by_hw(1));

 tmp_reg=rtd_inl(MJPEG_ENC_vlc_ctrl0_reg);
 rtd_outl(MJPEG_ENC_vlc_ctrl0_reg,tmp_reg&~MJPEG_ENC_vlc_ctrl0_fixed_opt_level_mask);

 val=MJPEG_ENC_mjpg_enc_ctrl_write_enable3(1)|((unsigned int)enc_enable<<27);
 rtd_outl(MJPEG_ENC_mjpg_enc_ctrl_reg , val);

 mjpeg_enc_init(pic_width,pic_height);
 
 mjpeg_enc_write_jpeg_SOS();
 mjpeg_enc_write_jpeg_SOF(pic_width,pic_height);

 val=MJPEG_ENC_mjpg_enc_int_en_enc_done_ie(1)|MJPEG_ENC_mjpg_enc_int_en_write_data(1);
 rtd_outl(MJPEG_ENC_mjpg_enc_int_en_reg , val);

}
