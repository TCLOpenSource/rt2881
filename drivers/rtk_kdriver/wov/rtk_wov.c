/*
                     ╭◜◝ ͡ ◜◝ ◜◝ ͡ ◜◝ ╮
                    (   RTK WOV driver   )
                    ╰◟ ͜ ◞ ͜ ◟ ͜◞ ͜ ◟ ͜ ◞◞╯
             _____ ᐝ
　　　　　 ／＞　　  フ
　　　　　|  　_　_ l
　 　　　／` ミ＿xノ
　　 　 /　　　 　 |
　　　 /　 ヽ　　 ﾉ
　 　 │　　|　|　|
　／￣|　　 |　|　|
　| (￣ヽ＿_ヽ_)__)
　＼二つ
 */

#include <asm/io.h>
#include <asm/cacheflush.h> // for dmac_flush_range, outer_inv_range
#include <linux/fs.h>
#include <linux/io.h> // for phys_to_virt
#include <linux/delay.h>
#include <linux/mutex.h> // for DEFINE_MUTEX, mutex_lock, mutex_unlock
#include <linux/string.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/freezer.h> // for set_freezable
#include <linux/uaccess.h> // for copy_from_user
#include <linux/kobject.h>
#include <linux/kthread.h>
#include <linux/pageremap.h> // for get_memory_size_total
#include <linux/version.h> // for KERNEL_VERSION
#include <linux/completion.h>
#include <linux/syscore_ops.h>
#include <linux/platform_device.h>

#ifdef CONFIG_COMPAT
#include <linux/compat.h>
#define to_user_ptr(x) compat_ptr((unsigned int)x)
#else
#define to_user_ptr(x)                                                         \
	((void *__user)(x)) // convert 32 bit value to user pointer
#endif /* CONFIG_COMPAT */

#include <rtk_kdriver/io.h>
#include <rtd_log/rtd_module_log.h>

#include <rbus/wov_reg.h>
#include <rbus/stb_reg.h>
#include <rbus/emcu_reg.h>
#include <rbus/audio_reg.h>

#include "wov_def.h"
#include "wov_common.h"
#include "wov_wrapper.h"

static int major_number;
static struct class *cls = NULL;
static struct device *dev = NULL;

static char *envp[] = {
	"CMD=COMMAND_RECOGNITION_TRIGGER",
	"UUID=7038ddc8-30f2-11e6-b0ac-40a8f03d3f15",
	NULL,
};

struct wov_info_t *wov_info;

static DEFINE_MUTEX(wov_mutex);
static DEFINE_MUTEX(wov_thread_mutex);

static void *wov_carvedout_start_va;

static unsigned int input;
static unsigned int screen = 0xff;
static unsigned int audio_record = 0xff;
static unsigned int sound_trigger = 0xff;

static struct task_struct *thread = NULL;
static int thread_running = 0;

static struct completion wov_completion;

static u32 sram_data_size = 0;
static bool wov_ap_ready = 0;

static u32 __attribute__((unused)) wov_read_va(void *va)
{
	u32 value;

	value = *(u32 *)va;
	value = cpu_to_be32(value);
	rtd_pr_wov_debug("[%s][%d] va addr 0x%lx => 0x%x\n", __func__, __LINE__,
			 (unsigned long)va, value);
	return value;
}

static u32 wov_read_phy(unsigned long addr)
{
	void *va;
	u32 value;

	if (thread_running) {
		va = phys_to_virt(addr);
		if (!va) {
			rtd_pr_wov_emerg(
				"[%s][%d] phys_to_virt fail at 0x%lx\n",
				__func__, __LINE__, (unsigned long)addr);
			return -EIO;
		}

		value = *(u32 *)va;
		value = cpu_to_be32(value);
		rtd_pr_wov_debug("[%s][%d] phy addr 0x%lx => 0x%x\n", __func__,
				 __LINE__, (unsigned long)addr, value);
		return value;
	} else {
		rtd_pr_wov_info("Drop read (0x%lx)\n", addr);
		return 0;
	}
}

