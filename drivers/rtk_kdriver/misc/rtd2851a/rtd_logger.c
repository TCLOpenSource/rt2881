
#define pr_fmt(fmt) "[rtkd]:  " fmt

#include <linux/file.h>
#include <linux/uaccess.h>
#include <linux/string.h>       /* memcpy */
#include <rtd_log/rtd_module_log.h>
#include <linux/platform_device.h>
#include <linux/slab.h>     /* kmalloc() */
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/freezer.h>

#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0))
#include <linux/sched/clock.h>
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 13, 0))
#include <linux/panic_notifier.h>
//#include <linux/time.h>
#endif
#include <rtk_kdriver/rtd_log.h>
#include <rtd_logger.h>
#include <rtk_kdriver/io.h>

#include <linux/syscalls.h>
#include <linux/sched/xacct.h>


#if (IS_ENABLED(CONFIG_RTK_KDRV_TEE) && !defined(CONFIG_CUSTOMER_TV006))
#include <linux/arm-smccc.h>
struct optee_rpc_param
{
    u32     a0;
    u32     a1;
    u32     a2;
    u32     a3;
    u32     a4;
    u32     a5;
    u32     a6;
    u32     a7;
};
extern int optee_logbuf_setup_info_send(struct optee_rpc_param *p_rtc_set_info);
#endif

#ifdef CONFIG_ANDROID
#define RTDLOG_DEFAULT_PATH "/mnt/vendor/rtdlog"
#else
#define RTDLOG_DEFAULT_PATH "/tmp/var/log"
#endif

#define SUPPORT_LOGBUF_FILE_OPERATIONS
#define RTDLOG_NODE "rtd-logger"
#define RTDLOG_DEV_NODE "/dev/"RTDLOG_NODE
#define RTD_LOGBUF_BOOT_SHARE_PARAM_OFFSET 0x00001000

//#define RTD_LOGBUF_PHY_START_ADDR 0x1ca00000

/********************************************************
* global variables
*********************************************************/
#if defined(CONFIG_REALTEK_LOGBUF) && !(LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
extern char *native_logbuf_addr;
#endif

#define RTD_PANICLOG_MAGIC 0x20240801
int *paniclog_magic = NULL;
int *paniclog_num = NULL;
char *paniclog_text = NULL;
char *panic_buff = NULL;

extern struct MODULE_MAP module_map[RTD_LOG_MODULE_MAX_ID];
static rtdlog_dbg_info gDbgInfo;
struct kfifo * array_fifo[MAX_LOGBUF_NUM]= {};
volatile u64 * gpRtdlogTimestamp;           //time stamp ptr
logbuf_rbcb * pRtdLogCB[MAX_LOGBUF_NUM] = {0};
bool is_setup_rtd_logbuf = false;
bool is_save_early_log_done = false;           //printk save log to rtd logbuf when true only
EnumPanicCpuType g_rtdlog_panic_cpu_type = NORMAL;
EXPORT_SYMBOL(g_rtdlog_panic_cpu_type);
unsigned long rtd_virt_addr;
#if defined(CONFIG_TRACEPOINTS) && defined(CONFIG_ANDROID_VENDOR_HOOKS)
unsigned int str_status = 0;
EXPORT_SYMBOL(str_status);
#endif
#define RTDLOG_BUF_SIZE 256
#define RTDLOG_HEADER_BUF_SIZE 256

#define MAX_PATH_LEN 128
typedef struct _RTDLOG_BOOT_SHARE_PARAM
{
    char default_log_path[MAX_PATH_LEN];
    char current_log_path[MAX_PATH_LEN];
    char usb_path[MAX_PATH_LEN];

    u32 total_bufsize;

    u32 flag_status; //0 --> suspend, 1 --> running
    u32 flag_sleep;
    u32 flag_filter;
    u32 flag_compress;
    u32 flag_contain_av_log;

    u32 flag_filter_in[MAX_LOGBUF_NUM];
    u32 flag_filter_out[MAX_LOGBUF_NUM];

    u32 max_file_size[MAX_LOGBUF_NUM];

    u32 fd[MAX_LOGBUF_NUM];
    u32 usb_fd[MAX_LOGBUF_NUM];
} RTDLOG_BOOT_SHARE_PARAM;

#define RTKD_VERSION 20210915
#define RTDLOG_BUF_SIZE 256
#define FILE_NAME_LEN 16
typedef struct RTDLOG_FILE
{
    unsigned int pRtdLogCB;
    unsigned int file_len;
    unsigned int fd;
    unsigned int usb_fd;
    unsigned int gz_file_num;
    int flag_usb_sync;
    unsigned long long g_logfile_fp;
    unsigned long long g_logfile_usb_fp;
    char g_file_name[FILE_NAME_LEN];
}rtdlog_file;

struct SHARE_STRUCT
{
    unsigned int version;
    unsigned int total_file_num;
    unsigned char flag_compress;
    unsigned char usb_status_flag;
    unsigned short reserve2;
    unsigned int reserve3;
    char g_dir_path[RTDLOG_BUF_SIZE];
    char usb_path[RTDLOG_BUF_SIZE];
    rtdlog_file file_info;
};
unsigned char * rtkd_share_buffer;
rtdlog_file * rtkd_file_info;
#if 0
typedef enum _LOGLEVEL_TYPE
{
    LOGLEVEL_EMERG=0,
    LOGLEVEL_ALERT=1,
    LOGLEVEL_CRIT=2,
    LOGLEVEL_ERR=3,
    LOGLEVEL_WARNING=4,
    LOGLEVEL_NOTICE=5,
    LOGLEVEL_INFO=6,
    LOGLEVEL_DEBUG=7
} LOGLEVEL_TYPE;
#endif

unsigned char * share_buffer;

unsigned long rtdlog_bufsize;
unsigned long rtdlog_param_size;

unsigned char * acpu1_cpu_log_buffer = NULL;
unsigned char * kcpu2_cpu_log_buffer = NULL;
unsigned char * vcpu1_cpu_log_buffer = NULL;
unsigned char * vcpu2_cpu_log_buffer = NULL;
unsigned char * kcpu_cpu_log_buffer = NULL;
unsigned char * kernel_cpu_log_buffer = NULL;
unsigned char * hal_cpu_log_buffer = NULL;
char *cpu_type_str[MAX_LOGBUF_NUM] =
{
    "ACPU1",
    "KCPU2",
    "VCPU1",
    "VCPU2",
    "KCPU1",
    "S-KER",
    "S-HAL"
};
const char *rtdlog_file_name[MAX_LOGBUF_NUM] =
{
    "acpu1",
    "kcpu2",
    "vcpu1",
    "vcpu2",
    "kcpu",
    "kernel",
    "hal"
};
#define RTDLOG_IOC_WRITE_CMD _IOWR(RTDLOG_DEV_MAGIC, 0, int )

#if 0
static struct file* g_logfile[MAX_LOGBUF_NUM];
static struct file* g_usb_logfile[MAX_LOGBUF_NUM];
static char g_file_path[LOGBUF_DIR_MAX_SIZE*2];

static loff_t g_fw_pos[MAX_LOGBUF_NUM];
static loff_t g_fw_usb_pos[MAX_LOGBUF_NUM];

#define RTDLOG_USB_LOGDIR "rtklog_usb"
static char g_usb_root_dir_path[LOGBUF_DIR_MAX_SIZE];
static char g_usb_dir_path[LOGBUF_DIR_MAX_SIZE];

#endif

#define READ_SLEEP_SIZE 0x800

#if 1
#define LOG_FILE_FW_MAX_SIZE   (512*1024)
#define LOG_FILE_KERNEL_MAX_SIZE      (1024*1024)
#else
#define LOG_FILE_FW_MAX_SIZE   (10*1024)
#define LOG_FILE_KERNEL_MAX_SIZE      (10*1024)
#endif

static DEFINE_MUTEX(save_acpu1_mutex);
static DEFINE_MUTEX(save_acpu2_mutex);
static DEFINE_MUTEX(save_vcpu1_mutex);
static DEFINE_MUTEX(save_vcpu2_mutex);
static DEFINE_MUTEX(save_kcpu_mutex);
static DEFINE_MUTEX(save_kcpu2_mutex);
static DEFINE_MUTEX(save_kernel_mutex);
static DEFINE_MUTEX(save_hal_mutex);
static DEFINE_MUTEX(save_hal_read_mutex);
static DEFINE_MUTEX(logcat_mutex);
static DEFINE_MUTEX(read_loop_mutex);
static DEFINE_MUTEX(keylog_mutex);

struct mutex * log_save_mutex[MAX_LOGBUF_NUM]=
{
    &save_acpu1_mutex,
    &save_acpu2_mutex,
    &save_vcpu1_mutex,
    &save_vcpu2_mutex,
    &save_kcpu_mutex,
    &save_kernel_mutex,
    &save_hal_mutex
};
void rtdlog_sleep (EnumLogbufType type, long write_count );
#ifdef CONFIG_REALTEK_LOGBUF_MODULE
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 6, 0))
static loff_t *rtd_file_ppos(struct file *file)
{
        return file->f_mode & FMODE_STREAM ? NULL : &file->f_pos;
}
static unsigned long rtd_fdget_pos(unsigned int fd)
{
    unsigned long v = __fdget(fd);
    struct file *file = (struct file *)(v & ~3);

    if (file && (file->f_mode & FMODE_ATOMIC_POS)) {
        if (file_count(file) > 1) {
            v |= FDPUT_POS_UNLOCK;
            mutex_lock(&file->f_pos_lock);
        }
    }
    return v;
}
static void rtd_unlock_pos(struct file *f)
{
    mutex_unlock(&f->f_pos_lock);
}
static void rtd_fdput_pos(struct fd f)
{
        if (f.flags & FDPUT_POS_UNLOCK)
                rtd_unlock_pos(f.file);
        fdput(f);
}
static struct fd rtd_to_fd(unsigned long v)
{
        return (struct fd){(struct file *)(v & ~3),v & 3};
}
static struct fd rtd_get_pos(int fd)
{
        return rtd_to_fd(rtd_fdget_pos(fd));
}
ssize_t rtk_ksys_write(unsigned int fd, const char *buf, size_t count)
{
        struct fd f = rtd_get_pos(fd);
        ssize_t ret = -EBADF;
        if (f.file) {
                loff_t pos, *ppos = rtd_file_ppos(f.file);
                if (ppos) {
                        pos = *ppos;
                        ppos = &pos;
                }
                ret = kernel_write(f.file, buf, count, ppos);
                if (ret >= 0 && ppos)
                        f.file->f_pos = pos;
               rtd_fdput_pos(f);
        }
        return ret;
}
#endif
ssize_t rtk_ksys_write(unsigned int fd, const char *buf, size_t count)
{
        return 0;
}
#else
static inline loff_t *rtk_file_ppos(struct file *file)
{
    return file->f_mode & FMODE_STREAM ? NULL : &file->f_pos;
}
ssize_t rtk_ksys_write(unsigned int fd, const char *buf, size_t count)
{
    struct fd f = fdget_pos(fd);
    ssize_t ret = -EBADF;
    if (f.file) {
        loff_t pos, *ppos = rtk_file_ppos(f.file);
        if (ppos) {
            pos = *ppos;
            ppos = &pos;
        }
        ret = kernel_write(f.file, buf, count, ppos);
        if (ret >= 0 && ppos)
            f.file->f_pos = pos;
        fdput_pos(f);
    }
        return ret;
}
#endif
/********************************************************
* kernel APIs
*********************************************************/
void* rtdlog_phys_to_virt (unsigned long phys)
{
    return (void * )( rtd_virt_addr + ( phys - RTD_LOGBUF_PHY_START_ADDR) );
}

#if 0
//#define UART_BASE 0x18062300
#define UART_BASE 0xb801b100
static void putc_ (char c)
{
    while (( rtd_inl(UART_BASE+0x14) & 32 ) == 0);
    if (c=='\n')
    {
        rtd_outl(UART_BASE,'\n');
        rtd_outl(UART_BASE,'\r');
    }
    else
    {
        rtd_outl(UART_BASE, c);
    }
}

static void puts_ (const char * str)
{
    for(; *str; str++)
    {
        putc_(*str);
    }
}
#endif

/********************************************************
* rtd_kernel_write_file Reserved for DB1702 linux5.4
*********************************************************/
static int error_flag=0;
static int usb_error_flag=0;
ssize_t rtd_kernel_write_file (EnumLogbufType type, unsigned char * buf, int count, ssize_t* result)
{
    ssize_t rv;
    if(rtkd_file_info[type].usb_fd)
    {
        rv = rtk_ksys_write(rtkd_file_info[type].usb_fd, buf, count);
        if(rv<0)
        {
            rtd_pr_rtdlog_err("%s %d. write usb log file %d \e[1;31mfail\e[0m! ret is %d\n",__func__,__LINE__,type,rv);
            rtkd_file_info[type].usb_fd = 0;
            usb_error_flag = rv;
        }
    }

    if(rtkd_file_info[type].fd)
    {
        rv = rtk_ksys_write(rtkd_file_info[type].fd, buf, count);
        if(rv<0)
        {
            rtd_pr_rtdlog_err("%s %d. write log file %d \e[1;31mfail\e[0m! ret is %d\n",__func__,__LINE__,type,rv);
            error_flag=rv;
            rv=0;
        }
    }
    else
    {
        rv=0;
    }

    return rv;
}

/********************************************************
* kfifo APIs
*********************************************************/
static inline unsigned int kfifo_out_len (struct kfifo *fifo)
{
    register unsigned int out;
    out = fifo->out;
    smp_rmb();
    return (fifo->in) - out;
}

static inline __must_check unsigned int kfifo_size (struct kfifo *fifo)
{
    return fifo->size;
}

static inline unsigned int kfifo_len (struct kfifo *fifo)
{
    return kfifo_out_len(fifo);
}

static inline unsigned int __kfifo_off (struct kfifo *fifo, unsigned int off)
{
    return off & (fifo->size - 1);
}

static inline __must_check unsigned int kfifo_avail (struct kfifo *fifo)
{
    unsigned int out_len;//,cat_len,use_len;
    out_len = kfifo_len(fifo);
    return kfifo_size(fifo) - out_len;
}

