/*
 * Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
 * Copyright (c) 2014, Sony Mobile Communications Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*====================================================================
  Base on "linux-4.4.3\drivers\input\misc\pm8941-pwrkey.c" to modify
  ====================================================================*/
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/log2.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/reboot.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/input/sparse-keymap.h>
#include <linux/gpio_keys.h>
#include <linux/version.h>

#include "rtk_kdv_keypad.h"
#include "rtk_kdv_keypad_attr.h"
#include <rtk_lsadc.h>

#ifdef CONFIG_OF
#include <linux/of_irq.h>
#include <linux/of_gpio.h>
#include <linux/of_address.h>
#endif

KEYPAD_INFO rtk_kepad_info[MAX_KEYPAD_CNT];
struct rtk_keypad_host rtk_kp_host;
struct rtk_keypad_data	*g_keypads =NULL;

static DEFINE_MUTEX(keypad_suspend_mutex);
volatile unsigned char g_keypad_suspend_flag = 0;

volatile unsigned int hp_value_count = 0;

void rtk_set_hp_value_count(unsigned int val)
{
    hp_value_count = val;
}
EXPORT_SYMBOL(rtk_set_hp_value_count);

#ifdef CONFIG_OF
static const struct of_device_id of_rtk_keypad_ids[] = {
    { .compatible = "realtek, rtk-keypad" },
    {}
};

MODULE_DEVICE_TABLE(of, of_rtk_keypad_ids);
#endif

int keypad_init(void);
void keypad_exit(void);

static int rtk_keypad_setkeycode(struct input_dev *dev,
                                 const struct input_keymap_entry *ke,
                                 unsigned int *old_keycode)
{
    KP_ALERT("%s(%d)Do nothing\n",__func__,__LINE__);
    return 0;
}

static int rtk_keypad_getkeycode(struct input_dev *dev,
                                 struct input_keymap_entry *ke)
{
    KP_ALERT("%s(%d)Do nothing\n",__func__,__LINE__);
    return 0;
}

static void rtk_init_keypad_host(void)
{
    int id = 0;
    int i;

    for(i = 0; i < PCB_ENUM_MAX && id < MAX_KEYPAD_CNT; i++){
        if(memcmp(pcb_enum_all[i].name, "PIN_KEYPAD", 10) == 0 && (pcb_enum_all[i].value != 0) && ( (pcb_enum_all[i].value & 0xff) != 0xff) ){
            strncpy(rtk_kepad_info[id].pKeyname, pcb_enum_all[i].name, PCB_ENUM_NAME_LEN_MAX-1);
            rtk_kepad_info[id].info = pcb_enum_all[i].value;
            id++;
        }
    }
    rtk_kp_host.keyinfo = rtk_kepad_info;
    rtk_kp_host.key_cnt = id;

    rtk_kp_host.key_mode = UNLOCK_MODE;
}

