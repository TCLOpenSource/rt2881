#ifndef __RTK_MODULE_EVENT_TEST1_H__
#define __RTK_MODULE_EVENT_TEST1_H__
/*
######################################################################################
# TEST1 EVENT DEFINE
######################################################################################
*/
typedef enum
{
    UBOOT_TEST1_TEST1_TEST1_EVENT_ID,
    UBOOT_TEST1_TEST1_EVENT_MAX
}UBOOT_TEST1_TEST1_EVENT_ID;

/*
######################################################################################
# TEST1 EVENT TYPE DEFINE
######################################################################################
*/
typedef enum
{
    UBOOT_TEST1_TEST1_TYPE_ID,
    UBOOT_TEST1_TYPE_MAX
}UBOOT_TEST1_TYPE_ID;

/*
######################################################################################
# TEST1 EVENT MERGER DW1 DEFINE
######################################################################################
*/
#define   TEST1_DW1_test1_test1    MERGER_EVENT_DW1(EVENT_FW_UBOOT, UBOOT_TEST1_MODULE_ID, UBOOT_TEST1_TEST1_type_ID, UBOOT_TEST1_TEST1_TEST1_EVENT_ID)

#define rtd_test1_event_log(type, event, event_val, module_reserved)     rtd_uboot_event_log(type, UBOOT_TEST1_MODULE_ID, event, event_val, module_reserved)
#endif /* __RTK_MODULE_EVENT_TEST1_H__ */