static inline void __kfifo_in_data (struct kfifo *fifo, const void *from, unsigned int len, unsigned int off)
{
    unsigned int l;

    /*
     * Ensure that we sample the fifo->out index -before- we
     * start putting bytes into the kfifo.
     */

    smp_mb();

    off = __kfifo_off(fifo, fifo->in + off);

    /* first put the data starting from fifo->in to buffer end */
    l = min((unsigned long)len, (unsigned long)(fifo->size - off));
    memcpy((unsigned char *)rtdlog_phys_to_virt((unsigned long)fifo->buffer) + off, from, l);

    /* then put the rest (if any) at the beginning of the buffer */
    memcpy((unsigned char *)rtdlog_phys_to_virt((unsigned long)fifo->buffer), from + l, len - l);
}

static inline void __kfifo_out_data (struct kfifo *fifo,void *to, unsigned int len, unsigned int off)
{
    unsigned int l;

    /*
     * Ensure that we sample the fifo->in index -before- we
     * start removing bytes from the kfifo.
     */

    smp_rmb();

    off = __kfifo_off(fifo, fifo->out + off);

    /* first get the data from fifo->out until the end of the buffer */
    l = min((unsigned long)len, (unsigned long)(fifo->size - off));
    memcpy(to, (unsigned char *)rtdlog_phys_to_virt((unsigned long)fifo->buffer) + off, l);

    /* then get the rest (if any) from the beginning of the buffer */
    memcpy(to + l, (unsigned char *)rtdlog_phys_to_virt((unsigned long)fifo->buffer), len - l);
}

static inline void __kfifo_add_in (struct kfifo *fifo,unsigned int off)
{
    smp_wmb();
    fifo->in += off;
}

static inline void __kfifo_add_out (struct kfifo *fifo,unsigned int off)
{
    smp_mb();
    fifo->out += off;
}

static unsigned int kfifo_in (struct kfifo *fifo, const void *from, unsigned int len)
{
    len = min(kfifo_avail(fifo), len);

    __kfifo_in_data(fifo, from, len, 0);
    __kfifo_add_in(fifo, len);

    return len;
}

static __maybe_unused unsigned int kfifo_out (struct kfifo *fifo, void *to, unsigned int len)
{
    len = min(kfifo_len(fifo), len);

    __kfifo_out_data(fifo, to, len, 0);
    __kfifo_add_out(fifo, len);
    return len;
}

static unsigned int kfifo_out_peek (struct kfifo *fifo, void *to, unsigned int len,unsigned offset)
{
    len = min(kfifo_len(fifo), len + offset);
    __kfifo_out_data(fifo, to, len, offset);
    return len;
}

static inline void kfifo_drop (struct kfifo *fifo)
{
    fifo->out = fifo->in;
}

static inline void kfifo_cat_sync (struct kfifo *fifo)
{
    fifo->out_cat = fifo->out;
}

/********************************************************
* MISC APIs
*********************************************************/
static char  logbuf_dir[LOGBUF_DIR_MAX_SIZE] = RTDLOG_DEFAULT_PATH;
int rtk_get_log_path (char *buf, int buf_len)
{
    if(strlen(logbuf_dir) == 0)
    {
        return -1;
    }

    strncpy(buf, logbuf_dir, buf_len-1);
    buf[buf_len - 1] = 0;
    return 0;
}

void rtdlog_parse_bufsize (char * str)
{
#if 0
    unsigned long default_value[RTD_BUFSIZE_NUM]=
    {
        0x80000,
        0x100000,
        0x200000,
        0x400000
    };
#endif

    char *sub_str[RTD_BUFSIZE_NUM]=
    {
        "bufsize=80000",
        "bufsize=100000",
        "bufsize=200000",
        "bufsize=400000"
    };
    int i;

    for(i=0; i<RTD_BUFSIZE_NUM; i++)
    {
        if(strstr(str, sub_str[i])!= NULL)
        {
            //rtdlog_bufsize = default_value[i];
            rtdlog_bufsize = RTD_LOGBUF_DEFAULT_LIMIT << i ;
            rtdlog_param_size = RTD_LOGBUF_DEFAULT_PARAM << i ;
            //rtd_pr_rtdlog_err("get bufsize=%x, rtdlog_param_size=%x\n",rtdlog_bufsize, rtdlog_param_size);
            return;
        }
    }

    rtd_pr_rtdlog_err("%s %d.  warning! rtdlog_bufsize is not setted!\n",__func__,__LINE__);
    rtdlog_bufsize = 0;
}
EXPORT_SYMBOL(rtdlog_parse_bufsize);
extern bool rtk_parse_commandline_equal(const char *string, char *output_string, int string_size);
#if defined(CONFIG_REALTEK_LOGBUF_MODULE) || (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
void rtdlog_parse_bufsize_init(void)
{
    char strings[16] = {0};

    if(rtk_parse_commandline_equal("bufsize", strings,sizeof(strings)-1) == 0){
        rtd_pr_rtdlog_err("Error : can't get bufsize from bootargs\n");
        return;
    }
    rtd_pr_rtdlog_err("bufsize=**%s**\n",strings);
    if (kstrtoul(strings, 16, &rtdlog_bufsize)){
        rtd_pr_rtdlog_err("%s invalid str:%s\n",__FUNCTION__, strings);
        return;
    }

    rtdlog_param_size = (RTD_LOGBUF_DEFAULT_PARAM * rtdlog_bufsize / RTD_LOGBUF_DEFAULT_LIMIT);
    rtd_pr_rtdlog_err("rtdlog_bufsize=%lx, rtdlog_param_size=%lx\n",rtdlog_bufsize, rtdlog_param_size);
}
#endif

unsigned long rtdlog_get_buffer_size (void)
{
    return rtdlog_bufsize;
}
EXPORT_SYMBOL(rtdlog_get_buffer_size);

struct workqueue_struct *p_dumpqueue = NULL;
struct rtd_logger_work_struct DumpWork_cpu[MAX_LOGBUF_NUM];
int rtdlog_watchdog_dump_work(EnumLogbufType type)
{
    if (unlikely(p_dumpqueue == NULL))
    {
        return -EPERM;
    }

    DumpWork_cpu[type].param= type;
    queue_work(p_dumpqueue, &(DumpWork_cpu[type].work));
    return 0;
}
EXPORT_SYMBOL(rtdlog_watchdog_dump_work);
static void dump_ddr_range (char * msg,void * start,unsigned int len)
{
    char buf[RTD_BUFSIZE_512];
    int i = 0,j = 0;
    unsigned char * tmp = start;

    rtd_pr_rtdlog_err("%s\n",msg);
    for(i = 0; i < len; ++i)
    {
        if((i % 16 == 0)&&(i != 0))
        {
            buf[511] = 0;
            rtd_pr_rtdlog_err("%lx: %s\n",(unsigned long)(tmp-16),buf);
            j = 0;
        }
        if(j < sizeof(buf))
        {
            snprintf(&buf[j], RTD_BUFSIZE_512 - j," %02x", *tmp);
        }
        j += 3;
        tmp++;
    }

    if(i % 16 != 0)
    {
        buf[511] = 0;
        rtd_pr_rtdlog_err("%lx: %s\n",(unsigned long)(tmp-16),buf);
    }
}

/********************************************************
* rtdlog APIs
*********************************************************/
void update_rtdlog_timestamp(void)
{
    if(is_setup_rtd_logbuf)
    {
        *gpRtdlogTimestamp = local_clock();
    }
}

u32 get_msghead_sum(rtd_msg_header * header)
{
    int i;
    u32 val = 0;

    val += header->cpu_core;
    val += header->cpu_type;
    val += header->len;
    val += header->level;
    val += header->module;
    val += header->pid;
    val += header->tid;
    val += header->text_len;
    val += (header->timestamp & 0xffffffff);
    val += (header->timestamp>>32)&0xffffffff;

    for(i = 0; i < sizeof(header->name); ++i)
    {
        val += header->name[i];
    }

    return val;
}

static int check_msghead_sum(rtd_msg_header * header)
{
    u32 sum_store = header->check_sum;
    u32 sum_cacu = get_msghead_sum(header);

    if(sum_store == sum_cacu)
    {
        return 0;
    }

    return -1;
}

static void dump_rtd_msg_header (rtd_msg_header * start,unsigned int len)
{
    char buf[81];
    int i , j=0;

    for(i=0; i<16; i++)
    {
        snprintf(&buf[j], sizeof(buf)-j,"0x%02x ",start->name[i]);
        j += 5;
    }

    rtd_pr_rtdlog_err("*****************dump_rtd_msg_header start**********************\n");
    rtd_pr_rtdlog_err("timestamp = 0x%016llx\n",start->timestamp);
    rtd_pr_rtdlog_err("len = 0x%08x\n",start->len);
    rtd_pr_rtdlog_err("pid = 0x%08x\n",start->pid);
    rtd_pr_rtdlog_err("tid = 0x%08x\n",start->tid);
    rtd_pr_rtdlog_err("module = 0x%08x\n",start->module);
    rtd_pr_rtdlog_err("text_len = 0x%08x\n",start->text_len);
    rtd_pr_rtdlog_err("check_sum = 0x%08x\n",start->check_sum);
    rtd_pr_rtdlog_err("name = %s\n",buf);
    rtd_pr_rtdlog_err("level = 0x%02x\n",start->level);
    rtd_pr_rtdlog_err("cpu_type = 0x%02x\n",start->cpu_type);
    rtd_pr_rtdlog_err("cpu_core = 0x%02x\n",start->cpu_core);
    rtd_pr_rtdlog_err("reserve0 = 0x%02x\n",start->reserve0);
    rtd_pr_rtdlog_err("*****************dump_rtd_msg_header end***********************\n");
}

static int check_msghead_valid ( rtd_msg_header * header, bool debug)
{
    if((header->len > header->text_len)&&((header->len - header->text_len) == sizeof(rtd_msg_header)))
    {
        if(check_msghead_sum(header))
        {
            rtd_pr_rtdlog_err("%s %d. msg header checksum error\n",__func__,__LINE__);
            goto check_fail;
        }
        else
        {
            goto check_pass;
        }
    }
    else if (debug)
    {
        rtd_pr_rtdlog_err("%s %d. msg header len error %x|%x  %x\n",__func__,__LINE__,header->len,header->text_len,sizeof(rtd_msg_header));
    }

check_fail:
    return -1;
check_pass:
    return 0;
}


// 1:PASS ; -1:VERIFY FAIL ; 0:EMPTY
int get_msg_header (EnumLogbufType type,rtd_msg_header* header, struct kfifo * fifo)
{
    unsigned int offset=(fifo->out)%(fifo->size);
    unsigned char* start= rtdlog_phys_to_virt((unsigned long)fifo->buffer);
    //unsigned char* end = rtdlog_phys_to_virt((unsigned long)fifo->buffer)+fifo->size;
    unsigned char* out = start + offset ;

    if (kfifo_len(fifo) > sizeof(rtd_msg_header))
    {
        kfifo_out_peek(fifo,header,sizeof(rtd_msg_header),0);
        if( check_msghead_valid(header,true) )
        {
            goto FAIL;
        }

#if 0
        if( (kfifo_len(fifo) < header->len))
        {
            rtd_pr_rtdlog_err("%s %d. fifo len \n",__func__,__LINE__);
            goto FAIL;
        }
#endif

        return 1;

FAIL:
        dump_ddr_range("dump header ddr",(void *)header,sizeof(rtd_msg_header));

        rtd_pr_rtdlog_err("offset=%x\n",offset);
        dump_ddr_range("dump more",(void *)(out-0x20),0x100);
        dump_rtd_msg_header(header,sizeof(rtd_msg_header));

        kfifo_drop(fifo);
        gDbgInfo.log_reset_cnt[type]++;
        rtd_pr_rtdlog_err("type[\e[1;31m%s\e[0m] check msg header fail!\n\n\n",cpu_type_str[type]);
        return -1;
    }
    return 0;
}

static int rtd_get_savelevel(int type)
{
        int savelevel = RTD_DEFAULT_SAVELEVEL;

        if(type < MAX_LOGBUF_NUM){
            savelevel = pRtdLogCB[type]->savelevel;
        }

        return savelevel;
}

int rtdlog_save_msg (EnumLogbufType type, const char * text, rtd_msg_header * header)
{
    struct kfifo * fifo=array_fifo[type];
    //struct mutex * mtx=log_save_mutex[type];
    static unsigned char num = 0;

    if(!is_setup_rtd_logbuf)
    {
        return -1;
    }

    if(rtd_get_savelevel(type) < (header->level))
    {
        return 0;
    }
#if 0
    if((type == HAL_BUF)||(type == KERNEL_BUF))
    {
        if(check_log_filter(header,log_filter))
        {
            return 0;
        }
    }
#endif

    //mutex_lock(mtx);
    if (kfifo_avail(fifo) > (header->len))
    {
        header->reserve0 = (num++);
        header->check_sum = get_msghead_sum(header);

        kfifo_in(fifo,header,sizeof(rtd_msg_header));
        kfifo_in(fifo,text,header->text_len);

        gDbgInfo.save_log_len[type] += header->text_len;
        gDbgInfo.save_log_cnt[type]++;
    }
    else
    {
#if 0
        puts_("\n\n\n\e[1;34m");
        puts_(text);
        puts_("\e[0m\n\n\n");
#endif
        pRtdLogCB[type]->drop_cnt++;
    }
    //mutex_unlock(mtx);

    return 0;
}
#if defined(CONFIG_TRACEPOINTS) && defined(CONFIG_ANDROID_VENDOR_HOOKS) && (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
DEFINE_RAW_SPINLOCK(logbuf_lock);
#endif
void rtd_logbuf_save_log ( int level, const char *text, u16 text_len)
{
    rtd_msg_header tmp_header;
    if(is_save_early_log_done==0)
    {
        //if(is_setup_rtd_logbuf==0){
        return;
    }

    /*rtdlog buf msg*/
    tmp_header.cpu_type = (u8)KERNEL_BUF;
    tmp_header.cpu_core = smp_processor_id();
    tmp_header.module = 0;
    tmp_header.pid = task_tgid_nr(current);
    tmp_header.tid = task_pid_nr(current);
    tmp_header.timestamp = *gpRtdlogTimestamp;
    tmp_header.level = level;
    tmp_header.text_len = text_len;
    tmp_header.len = sizeof(tmp_header) + text_len;

    //memset(tmp_header.name,'#',sizeof(tmp_header.name));
    snprintf(tmp_header.name,sizeof(tmp_header.name),current->comm);
    //memcpy(tmp_header.name,current->comm,sizeof(tmp_header.name));
    //tmp_header.name[sizeof(tmp_header.name)-1] = '\0';

#if defined(CONFIG_TRACEPOINTS) && defined(CONFIG_ANDROID_VENDOR_HOOKS) && (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
    raw_spin_lock(&logbuf_lock);
#endif
    rtdlog_save_msg(KERNEL_BUF,text, &tmp_header);
#if defined(CONFIG_TRACEPOINTS) && defined(CONFIG_ANDROID_VENDOR_HOOKS) && (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
    raw_spin_unlock(&logbuf_lock);
#endif
}
EXPORT_SYMBOL(rtd_logbuf_save_log);