static int rtk_parse_key_info(struct rtk_keypad_data *keypads)
{
    struct rtk_keypad_host  *kpad_host   = keypads->kpad_host;
    KEYPAD_INFO             *keyinfolist = kpad_host->keyinfo;
    KEYPAD_INFO             *pkeyinfo  = NULL;
    unsigned long long      pininfo;
    PCB_PIN_TYPE_T          pin_type;
    RTK_GPIO_ID             gid;
    int pin_index = 0;
    LSADC_MODE kpad_mode;
    int ret;
    int i;

    for(i=0; i<kpad_host->key_cnt; i++ ){
        pkeyinfo = &keyinfolist[i];
        pininfo  = (pkeyinfo->info);
        pkeyinfo->keycode = GET_KEY_CODE(pininfo);
        set_bit(pkeyinfo->keycode, keypads->input_dev->keybit);

        if(0 == pkeyinfo->keycode){
            KP_ALERT("!!NOTE:keycode = 0 please check bootcode config. keypad name:%s\n", (pkeyinfo->pKeyname));
        }

        pin_type = GET_PIN_TYPE(pininfo);
        pin_index = GET_PIN_INDEX(pininfo);

        if(pin_type == PCB_PIN_TYPE_LSADC){
            kpad_mode = GET_PIN_PARAM1(pininfo);
            pkeyinfo->lsadc_idx = pin_index;
            ret = rtk_lsadc_defualt_enable(pkeyinfo->lsadc_idx, kpad_mode);
            pkeyinfo->keypad_type = LSADC_TYPE;

            KP_INFO("PCB_PIN_TYPE_LSADC; TYPE:0x%x; HITVALUE:0x%x; TOLERANCE:0x%x, pinidx:%d,ret=%d\n",
                    GET_LSADC_TYPE(pininfo),
                    GET_LSADC_HITVALUE(pininfo),
                    GET_LSADC_TOLERANCE(pininfo),
                    pin_index,
                    ret);
        }else if((pin_type == PCB_PIN_TYPE_GPIO) ||(pin_type ==  PCB_PIN_TYPE_ISO_GPIO) ||
                 (pin_type == PCB_PIN_TYPE_GPI) || (pin_type == PCB_PIN_TYPE_ISO_GPI)){

            if(pin_type == PCB_PIN_TYPE_GPIO){
                gid = rtk_gpio_id(MIS_GPIO, pin_index);
            }
            else if(pin_type == PCB_PIN_TYPE_ISO_GPIO){
                gid = rtk_gpio_id(ISO_GPIO, pin_index);
            }
            else if(pin_type == PCB_PIN_TYPE_GPI){
                gid = rtk_gpio_id(MIS_GPI, pin_index);
            }
            else {
                gid = rtk_gpio_id(ISO_GPI, pin_index);
            }
            pkeyinfo->keypad_type = GPIO_TYPE;
            pkeyinfo->gid         = gid;
            pkeyinfo->bActive     = GET_PIN_PARAM2(pininfo);
            rtk_gpio_set_dir(gid, 0);

            KP_INFO("%s; TYPE:0x%x; INVERT:0x%x; INIT_VALUE:0x%x\n",
                    (pin_type == PCB_PIN_TYPE_GPIO)?"PCB_PIN_TYPE_GPIO":"PCB_PIN_TYPE_ISO_GPIO",
                    GET_GPIO_TYPE(pininfo),
                    GET_GPIO_INVERT(pininfo),
                    GET_GPIO_INIT_VALUE(pininfo));
        }else{
            KP_ALERT("Unaccept pin type\n");
        }
    }
    return 0;
}


/************************************************************************
 *
 ************************************************************************/
static void rtk_keypad_handle_work(void);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)) // code for 5.4
static void rtk_keypad_poll_timer(struct timer_list *t)
#else
static void rtk_keypad_poll_timer(unsigned long data)
#endif
{
    struct rtk_keypad_data  *keypads = NULL;
	
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)) // code for 5.4
		keypads = g_keypads;
#else
		keypads   = (struct rtk_keypad_data *)data;
#endif
        //schedule_delayed_work(&keypads->work,0);
	rtk_keypad_handle_work();
}


// static void rtk_keypad_handle_work(struct work_struct *work)
static void rtk_keypad_handle_work(void)
{
    struct rtk_keypad_data  *keypads = NULL;
    struct rtk_keypad_host  *kpad_host;
    struct input_dev        *input_dev;
    KEYPAD_INFO             *keyinfolist;
    KEYPAD_INFO             *pkeyinfo;
    unsigned long long      pininfo;
    unsigned int            keycode;
    int curr_keystatus;
    int i;
    unsigned char lsadc_val;
    unsigned char hit_val;
    unsigned char tol_val;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)) // code for 5.4
    keypads = g_keypads;
#else
    keypads   = (struct rtk_keypad_data *)data;
