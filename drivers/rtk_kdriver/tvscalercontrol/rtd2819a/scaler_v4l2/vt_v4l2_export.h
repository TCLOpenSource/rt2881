#ifndef _VT_V4L2_EXPORT_H_
#define _VT_V4L2_EXPORT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*external function*/
void set_hdmi_port_by_dev_num(unsigned char dev_num, unsigned char hdmi_port);
unsigned char get_hdmi_port_by_dev_num(unsigned char dev_num);
void set_vt_output_size(unsigned char dev_num, unsigned short x, unsigned short y, unsigned short w, unsigned short h);
StructSrcRect get_vt_output_size(unsigned char dev_num);
void set_vt_dev_buffer_num(unsigned char dev_num, unsigned char buffer_num);
unsigned char get_vt_dev_buffer_num(unsigned char dev_num);
void set_vt_capture_source_type(unsigned char dev_num, unsigned char src_type);
unsigned char get_vt_capture_source_type(unsigned char dev_num);
void set_vt_memory_type(unsigned char dev_num, unsigned char type);
unsigned char get_vt_memory_type(unsigned char dev_num);
void set_vt_pixel_format(unsigned char dev_num, unsigned char format);
unsigned char get_vt_pixel_format(unsigned char dev_num);
void set_vt_pitch(unsigned char dev_num, unsigned int pitch);
unsigned int get_vt_pitch(unsigned char dev_num);
void set_vt_sizeimage(unsigned char dev_num, unsigned int size);
unsigned int get_vt_sizeimage(unsigned char dev_num);
#ifdef __cplusplus
}
#endif

#endif //_VT_V4L2_EXPORT_H_