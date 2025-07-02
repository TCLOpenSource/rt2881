#ifndef __RTK_MODULE_EMCU_TEST_H__
#define __RTK_MODULE_EMCU_TEST_H__
/*
######################################################################################
# TEST EVENT DEFINE
######################################################################################
*/
typedef enum
{
    EMCU_TEST_TEST_TEST_EVENT_ID,
    EMCU_TEST_TEST_EVENT_MAX
}EMCU_TEST_TEST_EVENT_ID;

/*
######################################################################################
# TEST EVENT TYPE DEFINE
######################################################################################
*/
typedef enum
{
    EMCU_TEST_TEST_TYPE_ID,
    EMCU_TEST_TYPE_MAX
}EMCU_TEST_TYPE_ID;

/*
######################################################################################
# TEST EVENT MERGER DW1 DEFINE
######################################################################################
*/
#define   EMCU_TEST_DW1_test_test    MERGER_EVENT_DW1(EVENT_FW_EMCU, EMCU_TEST_MODULE_ID, EMCU_TEST_TEST_TYPE_ID, EMCU_TEST_TEST_TEST_EVENT_ID)

#define rtd_emcu_test_event_log(type, event, event_val, module_reserved)     rtd_emcu_event_log(type, EMCU_TEST_MODULE_ID, event, event_val, module_reserved)
#endif /* __RTK_MODULE_EMCU_TEST_H__ */