#if defined(CONFIG_REALTEK_LOGBUF_MODULE) || (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
static struct timer_list logbuffer_timer;
static void rtk_logbuffer_timer_func (struct timer_list *t)
{
    update_rtdlog_timestamp();
    mod_timer(&logbuffer_timer, (jiffies + msecs_to_jiffies(20)));
}
static int __init init_logbuffer_timer (void)
{
    timer_setup(&logbuffer_timer, rtk_logbuffer_timer_func, 0);
    //setup_timer(&logbuffer_timer, rtk_logbuffer_timer_func, 0); //code for < 5.4
    mod_timer(&logbuffer_timer, (jiffies + msecs_to_jiffies(10)));
    return 0;
}
#if !(defined(CONFIG_TRACEPOINTS) && defined(CONFIG_ANDROID_VENDOR_HOOKS) && (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)))
#include <linux/kmsg_dump.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
    static struct kmsg_dump_iter dumper;
#else
    static struct kmsg_dumper dumper = { .active = 1 };
#endif
void rtd_get_native_log (void)
{
    int count=10;
    size_t len;
    unsigned char buf[1024];

    while (count--)
    {
        //local_irq_disable();
        memset(buf,0,sizeof(buf));
        if(!kmsg_dump_get_line(&dumper, false, buf, sizeof(buf)-2, &len))
        {
            //local_irq_enable();
            return;
        }

        if(!strlen(buf)){
            continue;
        }

        //local_irq_enable();
        rtd_logbuf_save_log(0,buf,strlen(buf));
    }
}

int thread_logbuf_reader(void* p)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
    kmsg_dump_rewind(&dumper);
#endif
    for(;;)
    {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
        rtd_get_native_log();
#endif
        rtdlog_sleep(0,0);
    }
    return 0;
}
#endif
#endif


/********************************************************
* rtdlog cat related APIs
*********************************************************/
static inline unsigned int kfifo_cat_len(struct kfifo *fifo)
{
    register unsigned int out_cat;
    out_cat = fifo->out_cat;
    smp_rmb();
    return (fifo->in) - out_cat;
}

static inline void __kfifo_cat_out_data(struct kfifo *fifo,void *to, unsigned int len, unsigned int off)
{
    unsigned int l;

    /*
     * Ensure that we sample the fifo->in index -before- we
     * start removing bytes from the kfifo.
     */

    smp_rmb();

    off = __kfifo_off(fifo, fifo->out_cat + off);

    /* first get the data from fifo->out until the end of the buffer */
    l = min((unsigned long)len, (unsigned long)(fifo->size - off));
    memcpy(to, (unsigned char *)rtdlog_phys_to_virt((unsigned long)fifo->buffer) + off, l);

    /* then get the rest (if any) from the beginning of the buffer */
    memcpy(to + l, (unsigned char *)rtdlog_phys_to_virt((unsigned long)fifo->buffer), len - l);
}

static unsigned int kfifo_cat_out_peek (struct kfifo *fifo, void *to, unsigned int len, unsigned offset)
{
    len = min(kfifo_cat_len(fifo), len + offset);
    __kfifo_cat_out_data(fifo, to, len, offset);
    return len;
}

static inline void __kfifo_add_cat_out (struct kfifo *fifo,unsigned int off)
{
    smp_mb();
    fifo->out_cat += off;
}

#if 0
static unsigned int kfifo_cat_out(struct kfifo *fifo, void *to, unsigned int len)
{
    len = min(kfifo_cat_len(fifo), len);

    __kfifo_cat_out_data(fifo, to, len, 0);
    __kfifo_cat_add_out(fifo, len);
    return len;
}
#endif

//RET VAL:
// 1:PASS ; -1:VERIFY FAIL; 0:EMPTY
int get_catmsg_header (EnumLogbufType type, rtd_msg_header* header)
{
    struct kfifo * fifo=array_fifo[type];
    if (kfifo_cat_len(fifo) > sizeof(rtd_msg_header))
    {
        kfifo_cat_out_peek(fifo,header,sizeof(rtd_msg_header),0);
        if(check_msghead_valid(header,1))//if fail
        {
            rtd_pr_rtdlog_err("type[%d] check logcat msg header fail!\n",type);
            dump_ddr_range("dump header ddr",(void *)header,sizeof(rtd_msg_header));
            return -1;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        //rtd_pr_rtdlog_err("FAIL:[%s](%d)...\n",__FUNCTION__,__LINE__);
    }

    return 0;
}

static char cat_buffer[2048];
int rtd_build_msg_header (rtd_msg_header* header, char* msg_header);
unsigned int rtd_logcat_data_get (EnumLogbufType type, rtd_msg_header* header, char __user * buf, size_t count )
{
    struct kfifo * fifo = array_fifo[type];
    //unsigned int len,ret_len,offset,eoff;
    int pos;
    unsigned int out_len,ret;

    if((header->text_len) > 1536)
    {
        rtd_pr_rtdlog_err("%s %d.  drop type %s's log\n",__func__,__LINE__,cpu_type_str[type]);
        __kfifo_add_cat_out(fifo, header->len);
        return 0;
    }

    //memset(msg_tag,0,sizeof(msg_tag));
    pos=rtd_build_msg_header(header,cat_buffer);
    __kfifo_add_cat_out(fifo, sizeof(rtd_msg_header));

    //memset(buffer,0,sizeof(msg_tmp_buf));
    out_len=kfifo_cat_out_peek(fifo,cat_buffer+pos,header->text_len,0);
    __kfifo_add_cat_out(fifo, out_len);

    if(cat_buffer[pos+out_len-1]!='\n')
    {
        cat_buffer[pos+out_len]='\n';
        out_len++;
    }
    //buffer[out_len]=0;

    ret=((pos+out_len)>count)?(count):(pos+out_len);
    if(copy_to_user(buf, (void *)cat_buffer, ret ))
    {
        rtd_pr_rtdlog_err("%s %d.  copy_to_user  err\n",__func__,__LINE__);
        return 0;
    }
    //rtd_pr_rtdlog_err("%s %d %s\n",__func__,__LINE__,cat_buffer);
    return ret;
    //kfree(buffer);
}

ssize_t rtd_logcat_data (EnumLogbufType type, char __user * buf, size_t count )
{
    rtd_msg_header header;
    ssize_t ret_len = -1;
    int header_check = 0;

    header_check = get_catmsg_header(type,&header);
    if (1 == header_check )
    {
#if 0
        if(check_log_filter(&header,cat_filter))
        {
            skip_catmsg_data(type,&header);
            ret_len = 0;
        }
        else
#endif

        {
            ret_len = rtd_logcat_data_get(type,&header,buf, count);
        }
    }
    else if ( -1 == header_check )
    {
        //mutex_lock(&logcat_mutex);
        kfifo_cat_sync(array_fifo[type]);
        //mutex_unlock(&logcat_mutex);
    }

    return ret_len;
}


/********************************************************
* rtdlog file operations APIs
*********************************************************/
int rtd_build_msg_header (rtd_msg_header* header, char* msg_header)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
    struct __kernel_old_timeval log_time=ns_to_kernel_old_timeval((s64)header->timestamp);
#else
    struct timeval log_time=ns_to_timeval((s64)header->timestamp);
#endif
    int pos=0, len = 0;

    len = snprintf(msg_header+pos, RTDLOG_HEADER_BUF_SIZE - pos, "<%d>[%08d.%06d](%5s:",
                   header->level,
                   (int)log_time.tv_sec,//(u32)(header->timestamp>>32),
                   (int)log_time.tv_usec,//(u32)(header->timestamp&0xffffffff),
                   (char *)(cpu_type_str[header->cpu_type]));
    pos += len;

    if(header->cpu_core != 0xff)
    {
        len = snprintf(msg_header+pos, RTDLOG_HEADER_BUF_SIZE - pos, "%d)",header->cpu_core);
    }
    else
    {
        len = snprintf(msg_header+pos, RTDLOG_HEADER_BUF_SIZE - pos, "?)");
    }
    pos += len;

    if ( header->cpu_type == KERNEL_BUF )
    {
        len = snprintf(msg_header+pos, RTDLOG_HEADER_BUF_SIZE - pos, " [%03d] #%-15s# ",(unsigned char)(header->reserve0),header->name);
        pos += len;

        //len = snprintf(msg_header+pos, 256 - pos, "[%08x]: ", header->module);
        //pos += len;
    }
    else if ( header->cpu_type == VCPU1_BUF )
    {
        len = snprintf(msg_header+pos, RTDLOG_HEADER_BUF_SIZE - pos, " [%03d] ",(unsigned char)(header->reserve0));
        pos += len;
    }
    return pos;
}

