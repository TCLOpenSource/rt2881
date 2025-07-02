
#ifndef __RTK_MODULE_AUDIO_STRING_DEFINE_H__
#define __RTK_MODULE_AUDIO_STRING_DEFINE_H__
#include <fw/audio/module_list.h>
#include <fw/audio/event_string_list-decoder.h>
#include <fw/audio/event_string_list-ao.h>
/*
######################################################################################
# MODULE STRING DEFINE
######################################################################################
*/
struct event_module_s audio_module[]={
    {AUDIO_DECODER_MODULE_ID, "decoder", audio_decoder_eventtype},
    {AUDIO_AO_MODULE_ID, "ao", audio_ao_eventtype},
    {EVENT_LIMIT_MAX,NULL,NULL},
};
#endif /* __RTK_MODULE_AUDIO_STRING_DEFINE_H__ */