static void wov_write_phy(struct ioctl_data data)
{
	if (thread_running) {
		// Perform the physical to virtual address translation and write the value
		void *virt_addr = phys_to_virt(data.addr);
		*((unsigned long *)virt_addr) = cpu_to_be32(data.value);
		rtd_pr_wov_info("Wrote 0x%lx to address 0x%lx (virt addr %p)\n",
				data.value, data.addr, virt_addr);
	} else {
		rtd_pr_wov_info("Drop write (0x%lx, 0x%lx)\n", data.addr,
				data.value);
	}
}

static void trap_emcu(void)
{
	// interrupt to emcu, stop/start access rbus
	rtd_outl(WOV_TO_EMCU_DUMMY_REG, 0xbacabaca);
	rtd_outl(EMCU_CPU_INT2_reg, (0x00000002 | 0x00000001));
}

static void __attribute__((unused)) wov_get_sram(void)
{
	u32 i;
	u32 tmp;
	u32 length = 0;
	u16 *ptr16 = (u16 *)(wov_carvedout_start_va + WOV_SRAM_RING_OFFSET);
	u32 *ptr32 = (u32 *)(wov_carvedout_start_va + WOV_32_BIT_BUF_OFFSET);

	if (WOV_wov_ctrl_1_get_reg_wov_sram_wr_en(
		    rtd_inl(WOV_wov_ctrl_1_reg)) == 0) {
		sram_data_size = 0;
		return;
	}

	trap_emcu();

	// wait until emcu stop rw rbus
	while (rtd_inl(WOV_TO_EMCU_DUMMY_REG) != 0xbedeaf)
		;

	// enable MD: move wov sram audio data to DDR
	rtd_setbits(WOV_wov_ctrl_1_reg, 0x2);

	msleep(5);

	// disable MD
	rtd_clearbits(WOV_wov_ctrl_1_reg, 0x2);

	length = (rtd_inl(WOV_wov_ctrl_6_reg) %
		  (rtd_inl(WOV_wov_ctrl_5_reg) & 0x0007ffff));
	// rtd_pr_wov_info("wov data length = 0x%x\n", length);

	trap_emcu();

	// change audio into 32bit format
	for (i = 0; i < length / sizeof(u16); i++) {
		tmp = ((u32)ptr16[i] << 16);
		// mdns_printf_internal("%x\n", tmp);
		ptr32[i] = tmp;
	}

	sram_data_size = length * 2;
}

static void __attribute__((unused)) stop_wov_sram(void)
{
#if 1
	// Check if wov is ready running
	if (rtd_inl(WOV_wov_ctrl_2_reg) == 0x0)
		return;

	rtd_pr_wov_notice("Stop wov sram\n");

	// disable wov irq
	rtd_maskl(WOV_wov_ctrl_1_reg, 0x9fffffff, 0x00000000);

	// disable wov dmic clk, vad detect
	rtd_maskl(WOV_stby_tcon_1_reg, 0xfffdefff, 0x00000000);

	// disable wov sram rd/wr enable, wov function enable
	rtd_maskl(WOV_wov_ctrl_1_reg, 0xfffffff8,
		  0x00000000); // [2:0] function enable

	// disable wov clk & rst wov
	rtd_outl(STB_ST_CLKEN1_reg, 0x00020000); //STB_ST_CLKEN1_reg_addr
	rtd_outl(STB_ST_SRST1_reg, 0x00040000); //STB_ST_SRST1_reg_addr
	rtd_outl(STB_ST_SRST1_reg, 0x00040001);
#endif
}

