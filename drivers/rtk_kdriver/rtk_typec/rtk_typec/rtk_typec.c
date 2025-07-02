#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/of_platform.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/err.h>
#include <linux/freezer.h>
#include <rtk_kdriver/io.h>
#include <rtk_kdriver/rtk-kdrv-common.h>
#include "rtk_typec.h"
#include "rtk_typec_intf.h"
#include "rtk_typec_debug.h"
#include <base_types.h>

static struct task_struct *rtk_typec_poll_thread = NULL;
static struct completion rtk_typec_poll_thread_exited;
static int rtk_typec_poll_thread_exited_flag = 0;

static int rtk_typec_read_partner_identity(struct rtk_typec_info *typec_info)
{
	struct rtk_typec_info_rx_identity_reg id;
	
	memset(&id, 0, sizeof(id));
	typec_info->partner_identity = id.identity;

	return 0;
}

static void rtk_typec_disconnect(struct rtk_typec_info *typec_info, u32 status)
{
	if (!IS_ERR(typec_info->partner))
		typec_unregister_partner(typec_info->partner);
	typec_info->partner = NULL;
	typec_set_pwr_opmode(typec_info->port, TYPEC_PWR_MODE_USB);
	typec_set_pwr_role(typec_info->port, TYPEC_CONNECT_STATUS_PORTROLE(status));
	typec_set_vconn_role(typec_info->port, TYPEC_CONNECT_STATUS_VCONNROLE(status));
	typec_set_data_role(typec_info->port, TYPEC_CONNECT_STATUS_DATAROLE(status));
	//typec_set_orientation(typec_info->port, TYPEC_ORIENTATION_NONE);
}

int rtk_typec_connect(struct rtk_typec_info *typec_info, u32 status)
{
	struct typec_partner_desc partner_desc;
	enum typec_pwr_opmode mode;
	int ret;
	RTK_TYPEC_DBG("SLL connect status:%x, %x, %x\n", status,
		TYPEC_CONNECT_STATUS_PORTROLE(status), 
		TYPEC_CONNECT_STATUS_POWER_OPMODE(status));
	if (typec_info->partner) {
		typec_set_pwr_opmode(typec_info->port, TYPEC_CONNECT_STATUS_POWER_OPMODE(status));
		typec_set_pwr_role(typec_info->port, TYPEC_CONNECT_STATUS_PORTROLE(status));
		typec_set_vconn_role(typec_info->port, TYPEC_CONNECT_STATUS_VCONNROLE(status));
		typec_set_data_role(typec_info->port, TYPEC_CONNECT_STATUS_DATAROLE(status));
		//typec_set_orientation(typec_info->port, TYPEC_ALT_MODE_STATUS_ORIENTATION(rtk_typec_get_alt_mode_status(typec_info->id)) ? TYPEC_ORIENTATION_NORMAL : TYPEC_ORIENTATION_REVERSE);
		return 0;
	}

	mode = TYPEC_CONNECT_STATUS_POWER_OPMODE(status);

	partner_desc.usb_pd = mode == TYPEC_PWR_MODE_PD;
	partner_desc.accessory = TYPEC_ACCESSORY_NONE; /* XXX: handle accessories */
	partner_desc.identity = NULL;

	if (partner_desc.usb_pd) {
		ret = rtk_typec_read_partner_identity(typec_info);
		if (ret)
			return ret;
		//partner_desc.identity = &typec_info->partner_identity;
	}

	typec_set_pwr_opmode(typec_info->port, mode);
	typec_set_pwr_role(typec_info->port, TYPEC_CONNECT_STATUS_PORTROLE(status));
	typec_set_vconn_role(typec_info->port, TYPEC_CONNECT_STATUS_VCONNROLE(status));
	typec_set_data_role(typec_info->port, TYPEC_CONNECT_STATUS_DATAROLE(status));
	//typec_set_orientation(typec_info->port, TYPEC_ALT_MODE_STATUS_ORIENTATION(rtk_typec_get_alt_mode_status(typec_info->id)) ? TYPEC_ORIENTATION_NORMAL : TYPEC_ORIENTATION_REVERSE);

	typec_info->partner = typec_register_partner(typec_info->port, &partner_desc);
	if (IS_ERR(typec_info->partner))
		return PTR_ERR(typec_info->partner);

	if (partner_desc.identity)
		typec_partner_set_identity(typec_info->partner);

	return 0;
}
#if 0
static int rtk_typec_dr_set(struct typec_port *port, enum typec_data_role role)
{

	return -EOPNOTSUPP;
}

static int rtk_typec_pr_set(struct typec_port *port, enum typec_role role)
{
	
	return -EOPNOTSUPP;
}