#if 1
static unsigned char msg_tmp_buf[2048];
static ssize_t kfifo_out_peek_write ( EnumLogbufType type, struct kfifo *fifo, rtd_msg_header* header)
{
    unsigned char * buffer=msg_tmp_buf;
    unsigned int out_len;
    int pos;
    ssize_t ret=0, result;

    if((header->text_len) > 1536)
    {
        rtd_pr_rtdlog_err("%s %d.  drop type %s's log\n",__func__,__LINE__,cpu_type_str[type]);
        __kfifo_add_out(fifo, header->text_len);
        return 0;
    }

    //memset(msg_tag,0,sizeof(msg_tag));
    pos=rtd_build_msg_header(header,buffer);

    //memset(buffer,0,sizeof(msg_tmp_buf));
    out_len=kfifo_out_peek(fifo,buffer+pos,header->text_len,0);
    __kfifo_add_out(fifo, out_len);

    if(buffer[pos+out_len-1]!='\n')
    {
        buffer[pos+out_len]='\n';
        out_len++;
    }
    //buffer[out_len]=0;

    ret=rtd_kernel_write_file(type, buffer, pos+out_len,&result);

    if( ((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->flag_contain_av_log && ((ACPU1_BUF==type)||(VCPU1_BUF==type)) )
    {
        buffer[pos+out_len]=0;
        rtd_pr_rtdlog_err("%s",buffer);
    }

    return ret;
}

#else
#endif

ssize_t rtd_write_msg_data ( EnumLogbufType type, rtd_msg_header* header, struct kfifo * fifo )
{
    __kfifo_add_out(fifo, sizeof(rtd_msg_header));
    return kfifo_out_peek_write(type, fifo,header);
}

ssize_t rtd_write_data (EnumLogbufType type)
{
    rtd_msg_header header;
    int header_check;
    ssize_t ret=0;
    header_check = get_msg_header(type,&header, array_fifo[type]);
    if(header_check==0)
    {
        return ret;
    }
    else if (header_check==-1)
    {
        rtd_pr_rtdlog_err("%s %d. error happen! insert miss log msg for type:%d\n",__func__,__LINE__,type);
        return -2;
    }

    //mutex_lock(&logcat_mutex);
    ret=rtd_write_msg_data(type, &header, array_fifo[type]);
    //mutex_unlock(&logcat_mutex);

#if 0
    gDbgInfo.file_log_len[type] += header.text_len;
    gDbgInfo.file_log_cnt[type]++;
    gDbgInfo.read_log_len[type] += header.text_len;
    gDbgInfo.read_log_cnt[type]++;
#endif

    return ret;
}

static int do_fsync(unsigned int fd, int datasync)
{
	struct fd f = fdget(fd);
	int ret = -EBADF;

	if (f.file) {
		ret = vfs_fsync(f.file, datasync);
		fdput(f);
		inc_syscfs(current);
	}
	return ret;
}
int rtd_sys_fdatasync(unsigned int fd)
{
    return do_fsync(fd, 1);
}
void rtdlog_sleep (EnumLogbufType type, long write_count )
{
    static int count[MAX_LOGBUF_NUM];
    RTDLOG_BOOT_SHARE_PARAM* p_share = (RTDLOG_BOOT_SHARE_PARAM*) share_buffer;
    if ( ((count[type]++%20)==0) && p_share && rtkd_file_info[type].usb_fd )
    {
        rtd_sys_fdatasync(rtkd_file_info[type].usb_fd);
    }

#if 0
    if(KERNEL_BUF!=type)
    {
        return;
    }

    if(!(g_fw_pos[type] % READ_SLEEP_SIZE))
    {
        msleep(20);
    }
#else

#if 0
    static int flag=0;
    if( (write_count>0) && (flag++ > 0 ) )
    {
        msleep_interruptible(20);
        set_freezable();
        flag=0;
        //rtd_pr_rtdlog_err("%s %d  sleep for a while ! type is %d \n",__func__,__LINE__, type);
    }
#else
    set_freezable();

    if (p_share->flag_sleep)
    {
        usleep_range(200, 400);//msleep_interruptible(10);
    }
#endif

#endif
}

long rtd_logfile_size_check ( EnumLogbufType type, ssize_t ret )
{
    static ssize_t file_len[MAX_LOGBUF_NUM]= {0};
    //long check_size=(KERNEL_BUF==type)?(LOG_FILE_KERNEL_MAX_SIZE):(LOG_FILE_FW_MAX_SIZE);
    long check_size=((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->max_file_size[type];

#if 0
    struct SHARE_STRUCT* p_share = (struct SHARE_STRUCT*) share_buffer;
    if ((NULL==p_share) || ( 0== p_share->fd[type]))
    {
        return 0;
    }

    rtd_pr_rtdlog_err("%s %d  %x %x\n",__func__,__LINE__,(fdget(p_share->fd[type])->f_pos),check_size);
    if((fdget(p_share->fd[type])->f_pos)>check_size) // if(g_fw_pos[type] > check_size)
    {
        //g_fw_pos[type]=0;
        return 1;
    }
#else
    file_len[type]+=ret;
    if(file_len[type] >= check_size)
    {
        file_len[type]=0;
        return 1;
    }
#endif
    return 0;
}

//#define RTD_TIME_CHECK
unsigned int drvif_Get_90k_Lo_clk(void);
int64_t MEMC_GetPTS(void);
long rtdlog_ioctl_write_type (EnumLogbufType type)
{
#ifdef RTD_TIME_CHECK
    int64_t t1,t2;
#endif

    ssize_t ret;

#ifdef RTD_TIME_CHECK
    t1=MEMC_GetPTS();//drvif_Get_90k_Lo_clk();
#endif

    ret=rtd_write_data(type);

#ifdef RTD_TIME_CHECK
    t2=MEMC_GetPTS();//drvif_Get_90k_Lo_clk();
    //rtd_pr_rtdlog_err("%s %d.  type %s cost %03d ms .    (t2=%x  t1=%x)\n",__func__,__LINE__,cpu_type_str[type],(int)((t2-t1)*1000/90),t2,t1);
    rtd_pr_rtdlog_err("%s %d.  type %s cost %d ms\n",__func__,__LINE__,cpu_type_str[type],(int)((t2-t1)*1000/90));
#endif

    rtdlog_sleep(type,ret);

    if(rtd_logfile_size_check(type,ret))
    {
        return type+0x1;
    }
    return 0;
}

#ifdef CONFIG_RTK_USBDUMP_ENABLE
static int flag_usb=0;
void rtk_reset_usb_path (void);
int rtk_get_usb_path (char *buf, int buf_len);
int rtd_check_usb_logfile (void)
{
    struct SHARE_STRUCT* p_share = (struct SHARE_STRUCT*) rtkd_share_buffer;
    if(NULL==p_share)
    {
        return 0;
    }

    if ((0==flag_usb) && !rtk_get_usb_path(p_share->usb_path, MAX_PATH_LEN-1) )
    {
        flag_usb=1;
        return 1;
    }

    if((1==flag_usb) && rtk_get_usb_path(NULL,0) )
    {
        flag_usb=0;
        memset(p_share->usb_path,0,MAX_PATH_LEN-1);
        return 2;
    }

    return 0;
}
#endif


#ifdef CONFIG_RTK_USBDUMP_ENABLE
volatile int rtdlog_sync_flag1;
volatile int rtdlog_sync_flag2;
void rtdlog_sync (void)
{
    rtd_pr_rtdlog_err("%s %d\n",__func__,__LINE__);
    rtdlog_sync_flag1=1;
    while (1==rtdlog_sync_flag2)
    {
    }
    rtd_pr_rtdlog_err("%s %d\n",__func__,__LINE__);
    return;
}
#endif
int rtd_check_usb_changeflag (void)
{
    struct SHARE_STRUCT* p_share = (struct SHARE_STRUCT*) rtkd_share_buffer;
    if(NULL==p_share)
    {
        return 0;
    }

    if ((p_share->usb_status_flag) == 1) // 1,flag set int rtkd 2,Check whether the USB is plugged and removed
    {
        return 1;
    }

    return 0;
}

static DEFINE_SEMAPHORE(rtdlog_sem);
long rtdlog_ioctl_write (unsigned long arg)
{
    EnumLogbufType type;
    long ret;
    error_flag=0;
    usb_error_flag=0;
#ifdef CONFIG_RTK_USBDUMP_ENABLE
    rtdlog_sync_flag2=(2==rtdlog_sync_flag1)?0:1;
#endif
    while(1)
    {
        if(rtd_check_usb_changeflag())
        {
            return 0;
        }
        for(type=ACPU1_BUF; type<MAX_LOGBUF_NUM; type++)
        {
            if(((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->flag_status==0)
            {
                int val=down_interruptible(&rtdlog_sem);
                rtd_pr_rtdlog_err("%s %d.  rtdlog flow suspended!\n",__func__,__LINE__);
                if (val)
                {
                    return val;
                }
            }
            
            if((ret=rtdlog_ioctl_write_type(type))>0)
            {
                return ret;
            }
            if(error_flag<0)
            {
                rtd_pr_rtdlog_err("%s %d.  got error %d\n",__func__,__LINE__,error_flag);
                return error_flag;
            }

#ifdef CONFIG_RTK_USBDUMP_ENABLE
            if(rtd_check_usb_logfile())
            {
                return 0;
            }

            if(1==rtdlog_sync_flag1)
            {
                struct SHARE_STRUCT* p_share = (struct SHARE_STRUCT*) rtkd_share_buffer;
                memset(p_share->usb_path,0,MAX_PATH_LEN-1);
                rtk_reset_usb_path();
                flag_usb=0;
                rtdlog_sync_flag1=2;
                return 0;
            }
#endif
            if(usb_error_flag == -EIO)
            {
                rtd_pr_rtdlog_err("%s %d. usb write log error %d\n",__func__,__LINE__,usb_error_flag);
                return usb_error_flag;
            }
        }
    }
    return 0;
}

long rtdlog_ioctl ( struct file*filp, unsigned int cmd, unsigned long arg)
{
    long ret=-EINVAL;
    switch(cmd)
    {
        case RTDLOG_IOC_WRITE_CMD:
            ret=rtdlog_ioctl_write(arg);
            break;
        default:
            rtd_pr_rtdlog_err( "%s %d. wrong ioctrol cmd\n",__func__,__LINE__);
    }

    return ret;
}

int rtdlog_open ( struct inode* inode, struct file* filp )
{
    struct rtdlog_dev *pdev = container_of(inode->i_cdev, struct rtdlog_dev, cdev);
    filp->private_data = pdev; /* for other methods */

    //rtd_pr_rtdlog_warn("rtdlog device open\n");
    return 0;
}
int rtdlog_release ( struct inode* inode, struct file* filp)
{
    filp->private_data = NULL;
    return 0;
}

ssize_t rtdlog_read (struct file * filp, char __user * buf, size_t count, loff_t * f_pos)
{
    int i = 0;
    static int j = 0;
    ssize_t len = 0;
    static int need_sync = 1;

    if(!is_setup_rtd_logbuf)
    {
        return -1;
    }
    if(need_sync)
    {
        for(i = 0; i < MAX_LOGBUF_NUM; i++)
        {
            //mutex_lock(&logcat_mutex);
            kfifo_cat_sync(array_fifo[i]);
            //mutex_unlock(&logcat_mutex);
        }
        need_sync = 0;
    }

    while(1)
    {
        j %= MAX_LOGBUF_NUM;
        len = rtd_logcat_data(j++, buf, count );
        if (len > 0)
        {
            break;
        }

        if (signal_pending(current))
        {
            need_sync = 1;
            break;
        }
    }

    return len;
}

ssize_t rtdlog_write (struct file * filp, const char __user * buf, size_t count, loff_t * f_pos)
{
    u32 info_size = sizeof(rtd_logbuf_hal_info);
    u32 text_len;
    const char * text;
    rtd_msg_header tmp_header;
    rtd_logbuf_hal_info info;
    char databuf[RTD_BUFSIZE_512] = {0};
    unsigned long copy_ret;
    struct mutex * mtx=log_save_mutex[HAL_BUF];

    if(is_setup_rtd_logbuf)
    {
        if(count < info_size)
        {
            rtd_pr_rtdlog_err("%s : lost hal head info!\n", __FUNCTION__);
            return 0;
        }

        if(count > RTD_BUFSIZE_512)
        {
            rtd_pr_rtdlog_err("%s : hal log length:%d > 512!\n", __FUNCTION__, (unsigned int)count);
            return 0;
        }
        //parse buffer info.
        copy_ret = copy_from_user(databuf, buf, count);

        if(copy_ret)
        {
            rtd_pr_rtdlog_err("error : copy_from_user return 0x%x,drop 1 msg\n",(unsigned int)copy_ret);
            return 0;
        }

        //level,text_len,cpu_core,text
        memcpy(&info, databuf, info_size);

        /*****special process for hal buffer,because hal log is too much*****
        ***** we will block the log into hal buffer,if not match filter *****
        ********************************************************************/

        text_len = count - info_size;
        text = databuf + info_size;

        //save log
        tmp_header.cpu_type = (u8)HAL_BUF;
        tmp_header.cpu_core = info.core;
        tmp_header.pid = task_tgid_nr(current);
        tmp_header.tid = task_pid_nr(current);
        tmp_header.timestamp = *gpRtdlogTimestamp;
        tmp_header.level = info.level;
        tmp_header.module = info.module;
        tmp_header.text_len = text_len;
        tmp_header.len = sizeof(tmp_header) + text_len;

        memset(tmp_header.name,'#',sizeof(tmp_header.name));
        memcpy(tmp_header.name,current->comm,sizeof(tmp_header.name));
        tmp_header.name[sizeof(tmp_header.name)-1] = '\0';

        mutex_lock(mtx);
        rtdlog_save_msg(HAL_BUF, text, &tmp_header);
        mutex_unlock(mtx);
    }
    else
    {
        //rtd_pr_rtdlog_err("rtd_log buf is not setup yet...\n");
        return 0;
    }

    return tmp_header.len;
}

static int rtdlog_mmap (struct file *file, struct vm_area_struct *vma)
{
    if(NULL==rtkd_share_buffer)
    {
        return -EAGAIN;
    }

    if ((vma->vm_end - vma->vm_start)!=PAGE_SIZE)
    {
        rtd_pr_rtdlog_err("%s %d. only kmalloc %x  byte for  share memory for rtkd !  %x\n",__func__,__LINE__,(unsigned int)PAGE_SIZE,(unsigned int)(vma->vm_end - vma->vm_start));
        return -EAGAIN;
    }

    if(remap_pfn_range(vma,vma->vm_start,
                       virt_to_phys(rtkd_share_buffer)>>PAGE_SHIFT,
                       vma->vm_end - vma->vm_start, vma->vm_page_prot))
    {
        rtd_pr_rtdlog_err("%s %d. cannot remap share memory for rtkd !\n",__func__,__LINE__);
        return -EAGAIN;
    }
    return 0;

}


/********************************************************
* rtdlog keylog APIs
*********************************************************/

static unsigned int rtd_keylog_pos = 0; // record keylog buffer offset
static unsigned int rtd_keylog_last_pos = 0; // record last boot buffer offset
unsigned char * rtd_keylog_addr = NULL;
int init_rtd_keylog_buf(void)
{
    struct mutex * mtx = &keylog_mutex;

    mutex_lock(mtx);
    rtd_keylog_pos = 0;
    rtd_keylog_last_pos = 0;
    rtd_keylog_addr = (unsigned char *)(rtdlog_phys_to_virt(RTD_LOGBUF_PHY_START_ADDR + RTD_LOGBUF_KEYLOG_OFFSET));
    rtd_pr_rtdlog_warn("keylog start addr : 0x%08x, size:0x%x\n", (RTD_LOGBUF_PHY_START_ADDR + RTD_LOGBUF_KEYLOG_OFFSET), RTD_LOGBUF_KEYLOG_SIZE);

    //dump_ddr_range("keylog before compare:", rtd_keylog_addr, 0x100);
    *(rtd_keylog_addr+RTD_LOGBUF_KEYLOG_SIZE-1) = '\0';     // for strlen
    if(strncmp(rtd_keylog_addr, RTD_KEYLOG_MAGIC, strlen(RTD_KEYLOG_MAGIC)) == 0)         // reset, backup history keylog pos.
    {
        rtd_keylog_pos = strlen(rtd_keylog_addr);
        rtd_keylog_last_pos = rtd_keylog_pos;
    }

    // add this boot keylog magic string
    snprintf((rtd_keylog_addr+rtd_keylog_pos), RTD_LOGBUF_KEYLOG_SIZE-rtd_keylog_pos-1 , "%s", RTD_KEYLOG_MAGIC);
    rtd_keylog_pos = strlen(rtd_keylog_addr);
    mutex_unlock(mtx);
    return 0;
}

int rtdlog_save_keylog (const char * fmt, ...)
{
    va_list args;
    struct mutex * mtx = &keylog_mutex;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
    struct __kernel_old_timeval log_time;
#else
    struct timeval log_time;
#endif

    if((!is_setup_rtd_logbuf) || (rtd_keylog_addr == NULL) || (gpRtdlogTimestamp == NULL))
    {
        rtd_pr_rtdlog_err("Save keylog fail. is_setup_rtd_logbuf=%d, rtd_keylog_addr or gpRtdlogTimestamp is Null\n",is_setup_rtd_logbuf);
        return -1;
    }

    if(rtd_keylog_pos >= RTD_LOGBUF_KEYLOG_SIZE-1)  // keylog buffer full
    {
        rtd_pr_rtdlog_err("Save keylog fail. keylog buffer is full\n");
        return -2;
    }

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
    log_time = ns_to_kernel_old_timeval((s64)(*gpRtdlogTimestamp));
#else
    log_time = ns_to_timeval((s64)(*gpRtdlogTimestamp));
#endif
    mutex_lock(mtx);
    // save time stamp
    snprintf((rtd_keylog_addr+rtd_keylog_pos), RTD_LOGBUF_KEYLOG_SIZE-rtd_keylog_pos-1 , "[%08d.%06d]", (int)log_time.tv_sec, (int)log_time.tv_usec);

    // save log
    va_start(args, fmt);
    rtd_keylog_pos = strlen(rtd_keylog_addr);
    if(rtd_keylog_pos < RTD_LOGBUF_KEYLOG_SIZE-1)
    {
        vsnprintf((rtd_keylog_addr+rtd_keylog_pos), RTD_LOGBUF_KEYLOG_SIZE-rtd_keylog_pos-1, fmt, args);
    }
    va_end(args);

    // append \n after each log
    rtd_keylog_pos = strlen(rtd_keylog_addr);
    if((rtd_keylog_pos<RTD_LOGBUF_KEYLOG_SIZE) && (*(rtd_keylog_addr+rtd_keylog_pos-1)!='\n'))
    {
        *(rtd_keylog_addr+rtd_keylog_pos) = '\n';
        rtd_keylog_pos = rtd_keylog_pos+1;
    }
    mutex_unlock(mtx);
    return 0;
}
EXPORT_SYMBOL(rtdlog_save_keylog);

static int init_rtd_share_buffer (void)
{
    struct SHARE_STRUCT* p_share;
    int i;
    rtkd_share_buffer=kmalloc(PAGE_SIZE, GFP_KERNEL);
    if(NULL == rtkd_share_buffer)
    {
        rtd_pr_rtdlog_err("%s %d. cannot kmalloc share memory for rtkd !\n",__func__,__LINE__);
        return -1;
    }
    memset(rtkd_share_buffer,0,PAGE_SIZE);

    p_share = (struct SHARE_STRUCT*) rtkd_share_buffer;
    p_share->flag_compress = 1;
    p_share->version= RTKD_VERSION;
    p_share->total_file_num = MAX_LOGBUF_NUM;
    //p_share->flag_sleep = 1;
   // strcpy(p_share->default_log_path,RTD_LOGBUF_DEFLAUT_DIR); 
    strcpy(p_share->g_dir_path,logbuf_dir); //FIXME
    rtkd_file_info = &(p_share->file_info);
    for(i = 0; i < MAX_LOGBUF_NUM; ++i) {
            snprintf(rtkd_file_info[i].g_file_name,FILE_NAME_LEN-1,"%s",rtdlog_file_name[i]);
    }
    return 0;
}

/********************************************************
* rtdlog init APIs
*********************************************************/

static void init_rtd_buffer (logbuf_global_status * status)
{
    unsigned long * native_logbuf_saver;
    int i;

    //init rtd log buf
    gpRtdlogTimestamp = &(status->nsec);

    //init rbcb info and sema_addr
    for(i = 0; i < MAX_LOGBUF_NUM; ++i)
    {
        pRtdLogCB[i] = rtdlog_phys_to_virt(status->rbcb_addr[i]);
        array_fifo[i] = &(pRtdLogCB[i]->fifo);
        //dump kfifo
        rtd_pr_rtdlog_err("KFIFO[%d]_INIT:buf(%08lx),size(%08lx),in(%08lx),out(%08lx) (%016lx)\n",
               i,(unsigned long)(array_fifo[i]->buffer),(unsigned long)(array_fifo[i]->size),
               (unsigned long)(array_fifo[i]->in),(unsigned long)(array_fifo[i]->out),(unsigned long)pRtdLogCB[i]);
    }
    pRtdLogCB[KERNEL_BUF]->drop_cnt=0;
    gDbgInfo.log_reset_cnt[KERNEL_BUF]=0;

    //write native log buf addr in param area
    native_logbuf_saver = (unsigned long *)rtdlog_phys_to_virt(RTD_LOGBUF_PHY_START_ADDR+RTD_LOGBUF_DEFAULT_PARAM-8);
#if defined(CONFIG_REALTEK_LOGBUF) && !(LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
    *native_logbuf_saver = (unsigned long)(native_logbuf_addr - PAGE_OFFSET);
    rtd_pr_rtdlog_err("native logbuf addr : 0x%08lx, CONFIG_PAGE_OFFSET : 0x%08lx\n",(unsigned long)native_logbuf_addr,PAGE_OFFSET);
#endif
    share_buffer=(unsigned char *)((void *)rtd_virt_addr+RTD_LOGBUF_BOOT_SHARE_PARAM_OFFSET);
}

extern void save_early_log(unsigned char type);   //move scpu kernel log from linux logbuf to rtd logbuf
#include <linux/pageremap.h>
static int check_version_and_magic(logbuf_global_status * status)
{
        if(status->version != RTD_LOGBUF_VERSION){
                rtd_pr_rtdlog_err("rtd logbuf version mismatch. global version:%d, kernel version:%d",status->version,RTD_LOGBUF_VERSION);
                return -1;
        }
        if((status->magic_seg1 != RTD_LOGBUF_MAGIC_1)||
                (status->magic_seg2 != RTD_LOGBUF_MAGIC_2)||
                (status->magic_seg3 != RTD_LOGBUF_MAGIC_3)||
                (status->magic_seg4 != RTD_LOGBUF_MAGIC_4))
        {
                rtd_pr_rtdlog_err("rtd logbuf magic mismatch. global magic:%08x-%08x-%08x-%08x, kernel magic:%08x-%08x-%08x-%08x",
                status->magic_seg1,status->magic_seg2,status->magic_seg3,status->magic_seg4,
                RTD_LOGBUF_MAGIC_1,RTD_LOGBUF_MAGIC_2,RTD_LOGBUF_MAGIC_3,RTD_LOGBUF_MAGIC_4);
                return -1;
        }

        return 0;
}
#if defined(CONFIG_TRACEPOINTS) && defined(CONFIG_ANDROID_VENDOR_HOOKS) && (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
#include <linux/kmsg_dump.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
    static struct kmsg_dump_iter dumper;
#else
    static struct kmsg_dumper dumper = { .active = 1 };
#endif
void rtd_logbuf_save_early_log ( int level, const char *text, u16 text_len)
{
    rtd_msg_header tmp_header;
    tmp_header.cpu_type = (u8)KERNEL_BUF;
    tmp_header.cpu_core = smp_processor_id();
    tmp_header.module = 0;
    tmp_header.pid = task_tgid_nr(current);
    tmp_header.tid = task_pid_nr(current);
    tmp_header.timestamp = *gpRtdlogTimestamp;
    tmp_header.level = level;
    tmp_header.text_len = text_len;
    tmp_header.len = sizeof(tmp_header) + text_len;
    snprintf(tmp_header.name,sizeof(tmp_header.name),current->comm);
    rtdlog_save_msg(KERNEL_BUF,text, &tmp_header);
}
void save_early_log(unsigned char type)
{
    size_t len;
    unsigned char buf[1024];
    static int num = 0;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
    kmsg_dump_rewind(&dumper);
#endif
    for(;;)
    {
         num++;
        memset(buf,0,sizeof(buf));
        if(!kmsg_dump_get_line(&dumper, false, buf, sizeof(buf)-2, &len))
        {
            rtd_pr_rtdlog_err(" ***************** save_early_log *****%d*****%d*********** \n",num,strlen(buf));
            return;
        }
        if(!strlen(buf)){
            continue;
        }
        rtd_logbuf_save_early_log(0,buf,strlen(buf));
    }
}
#else
extern void save_early_log(unsigned char type);   //move scpu kernel log from linux logbuf to rtd logbuf
#endif
#include <linux/pageremap.h>
#if defined(CONFIG_TRACEPOINTS) && defined(CONFIG_ANDROID_VENDOR_HOOKS) && (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
#include<trace/hooks/logbuf.h>
#include <../kernel/printk/printk_ringbuffer.h>
#include <../kernel/printk/internal.h>
#include <linux/dev_printk.h>

extern struct timezone sys_tz;
void rtk_get_local_time (unsigned int *hour,unsigned int *minute,unsigned int *second,unsigned int* millisecond)
{
    unsigned int local_time;
    unsigned int sec;
    unsigned int msec;
    struct timespec64 t64;
    ktime_get_real_ts64(&t64);
    sec = t64.tv_sec;
    msec = (t64.tv_nsec/1000000);
    local_time = (sec - (sys_tz.tz_minuteswest * 60))%(3600*24);
    *hour=local_time/3600;
    *minute=(local_time%3600)/60;
    *second=local_time%60;
    *millisecond=msec;
    return;
}

#define MAX_TEXT_LEN 1024
static void rtk_logbuf(void *arg,struct printk_ringbuffer *rb, struct printk_record *r)
{
    u32 extra_len;
    u32 text_len;
    char extra[MAX_TEXT_LEN];
    unsigned int hour=0,minute=0,second=0,millisecond=0;

    if(str_status == 0)// fix suspend flow 先后顺序
    {
        rtk_get_local_time(&hour,&minute,&second,&millisecond);
    }

    extra_len = sprintf(extra, "%02d:%02d:%02d.%03d (%d)-%04d\t", hour,minute,second,millisecond,
                    smp_processor_id(), task_pid_nr(current)); // RTK_patch: print cpu id and task pid

    if((extra_len + r->info->text_len) >= MAX_TEXT_LEN)
        text_len = MAX_TEXT_LEN - extra_len -1;
    else
        text_len = r->info->text_len;

    memcpy(&extra[extra_len], r->text_buf, text_len);
    extra_len = extra_len + text_len;
    extra[extra_len++] = 0;
    rtd_logbuf_save_log(r->info->level ,extra, extra_len);
}

static void rtk_logbuf_pr_cont(void *arg,struct printk_record *r, size_t text_len)
{ 
    if(r->info->flags & LOG_NEWLINE)
        rtk_logbuf(arg,NULL,r);
}
#endif

static void check_rtklog_module(void)
{
    unsigned int module_id = 0;
    for(module_id=0;module_id < (sizeof(module_map)/sizeof(struct MODULE_MAP));module_id++)
    {
        if(module_id != module_map[module_id].module_id)
            rtd_pr_rtdlog_err("module %s id is error id=%d module_id=%d",module_map[module_id].name,module_id,module_map[module_id].module_id);
    }
        
    return;
}

int rtdlog_setup (void)
{
#if (IS_ENABLED(CONFIG_RTK_KDRV_TEE) && !defined(CONFIG_CUSTOMER_TV006))
    struct optee_rpc_param rtc_set_info = {0};
#endif

    logbuf_global_status * global_status ;
    if( 0 == rtdlog_bufsize )
    {
        return -1;
    }

    rtd_virt_addr = (unsigned long)dvr_remap_uncached_memory(RTD_LOGBUF_PHY_START_ADDR, rtdlog_get_buffer_size(), __builtin_return_address(0));
    if(0 == rtd_virt_addr)
    {
        return -2;
    }

    global_status = (logbuf_global_status *)(rtdlog_phys_to_virt(RTD_LOGBUF_PHY_START_ADDR));
    //logbuf_cmdline * boot_param = (logbuf_cmdline *)(phys_to_virt(RTD_LOGBUF_PHY_START_ADDR + RTD_LOGBUF_CMDLINE_OFFSET));


    if(check_version_and_magic(global_status))
    {
        return -3;
    }
#if 0
    /* init logswi, logifle, logdir,  logfilter*/
    if(rtd_get_boot_param(boot_param))
    {
        return;
    }
#endif

#if 0
    if(check_bufsize(global_status))
    {
        return;
    }
#endif
    if(init_rtd_share_buffer())
    {
        return -4;
    }
#if (IS_ENABLED(CONFIG_RTK_KDRV_TEE) && !defined(CONFIG_CUSTOMER_TV006))
    optee_logbuf_setup_info_send(&rtc_set_info);
#endif
    check_rtklog_module();
    init_rtd_buffer(global_status);
    //setup filter
    //logfilter_setup();
    //catfilter_setup();

    //now rtd logbuf can be used
    is_setup_rtd_logbuf = true;
#if defined(CONFIG_REALTEK_LOGBUF) && !(LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
    save_early_log(KERNEL_BUF);       //parse&save early log in linux logbuf before now time
#endif
#if defined(CONFIG_TRACEPOINTS) && defined(CONFIG_ANDROID_VENDOR_HOOKS) && (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
    save_early_log(KERNEL_BUF);
    register_trace_android_vh_logbuf(rtk_logbuf,NULL);
    register_trace_android_vh_logbuf_pr_cont(rtk_logbuf_pr_cont,NULL);//printk.c vprintk_store
#endif
    is_save_early_log_done = true;

    // init keylog buffer
    //init_rtd_keylog_buf();
    return 0;
}

static int rtdlog_pm_suspend ( struct platform_device *dev, pm_message_t state )
{
    return 0;
}
static int rtdlog_pm_resume (struct platform_device *dev)
{
    return 0;
}

static int rtdlog_panic (struct notifier_block *this, unsigned long ev, void *ptr);
static struct notifier_block rtdlog_panic_block =
{
    .notifier_call = rtdlog_panic,
};


struct file_operations rtdlog_fops =
{
    .owner          = THIS_MODULE,

#ifdef CONFIG_ARM64
    .compat_ioctl = rtdlog_ioctl,
#else
    .unlocked_ioctl = rtdlog_ioctl,
#endif

    .open           = rtdlog_open,
    .release        = rtdlog_release,
    .read          = rtdlog_read,
    .write          = rtdlog_write,
    .mmap          = rtdlog_mmap,
};

int rtdlog_major = RTD_LOGBUF_DEV_MAJOR;    //rtdlog device node major
int rtdlog_minor = RTD_LOGBUF_DEV_MINOR;    //rtdlog device node minor
static struct class *rtdlog_class;
static struct rtdlog_dev *rtdlog_drv_dev;               /* allocated in rtdlog_init_module */

static struct platform_device *rtdlog_platform_devs;
static struct platform_driver rtdlog_device_driver =
{
    .suspend    = rtdlog_pm_suspend,
    .resume     = rtdlog_pm_resume,
    .driver = {
        .name       = "rtd_logger",
        .bus        = &platform_bus_type,
    } ,
} ;

#ifdef SUPPORT_LOGBUF_FILE_OPERATIONS //Support logbuf file operation
#include <linux/miscdevice.h>
static ssize_t kfifo_out_peek_copy ( char  __user * buf,int type, struct kfifo *fifo, rtd_msg_header* header,unsigned char *buffer)
{
    unsigned int out_len;
    int pos;
    ssize_t ret=0;
    if(buffer == NULL)
        return 0;
    if((header->text_len) > 1536)
    {
        rtd_pr_rtdlog_err("%s %d.  drop type %s's log\n",__func__,__LINE__,cpu_type_str[type]);
        __kfifo_add_out(fifo, header->text_len);
        return 0;
    }
    pos=rtd_build_msg_header(header,buffer);
    out_len=kfifo_out_peek(fifo,buffer+pos,header->text_len,0);
    __kfifo_add_out(fifo, out_len);
    if(buffer[pos+out_len-1]!='\n')
    {
        buffer[pos+out_len]='\n';
        out_len++;
    }
    buffer[pos+out_len]=0;
    ret = pos+out_len;

    if(copy_to_user(buf, (void *)buffer, ret ))
    {
        rtd_pr_rtdlog_err("%s %d.  copy_to_user  %d err\n",__func__,__LINE__,ret);
        return 0;
    }

    if( ((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->flag_contain_av_log && ((ACPU1_BUF==type)||(VCPU1_BUF==type)) )
    {
        buffer[pos+out_len]=0;
        rtd_pr_rtdlog_err("%s",buffer);
    }
    return ret;
}
ssize_t rtd_copy_msg_data ( char __user * buf,int type, rtd_msg_header* header, struct kfifo * fifo, unsigned char *buffer)
{
    __kfifo_add_out(fifo, sizeof(rtd_msg_header));
    return kfifo_out_peek_copy(buf,type, fifo,header,buffer);
}
ssize_t rtkd_copy_log_to_user (int type, char __user * buf, size_t count,unsigned char *buffer)
{
    rtd_msg_header header;
    int header_check;
    ssize_t ret = 0;
    header_check = get_msg_header(type,&header, array_fifo[type]);
    if(header_check==0)
    {
       return ret;
    }
    else if (header_check==-1)
    {
        rtd_pr_rtdlog_err("%s %d. error happen! insert miss log msg for type:%d\n",__func__,__LINE__,type);
        return ret;
    }
    if((header.len+100) > count)
        return ret;
    ret=rtd_copy_msg_data(buf,type, &header, array_fifo[type],buffer);
    return ret;
}
ssize_t rtkd_copy_to_user (int type, char __user * buf, size_t count, unsigned char *buffer)
{
    int data_len = 0;
    ssize_t len = 0;
    struct mutex * mtx = &read_loop_mutex;
    if(!is_setup_rtd_logbuf)
        return 0;
    mutex_lock(mtx);
    do
    {
        len=rtkd_copy_log_to_user(type, buf+data_len, count-data_len, buffer);
        if(len == 0)
            break;
        data_len += len;
    }while(count > data_len);
    mutex_unlock(mtx);
    return data_len;
}
ssize_t rtk_kernellog_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    ssize_t len = 0;

    if(kernel_cpu_log_buffer == NULL)
        kernel_cpu_log_buffer = kmalloc(2048,GFP_KERNEL);
    if(kernel_cpu_log_buffer == NULL)
        return len;
    mutex_lock(&save_kernel_mutex);
    len=rtkd_copy_to_user(KERNEL_BUF, buf, count,kernel_cpu_log_buffer);
    mutex_unlock(&save_kernel_mutex);
    return len;
}
int rtk_kernellog_release(struct inode *inode, struct file *file)
{
    rtd_pr_rtdlog_info("%s(%d) Release\n",__func__,__LINE__);
    return 0;
}
int rtk_kernellog_open(struct inode *inode, struct file *file)
{
    rtd_pr_rtdlog_info("%s(%d) Open\n",__func__,__LINE__);
    return 0;
}
long rtk_kernellog_ioctl(struct file *file, unsigned int cmd,
                 unsigned long arg)
{
    rtd_pr_rtdlog_info("%s(%d) \n",__func__,__LINE__);
    return 0;
}
struct file_operations rtk_kernellog_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = rtk_kernellog_ioctl,
    .open = rtk_kernellog_open,
    .read = rtk_kernellog_read,
    .release = rtk_kernellog_release,
};
static struct miscdevice rtk_kernellog_miscdev = {
    MISC_DYNAMIC_MINOR,
    "rtd_log_kernel",
    &rtk_kernellog_fops
};
ssize_t rtk_acpu1log_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    ssize_t len = 0;

    if(acpu1_cpu_log_buffer == NULL)
        acpu1_cpu_log_buffer = kmalloc(2048,GFP_KERNEL);
    if(acpu1_cpu_log_buffer == NULL)
        return len;

    mutex_lock(&save_acpu1_mutex);
    len=rtkd_copy_to_user(ACPU1_BUF, buf, count,acpu1_cpu_log_buffer);
    mutex_unlock(&save_acpu1_mutex);
    return len;
}
int rtk_acpu1log_release(struct inode *inode, struct file *file)
{
    rtd_pr_rtdlog_info("%s(%d) Release\n",__func__,__LINE__);
    return 0;
}
int rtk_acpu1log_open(struct inode *inode, struct file *file)
{
    rtd_pr_rtdlog_info("%s(%d) Open\n",__func__,__LINE__);
    return 0;
}
long rtk_acpu1log_ioctl(struct file *file, unsigned int cmd,
                 unsigned long arg)
{
    rtd_pr_rtdlog_info("%s(%d) \n",__func__,__LINE__);
    return 0;
}
struct file_operations rtk_acpu1log_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = rtk_acpu1log_ioctl,
    .open = rtk_acpu1log_open,
    .read = rtk_acpu1log_read,
    .release = rtk_acpu1log_release,
};
static struct miscdevice rtk_acpu1log_miscdev = {
    MISC_DYNAMIC_MINOR,
    "rtd_log_acpu1",
    &rtk_acpu1log_fops
};
ssize_t rtk_kcpu2log_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    ssize_t len = 0;

    if(kcpu2_cpu_log_buffer == NULL)
        kcpu2_cpu_log_buffer = kmalloc(2048,GFP_KERNEL);
    if(kcpu2_cpu_log_buffer == NULL)
        return len;

    mutex_lock(&save_kcpu2_mutex);
    len=rtkd_copy_to_user(ACPU2_BUF, buf, count,kcpu2_cpu_log_buffer);
    mutex_unlock(&save_kcpu2_mutex);
    return len;
}
int rtk_kcpu2log_release(struct inode *inode, struct file *file)
{
    rtd_pr_rtdlog_info("%s(%d) Release\n",__func__,__LINE__);
    return 0;
}
int rtk_kcpu2log_open(struct inode *inode, struct file *file)
{
    rtd_pr_rtdlog_info("%s(%d) Open\n",__func__,__LINE__);
    return 0;
}
long rtk_kcpu2log_ioctl(struct file *file, unsigned int cmd,
                 unsigned long arg)
{
    rtd_pr_rtdlog_info("%s(%d) \n",__func__,__LINE__);
    return 0;
}
struct file_operations rtk_kcpu2log_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = rtk_kcpu2log_ioctl,
    .open = rtk_kcpu2log_open,
    .read = rtk_kcpu2log_read,
    .release = rtk_kcpu2log_release,
};
static struct miscdevice rtk_kcpu2log_miscdev = {
    MISC_DYNAMIC_MINOR,
    "rtd_log_atf",
    &rtk_kcpu2log_fops
};
ssize_t rtk_vcpu1log_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    ssize_t len = 0;

    if(vcpu1_cpu_log_buffer == NULL)
        vcpu1_cpu_log_buffer = kmalloc(2048,GFP_KERNEL);
    if(vcpu1_cpu_log_buffer == NULL)
        return len;

    mutex_lock(&save_vcpu1_mutex);
    len=rtkd_copy_to_user(VCPU1_BUF, buf, count,vcpu1_cpu_log_buffer);
    mutex_unlock(&save_vcpu1_mutex);
    return len;
}
int rtk_vcpu1log_release(struct inode *inode, struct file *file)
{
    rtd_pr_rtdlog_info("%s(%d) Release\n",__func__,__LINE__);
    return 0;
}
int rtk_vcpu1log_open(struct inode *inode, struct file *file)
{
    rtd_pr_rtdlog_info("%s(%d) Open\n",__func__,__LINE__);
    return 0;
}
long rtk_vcpu1log_ioctl(struct file *file, unsigned int cmd,
                 unsigned long arg)
{
    rtd_pr_rtdlog_info("%s(%d) \n",__func__,__LINE__);
    return 0;
}
struct file_operations rtk_vcpu1log_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = rtk_vcpu1log_ioctl,
    .open = rtk_vcpu1log_open,
    .read = rtk_vcpu1log_read,
    .release = rtk_vcpu1log_release,
};
static struct miscdevice rtk_vcpu1log_miscdev = {
    MISC_DYNAMIC_MINOR,
    "rtd_log_vcpu1",
    &rtk_vcpu1log_fops
};
ssize_t rtk_vcpu2log_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    ssize_t len = 0;

    if(vcpu2_cpu_log_buffer == NULL)
        vcpu2_cpu_log_buffer = kmalloc(2048,GFP_KERNEL);
    if(vcpu2_cpu_log_buffer == NULL)
        return len;

    mutex_lock(&save_vcpu2_mutex);
    len=rtkd_copy_to_user(VCPU2_BUF, buf, count,vcpu2_cpu_log_buffer);
    mutex_unlock(&save_vcpu2_mutex);
    return len;
}
int rtk_vcpu2log_release(struct inode *inode, struct file *file)
{
    rtd_pr_rtdlog_info("%s(%d) Release\n",__func__,__LINE__);
    return 0;
}
int rtk_vcpu2log_open(struct inode *inode, struct file *file)
{
    rtd_pr_rtdlog_info("%s(%d) Open\n",__func__,__LINE__);
    return 0;
}
long rtk_vcpu2log_ioctl(struct file *file, unsigned int cmd,
                 unsigned long arg)
{
    rtd_pr_rtdlog_info("%s(%d) \n",__func__,__LINE__);
    return 0;
}
struct file_operations rtk_vcpu2log_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = rtk_vcpu2log_ioctl,
    .open = rtk_vcpu2log_open,
    .read = rtk_vcpu2log_read,
    .release = rtk_vcpu2log_release,
};
static struct miscdevice rtk_vcpu2log_miscdev = {
    MISC_DYNAMIC_MINOR,
    "rtd_log_vcpu2",
    &rtk_vcpu2log_fops
};
ssize_t rtk_kcpulog_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    ssize_t len = 0;

    if(kcpu_cpu_log_buffer == NULL)
        kcpu_cpu_log_buffer = kmalloc(2048,GFP_KERNEL);
    if(kcpu_cpu_log_buffer == NULL)
        return len;

    mutex_lock(&save_kcpu_mutex);
    len=rtkd_copy_to_user(KCPU_BUF, buf, count,kcpu_cpu_log_buffer);
    mutex_unlock(&save_kcpu_mutex);
    return len;
}
int rtk_kcpulog_release(struct inode *inode, struct file *file)
{
    rtd_pr_rtdlog_info("%s(%d) Release\n",__func__,__LINE__);
    return 0;
}
int rtk_kcpulog_open(struct inode *inode, struct file *file)
{
    rtd_pr_rtdlog_info("%s(%d) Open\n",__func__,__LINE__);
    return 0;
}
long rtk_kcpulog_ioctl(struct file *file, unsigned int cmd,
                 unsigned long arg)
{
    rtd_pr_rtdlog_info("%s(%d) \n",__func__,__LINE__);
    return 0;
}
struct file_operations rtk_kcpulog_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = rtk_kcpulog_ioctl,
    .open = rtk_kcpulog_open,
    .read = rtk_kcpulog_read,
    .release = rtk_kcpulog_release,
};
static struct miscdevice rtk_kcpulog_miscdev = {
    MISC_DYNAMIC_MINOR,
    "rtd_log_optee",
    &rtk_kcpulog_fops
};
ssize_t rtk_hallog_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    ssize_t len = 0;

    if(hal_cpu_log_buffer == NULL)
        hal_cpu_log_buffer = kmalloc(2048,GFP_KERNEL);
    if(hal_cpu_log_buffer == NULL)
        return len;

    mutex_lock(&save_hal_read_mutex);
    len=rtkd_copy_to_user(HAL_BUF, buf, count, hal_cpu_log_buffer);
    mutex_unlock(&save_hal_read_mutex);
    return len;
}
int rtk_hallog_release(struct inode *inode, struct file *file)
{
    rtd_pr_rtdlog_info("%s(%d) Release\n",__func__,__LINE__);
    return 0;
}
int rtk_hallog_open(struct inode *inode, struct file *file)
{
    rtd_pr_rtdlog_info("%s(%d) Open\n",__func__,__LINE__);
    return 0;
}
long rtk_hallog_ioctl(struct file *file, unsigned int cmd,
                 unsigned long arg)
{
    rtd_pr_rtdlog_info("%s(%d) \n",__func__,__LINE__);
    return 0;
}
struct file_operations rtk_hallog_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = rtk_hallog_ioctl,
    .open = rtk_hallog_open,
    .read = rtk_hallog_read,
    .release = rtk_hallog_release,
};
static struct miscdevice rtk_hallog_miscdev = {
    MISC_DYNAMIC_MINOR,
    "rtd_log_rhal",
    &rtk_hallog_fops
};