#endif
    kpad_host = keypads->kpad_host;
    input_dev = keypads->input_dev;
    keyinfolist   = kpad_host->keyinfo;

    if(LOCK_MODE == kpad_host->key_mode){
        goto END;
    }

    if(IO_Direct_Get("PIN_HP_OUT_JD") == 0){
        hp_value_count++;
        if(hp_value_count > 10000)
            hp_value_count = 10;
    }else{
        hp_value_count = 0;
    }

    for(i=0; i < kpad_host->key_cnt; i++ ){
        pkeyinfo = &keyinfolist[i];
        pininfo  = pkeyinfo->info;
        keycode  = pkeyinfo->keycode;

        if(pkeyinfo->keypad_type == LSADC_TYPE){
            hit_val   = GET_LSADC_HITVALUE(pininfo);
            tol_val   = GET_LSADC_TOLERANCE(pininfo);
            rtk_lsadc_get_adcValue(pkeyinfo->lsadc_idx,&lsadc_val);

            #if 0
            KP_ALERT("index=0x%x, hit value:%d adc value:%d, c:%d, l:%d, t:%d\n",
                      pkeyinfo->lsadc_idx, hit_val,lsadc_val,
                      curr_keystatus, pkeyinfo->last_status,
                      tol_val);
            #endif
            if ( (_ABS(lsadc_val, hit_val) < tol_val)){
                curr_keystatus = 1;
            }else{
                curr_keystatus = 0;
            }

            if((strstr(pkeyinfo->pKeyname,"PIN_KEYPAD_HP")!=NULL) && hp_value_count <10)
            {
				continue;
            }


            if((bool)curr_keystatus !=  (pkeyinfo->last_status)){
                KP_INFO("%s,keycode:%d,Key %s Active\n",pkeyinfo->pKeyname, keycode, (curr_keystatus)?"Press":"Release");
                if(!rtk_kp_host.is_test_mode) {
                    input_report_key(input_dev, keycode, curr_keystatus); //1: key pressed;0: key release
                    input_sync(input_dev);
                } else {
                    unsigned int idx = rtk_kp_host.key_event_num;
                    if(idx < MAX_KEY_EVENT_NUM) {
                        rtk_kp_host.key_event_num++;
                        rtk_kp_host.key_event[idx].code = keycode;
                        rtk_kp_host.key_event[idx].value = curr_keystatus;
                    }
                }
            }
            pkeyinfo->last_status = (bool)curr_keystatus;
        }else if(pkeyinfo->keypad_type == GPIO_TYPE){

            curr_keystatus = rtk_gpio_input(pkeyinfo->gid);

            if(curr_keystatus < 0){
                KP_ALERT("Get key status fail\n");
            }else{
                /* If current status same with last status, do nothing */
                if((bool)curr_keystatus !=  (pkeyinfo->last_status)){
                    int pressed;

                    if((bool)curr_keystatus == (pkeyinfo->bActive))
                        pressed = 1;
                    else
                        pressed = 0;

                    KP_INFO("%s,keycode:%d,Key %s Active\n",pkeyinfo->pKeyname, keycode,(pressed)?"Press":"Release");
                    if(!rtk_kp_host.is_test_mode) {
                        input_report_key(input_dev, keycode, pressed); //1: key pressed;0: key release
                        input_sync(input_dev);
                    }else {
                        unsigned int idx = rtk_kp_host.key_event_num;
                        if(idx < MAX_KEY_EVENT_NUM) {
                            rtk_kp_host.key_event_num++;
                            rtk_kp_host.key_event[idx].code = keycode;
                            rtk_kp_host.key_event[idx].value = curr_keystatus;
                        }
                    }
                }else{
                    /* Do nothing */
                }
                pkeyinfo->last_status = (bool)curr_keystatus;
            }
        }else{
            KP_ALERT("Unaccept pin type\n");
            WARN_ON(1);
        }
    }
END:
    mutex_lock(&keypad_suspend_mutex);
    if(!g_keypad_suspend_flag)
        mod_timer(&rtk_kp_host.timer, (jiffies + msecs_to_jiffies(100)));
    mutex_unlock(&keypad_suspend_mutex);

    return;
}



