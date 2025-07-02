#ifndef __RTK_MODULE_DANTE_TEST_H__
#define __RTK_MODULE_DANTE_TEST_H__
/*
######################################################################################
# TEST EVENT DEFINE
######################################################################################
*/
typedef enum
{
    DANTE_TEST_TEST_TEST_EVENT_ID,
    DANTE_TEST_TEST_EVENT_MAX
}DANTE_TEST_TEST_EVENT_ID;

/*
######################################################################################
# TEST EVENT TYPE DEFINE
######################################################################################
*/
typedef enum
{
    DANTE_TEST_TEST_TYPE_ID,
    DANTE_TEST_TYPE_MAX
}DANTE_TEST_TYPE_ID;

/*
######################################################################################
# TEST EVENT MERGER DW1 DEFINE
######################################################################################
*/
#define   DANTE_TEST_DW1_test_test    MERGER_EVENT_DW1(EVENT_FW_DANTE, DANTE_TEST_MODULE_ID, DANTE_TEST_TEST_TYPE_ID, DANTE_TEST_TEST_TEST_EVENT_ID)

#define rtd_dante_test_event_log(type, event, event_val, module_reserved)     rtd_dante_event_log(type, DANTE_TEST_MODULE_ID, event, event_val, module_reserved)
#endif /* __RTK_MODULE_DANTE_TEST_H__ */