static int paniclog_pos = 0;
ssize_t rtd_copy_panicmsg_data (char __user * buf, size_t count,unsigned char *buffer)
{
    int len;

    if((paniclog_magic == NULL) || (*paniclog_magic != RTD_PANICLOG_MAGIC))
        return 0;

    if(paniclog_pos >= *paniclog_num)
    {
        *paniclog_magic = 0;
        paniclog_pos = -1;

        if(panic_buff != NULL)
        {
            kfree(panic_buff);
            panic_buff = NULL;
            paniclog_magic = NULL;
            paniclog_num = NULL;
            paniclog_text = NULL;
        }

        return 0;
    }

    if((*paniclog_num - paniclog_pos) >= count)
    {
        if(copy_to_user(buf, (void *)(paniclog_text+paniclog_pos), count))
        {
            rtd_pr_rtdlog_err("%s %d.  copy_to_user  %d err\n",__func__,__LINE__,count);
            return 0;
        }
        paniclog_pos += count;
        return count;
    }
    else
    {
        len = *paniclog_num - paniclog_pos;
        if(copy_to_user(buf, (void *)(paniclog_text+paniclog_pos), *paniclog_num - paniclog_pos))
        {
            rtd_pr_rtdlog_err("%s %d.  copy_to_user  %d err\n",__func__,__LINE__,(*paniclog_num - paniclog_pos));
            return 0;
        }
        paniclog_pos = *paniclog_num;
        return len;
    }
    return 0;
}

