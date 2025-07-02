#ifndef __RTK_MODULE_EVENT_TEST1_STRING_H__
#define __RTK_MODULE_EVENT_TEST1_STRING_H__
/*
######################################################################################
# TEST1 EVENT STRING DEFINE
######################################################################################
*/
static struct event_event_s uboot_test1_test1_event[]={
    {UBOOT_TEST1_TEST1_TEST1_EVENT_ID, "test1"},
    {EVENT_LIMIT_MAX,NULL},
};

/*
######################################################################################
# TEST1 EVENT TYPE STRING DEFINE
######################################################################################
*/
struct event_eventtype_s uboot_test1_eventtype[]={
    {UBOOT_TEST1_TEST1_TYPE_ID, "test1", uboot_test1_test1_event},
    {EVENT_LIMIT_MAX,NULL,NULL},
};
#endif /* __RTK_MODULE_EVENT_TEST1_STRING_H__ */

