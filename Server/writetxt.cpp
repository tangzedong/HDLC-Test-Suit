#include"framedef.h"
void writetxt(FILE* z,u_char write[MAX_LEN],u_char out_len)
{
	//unsigned char write_str[] = {0x7E,0xA0,0x21,0x21,0x02,0x23,0x73,0x8F,0x72, 0x7E};
	int m;
	for(m = 0;m<out_len ;m++)
	{
		fprintf(z,"%02x ",write[m]);
	}
	fprintf(z,"\n");
//	fclose(z);
	//system("pause");
	//return 0;
}