static const struct typec_operations rtk_typec_ops = {
	.dr_set = rtk_typec_dr_set,
       .pr_set = rtk_typec_pr_set,
};
#endif
static int typec_print_cnt = 0;

static unsigned int Read_idx_frm_adc(unsigned int saradc_ch)
{
    unsigned int adc_val = 0;

    if(saradc_ch == 0)
    {
        while((rtd_inl(P7F_02_10B_ADC_OUTPUT) & _BIT0) != _BIT0) //SARADC0
        {
            //RTK_TYPEC_DBG("saradc polling");    //nSystemPower * 0.02307 = actual power
            msleep(50);
        }
        //0xb8065048[7:0] adc_a1_data[9:2]
        //0xb806504C[1:0] adc_a1_data[1:0]
        adc_val = ( ((rtd_inl(P7F_10_10B_ADC_A0_RSLT_H) & 0xff) << 2u) | (rtd_inl(P7F_11_10B_ADC_A0_RSLT_L) & 0x3) );
        RTK_TYPEC_DBG("adc_val = %d,  ", adc_val);    //nSystemPower * 0.02307 = actual power
    }

    return adc_val;
}

void check_vbus_power(void)
{
    unsigned int nSystemPower;

    rtd_maskl(P7F_02_10B_ADC_OUTPUT, ~(_BIT0), _BIT0);

    nSystemPower = (unsigned int)Read_idx_frm_adc(0);

    //RTK_TYPEC_DBG("Typec_vbus = %d,  ", nSystemPower);    //nSystemPower * 0.02307 = actual power

    if(nSystemPower > 780) {
        RTK_TYPEC_DBG("Typec_vbus_domain about 20V\n");
    }
    else if((nSystemPower > 606) && (nSystemPower <= 780)) {
        RTK_TYPEC_DBG("Typec_vbus_domain about 15V\n");
    }
    else if((nSystemPower > 476) && (nSystemPower <= 606)) {
        RTK_TYPEC_DBG("Typec_vbus_domain about 12V\n");
    }
    else if((nSystemPower > 346) && (nSystemPower <= 476)) {
        RTK_TYPEC_DBG("Typec_vbus_domain about 9V\n");
    }
    else if((nSystemPower > 173) && (nSystemPower <= 346)) {
        RTK_TYPEC_DBG("Typec_vbus_domain about 5V\n");
    }
    else {
        RTK_TYPEC_DBG("Typec_vbus_domain UNKNOWN V\n");
    }
}

void check_typec_status(void)
{
    if(((rtd_inl(P66_0C_TPC_CC_CTRL) & (_BIT2)) == (_BIT2)) ||
        ((rtd_inl(P66_0C_TPC_CC_CTRL) & (_BIT3)) == (_BIT3)))
    {
        if(++typec_print_cnt > 200) // 12s
        {
            // CC
            RTK_TYPEC_DBG("0xb80644c8=%x\n",  rtd_inl(ALT_MODE_CHECK_REGISTER));
            RTK_TYPEC_DBG("0xb80644cc=%x\n",  rtd_inl(P66_33_CC_HW_FSM));
            RTK_TYPEC_DBG("0xb80644c0=%x\n",  rtd_inl(P66_30_CC_HW_MODE_CTRL));

            // SARADC
            check_vbus_power();

            typec_print_cnt = 0;
        }
    }
}

static int _rtk_typec_poll_thread_func(void *arg)
{
	struct rtk_typec_info *typec_info = NULL;
	
	typec_info = (struct rtk_typec_info *)arg;
	while(!rtk_typec_poll_thread_exited_flag) {
		unsigned int connect_status = rtk_typec_get_connect_status(typec_info->id);
		if(typec_info->last_connect_status != connect_status) {
			if(TYPEC_CONNECT_STATUS_PLUG_PRESENT(connect_status))
				rtk_typec_connect(typec_info, connect_status);
			else
				rtk_typec_disconnect(typec_info, connect_status);
			typec_info->last_connect_status = connect_status;
		}
		check_typec_status();
		msleep(50);
	}
	complete_and_exit(&rtk_typec_poll_thread_exited, 1);
	return 0;
}

static int rtk_typec_create_poll_thread(void *arg)
{
	rtk_typec_poll_thread_exited_flag = 0;
	rtk_typec_poll_thread = kthread_create(_rtk_typec_poll_thread_func, arg, "rtk_typec_poll_thread");
	if(IS_ERR(rtk_typec_poll_thread)) {
		
		rtk_typec_poll_thread = NULL;
		return -ENOMEM;
	}
	init_completion(&rtk_typec_poll_thread_exited);
    	wake_up_process(rtk_typec_poll_thread);
	return 0;
}

static void rtk_typec_destroy_poll_thread(void *arg)
{
	if(rtk_typec_poll_thread) {
		rtk_typec_poll_thread_exited_flag = 1;
		wait_for_completion(&rtk_typec_poll_thread_exited);
		rtk_typec_poll_thread = NULL;
	}
}

