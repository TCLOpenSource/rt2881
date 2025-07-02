
#ifndef __RTK_MODULE_DANTE_STRING_DEFINE_H__
#define __RTK_MODULE_DANTE_STRING_DEFINE_H__
#include <fw/dante/module_list.h>
#include <fw/dante/event_string_list-test.h>
/*
######################################################################################
# MODULE STRING DEFINE
######################################################################################
*/
struct event_module_s dante_module[]={
    {DANTE_TEST_MODULE_ID, "test", dante_test_eventtype},
    {EVENT_LIMIT_MAX,NULL,NULL},
};
#endif /* __RTK_MODULE_DANTE_STRING_DEFINE_H__ */
