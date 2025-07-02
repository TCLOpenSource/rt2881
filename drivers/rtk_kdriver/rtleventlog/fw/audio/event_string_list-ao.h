#ifndef __RTK_MODULE_AUDIO_AO_STRING_H__
#define __RTK_MODULE_AUDIO_AO_STRING_H__
/*
######################################################################################
# AO EVENT STRING DEFINE
######################################################################################
*/
static struct event_event_s audio_ao_event_event[]={
    {AUDIO_AO_EVENT_MAX_UPDATE_INPUT_WP_PERIOD_EVENT_ID, "max_update_input_wp_period"},
    {AUDIO_AO_EVENT_MAX_MIC_DELAY_EVENT_ID, "max_mic_delay"},
    {AUDIO_AO_EVENT_BT_MIC_TOTAL_DELAY_EVENT_ID, "bt_mic_total_delay"},
    {AUDIO_AO_EVENT_BT_MIC_OUTPUT_DMA_DELAY_EVENT_ID, "bt_mic_output_dma_delay"},
    {AUDIO_AO_EVENT_BT_MIC_OUTPUT_DMA_VALID_SIZE_EVENT_ID, "bt_mic_output_dma_valid_size"},
    {AUDIO_AO_EVENT_BT_MIC_OUTPUT_DMA_BUF_LENGTH_EVENT_ID, "bt_mic_output_dma_buf_length"},
    {AUDIO_AO_EVENT_BT_MIC_OUTPUT_DMA_BASE_EVENT_ID, "bt_mic_output_dma_base"},
    {AUDIO_AO_EVENT_BT_MIC_OUTPUT_DMA_WP_EVENT_ID, "bt_mic_output_dma_wp"},
    {AUDIO_AO_EVENT_BT_MIC_OUTPUT_DMA_RP_EVENT_ID, "bt_mic_output_dma_rp"},
    {AUDIO_AO_EVENT_BT_MIC_INPUT_VALID_SIZE_EVENT_ID, "bt_mic_input_valid_size"},
    {AUDIO_AO_EVENT_BT_MIC_INPUT_DELAY_EVENT_ID, "bt_mic_input_delay"},
    {AUDIO_AO_EVENT_BT_MIC_INPUT_BASE_EVENT_ID, "bt_mic_input_base"},
    {AUDIO_AO_EVENT_BT_MIC_INPUT_BUF_LENGTH_EVENT_ID, "bt_mic_input_buf_length"},
    {AUDIO_AO_EVENT_BT_MIC_INPUT_WP_EVENT_ID, "bt_mic_input_wp"},
    {AUDIO_AO_EVENT_BT_MIC_INPUT_RP_EVENT_ID, "bt_mic_input_rp"},
    {EVENT_LIMIT_MAX,NULL},
};

/*
######################################################################################
# AO EVENT TYPE STRING DEFINE
######################################################################################
*/
struct event_eventtype_s audio_ao_eventtype[]={
    {AUDIO_AO_EVENT_TYPE_ID, "Event", audio_ao_event_event},
    {EVENT_LIMIT_MAX,NULL,NULL},
};
#endif /* __RTK_MODULE_AUDIO_AO_STRING_H__ */

