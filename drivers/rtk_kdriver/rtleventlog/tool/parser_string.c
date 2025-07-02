#include <core/event_log_core.h>

int main(int argc, char* argv[]){
	char *file_name = argv[1];
	char *filter_name = argv[2];
	char *ouput_name = argv[3];
	char *str_time = argv[4];
	FILE *fp_in = NULL;
	FILE *fp_out = NULL;
	char buff[255];
	char ch_data[4][255];
	const char* d = "-";
    char *p1,*p2,*p3,*p4;
	char *index = NULL;
	int time = 0;
	fp_in = fopen(file_name, "r");
	fp_out = fopen(ouput_name,"w");
	time = strtol(str_time, NULL, 10);

	while ( fgets(buff, 255, (FILE*)fp_in) )
	{
		if (NULL != (index = strstr(buff, filter_name)))
		{
			//index = strstr(buff, "0x47");
			p1 = strtok(index, d);
			p2 = strtok(NULL, d);
			p3 = strtok(NULL, d);
			p4 = strtok(NULL, d);
			//printf("p1 = %s\n",p1);
			//printf("p2 = %s\n",p2);
			//printf("p3 = %s\n",p3);
			//printf("p4 = %s\n",p4);
			if (time == 1)
				parser_to_file(strtol(p1, NULL, 16),strtol(p2, NULL, 16),strtol(p3, NULL, 16),11*strtol(p4, NULL, 16),1,fp_out);
			else
				parser_to_file(strtol(p1, NULL, 16),strtol(p2, NULL, 16),strtol(p3, NULL, 16),strtol(p4, NULL, 16)/90,0,fp_out);
		}
	}
	fclose(fp_in);
	fclose(fp_out);
	return 0;
}