/***************************************************************
 To add new driver entry here
 reference with "linux-4.4.3\drivers\input\misc\dm355evm_keys.c"
 ***************************************************************/
static int rtk_keypad_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct input_dev        *input_dev;
    struct rtk_keypad_data  *keypads;
    int                     result;

    KP_ALERT("%s(%d) key status:%d (0 means unlock):\n",__func__,__LINE__, rtk_kp_host.key_mode);
    if (!dev) {
        KP_ALERT("%s(%d)err! \n",__func__,__LINE__);
        result = -ENOMEM;
        goto exit;
    }
    keypads = kzalloc(sizeof (*keypads), GFP_KERNEL);
    if (!keypads) {
        result = -ENOMEM;
        goto exit;
    }
    g_keypads = keypads;

    input_dev = input_allocate_device();
    if (!keypads || !input_dev) {
        result = -ENOMEM;
        goto fail_alloc_input_dev;
    }

    keypads->dev = &pdev->dev;
    keypads->input_dev = input_dev;

    /* if key have attribute about repeat to add "EV_REP  0x14//repeat" later. */
    keypads->input_dev->evbit[0] = BIT_MASK(EV_KEY);
    keypads->input_dev->name = "rtk_keypad";
    keypads->input_dev->phys = "rtkTV/input1";

    keypads->input_dev->setkeycode = rtk_keypad_setkeycode;
    keypads->input_dev->getkeycode = rtk_keypad_getkeycode;

    keypads->input_dev->keybit[BIT_WORD(BTN_MOUSE)] = BIT_MASK(BTN_LEFT) | BIT_MASK(BTN_RIGHT) | BIT_MASK(BTN_MIDDLE);
    keypads->input_dev->relbit[0] = BIT_MASK(REL_X) | BIT_MASK(REL_Y) | BIT_MASK(REL_WHEEL);
    keypads->input_dev->mscbit[0] = BIT_MASK(EV_MSC);
#if 0
    set_bit(KEY_BACK, keypads->input_dev->keybit);
    set_bit(KEY_UP, keypads->input_dev->keybit);
    set_bit(KEY_DOWN, keypads->input_dev->keybit);
    set_bit(KEY_ENTER, keypads->input_dev->keybit);
    set_bit(KEY_POWER, keypads->input_dev->keybit);

    set_bit(KEY_LEFT, keypads->input_dev->keybit);
    set_bit(KEY_RIGHT, keypads->input_dev->keybit);
    set_bit(KEY_VOLUMEDOWN, keypads->input_dev->keybit);
    set_bit(KEY_VOLUMEUP, keypads->input_dev->keybit);
    set_bit(KEY_EXIT, keypads->input_dev->keybit);
    set_bit(KEY_SELECT, keypads->input_dev->keybit);
#endif
    keypads->input_dev->id.bustype	= BUS_VIRTUAL;
    keypads->input_dev->id.vendor	= 0;
    keypads->input_dev->id.product	= 1;
    keypads->input_dev->id.version	= 1;

    keypads->kpad_host = &rtk_kp_host;
    rtk_init_keypad_host();
    rtk_parse_key_info(keypads);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)) // code for 5.4
    timer_setup(&rtk_kp_host.timer, rtk_keypad_poll_timer, 0);
#else // code for < 5.4
    setup_timer(&rtk_kp_host.timer, rtk_keypad_poll_timer, (unsigned long)keypads);
#endif
    // INIT_DELAYED_WORK(&keypads->work, rtk_keypad_handle_work);

    result = input_register_device(keypads->input_dev);
    if (result) {
        KP_ALERT("RTK keypad: cannot register input device.\n");
        goto fail_register_input_dev;
    }
    rtk_keypad_create_attr(dev);
    mod_timer(&rtk_kp_host.timer, (jiffies + msecs_to_jiffies(200)));

    return 0;