static void start_wov_sram(void)
{
	// Check if wov is ready running
	if (rtd_inl(WOV_wov_ctrl_2_reg) != 0x0)
		return;

	rtd_pr_wov_notice("Start wov sram\n");

	//Enable STB WOV
	rtd_outl(STB_ST_CLKEN1_reg, 0x00020000); //STB_ST_CLKEN1_reg_addr
	rtd_outl(STB_ST_SRST1_reg, 0x00040000); //STB_ST_SRST1_reg_addr
	rtd_outl(STB_ST_SRST1_reg, 0x00040001); //STB_ST_SRST1_reg_addr
	rtd_outl(STB_ST_CLKEN1_reg, 0x00020001); //STB_ST_CLKEN1_reg_addr

	//reinit
	rtd_maskl(WOV_wov_vad_1_reg, 0xfff1ffff,
		  0x00020000); //3'b001: ad0_out_r (lg 2ch dmic use ad0)

	rtd_outl(WOV_stby_tcon_1_reg, 0xd54bfb00); // 3.375Mhz
	rtd_maskl(WOV_mm_vad_energy7_reg, 0xffffffff, 0x80000000);
	msleep(1);
	rtd_maskl(WOV_mm_vad_energy7_reg, 0x7fffffff, 0x00000000);

	// adc gain +20dB
	rtd_outl(WOV_stby_ad0_1_reg, 0x00006464); // dmic0 for MMD201_65DB dmic
	rtd_outl(WOV_stby_ad1_1_reg, 0x00006464); // dmic1 for MMD201_65DB dmic

	rtd_outl(WOV_wov_ctrl_5_reg, 0x80040000); // not modify

	rtd_outl(WOV_wov_ctrl_4_reg,
		 WOV_SRAM_RING_ADDR); //setup wov ddr dump register[need modify]

	/* The so-called best VAD setting from ML7BU-2024 */
	rtd_maskl(WOV_mm_vad_energy4_reg,
		  ~WOV_mm_vad_energy4_pitch_det_mv_thr_mask,
		  WOV_mm_vad_energy4_pitch_det_mv_thr(0x1));
	rtd_maskl(WOV_mm_vad_energy5_reg,
		  ~WOV_mm_vad_energy5_pitch_det_peak_search_thr_mask,
		  WOV_mm_vad_energy5_pitch_det_peak_search_thr(0x40));
	rtd_maskl(WOV_mm_vad_energy5_reg,
		  ~WOV_mm_vad_energy5_pitch_det_peak_search_thr2_mask,
		  WOV_mm_vad_energy5_pitch_det_peak_search_thr2(0x80));
	rtd_maskl(WOV_mm_vad_energy5_reg,
		  ~WOV_mm_vad_energy5_pitch_det_od_thr_default_mask,
		  WOV_mm_vad_energy5_pitch_det_od_thr_default(0x3));

	/* VAD Threshold:
	 * ptich_det_thr_st_valid [22:10]=3 ==> threshold=3
	 * ptich_det_thr_st_valid [22:10]=2 ==> threshold=2
	 * ptich_det_thr_st_valid [22:10]=1 ==> threshold=1
	 */
	rtd_maskl(WOV_mm_vad_energy6_reg,
		  ~WOV_mm_vad_energy6_pitch_det_thr_st_valid_mask,
		  WOV_mm_vad_energy6_pitch_det_thr_st_valid(0x2));

	rtd_maskl(
		WOV_wov_ctrl_1_reg, 0x0fffff80,
		0x6000000d); //bit[30:29]=11 bit[4:3]=1, bit[5:6]=0 (00: ad0 l +r)
}

static void wov_detection_init(void)
{
	// wov info init
	wov_info->data_buf_addr = cpu_to_be32(WOV_TMP_BUF_ADDR);

	// magic num notify wov_ap.bin do endian conversion
	// wov_info->endian = cpu_to_be32(0x8957);

	// chNum = (chNum >> 3) & 0x3; in wov_ap
	wov_info->ch_mode = cpu_to_be32(1 << 3); // rtd_inl(0xb806064C);
}

// Notify sound trigger that ok google detected
static void wov_notify_sound_trigger(void)
{
	int ret = 0;

	rtd_pr_wov_info("[%s] just do it.\n", __func__);

	// Emit an event with environment variables
	ret = kobject_uevent_env(&dev->kobj, KOBJ_CHANGE, envp);
	if (ret) {
		rtd_pr_wov_emerg("[%s][%d] Send uevent failed: %d\n", __func__,
				 __LINE__, ret);
	}
}

