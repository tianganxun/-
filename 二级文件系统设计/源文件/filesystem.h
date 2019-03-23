#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H
//===================================================================================================
#include <stdio.h>
#include <string.h>

#define BLOCK_SIZE 512                    //设置每个磁盘块大小为512B

#define TOTAL_BLOCK_NUMBER 532            //创建拥有532个磁盘块虚拟磁盘

/*
磁盘块号 0 为引导区
磁盘块号 1 为超级块
磁盘块号 2 为索引节点位示图    用int存储 一个int是4B  512B/4B = 128 可存128个
磁盘块号 3-18 为索引节点
磁盘块号 19 为数据块位示图
磁盘块号 20-531 为数据块
*/


#define INODE_NUM 128                     //索引节点数目128

#define INODE_BITMAP_START 1              //索引节点位示图开始磁盘块号 1
#define INODE_BITMAP_BKNUM 1              //索引节点位示图占据磁盘块数 1
#define INODE_BLOCK_START 2               //索引节点开始磁盘块号 2
#define INODE_BLOCK_NUM 16                //索引节点所占的磁盘块数目为16
#define INODE_NUM_PER_BLOCK 8            //每个磁盘块所拥有的索引节点数目为8
#define INODE_SIZE 60                     //每个索引节点占据60B大小的空间

#define DATA_BITMAP_START 18              //数据块位示图开始磁盘块号 18
#define DATA_BITMAP_BLNUM 1              //数据块位示图占据磁盘块数 1
#define DATA_BLOCK_START 19              //数据块开始磁盘块号 19
#define DATA_BLOCK_NUM  512               //数据块占据磁盘块数目  512

#define MAX_DISK_ADDRESS 13              //索引节点的寻址项，0~9为直接寻址，10,11为一次间址，12为二次间址

typedef struct region_t {
	int start_block;   // 起始块，每个region从块边界开始
	int block_count;   // region占用的块个数
	int byte_count;    // region占用的字节个数 

}region_t;


typedef struct super_block {
	int magic;
	int total_block_count;         //规定总共的磁盘块的数目
	int inode_count;               //规定总共的索引节点数目
	int data_block_count;          //规定总共的数据块的数目
	region_t inode_table_bitmap;   //索引节点位图
	region_t inode_table;          //索引节点表
	region_t data_area_bitmap;     //数据块位图
	region_t data_area;            //数据块区域
}super_block_t;


enum Type {
	INODE_REGULAR, // 普通文件，文本文件或则二进制文件
	INODE_DIR     // 目录文件
};


typedef struct inode_t {
	int type;      // 文件类型
	int size;      // 文件大小
	int disk_address[MAX_DISK_ADDRESS];  // 寻磁盘块方式
}inode_t;


typedef struct dir_entry
{
	char name[12];  // 目录项名称
	int ino;        // 索引节点号
}directory;



extern char filesystemname[20];           //创建的磁盘文件名
extern directory DirBuf[BLOCK_SIZE / sizeof(directory)];   //目录数组

bool Format();                       //格式化整个磁盘文件，如果没有，则创建该磁盘文件

bool Install();                      //装载磁盘系统

bool Showhelp();                    //显示用户可以的操作指令以及操作格式


inode_t Find_Inode(int ino);

bool Create_File(char file_name[], int index_node);
//lo
//通过传入的文件名，在本文件系统中创建一个文件


int Scan_Inode_Bitmap();                         //扫描索引节点位示图，返回一个可用的索引节点号

bool Scan_Data_Bitmap(int data_block_count);     //扫描数据块位示图，将空闲数据块的块号存入全局变量spare_datablk_num数组中


bool Fill_Index_In_Datablk(int indexblock_no, int buf_block_count);

void Spill(char big[], char small[], int data_count, int size);

int Lookup_File(char *path);                     //ls             

bool Copy_File(char filename1[], char filename2[]);                      //复制文件

bool isPath(char *name);                                         //判断该参数是文件名还是路径

int Lookup_Dir(int index_node);                              //检索目录，输入目录文件节点，结果是将目录节点对应的目录输入到全局变量DirBuf[]中


bool Read_File(int file_index);                   //  fr         //读取文件，根据文件的索引节点号，将对应的文件的内容读取到内存缓冲buffer中                             

bool Get_Data(int block_index);                   //     r     //通过磁盘块号，将磁盘上的数据读入缓冲区Block_Data中

bool Merge(int count);

bool Copy_File_out(char filename1[], char filename2[]);      //将本系统中的文件拷贝到物理磁盘上，参数2仅仅只有一个文件名

bool Copy_File_In(char filename1[], char filename2[]);     //将物理磁盘上的文件拷贝到本文件系统中，参数1只有一个文件名

bool Delete_File(char filename[]);        //  fd               //删除一个文件

bool Delete_From_Inode_Bitmap(int file_index);           //根据给定的文件的索引节点，在索引节点位示图中将对应的bit位置0

bool Delete_From_Data_Bitmap(int block_index);           //根据给定的磁盘的索引节点，在数据块位示图中将对应的bit位置0

bool Clear_Dir_Buf();                                       //将缓存Dir_Buf清空

bool Show_Content(char file_name[]);                      //显示文件内容

bool New_file(char filename[]);                    //   fc                   //创建一个新文件，文件内容由用户自己输入

bool isLegal(int dir_index, char filename[]);            //判断该文件名是否合法

bool Show_Dir();                           //   ls    //显示当前目录下的所有文件





bool CreateDir(char dir_path[]);               //创建一个目录

bool MakeDir(int dir_index, char dir_name[]);   //根据给定的目录索引号，在该目录文件下创建目录名为给定的名字的目录

bool DeleteDir(char dir_name[]);                //删除文件夹

int GetType(int file_index);                    //判断文件类型的函数，如果是普通文件，则返回1；如果是目录文件，则返回0

#endif