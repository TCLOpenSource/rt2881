#ifndef _RTK_WOV_COMMON_H_
#define _RTK_WOV_COMMON_H_

#define CLASS_NAME                            "wov"
// #define WOV_CLASS_MODE                        ((umode_t)(S_IRUGO|S_IWUGO|S_IXUGO))
#define DEVICE_NAME                           "wov_gaowan"
#define PHYS_ADDR                             (0x1a800010)
#define OK_GOOGLE_DETECTED                    (0xaccede)

#define WOV_MAGIC                             'w'
#define RTK_WOV_WAIT                          _IOR(WOV_MAGIC, 1, int)
#define RTK_WOV_READ_PHY                      _IOR(WOV_MAGIC, 2, struct ioctl_data)
#define RTK_WOV_WRITE_PHY                     _IOR(WOV_MAGIC, 3, struct ioctl_data)
#define RTK_WOV_SET_ATTR_SCREEN               _IOW(WOV_MAGIC, 4, unsigned int)
#define RTK_WOV_SET_ATTR_SOUND_TRIGGER        _IOW(WOV_MAGIC, 5, unsigned int)

#define WOV_CARVEDOUT_START_ADDR              (0x1a500000)
#define WOV_INFO_ADDR_OFFSET                  (0x00300000) // 0x1a800000
#define WOV_DMA_BUF_PHY_ADDR                  (0x1a600000)
#define WOV_DMA_BUF_ADDR_OFFSET               (0x00100000) // 0x1a600000
#define WOV_DMA_BUF_SIZE                      (((rtd_inl(0xb8006618) & 0x7fff0000) >> 16) * 64)
#define WOV_TMP_BUF_ADDR                      (0x1a750000)
#define WOV_TMP_BUF_OFFSET                    (WOV_TMP_BUF_ADDR - WOV_CARVEDOUT_START_ADDR) // 0x1a750000
#define WOV_CARVEDOUT_MEM_ADDR_OFFSET         (0x00100000) // 0x1a600000
#define WOV_CARVEDOUT_MEM_SIZE                (0x300000)
#define WOV_32_BIT_BUF_ADDR                   (0x1a720000)
#define WOV_32_BIT_BUF_OFFSET                 (WOV_32_BIT_BUF_ADDR - WOV_CARVEDOUT_START_ADDR) // 0x1a720000
/* wov sram ring buffer start address */
#define WOV_SRAM_RING_ADDR                    (0x1a7d0000)
#define WOV_SRAM_RING_OFFSET                  (WOV_SRAM_RING_ADDR - WOV_CARVEDOUT_START_ADDR)
#define WOV_AUDIO_DATA_ADDR                   (0x1a830000)
#define WOV_AUDIO_DATA_OFFSET                 (WOV_AUDIO_DATA_ADDR - WOV_CARVEDOUT_START_ADDR)
/* sample rate 16K * 32bit * 2 sec = 16000*32*2 = 1024000 bit = 128000 bytes = 0x1F400 */
#define DMA_2S_DATA_SIZE                      (0x1F400)

struct ioctl_data {
	unsigned int addr;
	unsigned int value;
};

// shared memory: videofw/kdriver <-> wov_ap
struct wov_info_t {
	u32 silent; // 0x1a800000
	u32 ch_mode; // 0x1a800004
	u32 data_buf_addr; // 0x1a800008
	u32 data_buf_length; // 0x1a80000c
	u32 detect_result; // 0x1a800010
	u32 endian; // 0x1a800014
	u32 block_index; // 0x1a800018
	u32 audiofw_data_addr; // 0x1a80001c
    u32 audiofw_data_length; // 0x1a800020
};

#endif /* _RTK_WOV_COMMON_H_ */