// #define DEBUG_WOV
static int run_wov(void);
static int thread_fn(void *arg)
{
	void *dma_buf_ptr = wov_carvedout_start_va + WOV_DMA_BUF_ADDR_OFFSET;
	void *tmp_dma_buf_ptr = 0;
	u32 last_dma_buf_size = 0;
	void *cur_sram_data_ptr =
		(wov_carvedout_start_va + WOV_32_BIT_BUF_ADDR);
	u32 cur_dma_buf_size = 0;
	void *cur_dma_ptr = 0;
	u32 overflow_size = 0;
	u8 audio_data_ready = 0;
#ifdef DEBUG_WOV
	u32 count = 0;
	u32 max_dump = 5;
#else
	// u32 extra = 0;
	// u32 extra_size = 0;
	u32 move_size = DMA_2S_DATA_SIZE / 2;
#endif
	u8 first = 1;
	u8 virginity = 1;

	set_freezable(); //remove thread flag PF_NOFREEZE

	rtd_pr_wov_info("WOV thread is running\n");

	while (!kthread_freezable_should_stop(NULL)) {
		/* if ok g detected
		 * do not run detection
		 * make sure OK_GOOGLE_DETECTED would not be overwritten
		 */
		if (OK_GOOGLE_DETECTED ==
		    wov_read_va(&wov_info->detect_result)) {
			msleep(1);
			continue;
		}
#if 1
		if (virginity) {
			virginity = 0;
			wov_detection_init();
			enable_audio_data_path(); // audio DMA
		}
#if 0
		ssleep(1); // Sleep for 1 second
		complete(&wov_completion);
#else
		// 0x18006618[30:16] = 0x7530 => 30s data
		// 2s data = 0x7D0 = 2000
		cur_dma_buf_size = ((rtd_inl(0xb8006614) >> 16) & 0x00007fff) *
				   64; // bytes

		if ((first == 1) || (sram_data_size > 0)) {
			if (cur_dma_buf_size + sram_data_size >=
			    DMA_2S_DATA_SIZE) {
				first = 0;
				audio_data_ready = 1;
				rtd_pr_wov_info("cur_dma_buf_size=%x\n",
						cur_dma_buf_size);
			} else {
				audio_data_ready = 0;
			}
		} else {
			if (cur_dma_buf_size < last_dma_buf_size) {
				if (((WOV_DMA_BUF_SIZE - last_dma_buf_size) +
				     cur_dma_buf_size) >= move_size) {
					audio_data_ready = 1;
				} else {
					audio_data_ready = 0;
				}
			} else {
				if ((cur_dma_buf_size - last_dma_buf_size) >=
				    move_size) {
					audio_data_ready = 1;
				} else {
					audio_data_ready = 0;
				}
			}
		}

		if (audio_data_ready && wov_ap_ready) {
			wov_ap_ready = 0;

			rtd_pr_wov_info("cur_dma_buf_size=%x\n",
					cur_dma_buf_size);
			rtd_pr_wov_info("last_dma_buf_size=%x\n",
					last_dma_buf_size);

			// copy 2s audio to tmp buf for hotword detection
			if (sram_data_size > 0) {
				memcpy((void *)wov_carvedout_start_va +
					       WOV_TMP_BUF_OFFSET,
				       (void *)cur_sram_data_ptr,
				       sram_data_size);
				memcpy((void *)(wov_carvedout_start_va +
						WOV_TMP_BUF_OFFSET +
						sram_data_size),
				       (void *)wov_carvedout_start_va +
					       WOV_DMA_BUF_ADDR_OFFSET,
				       DMA_2S_DATA_SIZE - sram_data_size);

				cur_dma_ptr =
					(wov_carvedout_start_va +
					 WOV_DMA_BUF_ADDR_OFFSET +
					 DMA_2S_DATA_SIZE - sram_data_size);
			} else {
				// rtd_pr_wov_info("dma_buf_ptr=%x\n",
				// 		dma_buf_ptr -
				// 			wov_carvedout_start_va);
				if ((dma_buf_ptr + DMA_2S_DATA_SIZE) >
				    (wov_carvedout_start_va +
				     WOV_DMA_BUF_ADDR_OFFSET +
				     WOV_DMA_BUF_SIZE)) {
					overflow_size =
						(dma_buf_ptr +
						 DMA_2S_DATA_SIZE) -
						(wov_carvedout_start_va +
						 WOV_DMA_BUF_ADDR_OFFSET +
						 WOV_DMA_BUF_SIZE);

					// rtd_pr_wov_info("overflow_size=%x\n",
					// 		overflow_size);

					memcpy((void *)wov_carvedout_start_va +
						       WOV_TMP_BUF_OFFSET,
					       (void *)dma_buf_ptr,
					       DMA_2S_DATA_SIZE -
						       overflow_size);
					memcpy((void *)(wov_carvedout_start_va +
							WOV_TMP_BUF_OFFSET +
							DMA_2S_DATA_SIZE -
							overflow_size),
					       (void *)wov_carvedout_start_va +
						       WOV_DMA_BUF_ADDR_OFFSET,
					       overflow_size);

					cur_dma_ptr = (wov_carvedout_start_va +
						       WOV_DMA_BUF_ADDR_OFFSET +
						       overflow_size);
				} else {
					memcpy((void *)wov_carvedout_start_va +
						       WOV_TMP_BUF_OFFSET,
					       (void *)dma_buf_ptr,
					       DMA_2S_DATA_SIZE);

					cur_dma_ptr = (dma_buf_ptr +
						       DMA_2S_DATA_SIZE);
				}
			}

			wov_info->data_buf_addr = cpu_to_be32(WOV_TMP_BUF_ADDR);
			wov_info->data_buf_length =
				cpu_to_be32(DMA_2S_DATA_SIZE);

			rtd_pr_wov_notice("before complete\n");
			// start hotword detection
			complete(&wov_completion);
			rtd_pr_wov_notice("after complete\n");
			msleep(100);

// debug
#ifdef DEBUG_WOV
			if (count < max_dump) {
				count += 1;
				memcpy((void *)(wov_carvedout_start_va +
						(2 * count - 1) * 0x10000),
				       (void *)(wov_carvedout_start_va +
						WOV_TMP_BUF_OFFSET),
				       DMA_2S_DATA_SIZE);
				rtd_pr_wov_info("save %d to %x, length=%x\n",
						count,
						(0x1a500000 +
						 (2 * count - 1) * 0x10000),
						DMA_2S_DATA_SIZE);
			}

			if (count >= max_dump) {
				rtd_part_outl(0xb8006610, 4, 4,
					      0x0); // stop DMA
				screen = 1;
				run_wov();
			}
#else
			// ok google detected
			if (OK_GOOGLE_DETECTED ==
			    wov_read_va(&wov_info->detect_result)) {
				cur_dma_buf_size =
					((rtd_inl(0xb8006614) >> 16) &
					 0x00007fff) *
					64; // bytes

				rtd_part_outl(0xb8006610, 4, 4,
					      0x0); // stop audio DMA
				start_wov_sram();

				if ((dma_buf_ptr - DMA_2S_DATA_SIZE / 2) <
				    (wov_carvedout_start_va +
				     WOV_DMA_BUF_ADDR_OFFSET)) {
					overflow_size =
						(wov_carvedout_start_va +
						 WOV_DMA_BUF_ADDR_OFFSET) -
						(dma_buf_ptr -
						 DMA_2S_DATA_SIZE / 2);
					tmp_dma_buf_ptr =
						(wov_carvedout_start_va +
						 WOV_DMA_BUF_ADDR_OFFSET +
						 WOV_DMA_BUF_SIZE -
						 overflow_size);
				} else {
					tmp_dma_buf_ptr = dma_buf_ptr -
							  DMA_2S_DATA_SIZE / 2;
				}

				// audio data start address
				wov_info->audiofw_data_addr =
					cpu_to_be32(virt_to_phys(dma_buf_ptr));
				// audio data size
				wov_info->audiofw_data_length =
					cpu_to_be32(cur_dma_buf_size -
						    (virt_to_phys(dma_buf_ptr) -
						     WOV_DMA_BUF_PHY_ADDR));

				wov_notify_sound_trigger();

				// If OK G detected, shift 2s
				move_size = DMA_2S_DATA_SIZE;
			} else {
				// If OK G not detected, shift 1s
				move_size = DMA_2S_DATA_SIZE / 2;
			}
#endif /* DEBUG_WOV */

			// shift dma buf ptr
			if (sram_data_size > 0) {
				if (sram_data_size > move_size) {
					cur_sram_data_ptr += move_size;
					sram_data_size -= move_size;
				} else {
					dma_buf_ptr +=
						(move_size - sram_data_size);
					sram_data_size = 0;
				}
			} else {
				dma_buf_ptr += move_size;
			}

			// keep dma buf ptr in ring buf
			if (dma_buf_ptr >=
			    (wov_carvedout_start_va + WOV_DMA_BUF_ADDR_OFFSET +
			     WOV_DMA_BUF_SIZE)) {
				dma_buf_ptr -= WOV_DMA_BUF_SIZE;
			}

			last_dma_buf_size = cur_dma_buf_size;
		}
#endif
#endif
	}

	rtd_pr_wov_info("WOV thread stopped\n");
	return 0;
}

