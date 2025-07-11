#ifndef __RTK_MODULE_EVENT_TEST_H__
#define __RTK_MODULE_EVENT_TEST_H__
/*
######################################################################################
# MIRACAST EVENT TYPE DEFINE
######################################################################################
*/
typedef enum
{
    SAMPLE_TEST_TYPE_1 = 0,
    SAMPLE_TEST_TYPE_2,
    SAMPLE_TEST_TYPE_ID_MAX
}TEST_TYPE_ID;

/*
######################################################################################
# MIRACAST EVENT DEFINE
######################################################################################
*/

typedef enum  {
    SAMPLE_TEST1_START = 0,
    SAMPLE_TEST1_EVENT_ID_MAX
}TEST1_EVENT_ID;

typedef enum  {
    SAMPLE_TEST2_END = 0,
    SAMPLE_TEST2_EVENT_ID_MAX
}TEST2_EVENT_ID;

#define rtd_sample_test_event_log(type, event, event_val, module_reserved)      rtd_sample_event_log(type, SAMPLE_MODULE_ID_TEST, event, event_val, module_reserved)
#endif /* __RTK_MODULE_EVENT_TEST_H__ */