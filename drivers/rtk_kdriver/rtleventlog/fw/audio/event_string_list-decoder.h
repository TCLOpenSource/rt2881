#ifndef __RTK_MODULE_AUDIO_DECODER_STRING_H__
#define __RTK_MODULE_AUDIO_DECODER_STRING_H__
/*
######################################################################################
# DECODER EVENT STRING DEFINE
######################################################################################
*/
static struct event_event_s audio_decoder_debug_event[]={
    {AUDIO_DECODER_DEBUG_RECV_PTS_EVENT_ID, "RECV_PTS"},
    {EVENT_LIMIT_MAX,NULL},
};

/*
######################################################################################
# DECODER EVENT TYPE STRING DEFINE
######################################################################################
*/
struct event_eventtype_s audio_decoder_eventtype[]={
    {AUDIO_DECODER_DEBUG_TYPE_ID, "Debug", audio_decoder_debug_event},
    {EVENT_LIMIT_MAX,NULL,NULL},
};
#endif /* __RTK_MODULE_AUDIO_DECODER_STRING_H__ */

