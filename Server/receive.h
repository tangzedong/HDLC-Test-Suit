#ifndef _RECEIVE
#define _RECEIVE

#include <stdio.h>
#include<malloc.h>
# include <stdlib.h>
#include <cctype>
#include <assert.h>
#pragma comment(lib,"ws2_32.lib")
#include"string.h"
typedef unsigned int u_int;   //32λ
typedef unsigned char u_char;  //8λ
typedef unsigned short u_short;//16λ
/*typedef unsigned double u_double;//64λ  8�ֽ�*/

#define MAX_LEN 2048
#define PPPINITFCS16 0xffff;             //��ʼFCSֵ
#define PPPGOODFCS16 0xf0b8;   //�õ�����FCSֵ

typedef struct frame_format{
	u_char frame_type;   //֡����
	u_char frame_seg;    //�ֶ�λ
	u_short frame_sublen;  //֡��������
} frame_format  ,*frame_formatpointer;

typedef struct hdlc{
    u_char start_flag;//��ʼ��־1byte
	frame_format  f_format;//����ṹ�������hdlc֡�����������
	u_int dst_addr;//Ŀ�ĵ�ַ
	u_char src_addr; //Դ��ַ
	u_char frame_ctl;//������
	u_short h_cs;//ͷУ������
	u_char info_buff[MAX_LEN];//�洢��Ϣ�����λ128�ֽ�
	u_short f_cs;//֡У������
	u_char end_flag;//������־
}hdlc , *hdlcpointer;

int readoneline(FILE* q,u_char oneLineData[MAX_LEN]);//void readoneline(FILE* q,u_char* t);
void writetxt(FILE* z,u_char write[MAX_LEN],u_char out_len);
u_short h_cs_cal(u_char h_cs[MAX_LEN],u_char hcs_len);
u_short f_cs_cal(u_char f_cs[MAX_LEN],u_char fcs_len);
#endif