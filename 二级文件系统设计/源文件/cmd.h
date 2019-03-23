#ifndef CMD_H
#define CMD_H

#include "filesystem.h"



void Entersystem();           //格式化磁盘文件之后，物理磁盘中已经存在刚才创建的磁盘文件

int  Identifycmd(char cmd[]);      //对输入的命令进行区分，返回值为-1表明命令有误！

bool Dump_FS();                              //显示文件系统的设置

void Print_Byte(char bit);                 //将一个字节的各个bit位输出来

bool showhelpmain();
#endif