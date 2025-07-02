#define LOG_SIZE 100
#include <stdbool.h>
#include <stdio.h>
#include <core/event_log_core.h>
#include <fw/audio/module_string_list.h>
#include <fw/dante/module_string_list.h>
#include <fw/emcu/module_string_list.h>
#include <fw/video/module_string_list.h>
#include <fw/uboot/module_string_list.h>
#include <fw/kernel/module_string_list.h>
#include <fw/quickshow/module_string_list.h>
#include <fw/android/module_string_list.h>

#define BigtoLittle32(A) ((((int)(A) & 0xff000000) >> 24) | (((int)(A) & 0x00ff0000) >> 8) | \
(((int)(A) & 0x0000ff00) << 8) | (((int)(A) & 0x000000ff) << 24))

struct event_fw_s event_fw[]={
    {EVENT_FW_ANDROID, "ANDROID", event_android_module},
    {EVENT_FW_QUICKSHOW, "QUICKSHOW", quickshow_module},
    {EVENT_FW_KERNEL, "KERNEL", kernel_module},
    {EVENT_FW_UBOOT, "UBOOT", uboot_module},
    {EVENT_FW_EMCU, "EMCU", emcu_module},
    {EVENT_FW_VIDEO, "VIDEO", video_module},
    {EVENT_FW_DANTE, "DANTE", dante_module},
    {EVENT_FW_AUDIO, "AUDIO", audio_module},
	{EVENT_LIMIT_MAX,NULL,NULL},
};

int GET_FW_ID(unsigned int DW1)
{
	struct event_format event_format;
	int i = 0;

	event_format.fw_type = (DW1 & 0x00f00000)>>20;

	for( i = 0; EVENT_LIMIT_MAX != event_fw[i].fw_id; i++)
	{
		if( event_fw[i].fw_id == event_format.fw_type)
			return i;
	}
	printf("parse fw error, fw_type = %x\n", event_format.fw_type);
	return -1;
}

int GET_MODULE_ID(unsigned int DW1, int fw_id)
{
	struct event_format event_format;
	int i = 0;

	event_format.module = (DW1 & 0x0000ff00)>>8;


	for( i = 0;EVENT_LIMIT_MAX != event_fw[fw_id].module[i].module_id; i++)
	{
		if( event_fw[fw_id].module[i].module_id == event_format.module)
			return i;
	}
	printf("parse module error, module = %x\n", event_format.module);
	return -1;
}

int GET_EVENTTYPE_ID(unsigned int DW1, int fw_id, int module_id)
{
	struct event_format event_format;
	int i = 0;

	event_format.event_type = (DW1 & 0x000f0000)>>16;

	for( i = 0; EVENT_LIMIT_MAX != event_fw[fw_id].module[module_id].eventtype[i].eventtype_id; i++)
	{
		if( event_fw[fw_id].module[module_id].eventtype[i].eventtype_id == event_format.event_type)
			return i;
	}
	printf("parse type error, event_type = %x\n", event_format.event_type);
	return -1;
}

int GET_EVENT_ID(unsigned int DW1,unsigned int fw_id,unsigned int module_id,unsigned int eventtype_id)
{
	struct event_format event_format;
	int i = 0;

	event_format.event = DW1 & 0x000000ff;

	for( i = 0; EVENT_LIMIT_MAX != event_fw[fw_id].module[module_id].eventtype[eventtype_id].event[i].event_id; i++)
	{
		if( event_fw[fw_id].module[module_id].eventtype[eventtype_id].event[i].event_id == event_format.event)
			return i;
	}
	printf("parse event error, event = %x\n", event_format.event);

	return -1;
}


unsigned int eventlog_parser_printf(unsigned int DW1,unsigned int DW2, unsigned int DW3 ,unsigned int DW4, unsigned int time_unit)
{

	int fw_id = 0;
	int module_id = 0;
	int eventtype_id = 0;
	int event_id = 0;

	struct event_format event_format;

	event_format.syncbyte = (DW1 & 0xff000000)>>24;
	event_format.event_value = DW2;
	event_format.module_reserved = DW3;
	/* time stamp */
	event_format.timestamp = DW4;

	//event_warn_print(" DW1 = %08x\n", DW1);

	if(event_format.syncbyte != EVENT_SYNCBYTE)
	{
		printf("syncbyte error");
		printf("DW1 = %08x\n", DW1);
		return EVENT_ERROR;
	}

	fw_id = GET_FW_ID(DW1);

	if(-1 == fw_id)
	{
		printf("DW1 = %08x\n", DW1);
		return EVENT_ERROR;
	}

	module_id = GET_MODULE_ID(DW1, fw_id);

	if(-1 == module_id)
	{
		printf("DW1 = %08x\n", DW1);
		return EVENT_ERROR;
	}

	eventtype_id = GET_EVENTTYPE_ID(DW1, fw_id, module_id);

	if(-1 == eventtype_id)
	{
		printf("DW1 = %08x\n", DW1);
		return EVENT_ERROR;
	}

	event_id = GET_EVENT_ID(DW1, fw_id, module_id, eventtype_id);

	if(-1 == event_id)
	{
		printf("DW1 = %08x\n", DW1);
		return EVENT_ERROR;
	}

	if (time_unit)
		printf("[%d.%d%d%d%d%d%d][%s][%s][%s][%s],%x,%x \n",
					(event_format.timestamp/1000000),
					((event_format.timestamp%1000000)/100000),((event_format.timestamp%100000)/10000),
					((event_format.timestamp%10000)/1000),((event_format.timestamp%1000)/100),
					((event_format.timestamp%100)/10),((event_format.timestamp%10)),
					event_fw[fw_id].name, event_fw[fw_id].module[module_id].name,
             	    event_fw[fw_id].module[module_id].eventtype[eventtype_id].name,
             	    event_fw[fw_id].module[module_id].eventtype[eventtype_id].event[event_id].name,
					event_format.event_value,
					event_format.module_reserved);
	else
		printf("[%d.%d%d%d000][%s][%s][%s][%s],%x,%x \n",
					(event_format.timestamp/1000),((event_format.timestamp%1000)/100),
					((event_format.timestamp%100)/10),((event_format.timestamp%10)),
					event_fw[fw_id].name, event_fw[fw_id].module[module_id].name,
             	    event_fw[fw_id].module[module_id].eventtype[eventtype_id].name,
             	    event_fw[fw_id].module[module_id].eventtype[eventtype_id].event[event_id].name,
					event_format.event_value,
					event_format.module_reserved);

	return EVENT_OK;
}


int main(void){

	FILE *fp = NULL;
	char buff[255];
	char ch_data[4][255];
	char filter_buf[128]="EVENTLOG";
	int bin[4];
	int i;
	fp = fopen("./binary.log", "rb");
	printf("hello world\n");
	while (fread(bin,4,4,fp))
	{
		fread(bin,4,4,fp);
		eventlog_parser_printf(BigtoLittle32(bin[0]),BigtoLittle32(bin[1]),BigtoLittle32(bin[2]),BigtoLittle32(bin[3]),1);
	}
	/*for (i = 0; i<4; i++)
	{
		printf("0x%08x\n", BigtoLittle32(bin[i]));
	}*/
	printf("hello world\n");
	/*while ( fread(buffer,sizeof(buffer),1,ptr))
	{
		if (NULL != strstr(buff, filter_buf))
		{
			printf("%s", buff );
		}
			printf("%s", buff );
	}*/
	fclose(fp);
	printf("hello world\n");
	return 0;
}