static void stop_wov_thread(void)
{
	mutex_lock(&wov_thread_mutex);

	// stop wov detection
	if (thread_running) {
		kthread_stop(thread);
		thread_running = 0;
		rtd_pr_wov_info("WOV thread stopped\n");
	} else {
		rtd_pr_wov_info("WOV thread not running, no need to stop\n");
	}

	mutex_unlock(&wov_thread_mutex);
}

static int run_wov(void)
{
	mutex_lock(&wov_mutex);

	/* wov detection start conditions:
	 * 1. Sound Trigger startRcognition
	 * 2. Screen off
	 * 3. Audio Record off
	 */
	if ((sound_trigger == 1) && (screen == 0) && (audio_record == 0)) {
		// if ((sound_trigger == 1) && (audio_record == 0)) {
		// start wov detection
		if (!thread_running) {
			thread = kthread_run(thread_fn, NULL, "wov_thread");
			if (IS_ERR(thread)) {
				rtd_pr_wov_emerg(
					"Failed to create WOV thread\n");
				mutex_unlock(&wov_mutex);
				return PTR_ERR(thread);
			}
			thread_running = 1;
			rtd_pr_wov_info("WOV thread started\n");
		} else {
			rtd_pr_wov_info("WOV thread is already running\n");
		}
	} else {
		wov_info->detect_result = 0;
		stop_wov_thread();
	}

	mutex_unlock(&wov_mutex);

	return 0;
}

