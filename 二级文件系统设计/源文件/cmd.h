#ifndef CMD_H
#define CMD_H

#include "filesystem.h"



void Entersystem();           //��ʽ�������ļ�֮������������Ѿ����ڸղŴ����Ĵ����ļ�

int  Identifycmd(char cmd[]);      //�����������������֣�����ֵΪ-1������������

bool Dump_FS();                              //��ʾ�ļ�ϵͳ������

void Print_Byte(char bit);                 //��һ���ֽڵĸ���bitλ�����

bool showhelpmain();
#endif