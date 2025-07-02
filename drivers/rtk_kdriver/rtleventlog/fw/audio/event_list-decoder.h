#ifndef __RTK_MODULE_AUDIO_DECODER_H__
#define __RTK_MODULE_AUDIO_DECODER_H__
/*
######################################################################################
# DECODER EVENT DEFINE
######################################################################################
*/
typedef enum
{
    AUDIO_DECODER_DEBUG_RECV_PTS_EVENT_ID,
    AUDIO_DECODER_DEBUG_EVENT_MAX
}AUDIO_DECODER_DEBUG_EVENT_ID;

/*
######################################################################################
# DECODER EVENT TYPE DEFINE
######################################################################################
*/
typedef enum
{
    AUDIO_DECODER_DEBUG_TYPE_ID,
    AUDIO_DECODER_TYPE_MAX
}AUDIO_DECODER_TYPE_ID;

/*
######################################################################################
# DECODER EVENT MERGER DW1 DEFINE
######################################################################################
*/
#define   AUDIO_DECODER_DW1_Debug_RECV_PTS    MERGER_EVENT_DW1(EVENT_FW_AUDIO, AUDIO_DECODER_MODULE_ID, AUDIO_DECODER_DEBUG_TYPE_ID, AUDIO_DECODER_DEBUG_RECV_PTS_EVENT_ID)

#define rtd_audio_decoder_event_log(type, event, event_val, module_reserved)     rtd_audio_event_log(type, AUDIO_DECODER_MODULE_ID, event, event_val, module_reserved)
#endif /* __RTK_MODULE_AUDIO_DECODER_H__ */