// Function to handle device open
static int wov_device_open(struct inode *inode, struct file *file)
{
	rtd_pr_wov_info("Device opened.\n");
	return 0;
}

// Function to handle device release
static int wov_device_release(struct inode *inode, struct file *file)
{
	rtd_pr_wov_info("Device released.\n");
	return 0;
}

// Function to handle device write
static ssize_t wov_device_write(struct file *file, const char __user *buffer,
				size_t length, loff_t *offset)
{
	static int counter = 0;

	char command[256];

	if (length > sizeof(command) - 1)
		return -EINVAL;

	// Copy user space data to kernel space
	if (copy_from_user(command, buffer, length))
		return -EFAULT;

	// Null-terminate the string
	command[length] = '\0';

	/* Execute different code based on the received command
	 * echo 1 > /dev/wov_gaowan => send uevent (cmd && uuid) to ST HAL
	 * echo 2 > /dev/wov_gaowan => start wov detection thread
	 * echo 3 > /dev/wov_gaowan => stop wov detection thread
	 */
	if ((command[0] == '1') && (length == 2)) {
		wov_notify_sound_trigger();
		rtd_pr_wov_info(
			"[%s][%d] I already send %d times cmd & uuid to ST HAL.\n",
			__func__, __LINE__, ++counter);
	} else if ((command[0] == '2') && (length == 2)) {
		sound_trigger = 1;
		screen = 0;
		audio_record = 0;
		run_wov();
	} else if ((command[0] == '3') && (length == 2)) {
		sound_trigger = 0;
		screen = 1;
		audio_record = 1;
		run_wov();
	} else {
		// Unknown command
		rtd_pr_wov_info("[%s][%d] Received command: %s\n", __func__,
				__LINE__, command);
	}

	return length;
}

