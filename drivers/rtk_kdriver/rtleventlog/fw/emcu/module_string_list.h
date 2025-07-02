
#ifndef __RTK_MODULE_EMCU_STRING_DEFINE_H__
#define __RTK_MODULE_EMCU_STRING_DEFINE_H__
#include <fw/emcu/module_list.h>
#include <fw/emcu/event_string_list-test.h>
/*
######################################################################################
# MODULE STRING DEFINE
######################################################################################
*/
struct event_module_s emcu_module[]={
    {EMCU_TEST_MODULE_ID, "test", emcu_test_eventtype},
    {EVENT_LIMIT_MAX,NULL,NULL},
};
#endif /* __RTK_MODULE_EMCU_STRING_DEFINE_H__ */
