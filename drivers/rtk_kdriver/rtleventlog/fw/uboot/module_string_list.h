
#ifndef __RTK_MODULE_EVENT_UBOOT_STRING_DEFINE_H__
#define __RTK_MODULE_EVENT_UBOOT_STRING_DEFINE_H__
#include <fw/uboot/module_list.h>
#include <fw/uboot/event_string_list-test1.h>
/*
######################################################################################
# MODULE STRING DEFINE
######################################################################################
*/
struct event_module_s uboot_module[]={
    {UBOOT_TEST1_MODULE_ID, "test1", uboot_test1_eventtype},
    {EVENT_LIMIT_MAX,NULL,NULL},
};
#endif /* __RTK_MODULE_EVENT_UBOOT_STRING_DEFINE_H__ */
