#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H
//===================================================================================================
#include <stdio.h>
#include <string.h>

#define BLOCK_SIZE 512                    //����ÿ�����̿��СΪ512B

#define TOTAL_BLOCK_NUMBER 532            //����ӵ��532�����̿��������

/*
���̿�� 0 Ϊ������
���̿�� 1 Ϊ������
���̿�� 2 Ϊ�����ڵ�λʾͼ    ��int�洢 һ��int��4B  512B/4B = 128 �ɴ�128��
���̿�� 3-18 Ϊ�����ڵ�
���̿�� 19 Ϊ���ݿ�λʾͼ
���̿�� 20-531 Ϊ���ݿ�
*/


#define INODE_NUM 128                     //�����ڵ���Ŀ128

#define INODE_BITMAP_START 1              //�����ڵ�λʾͼ��ʼ���̿�� 1
#define INODE_BITMAP_BKNUM 1              //�����ڵ�λʾͼռ�ݴ��̿��� 1
#define INODE_BLOCK_START 2               //�����ڵ㿪ʼ���̿�� 2
#define INODE_BLOCK_NUM 16                //�����ڵ���ռ�Ĵ��̿���ĿΪ16
#define INODE_NUM_PER_BLOCK 8            //ÿ�����̿���ӵ�е������ڵ���ĿΪ8
#define INODE_SIZE 60                     //ÿ�������ڵ�ռ��60B��С�Ŀռ�

#define DATA_BITMAP_START 18              //���ݿ�λʾͼ��ʼ���̿�� 18
#define DATA_BITMAP_BLNUM 1              //���ݿ�λʾͼռ�ݴ��̿��� 1
#define DATA_BLOCK_START 19              //���ݿ鿪ʼ���̿�� 19
#define DATA_BLOCK_NUM  512               //���ݿ�ռ�ݴ��̿���Ŀ  512

#define MAX_DISK_ADDRESS 13              //�����ڵ��Ѱַ�0~9Ϊֱ��Ѱַ��10,11Ϊһ�μ�ַ��12Ϊ���μ�ַ

typedef struct region_t {
	int start_block;   // ��ʼ�飬ÿ��region�ӿ�߽翪ʼ
	int block_count;   // regionռ�õĿ����
	int byte_count;    // regionռ�õ��ֽڸ��� 

}region_t;


typedef struct super_block {
	int magic;
	int total_block_count;         //�涨�ܹ��Ĵ��̿����Ŀ
	int inode_count;               //�涨�ܹ��������ڵ���Ŀ
	int data_block_count;          //�涨�ܹ������ݿ����Ŀ
	region_t inode_table_bitmap;   //�����ڵ�λͼ
	region_t inode_table;          //�����ڵ��
	region_t data_area_bitmap;     //���ݿ�λͼ
	region_t data_area;            //���ݿ�����
}super_block_t;


enum Type {
	INODE_REGULAR, // ��ͨ�ļ����ı��ļ�����������ļ�
	INODE_DIR     // Ŀ¼�ļ�
};


typedef struct inode_t {
	int type;      // �ļ�����
	int size;      // �ļ���С
	int disk_address[MAX_DISK_ADDRESS];  // Ѱ���̿鷽ʽ
}inode_t;


typedef struct dir_entry
{
	char name[12];  // Ŀ¼������
	int ino;        // �����ڵ��
}directory;



extern char filesystemname[20];           //�����Ĵ����ļ���
extern directory DirBuf[BLOCK_SIZE / sizeof(directory)];   //Ŀ¼����

bool Format();                       //��ʽ�����������ļ������û�У��򴴽��ô����ļ�

bool Install();                      //װ�ش���ϵͳ

bool Showhelp();                    //��ʾ�û����ԵĲ���ָ���Լ�������ʽ


inode_t Find_Inode(int ino);

bool Create_File(char file_name[], int index_node);
//lo
//ͨ��������ļ������ڱ��ļ�ϵͳ�д���һ���ļ�


int Scan_Inode_Bitmap();                         //ɨ�������ڵ�λʾͼ������һ�����õ������ڵ��

bool Scan_Data_Bitmap(int data_block_count);     //ɨ�����ݿ�λʾͼ�����������ݿ�Ŀ�Ŵ���ȫ�ֱ���spare_datablk_num������


bool Fill_Index_In_Datablk(int indexblock_no, int buf_block_count);

void Spill(char big[], char small[], int data_count, int size);

int Lookup_File(char *path);                     //ls             

bool Copy_File(char filename1[], char filename2[]);                      //�����ļ�

bool isPath(char *name);                                         //�жϸò������ļ�������·��

int Lookup_Dir(int index_node);                              //����Ŀ¼������Ŀ¼�ļ��ڵ㣬����ǽ�Ŀ¼�ڵ��Ӧ��Ŀ¼���뵽ȫ�ֱ���DirBuf[]��


bool Read_File(int file_index);                   //  fr         //��ȡ�ļ��������ļ��������ڵ�ţ�����Ӧ���ļ������ݶ�ȡ���ڴ滺��buffer��                             

bool Get_Data(int block_index);                   //     r     //ͨ�����̿�ţ��������ϵ����ݶ��뻺����Block_Data��

bool Merge(int count);

bool Copy_File_out(char filename1[], char filename2[]);      //����ϵͳ�е��ļ���������������ϣ�����2����ֻ��һ���ļ���

bool Copy_File_In(char filename1[], char filename2[]);     //����������ϵ��ļ����������ļ�ϵͳ�У�����1ֻ��һ���ļ���

bool Delete_File(char filename[]);        //  fd               //ɾ��һ���ļ�

bool Delete_From_Inode_Bitmap(int file_index);           //���ݸ������ļ��������ڵ㣬�������ڵ�λʾͼ�н���Ӧ��bitλ��0

bool Delete_From_Data_Bitmap(int block_index);           //���ݸ����Ĵ��̵������ڵ㣬�����ݿ�λʾͼ�н���Ӧ��bitλ��0

bool Clear_Dir_Buf();                                       //������Dir_Buf���

bool Show_Content(char file_name[]);                      //��ʾ�ļ�����

bool New_file(char filename[]);                    //   fc                   //����һ�����ļ����ļ��������û��Լ�����

bool isLegal(int dir_index, char filename[]);            //�жϸ��ļ����Ƿ�Ϸ�

bool Show_Dir();                           //   ls    //��ʾ��ǰĿ¼�µ������ļ�





bool CreateDir(char dir_path[]);               //����һ��Ŀ¼

bool MakeDir(int dir_index, char dir_name[]);   //���ݸ�����Ŀ¼�����ţ��ڸ�Ŀ¼�ļ��´���Ŀ¼��Ϊ���������ֵ�Ŀ¼

bool DeleteDir(char dir_name[]);                //ɾ���ļ���

int GetType(int file_index);                    //�ж��ļ����͵ĺ������������ͨ�ļ����򷵻�1�������Ŀ¼�ļ����򷵻�0

#endif