static int rtk_typec_get_id_from_dts(struct platform_device *pdev, unsigned int *p_id)
{
        struct device_node *p_node;

        if(!pdev || !p_id)
                return -EINVAL;

        p_node =  pdev->dev.of_node;

        if (!p_node)
        {
                return -ENODEV;
        }

        if(of_property_read_u32_index(p_node, "id", 0, p_id) != 0)
        {
                return -EIO;
        }
        return 0;
}

static int rtk_typec_probe(struct platform_device *pdev)
{
	struct typec_capability *typec_cap = NULL;
	struct rtk_typec_info *typec_info;

	RTK_TYPEC_DBG("rtk_typec_probe enter \n");

	if(rtk_typec_get_id_from_dts(pdev, (unsigned int *)(&pdev->id)) != 0)
                return -EINVAL;

	typec_info = kzalloc(sizeof(*typec_info), GFP_KERNEL);
	if (!typec_info)
		return -ENOMEM;

	typec_info->id = pdev->id;
	mutex_init(&typec_info->lock);
	typec_info->dev = &pdev->dev;
	platform_set_drvdata(pdev, typec_info);
	
	typec_cap = &typec_info->caps;
	typec_cap->revision = USB_TYPEC_REV_1_2;
	typec_cap->pd_revision = 0x300;
	typec_cap->prefer_role = TYPEC_NO_PREFERRED_ROLE;
	//typec_cap->driver_data = typec_info;
	//typec_cap->ops = &rtk_typec_ops;

	typec_cap->type = TYPEC_PORT_DRP;
	typec_cap->data = TYPEC_PORT_DRD;

	typec_info->port = typec_register_port(&pdev->dev, typec_cap);
	if (IS_ERR(typec_info->port)) {
		RTK_TYPEC_ERR("typec_register_port fail, err:%ld\n", PTR_ERR(typec_info->port));
		kfree(typec_info);
		return PTR_ERR(typec_info->port);
	}

	if(rtk_typec_create_poll_thread(typec_info) != 0) {
		RTK_TYPEC_ERR("Create TYPEC Poll Thread fail \n");
		kfree(typec_info);
		return -1;
	}

	RTK_TYPEC_DBG("rtk_typec_probe exit \n");
	return 0;
}

static int rtk_typec_remove(struct platform_device *pdev)
{
	struct rtk_typec_info *typec_info = platform_get_drvdata(pdev);
	rtk_typec_destroy_poll_thread(typec_info);
	platform_set_drvdata(pdev, NULL);
	typec_unregister_port(typec_info->port);
	kfree(typec_info);
	return 0;
}

static int rtk_typec_resume(struct device *dev)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct rtk_typec_info *typec_info = platform_get_drvdata(pdev);
	
	if(rtk_typec_create_poll_thread(typec_info) != 0) {
		RTK_TYPEC_ERR("Create TYPEC Poll Thread fail \n");
		return -1;
	}
	return 0;
}


static int rtk_typec_suspend(struct device *dev)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct rtk_typec_info *typec_info = platform_get_drvdata(pdev);
	RTK_TYPEC_DBG("rtk_typec_suspend enter \n");
	rtk_typec_destroy_poll_thread(typec_info);
	rtk_typec_disconnect(typec_info, 0);
	RTK_TYPEC_DBG("rtk_typec_suspend exit \n");
	return 0;
}


static const struct dev_pm_ops rtk_typec_pm_ops = {
    .resume = rtk_typec_resume,
    .suspend = rtk_typec_suspend,
};

static const struct of_device_id rtk_typec_of_match[] =
{
        {
                .compatible = "rtk,rtk-typec-platform",
        },
        {}
};

static const struct acpi_device_id rtk_typec_acpi_match[] =
{
        /* XHCI-compliant USB Controller */
        { "PNP0D10", },
        { }
};
MODULE_DEVICE_TABLE(acpi, rtk_typec_acpi_match);

static struct platform_driver rtk_typec_plat_driver =
{
        .probe = rtk_typec_probe,
        .remove = rtk_typec_remove,
        .driver = {
                .name = "rtk-typec-platform",
                .pm = &rtk_typec_pm_ops,
                .of_match_table = of_match_ptr(rtk_typec_of_match),
                .acpi_match_table = ACPI_PTR(rtk_typec_acpi_match),
        },
};
MODULE_ALIAS("platform:rtk-typec-platform");

static int __init rtk_typec_driver_init(void)
{
    RTK_TYPEC_DBG("rtk_typec_driver_init  \n");
    return platform_driver_register(&rtk_typec_plat_driver);
}

module_init(rtk_typec_driver_init);
