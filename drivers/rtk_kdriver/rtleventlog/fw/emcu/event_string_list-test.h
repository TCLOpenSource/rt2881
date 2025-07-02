#ifndef __RTK_MODULE_EMCU_TEST_STRING_H__
#define __RTK_MODULE_EMCU_TEST_STRING_H__
/*
######################################################################################
# TEST EVENT STRING DEFINE
######################################################################################
*/
static struct event_event_s emcu_test_test_event[]={
    {EMCU_TEST_TEST_TEST_EVENT_ID, "test"},
    {EVENT_LIMIT_MAX,NULL},
};

/*
######################################################################################
# TEST EVENT TYPE STRING DEFINE
######################################################################################
*/
struct event_eventtype_s emcu_test_eventtype[]={
    {EMCU_TEST_TEST_TYPE_ID, "test", emcu_test_test_event},
    {EVENT_LIMIT_MAX,NULL,NULL},
};
#endif /* __RTK_MODULE_EMCU_TEST_STRING_H__ */