ssize_t rtkd_copy_paniclog_to_user (char __user * buf, size_t count)
{
    int data_len = 0;
    ssize_t len = 0;
    unsigned char * buffer = NULL;
    struct mutex * mtx = &read_loop_mutex;

    if(!is_setup_rtd_logbuf)
        return 0;

    buffer = msg_tmp_buf;
    mutex_lock(mtx);
    do
    {
        len=rtd_copy_panicmsg_data(buf+data_len, count-data_len, buffer);
        if(len == 0)
            break;
        data_len += len;
    }while(count > data_len);
    mutex_unlock(mtx);

    return data_len;
}

bool is_setup_panic_logbuf = false;
int rtdlog_read_panic_msg_init(void);
ssize_t rtk_paniclog_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    ssize_t len = 0;

    if(is_setup_panic_logbuf == false)
        rtdlog_read_panic_msg_init();

    if((is_setup_panic_logbuf == false) || (paniclog_magic == NULL) || (*paniclog_magic != RTD_PANICLOG_MAGIC))
        return len;

    len=rtkd_copy_paniclog_to_user(buf, count);
    return len;
}

int rtk_paniclog_release(struct inode *inode, struct file *file)
{
    rtd_pr_rtdlog_info("%s(%d) Release\n",__func__,__LINE__);
    return 0;
}
int rtk_paniclog_open(struct inode *inode, struct file *file)
{
    rtd_pr_rtdlog_info("%s(%d) Open\n",__func__,__LINE__);
    return 0;
}
long rtk_paniclog_ioctl(struct file *file, unsigned int cmd,
                 unsigned long arg)
{
    rtd_pr_rtdlog_info("%s(%d) \n",__func__,__LINE__);
    return 0;
}
struct file_operations rtk_paniclog_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = rtk_paniclog_ioctl,
    .open = rtk_paniclog_open,
    .read = rtk_paniclog_read,
    .release = rtk_paniclog_release,
};
static struct miscdevice rtk_paniclog_miscdev = {
    MISC_DYNAMIC_MINOR,
    "rtd_log_panic",
    &rtk_paniclog_fops
};


