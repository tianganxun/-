#include "cmd.h"

extern FILE *file;            //�ļ�ָ�룬���ڸ��ֶ��ļ��Ĳ���
extern char filesystemname[20];           //�����Ĵ����ļ���
extern super_block_t  file_system_super;    //�����ṹ�����
extern char inodeBP[INODE_NUM / 8];         //�����ڵ�λʾͼ
extern char dataBP[DATA_BLOCK_NUM / 8];    //���ݿ�λʾͼ
extern directory DirBuf[BLOCK_SIZE / sizeof(directory)];   //Ŀ¼����
extern struct inode_t iNode[BLOCK_SIZE / sizeof(inode_t)];                              //�����ڵ�


extern char CurDir[50];                     //��¼��ǰ���Ǹ��ļ�����
extern int cur_inode_no;                     //���ص�ǰ���ڵ�Ŀ¼�������ڵ�
extern int spare_datablk_num[512];           //��¼�������ݿ�Ŀ�ţ����ڷ������ݿ�
extern int b_index[128];                             //��ַʱ������Ѱַ���м���̿�

extern char Buffer[1048576];                //1M�Ļ����ڴ�ռ�
extern char Block_Data[513];                //���ڴ��һ�����̿��е����ݵ���ʱ�ռ�

char cmd1[50], cmd2[50], cmd3[50];           //��ŷָ��������Լ�����


void Entersystem()   //���������ļ�ϵͳ
{
	char cmd[50];
	strcpy(CurDir, "root");
	cur_inode_no = 0;
	int result;
	Showhelp();
	while (1)
	{
		printf("%s>", CurDir);
		gets(cmd);
		//��������������ʶ��
		result = Identifycmd(cmd);
		switch (result)
		{
		case 0:
			break;
		case 1:                  //��ʾ��ǰ�ļ����µ�����
			Show_Dir();
			break;


		case 3:                   //��ӡ�����������
			Dump_FS();
			break;
		case 4:                     //��ʾ����
			Showhelp();
			break;

		case 6:                       //����Ŀ¼
			CreateDir(cmd2);
			break;

		case 8:                     //ɾ�������ļ�
			Delete_File(cmd2);
			break;
		case 9:                     //ɾ��Ŀ¼�µ������ļ�
			DeleteDir(cmd2);
			break;
		case 10:                    //��ʾ�ļ�����
			Show_Content(cmd2);
			break;
		case 11:                   //�����ļ�
			Copy_File(cmd2, cmd3);
			break;
		case 12:                   //������ϵͳ�е��ļ����Ƶ�disk.img��
			Copy_File_In(cmd2, cmd3);
			break;
		case 13:                    //��disk.img�е��ļ����Ƶ�����ϵͳ��
			Copy_File_out(cmd2, cmd3);
			break;
		case 14:                     //�˳���ǰϵͳ
			goto lable;
			break;
		case 15:                   //�½�һ���ļ�
			New_file(cmd2);
			break;
		case -1:                    //�������������û�
			printf("Error!\n");
			break;
		}
	}
lable:
	int useless;
}