fail_register_input_dev:
    input_free_device(keypads->input_dev);
fail_alloc_input_dev:
    kfree(keypads);
exit:
    return -1;

}

static int rtk_keypad_remove(struct platform_device *pdev)
{
    struct rtk_keypad_data	*keypads = platform_get_drvdata(pdev);

    del_timer_sync(&rtk_kp_host.timer);
    cancel_delayed_work(&keypads->work);
    del_timer_sync(&rtk_kp_host.timer);

    //rtk_keypad_remove_attr(keypads->dev);
    input_unregister_device(keypads->input_dev);
    kfree(keypads);

    return 0;
}
#ifdef CONFIG_PM
static int rtk_keypad_suspend(struct device *dev)
{
    KP_ALERT("suspend\n");
    mutex_lock(&keypad_suspend_mutex);
    g_keypad_suspend_flag = 1;
    mutex_unlock(&keypad_suspend_mutex);
    del_timer_sync(&rtk_kp_host.timer);
    //cancel_delayed_work(&g_keypads->work);
    del_timer_sync(&rtk_kp_host.timer);

    return 0;
}

static int rtk_keypad_resume(struct device *dev)
{

    KP_ALERT("resume\n");
    mutex_lock(&keypad_suspend_mutex);
    g_keypad_suspend_flag = 0;
    mutex_unlock(&keypad_suspend_mutex);

    mod_timer(&rtk_kp_host.timer, (jiffies + msecs_to_jiffies(100)));
    return 0;
}

static const struct dev_pm_ops rtk_keypad_pm_ops = {
    .suspend = rtk_keypad_suspend,
    .resume = rtk_keypad_resume,
};
#endif

static struct platform_driver rtk_keypad_driver = {
    .probe      = rtk_keypad_probe,
    .remove     = rtk_keypad_remove,
    .driver     = {
        .name   = "rtk_keypad",
#ifdef CONFIG_OF
        .of_match_table = of_rtk_keypad_ids,
#endif
#ifdef CONFIG_PM
        .pm = &rtk_keypad_pm_ops,
#endif
    },
};

#ifdef CONFIG_RTK_KDRV_KEYPAD
static int __init keypad_lock_mode_parser(char *mode)
{
    KP_ALERT("[Debug] %s, %d.\n", __FUNCTION__, __LINE__);

    if(memcmp(mode, "on", 2) == 0) {
        rtk_kp_host.key_mode = LOCK_MODE;
    }else{
        rtk_kp_host.key_mode = UNLOCK_MODE;
    }
    return 0;
}

early_param("keypad_lock", keypad_lock_mode_parser);
#else
extern bool rtk_parse_commandline_equal(const char *string, char *output_string, int string_size);
void rtk_parse_keypad_lock_mode(void)
{
    char str[4] = {0};

    if (0 == rtk_parse_commandline_equal("keypad_lock", str, sizeof(str)))
    {
        KP_WARNING("[%s] line: %d, can't get keypad_lock from bootargs\n", __func__, __LINE__);
        return;
    }

    if(0 == memcmp(str, "on", 2))
    {
        rtk_kp_host.key_mode = LOCK_MODE;
        return;
    }

    rtk_kp_host.key_mode = UNLOCK_MODE;

    return;
}
#endif


static int __init rtk_keypad_init(void)
{
    int ret = 0;

    KP_ALERT("%s(%d)\n",__func__,__LINE__);
    keypad_init();

    ret = platform_driver_register(&rtk_keypad_driver);

    return ret;
}


static void __exit rtk_keypad_exit(void)
{
    platform_driver_unregister(&rtk_keypad_driver);
    keypad_exit();
}

late_initcall(rtk_keypad_init);
module_exit(rtk_keypad_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("xiaomao zhou<xiaomao_zhou@apowertec.com>");
MODULE_DESCRIPTION("KeybPad driver for Realtek TV");
MODULE_ALIAS("platform:Generic Keypad");