void set_wov_attr_audio_record(int32_t val)
{
	audio_record = val;
	rtd_pr_wov_info("[%s][%d]: audio_record=%x\n", __func__, __LINE__,
			audio_record);
	run_wov();
}

EXPORT_SYMBOL(set_wov_attr_audio_record);

static long wov_device_ioctl(struct file *file, unsigned int cmd,
			     unsigned long arg)
{
	struct ioctl_data data;

	rtd_pr_wov_debug("[%s][%d] cmd=%x\n", __func__, __LINE__, cmd);

	switch (cmd) {
	case RTK_WOV_SET_ATTR_SCREEN:
		rtd_pr_wov_info("[%s][%d] RTK_WOV_SET_ATTR_SCREEN\n", __func__,
				__LINE__);

		if (copy_from_user(&input, (unsigned int *)arg,
				   sizeof(unsigned int))) {
			return -EFAULT;
		}

		screen = input;
		run_wov();

		rtd_pr_wov_info("[%s][%d]: screen=%x\n", __func__, __LINE__,
				screen);
		break;
	case RTK_WOV_SET_ATTR_SOUND_TRIGGER:
		rtd_pr_wov_info("[%s][%d] RTK_WOV_SET_ATTR_SOUND_TRIGGER\n",
				__func__, __LINE__);

		if (copy_from_user(&input, (unsigned int *)arg,
				   sizeof(unsigned int))) {
			return -EFAULT;
		}

		sound_trigger = input;
		run_wov();

		rtd_pr_wov_info("[%s][%d]: sound_trigger=%x\n", __func__,
				__LINE__, sound_trigger);
		break;
	case RTK_WOV_WAIT:
		rtd_pr_wov_info("Waiting for completion...\n");
		wov_ap_ready = 1;
		wait_for_completion_interruptible(&wov_completion);
		reinit_completion(
			&wov_completion); // Reinitialize the completion structure
		rtd_pr_wov_info("Completion done!\n");
		break;
	case RTK_WOV_WRITE_PHY:
		if (copy_from_user(&data, (struct ioctl_data *)arg,
				   sizeof(struct ioctl_data))) {
			return -EFAULT;
		}

		wov_write_phy(data);
		break;
	case RTK_WOV_READ_PHY:
		if (copy_from_user(&data, (struct ioctl_data *)arg,
				   sizeof(struct ioctl_data))) {
			return -EFAULT;
		}

		data.value = wov_read_phy(data.addr);

		if (copy_to_user((struct ioctl_data *)arg, &data,
				 sizeof(struct ioctl_data))) {
			return -EFAULT;
		}
		break;
	default:
		rtd_pr_wov_err("[%s][%d]: unknown cmd=%x\n", __func__, __LINE__, cmd);
		return -EINVAL;
	}
	return 0;
}

static long wov_device_compat_ioctl(struct file *filp, unsigned int cmd,
				    unsigned long arg)
{
#ifdef CONFIG_COMPAT
	void __user *compat_arg = compat_ptr(arg);
#else
	void __user *compat_arg = to_user_ptr(arg);
#endif
	return wov_device_ioctl(filp, cmd, (unsigned long)compat_arg);
}

// File operations for the device
static struct file_operations fops = {
	.open = wov_device_open,
	.release = wov_device_release,
	.write = wov_device_write,
	.unlocked_ioctl = wov_device_ioctl,
	.compat_ioctl = wov_device_compat_ioctl,
};

// static int wov_suspend(struct platform_device *dev, pm_message_t state)
int wov_suspend(void)
{
	rtd_pr_wov_info("Suspend\n");

	if (sound_trigger == 1) {
		stop_wov_thread();
		start_wov_sram();
	} else {
		rtd_pr_wov_info("Sound trigger is disable\n");
	}

	return 0;
}

EXPORT_SYMBOL(wov_suspend);