//��������������ʶ��
int Identifycmd(char cmd[])
{
	if (cmd[0] == 0)
		return 0;
	int part_count = 1;
	int i, j;
	for (i = 0; cmd[i]; i++)
		if (cmd[i] == ' ')
			part_count++;
	switch (part_count)
	{
	case 1:
		if (strcmp(cmd, "ls") == 0)            //��ʾ��ǰ�ļ����µ�����
			return 1;

		else if (strcmp(cmd, "dumpfs") == 0)  //��ӡ�����������
			return 3;
		else if (strcmp(cmd, "help") == 0)   //��ʾ����
			return 4;
		else if (strcmp(cmd, "exit") == 0)    //�˳���ǰϵͳ
			return 14;
		else
			return -1;
		break;
	case 2:

		for (i = 0; cmd[i] != ' '; i++)
			cmd1[i] = cmd[i];
		cmd1[i] = 0;
		i++;
		for (j = 0; cmd[i]; i++, j++)
			cmd2[j] = cmd[i];
		cmd2[j] = 0;


		if (strcmp(cmd1, "mkdir") == 0)        //����Ŀ¼
			return 6;

		else if (strcmp(cmd1, "rm") == 0)      //ɾ�������ļ�
			return 8;
		else if (strcmp(cmd1, "rmdir") == 0)   //ɾ��Ŀ¼�µ������ļ�
			return 9;
		else if (strcmp(cmd1, "cat") == 0)    //��ʾ�ļ�����
			return 10;
		else if (strcmp(cmd1, "new") == 0)    //�½�һ���ļ�
			return 15;
		else
			return -1;
		break;
	case 3:

		for (i = 0; cmd[i] != ' '; i++)
			cmd1[i] = cmd[i];
		cmd1[i] = 0;
		i++;
		for (j = 0; cmd[i] != ' '; i++, j++)
			cmd2[j] = cmd[i];
		cmd2[j] = 0;
		i++;
		for (j = 0; cmd[i]; i++, j++)
			cmd3[j] = cmd[i];
		cmd3[j] = 0;

		if (strcmp(cmd1, "cp") == 0)              //�����ļ�
			return 11;
		else if (strcmp(cmd1, "cpin") == 0)      //������ϵͳ�е��ļ����Ƶ�disk.img��
			return 12;
		else if (strcmp(cmd1, "cpout") == 0)     //��disk.img�е��ļ����Ƶ�����ϵͳ��
			return 13;
		else
			return -1;
		break;
	}
}

bool Dump_FS()                              //��ʾ�ļ�ϵͳ������
{
	int i;
	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("���ļ�ʧ�ܣ�\n");
		return false;
	}
	//�Ƚ�����������ڴ�
	fseek(file, 0L, SEEK_SET);
	fread(&file_system_super, sizeof(super_block_t), 1, file);
	//�ٰ������ڵ�λʾͼ�����ڴ�
	fseek(file, BLOCK_SIZE, SEEK_SET);
	fread(inodeBP, sizeof(inodeBP), 1, file);
	//�ٰ����ݿ�λʾͼ�����ڴ�
	fseek(file, BLOCK_SIZE * 18, SEEK_SET);
	fread(dataBP, sizeof(dataBP), 1, file);
	fclose(file);
	printf("===================================================================\n");
	printf("The system's content are following:\n\n");

	printf("The super block:\n");
	printf("magic             = %d\n", file_system_super.magic);
	printf("total block count = %d\n", file_system_super.total_block_count);
	printf("inode count       = %d\n", file_system_super.inode_count);
	printf("data block count  = %d\n\n", file_system_super.data_block_count);

	printf("The four region:\n");
	printf("region                  start block        block count       byte count\n");
	printf("inode table bitmap       %d                  %d                  %d\n", file_system_super.inode_table_bitmap.start_block,
		file_system_super.inode_table_bitmap.block_count, file_system_super.inode_table_bitmap.byte_count);
	printf("inode table              %d                  %d                 %d\n", file_system_super.inode_table.start_block,
		file_system_super.inode_table.block_count, file_system_super.inode_table.byte_count);
	printf("data area bitmap         %d                 %d                  %d\n", file_system_super.data_area_bitmap.start_block,
		file_system_super.data_area_bitmap.block_count, file_system_super.data_area_bitmap.byte_count);
	printf("data area                %d                 %d                %d\n\n", file_system_super.data_area.start_block,
		file_system_super.data_area.block_count, file_system_super.data_area.byte_count);

	printf("The bitmap:\n");
	printf("inode table bitmap:\n");
	for (i = 0; i<16; i++)
	{
		Print_Byte(inodeBP[i]);
		//printf("\n");
	}
	printf("\ndata area bitmap:\n");
	for (i = 0; i<64; i++)
	{
		Print_Byte(dataBP[i]);
	}
	printf("\n");
	return true;
}

void Print_Byte(char bit)
{
	int i;
	for (i = 7; i >= 0; i--)
	{
		if (((bit >> i) & 0x01) == 0)
			printf("0");
		else
			printf("1");
	}
}

bool showhelpmain() {
	printf("help             Show all the commands you can use.\n");
	printf("sf               Select and enter a virtual disk.\n");
	printf("sf --mkfs        Create or initialize a virtual disk.\n");

	return true;
}
