#ifndef _HAL_VT_H_
#define _HAL_VT_H_

#include <ioctrl/scaler/vt_cmd_id.h>
#include <ioctrl/scaler/v4l2_ext_vt.h>

unsigned char get_vt_EnableFRCMode(void);
unsigned char get_vdec_securestatus(void);
unsigned char get_drm_case_securestatus(void);

void set_vdec_securestatus(unsigned char value);
void set_dtv_securestatus(unsigned char status);
void set_vt_StreamOn_flag(unsigned char flg);

void set_VT_Pixel_Format(VT_CAP_FMT value, unsigned int dc2h_num);
void vtforut_enable_dc2h(unsigned char state);

unsigned int get_vt_capBuf_Addr(unsigned int idx);
unsigned int get_vt_capBuf_Size(void);
unsigned int VT_Output_Colorspace(void);
void drvif_set_dc2h_capture_freeze(unsigned char onoff, unsigned int dc2h_num);
void save_vt_dc2h_capture_location(unsigned char dc2h_num, unsigned int location);
void save_vt_dc2h_output_size(unsigned char dc2h_num, unsigned int x, unsigned int y, unsigned int w, unsigned int h);
void set_vt_target_fps(unsigned int val, unsigned int dc2h_num);
void set_vt_VtBufferNum(unsigned int value, unsigned int dc2h_num);
unsigned int get_index_of_freezed(unsigned int dc2h_num);
unsigned char get_dc2h_dmaen_state(unsigned int dc2h_num);
int get_dc2h_cap_fmt_plane_number(unsigned int cap_fmt);
unsigned int get_vt_dc2h_plane_buf_addr(unsigned int dc2h_num, int plane_num, unsigned int idx);
unsigned int get_vt_dc2h_plane_buf_size(unsigned int dc2h_num, int plane_num);
void set_framerate_divide(unsigned int value, unsigned int dc2h_num);
unsigned int get_framerate_divide(unsigned int dc2h_num);
unsigned int get_vt_capture_framerate(unsigned int caploc);
void set_vt_dc2h_cap_mem_type(unsigned int dc2h_num, unsigned int type);
unsigned char do_vt_reqbufs(unsigned int buf_cnt, unsigned int dc2h_num);
DC2H_MEMORY_TYPE get_vt_dc2h_cap_mem_type(unsigned int dc2h_num);
unsigned char do_vt_dc2h_qbuf(unsigned int index, unsigned int dc2h_num, int fd, unsigned int size);
unsigned char drvif_vt_set_enqueue_index(unsigned int idx, unsigned int dc2h_num);
unsigned int get_dc2h_capbuf_size(unsigned int dc2h_num);
unsigned char drvif_set_dc2h_wait_vsync(unsigned int in_sel, unsigned int dc2h_num);
unsigned char do_vt_dc2h_dqbuf(unsigned int *getidx, unsigned int dc2h_num);
unsigned char do_vt_dc2h_streamon(unsigned int dc2h_num);
void do_vt_dc2h_streamoff(unsigned int dc2h_num);
unsigned char drv_set_vt_dc2h_crop(unsigned int dc2h_num, unsigned int x, unsigned int y, unsigned int w, unsigned int h);
void drvif_vt_finalize(unsigned int dc2h_num);

#endif