static void init_logbuf_file_dev(void)
{
    if (misc_register(&rtk_kernellog_miscdev)) {
        rtd_pr_rtdlog_err("rtk_kernellog_module_init failed - register misc device failed\n");
    }
    if (misc_register(&rtk_acpu1log_miscdev)) {
        rtd_pr_rtdlog_err("rtk_acpu1llog_module_init failed - register misc device failed\n");
    }
    if (misc_register(&rtk_kcpu2log_miscdev)) {
        rtd_pr_rtdlog_err("rtk_kcpu2llog_module_init failed - register misc device failed\n");
    }
    if (misc_register(&rtk_vcpu1log_miscdev)) {
        rtd_pr_rtdlog_err("rtk_vcpu1llog_module_init failed - register misc device failed\n");
    }
    if (misc_register(&rtk_vcpu2log_miscdev)) {
        rtd_pr_rtdlog_err("rtk_vcpu2llog_module_init failed - register misc device failed\n");
    }
    if (misc_register(&rtk_kcpulog_miscdev)) {
        rtd_pr_rtdlog_err("rtk_kcpullog_module_init failed - register misc device failed\n");
    }
    if (misc_register(&rtk_hallog_miscdev)) {
        rtd_pr_rtdlog_err("rtk_halllog_module_init failed - register misc device failed\n");
    }
    if (misc_register(&rtk_paniclog_miscdev)) {
        rtd_pr_rtdlog_err("rtk_panicllog_module_init failed - register misc device failed\n");
    }
    return;
}
static void free_logbuf_buffer(unsigned char *buffer)
{
    if(buffer != NULL)
    {
        kfree(buffer);
        buffer = NULL;
    }
    return;
}
static void release_logbuf_file_dev(void)
{
    misc_deregister(&rtk_kernellog_miscdev);
    misc_deregister(&rtk_acpu1log_miscdev);
    misc_deregister(&rtk_kcpu2log_miscdev);
    misc_deregister(&rtk_vcpu1log_miscdev);
    misc_deregister(&rtk_vcpu2log_miscdev);
    misc_deregister(&rtk_kcpulog_miscdev);
    misc_deregister(&rtk_hallog_miscdev);
    misc_deregister(&rtk_paniclog_miscdev);
    free_logbuf_buffer(kernel_cpu_log_buffer);
    free_logbuf_buffer(acpu1_cpu_log_buffer);
    free_logbuf_buffer(kcpu2_cpu_log_buffer);
    free_logbuf_buffer(vcpu1_cpu_log_buffer);
    free_logbuf_buffer(vcpu2_cpu_log_buffer);
    free_logbuf_buffer(kcpu_cpu_log_buffer);
    free_logbuf_buffer(hal_cpu_log_buffer);
    return;
}
#endif
static int __init rtdlog_init_module (void)
{
    int result;
    dev_t dev = 0;

    rtd_pr_rtdlog_warn(" ***************** rtdlog init module ********************* \n");
#if defined(CONFIG_REALTEK_LOGBUF_MODULE) || (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
    init_logbuffer_timer();
    rtdlog_parse_bufsize_init();
#endif

    if(rtdlog_setup())
    {
        rtd_pr_rtdlog_err("%s %d. rtdlog_setup fail!\n",__func__,__LINE__);
        return -1;
    }
    /*
     * Get a range of minor numbers to work with,
     * asking for a dynamic major unless directed otherwise at load time.
     */

    if (rtdlog_major)
    {
        dev = MKDEV(rtdlog_major, rtdlog_minor);
        result = register_chrdev_region(dev, 1, RTDLOG_NODE);
    }
    else
    {
        result = alloc_chrdev_region(&dev, rtdlog_minor, 1, RTDLOG_NODE);
        rtdlog_major = MAJOR(dev);
    }
    if (result < 0)
    {
        rtd_pr_rtdlog_err("rtdlog: can't get major %d\n", rtdlog_major);
        return result;
    }

    rtd_pr_rtdlog_err("rtdlog:get result:%d,get dev:0x%08x,major:%d\n", result, dev, rtdlog_major);

    rtdlog_class = class_create(THIS_MODULE, RTDLOG_NODE);
    if (IS_ERR_OR_NULL(rtdlog_class))
    {
        return PTR_ERR(rtdlog_class);
    }

    device_create(rtdlog_class, NULL, dev, NULL, RTDLOG_NODE);

    /*
     * allocate the devices
     */
    rtdlog_drv_dev = kmalloc(sizeof(struct rtdlog_dev), GFP_KERNEL);
    if (!rtdlog_drv_dev)
    {
        device_destroy(rtdlog_class, dev);
        result = -ENOMEM;
        goto fail;  /* Make this more graceful */
    }
    memset(rtdlog_drv_dev, 0, sizeof(struct rtdlog_dev));

    //initialize device structure
    sema_init(&rtdlog_drv_dev->sem, 1);
    cdev_init(&rtdlog_drv_dev->cdev, &rtdlog_fops);
    rtdlog_drv_dev->cdev.owner = THIS_MODULE;
    rtdlog_drv_dev->cdev.ops = &rtdlog_fops;
    result = cdev_add(&rtdlog_drv_dev->cdev, dev, 1);
    /* Fail gracefully if need be */
    if (result)
    {
        device_destroy(rtdlog_class, dev);
        kfree(rtdlog_drv_dev);
        rtd_pr_rtdlog_err("Error %d adding cdev rtdlog", result);
        goto fail;
    }

#ifdef CONFIG_PM
    rtdlog_platform_devs = platform_device_register_simple(RTDLOG_NODE, -1, NULL, 0);
    if(platform_driver_register(&rtdlog_device_driver) != 0)
    {
        device_destroy(rtdlog_class, dev);
        cdev_del(&rtdlog_drv_dev->cdev);
        kfree(rtdlog_drv_dev);
        rtdlog_platform_devs = NULL;
        goto fail;  /* Make this more graceful */
    }
#endif  //CONFIG_PM

#ifdef SUPPORT_LOGBUF_FILE_OPERATIONS
    init_logbuf_file_dev();
 #endif
    atomic_notifier_chain_register(&panic_notifier_list, &rtdlog_panic_block);
#if defined(CONFIG_REALTEK_LOGBUF_MODULE) || (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
#if !(defined(CONFIG_TRACEPOINTS) && defined(CONFIG_ANDROID_VENDOR_HOOKS))
    kthread_run(thread_logbuf_reader, NULL, "RtdLogReader");
#endif
#endif

#if 0
    /************init work quene for watchdog dump log**************/
    p_dumpqueue = create_workqueue("rtdlog dump queue");
    //p_dumpqueue = create_singlethread_workqueue("rtdlog dump queue");
    if(!p_dumpqueue)
    {
        rtd_pr_rtdlog_err("Error create dump queue worker fail!!!\n");
        goto fail;
    }

    INIT_WORK(&(DumpWork_cpu[ACPU1_BUF].work), rtdlog_watchdog_dump);
    INIT_WORK(&(DumpWork_cpu[ACPU2_BUF].work), rtdlog_watchdog_dump);
    INIT_WORK(&(DumpWork_cpu[VCPU1_BUF].work), rtdlog_watchdog_dump);
    INIT_WORK(&(DumpWork_cpu[VCPU2_BUF].work), rtdlog_watchdog_dump);
    INIT_WORK(&(DumpWork_cpu[KCPU_BUF].work), rtdlog_watchdog_dump);
#endif
    return 0; /* succeed */

fail:
    return result;
}

static void __exit rtdlog_cleanup_module (void)
{
    int i = 0;
    dev_t dev = MKDEV(rtdlog_major, rtdlog_minor);

    rtd_pr_rtdlog_warn("rtdlog clean module\n");
    if(rtkd_share_buffer)
    {
        kfree(rtkd_share_buffer);
        rtkd_share_buffer=NULL;
    }

    /* Get rid of our char dev entries */
    if (rtdlog_drv_dev)
    {
        device_destroy(rtdlog_class, dev);
        cdev_del(&rtdlog_drv_dev[i].cdev);
        kfree(rtdlog_drv_dev);
#ifdef CONFIG_PM
        platform_device_unregister(rtdlog_platform_devs);
#endif
    }
#ifdef SUPPORT_LOGBUF_FILE_OPERATIONS
    release_logbuf_file_dev();
#endif
    unregister_chrdev_region(dev, 1);
}

module_init(rtdlog_init_module);
module_exit(rtdlog_cleanup_module);

#include <linux/console.h>
int get_msg_header_avk_epc (EnumLogbufType type,rtd_msg_header* header,struct kfifo * pKfifo)
{
    struct kfifo * fifo = (pKfifo == NULL) ? array_fifo[type]: pKfifo;
    if (kfifo_len(fifo)> sizeof(rtd_msg_header))
    {
        kfifo_out_peek(fifo,header,sizeof(rtd_msg_header),0);

        if(check_msghead_valid(header,false)) // if fail
        {
            rtd_pr_rtdlog_err("\n fifo out=0x%x, fifo in=0x%x\n",(unsigned int)fifo->out,  (unsigned int)fifo->in);
            kfifo_drop(fifo);
            gDbgInfo.log_reset_cnt[type]++;
            rtd_pr_rtdlog_err("type[%d] check msg header fail!\n",type);
            return -1;
        }
        else
        {
            return 1;
        }
    }

    return 0;
}

static inline void __kfifo_set_out (struct kfifo *fifo,unsigned int value)
{
    smp_mb();
    fifo->out = value;
}

int rtdlog_find_header (struct kfifo * fifo, EnumLogbufType type)
{
    unsigned int offset=0;
    unsigned int find_pos = 0;
    //unsigned char * base = fifo->buffer;
    //unsigned int head_pos = 0;
    //rtd_msg_header tmp;
    //rtd_msg_header * pHeader = &tmp;
    unsigned int fifo_back_len = 0x4000;
    char * tmp_addr = NULL;
    unsigned int dump_start_pos = 0;

    if(type == ACPU2_BUF)
    {
        fifo_back_len = 0x2000;
    }
    else if(type == ACPU1_BUF)
    {
        fifo_back_len = 0x8000;
    }
    else if(type == KERNEL_BUF)
    {
        fifo_back_len = 0x20000;
    }
    if(fifo->in < fifo_back_len)
    {
        fifo_back_len = fifo->in;
    }
    dump_start_pos = fifo->in - fifo_back_len;
    __kfifo_set_out(fifo,dump_start_pos);

    find_pos = fifo->out;

    offset = find_pos%fifo->size;
    tmp_addr = rtdlog_phys_to_virt((unsigned long)fifo->buffer) + offset;

    while(find_pos != (fifo->in))
    {
        if(check_msghead_valid((rtd_msg_header *)tmp_addr,false))
        {
            find_pos++;
            tmp_addr++;
        }
        else
        {
            __kfifo_set_out(fifo,find_pos);
            return 0;
        }
    }
    return -1;
}

#include <rbus/emmc_wrap_reg.h>
#define EMMC_BLOCK_SIZE       (PAGE_SIZE / 8)
#define PAINC_LOG_START_ADDR     0x6A02000
#define PAINC_LOG_EMMC_SIZE (0x80000)
#define PAINC_LOG_TEXT_LEN (0x80000-2*sizeof(int))
#define PAINC_LOG_START_BLOCK     (PAINC_LOG_START_ADDR/EMMC_BLOCK_SIZE)
#define PAINC_LOG_BLOCK_SIZE (0x80000/EMMC_BLOCK_SIZE)
extern s32 export_raw_dw_em_write(u8 *buffer, u64 address, u32 size);
extern int mmc_fast_read(unsigned int address, unsigned int size, unsigned char *buffer);
s32 rtdlog_emmc_write(u8 *buffer, u64 address, u32 size)
{
#ifndef CONFIG_ARCH_RTK2819A
    // set emmc_scpu_sel bit[0]: scpu_sel -> 0, make sure we can access emmc
    rtd_clearbits(EMMC_WRAP_emmc_scpu_sel_reg, EMMC_WRAP_emmc_scpu_sel_scpu_sel_mask);
    return export_raw_dw_em_write(buffer,address,size);
#else
    return 0;
#endif
}

int rtdlog_emmc_read(unsigned int address, unsigned int size, unsigned char *buffer)
{
#ifndef CONFIG_ARCH_RTK2819A
        return mmc_fast_read(address,size,buffer);
#else
        return 0;
#endif

}

int rtdlog_save_panic_msg_to_buffer ( const char * text, u32 len)
{
    if(panic_buff == NULL || paniclog_text == NULL)
        return -1;

    if((paniclog_pos + len) >= PAINC_LOG_EMMC_SIZE)
        paniclog_pos = 0;

    memcpy(paniclog_text+paniclog_pos,text,len);
    paniclog_pos += len;

    return 0;
}

int rtdlog_save_panic_msg_init (void)
{
    paniclog_pos = 0;
    panic_buff = kmalloc(PAINC_LOG_EMMC_SIZE, __GFP_HIGH | __GFP_DMA | __GFP_ZERO);

    if(panic_buff == NULL)
        return -1;

    paniclog_magic = (int *)panic_buff;
    paniclog_num = (int *)(panic_buff+sizeof(int));
    paniclog_text = (char *)(panic_buff+2*sizeof(int));
    *paniclog_magic = RTD_PANICLOG_MAGIC;

    return 0;
}