#ifdef CONFIG_PM
static int wov_resume(struct platform_device *dev)
{
	rtd_pr_wov_info("Resume\n");

	if (sound_trigger == 1) {
		/* Due to vcpu memory mapping, 
		 * wov_info->data_buf_addr would be set 0x9a750000
		 * => Change it into real phy address 0x1a750000
		 */
		wov_info->data_buf_addr = cpu_to_be32(WOV_TMP_BUF_ADDR);

		if (OK_GOOGLE_DETECTED ==
		    wov_read_va(&wov_info->detect_result)) {
			wov_notify_sound_trigger();
		}
	}

	return 0;
}
#endif /* CONFIG_PM */

static struct platform_device *wov_platform_device;
static struct platform_driver wov_platform_driver = {
#ifdef CONFIG_PM
// 	.suspend		= wov_suspend,
	.resume			= wov_resume,
#endif
	.driver = {
		.name		= "rtk_wov",
		.owner		= THIS_MODULE,
	},
};

// Module initialization
static int __init wov_init(void)
{
	int ret = 0;

#ifdef CONFIG_ARCH_RTK6748
	/* Merlin7 1.5G subatomic do not support FFM
	 * We can release 4MB wov carvedout memory
	 * Therefore, no need to initailize wov module
	 */
	if (get_memory_size_total() == 0x60000000) {
		return ret;
	}
#endif

	// Register a character device
	major_number = register_chrdev(0, DEVICE_NAME, &fops);
	if (major_number < 0) {
		rtd_pr_wov_emerg(
			"[%s][%d] Failed to register a major number.\n");
		return major_number;
	}
	rtd_pr_wov_info("Registered device with major number %d.\n",
			major_number);

	// Create a class for the device
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
	cls = class_create(CLASS_NAME);
#else
	cls = class_create(THIS_MODULE, CLASS_NAME);
#endif
	if (IS_ERR(cls)) {
		unregister_chrdev(major_number, DEVICE_NAME);
		rtd_pr_wov_emerg("[%s][%d] Failed to create device class.\n",
				 __func__, __LINE__);
		return PTR_ERR(cls);
	}
	rtd_pr_wov_info("Device class created.\n");

	// cls->devnode = wov_devnode;

	// Create the device
	dev = device_create(cls, NULL, MKDEV(major_number, 0), NULL,
			    DEVICE_NAME);
	if (IS_ERR(dev)) {
		class_destroy(cls);
		unregister_chrdev(major_number, DEVICE_NAME);
		rtd_pr_wov_emerg("[%s][%d] Failed to create the device.\n",
				 __func__, __LINE__);
		return PTR_ERR(dev);
	}
	rtd_pr_wov_info("Device created.\n");

	wov_platform_device =
		platform_device_register_simple("rtk_wov", -1, NULL, 0);

	ret = platform_driver_register(&wov_platform_driver);
	if (ret) {
		rtd_pr_wov_emerg("[%s][%d] Unable to register driver\n",
				 __func__, __LINE__);
		platform_device_unregister(wov_platform_device);
		return ret;
	}

	init_completion(&wov_completion);

	wov_carvedout_start_va = phys_to_virt(WOV_CARVEDOUT_START_ADDR);
	if (!wov_carvedout_start_va) {
		rtd_pr_wov_emerg("[%s][%d] phys_to_virt fail at 0x%lx\n",
				 __func__, __LINE__,
				 (unsigned long)WOV_CARVEDOUT_START_ADDR);
		return -EIO;
	}

	// clear wov carvedout memeory
	memset(wov_carvedout_start_va + WOV_DMA_BUF_ADDR_OFFSET, 0,
	       WOV_CARVEDOUT_MEM_SIZE);

	wov_info = wov_carvedout_start_va + WOV_INFO_ADDR_OFFSET;

	return 0;
}

// Module exit
static void __exit wov_exit(void)
{
	// Destroy the device
	device_destroy(cls, MKDEV(major_number, 0));

	// Destroy the class
	class_unregister(cls);
	class_destroy(cls);

	// Unregister the character device
	unregister_chrdev(major_number, DEVICE_NAME);

	if (wov_platform_device)
		platform_driver_unregister(&wov_platform_driver);

	rtd_pr_wov_info("Device removed.\n");
}

module_init(wov_init);
module_exit(wov_exit);

MODULE_LICENSE("GPL");
