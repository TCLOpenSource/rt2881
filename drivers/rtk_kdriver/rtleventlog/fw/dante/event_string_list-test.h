#ifndef __RTK_MODULE_DANTE_TEST_STRING_H__
#define __RTK_MODULE_DANTE_TEST_STRING_H__
/*
######################################################################################
# TEST EVENT STRING DEFINE
######################################################################################
*/
static struct event_event_s dante_test_test_event[]={
    {DANTE_TEST_TEST_TEST_EVENT_ID, "test"},
    {EVENT_LIMIT_MAX,NULL},
};

/*
######################################################################################
# TEST EVENT TYPE STRING DEFINE
######################################################################################
*/
struct event_eventtype_s dante_test_eventtype[]={
    {DANTE_TEST_TEST_TYPE_ID, "test", dante_test_test_event},
    {EVENT_LIMIT_MAX,NULL,NULL},
};
#endif /* __RTK_MODULE_DANTE_TEST_STRING_H__ */