int rtdlog_save_panic_msg_to_emmc(void)
{
    int ret=0;

    if(panic_buff == NULL)
        return -1;

    *paniclog_num = paniclog_pos;
    ret = rtdlog_emmc_write(panic_buff,PAINC_LOG_START_ADDR,PAINC_LOG_EMMC_SIZE);

    paniclog_magic = (int *)panic_buff;
    paniclog_num = (int *)(panic_buff+sizeof(int));
    paniclog_text = (char *)(panic_buff+2*sizeof(int));
    rtd_pr_rtdlog_err(" write paniclog_magic=0x%08x paniclog_num=0x%08x",*paniclog_magic,*paniclog_num);
    rtd_pr_rtdlog_err("PAINC_LOG_START_ADDR=0x%08x PAINC_LOG_EMMC_SIZE=0x%08x panic_buff=x%08x ret=%d",PAINC_LOG_START_ADDR,PAINC_LOG_EMMC_SIZE,panic_buff,ret);
    rtd_pr_rtdlog_err("write PAINC_LOG_START_BLOCK=0x%08x PAINC_LOG_BLOCK_SIZE=0x%08x panic_buff=x%08x ret=%d",PAINC_LOG_START_BLOCK,PAINC_LOG_BLOCK_SIZE,panic_buff,ret);

    kfree(panic_buff);
    panic_buff = NULL;
    return 0;
}

int rtdlog_read_panic_msg_from_emmc(void)
{
    return rtdlog_emmc_read(PAINC_LOG_START_BLOCK,PAINC_LOG_BLOCK_SIZE,panic_buff);
}

int rtdlog_read_panic_msg_init(void)
{
    int ret=0;

    panic_buff = kmalloc(PAINC_LOG_EMMC_SIZE, __GFP_HIGH | __GFP_DMA | __GFP_ZERO);
    if(panic_buff == NULL)
        return 0;;

    ret = rtdlog_read_panic_msg_from_emmc();

    paniclog_magic = (int *)panic_buff;
    paniclog_num = (int *)(panic_buff+sizeof(int));
    paniclog_text = (char *)(panic_buff+2*sizeof(int));
    rtd_pr_rtdlog_err("paniclog_magic=0x%08x paniclog_num=0x%08x ",*paniclog_magic,*paniclog_num);
    rtd_pr_rtdlog_err("PAINC_LOG_START_ADDR=0x%08x PAINC_LOG_EMMC_SIZE=0x%08x panic_buff=x%08x ret=%d",PAINC_LOG_START_BLOCK,PAINC_LOG_BLOCK_SIZE,panic_buff,ret);
    is_setup_panic_logbuf = true;
    return 0;
}

int rtdlog_save_panic_msg_start(int type)
{
    int log_tmp_len=0;

    log_tmp_len = sprintf(msg_tmp_buf, "------kernel panic  trigger dump [panic] logbuf START------\n\n");
    rtdlog_save_panic_msg_to_buffer(msg_tmp_buf,log_tmp_len);

    return 0;
}

int rtdlog_save_panic_msg_end(int type)
{
    int log_tmp_len=0;

    log_tmp_len = sprintf(msg_tmp_buf, "\n\n------kernel panic  trigger dump [panic] logbuf END------\n\n");
    rtdlog_save_panic_msg_to_buffer(msg_tmp_buf,log_tmp_len);

    return 0;
}

void rtdlog_crash_dump (EnumLogbufType type)
{
    struct kfifo * pKfifo = NULL;
    rtd_msg_header header;
    unsigned int out_len, out_tmp_pos,pos;
    int header_check = 0;
    unsigned char * buffer;
    unsigned int log_max_len = 0x20000;
    unsigned int log_tmp_len = 0;
    bool tmp_printk_log = false;
    struct mutex * mtx = &read_loop_mutex;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
    struct __kernel_old_timeval log_time;
#else
    struct timeval log_time;
#endif
    if(type > KERNEL_BUF || (!is_setup_rtd_logbuf))
    {
        return;
    }

    tmp_printk_log = is_save_early_log_done;
    is_save_early_log_done = false;

    pr_err("\n\n\n\n\n\n------kernel panic  trigger dump [panic] logbuf START------\n\n");
    mutex_lock(mtx);        // forbidden read thread verify kfifo.out
    pKfifo = array_fifo[type];
    //decreas fifo->out value & find first header addr
    pr_err("before decrease Kfifo out=0x%08x, Kfifo in=0x%08x\n",(unsigned int)(pKfifo->out), (unsigned int)(pKfifo->in));
    if(rtdlog_find_header(pKfifo, type)<0)
    {
        if(pKfifo->in == 0)
        {
            goto RET;
        }
        pr_err("find header fail\n");
        pr_err("pKfifo->out=0x%08x, pKfifo->in=0x%08x\n",(unsigned int)(pKfifo->out), (unsigned int)(pKfifo->in));
        goto RET;
    }
    out_tmp_pos = pKfifo->out;
    pr_err("after decrease Kfifo out=0x%08x, Kfifo in=0x%08x\n",(unsigned int)(pKfifo->out), (unsigned int)(pKfifo->in));

    while((pKfifo->out) != (pKfifo->in))
    {
        rtd_outl(WDOG_TCWTR_reg, 0x01);
        //get msg header
        header_check = get_msg_header_avk_epc(type,&header,pKfifo);
        if (header_check == 1)
        {
            if(header.len < sizeof(msg_tmp_buf))
            {
                buffer = msg_tmp_buf;
                memset(buffer,0,sizeof(msg_tmp_buf));
            }
            else
            {
                buffer = kmalloc(header.len,GFP_KERNEL);
                if (buffer==NULL)
                {
                    goto RET ;
                }

                memset(buffer,0,header.len);
            }
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
            log_time = ns_to_kernel_old_timeval((s64)header.timestamp);
#else
            log_time = ns_to_timeval((s64)header.timestamp);
#endif
            __kfifo_add_out(pKfifo, sizeof(rtd_msg_header));

            pos = sprintf(buffer, "[%s] [%08d.%06d] ",rtkd_file_info[type].g_file_name, (int)log_time.tv_sec, (int)log_time.tv_usec);
            out_len = kfifo_out_peek(pKfifo,buffer+pos,header.text_len,0);
            //print log
            pr_err( "%s",buffer);
            __kfifo_add_out(pKfifo, out_len);            //if write success,move out offset to next msg
            if(buffer[out_len+pos-1] == 0)
            {
                buffer[out_len+pos-1] = '\n';
            }

            rtdlog_save_panic_msg_to_buffer(buffer,out_len+pos);

            if((buffer != msg_tmp_buf)&&(buffer != NULL))//free malloced buffer
            {
                kfree(buffer);
            }

            log_tmp_len = log_tmp_len + out_len + sizeof(rtd_msg_header);
            if(log_tmp_len > log_max_len)   //beyond log length
            {
                break;
            }

        }
        else
        {
            pr_err("Kfifo out=%d, Kfifo in=%d\n",(unsigned int)(pKfifo->out), (unsigned int)(pKfifo->in));
            break;
        }
    }
    __kfifo_set_out(pKfifo, out_tmp_pos);

RET:
    pr_err("\n\n------dump [%s] logbuf FINISH------!!!\n\n\n\n\n",rtkd_file_info[type].g_file_name);
    mutex_unlock(mtx);

    is_save_early_log_done = tmp_printk_log;
    return;
}

static int rtdlog_panic (struct notifier_block *this, unsigned long ev, void *ptr)
{
    int rtdlog_panic_cpu_type = ACPU1_BUF;

    if(!is_setup_rtd_logbuf)        //panic before logbuf set
    {
        return NOTIFY_DONE;
    }

    rtdlog_save_panic_msg_init();

    for(rtdlog_panic_cpu_type = 0; rtdlog_panic_cpu_type<=KERNEL_BUF; rtdlog_panic_cpu_type++)
    {
#ifdef CONFIG_REALTEK_LOGBUF
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
        console_flush_on_panic(CONSOLE_FLUSH_PENDING);       //dump native logbuf to console
#else
        console_flush_on_panic();
#endif
#endif
        rtdlog_save_panic_msg_start(rtdlog_panic_cpu_type);
        rtdlog_crash_dump(rtdlog_panic_cpu_type);
        rtdlog_save_panic_msg_end(rtdlog_panic_cpu_type);
    }

    rtdlog_save_panic_msg_to_emmc();
    //rtdlog_show_history_keylog(rtd_keylog_addr, RTD_LOGBUF_KEYLOG_SIZE);
    return NOTIFY_DONE;
}

/********************************************************
* rtdlog status APIs
*********************************************************/
ssize_t rtdlog_get (char *buffer)
{
    EnumLogbufType type;
    ssize_t pos=0;
    for(type = 0; type < MAX_LOGBUF_NUM; ++type)
    {
        pos+=snprintf(buffer+pos, PAGE_SIZE-1-pos, "\t %s abnormal cnt(drop %08lx,reset %08x)   buf(%08lx),size(%08lx),in(%08lx),out(%08lx)\n",
                      cpu_type_str[type],(unsigned long)(pRtdLogCB[type]->drop_cnt),gDbgInfo.log_reset_cnt[type],
                      (unsigned long)(array_fifo[type]->buffer),(unsigned long)(array_fifo[type]->size), (unsigned long)(array_fifo[type]->in),(unsigned long)(array_fifo[type]->out));
    }

    pos+=snprintf(buffer+pos, PAGE_SIZE-1-pos, "\n");

    pos+=snprintf(buffer+pos, PAGE_SIZE-1-pos, "\t rtdlog flag_status is %d\n",((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->flag_status);
    pos+=snprintf(buffer+pos, PAGE_SIZE-1-pos, "\t rtdlog default_log_path is %s\n",((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->default_log_path);
    pos+=snprintf(buffer+pos, PAGE_SIZE-1-pos, "\t rtdlog current_log_path is %s\n",((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->current_log_path);
    pos+=snprintf(buffer+pos, PAGE_SIZE-1-pos, "\t rtdlog usb_path is %s\n",((struct SHARE_STRUCT*) rtkd_share_buffer)->usb_path);
    pos+=snprintf(buffer+pos, PAGE_SIZE-1-pos, "\t rtdlog total_bufsize is %x\n",((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->total_bufsize);

    pos+=snprintf(buffer+pos, PAGE_SIZE-1-pos, "\t rtdlog flag_sleep is %d\n",((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->flag_sleep);
    pos+=snprintf(buffer+pos, PAGE_SIZE-1-pos, "\t rtdlog flag_filter is %d\n",((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->flag_filter);
    pos+=snprintf(buffer+pos, PAGE_SIZE-1-pos, "\t rtdlog flag_compress is %d\n",((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->flag_compress);
    pos+=snprintf(buffer+pos, PAGE_SIZE-1-pos, "\t rtdlog flag_contain_av_log is %d\n",((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->flag_contain_av_log);

    for(type = 0; type < MAX_LOGBUF_NUM; ++type)
    {
        pos+=snprintf(buffer+pos, PAGE_SIZE-1-pos, "\t rtdlog flag_filter_in[%s] is %d, flag_filter_out[%d] is %d, max_file_size[%d] is 0x%x\n",
            cpu_type_str[type],((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->flag_filter_in[type],
            type,((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->flag_filter_out[type],
            type,((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->max_file_size[type]);
    }

    return pos;
}
EXPORT_SYMBOL(rtdlog_get);

#define LOG_DUMP_FILE_NAME "rtd_logbuf.bin"
#ifdef SUPPORT_LOGBUF_FILE_OPERATIONS
static void rtdlog_dump_raw_bin (void)
{
   return;
}
#else
static void rtdlog_dump_raw_bin (void)
{
    char dump_log_path[LOGBUF_DIR_MAX_SIZE] = {0};
    struct file * log_file = NULL;
    loff_t pos = 0;
    int write_len;

    snprintf(dump_log_path, LOGBUF_DIR_MAX_SIZE-1, "%s/%s",RTDLOG_DEFAULT_PATH,LOG_DUMP_FILE_NAME);
    log_file = filp_open(dump_log_path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if(IS_ERR(log_file))
    {
        rtd_pr_rtdlog_err("%s %d.  open %s failed\n",__func__,__LINE__,dump_log_path);
        return;
    }

    write_len=kernel_write(log_file, (rtdlog_phys_to_virt(RTD_LOGBUF_PHY_START_ADDR)), rtdlog_bufsize, &pos);
    filp_close(log_file, NULL);
    rtd_pr_rtdlog_err("%s %d.  write_len = %x, actual len =%x \n",__func__,__LINE__,write_len,(unsigned int) rtdlog_bufsize);
}
#endif
void rtdlog_set (const char *buffer)
{
    if(strncmp("dump",buffer,strlen("dump"))==0)
    {
        rtdlog_dump_raw_bin();
    }
    else if(strncmp("suspend",buffer,strlen("suspend"))==0)
    {
        ((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->flag_status=0;
    }
    else if(strncmp("resume",buffer,strlen("resume"))==0)
    {
        ((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->flag_status=1;
        up(&rtdlog_sem);
    }
    else if(strncmp("nosleep",buffer,strlen("nosleep"))==0)
    {
        ((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->flag_sleep=0;
    }
    else if(strncmp("sleep",buffer,strlen("sleep"))==0)
    {
        ((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->flag_sleep=1;
    }
    else if(strncmp("nocompress",buffer,strlen("nocompress"))==0)
    {
        ((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->flag_compress=0;
    }
    else if(strncmp("compress",buffer,strlen("compress"))==0)
    {
        ((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->flag_compress=1;
    }
    else if(strncmp("noav_inside",buffer,strlen("noav_inside"))==0)
    {
        ((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->flag_contain_av_log=0;
    }
    else if(strncmp("av_inside",buffer,strlen("av_inside"))==0)
    {
        ((RTDLOG_BOOT_SHARE_PARAM*) share_buffer)->flag_contain_av_log=1;
    }

    return;
}
EXPORT_SYMBOL(rtdlog_set);

MODULE_LICENSE("GPL");
MODULE_IMPORT_NS(VFS_internal_I_am_really_a_filesystem_and_am_NOT_a_driver);

