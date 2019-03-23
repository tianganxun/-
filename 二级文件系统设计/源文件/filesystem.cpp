#include "filesystem.h"


FILE *file;            //�ļ�ָ�룬���ڸ��ֶ��ļ��Ĳ���
char filesystemname[20];           //�����Ĵ����ļ���
super_block_t  file_system_super;    //�����ṹ�����
char inodeBP[INODE_NUM / 8];         //�����ڵ�λʾͼ����λ��=8(char)*16=128bit,����һ�����Ա�ʾ128�������ڵ�
char dataBP[DATA_BLOCK_NUM / 8];    //���ݿ�λʾͼ����λ��=8(char)*64=512bit,����һ�����Ա�ʶ521�����ݿ�
directory DirBuf[BLOCK_SIZE / sizeof(directory)];   //Ŀ¼���飬��ʾһ�����̿���Դ��32��Ŀ¼��
struct inode_t iNode[BLOCK_SIZE / sizeof(inode_t)];                              //�����ڵ�


char CurDir[50];                     //��¼��ǰ���Ǹ��ļ�����
int cur_inode_no;                     //���ص�ǰ���ڵ�Ŀ¼�������ڵ�
int spare_datablk_num[512];           //��¼�������ݿ�Ŀ�ţ����ڷ������ݿ�
int b_index[128];                             //��ַʱ������Ѱַ���м���̿�

char Buffer[1048576];                //1M�Ļ����ڴ�ռ�
char Block_Data[513];                //���ڴ��һ�����̿��е����ݵ���ʱ�ռ�

									 //��ʽ�������ļ�����ʼ�����е�����
bool Format()
{
	int i;
	file = fopen(filesystemname, "wb");//������������ļ�
	if (NULL == file)
	{
		printf("Failed to create a virtual disk file��");
		return false;
	}

	//��ʼ��������
	file_system_super.magic = 0;
	file_system_super.total_block_count = TOTAL_BLOCK_NUMBER;          //�ļ�ϵͳ���ܴ�����
	file_system_super.inode_count = INODE_NUM;                         //�ļ�ϵͳ���������ڵ���
	file_system_super.data_block_count = DATA_BLOCK_NUM;               //�ļ�ϵͳ�������ݿ���Ŀ

	file_system_super.inode_table_bitmap.start_block = INODE_BITMAP_START;                //�����ڵ�λʾͼ��ʼ���̿�
	file_system_super.inode_table_bitmap.block_count = INODE_BITMAP_BKNUM;                //�����ڵ�λʾͼռ�ݴ��̿���
	file_system_super.inode_table_bitmap.byte_count = INODE_NUM / 8;                       //�����ڵ�λʾͼռ�õ��ֽڸ��� 

	file_system_super.inode_table.start_block = INODE_BLOCK_START;                       //�����ڵ㿪ʼ���̿�
	file_system_super.inode_table.block_count = INODE_BLOCK_NUM;                         //�����ڵ�ռ�ݴ��̿���
	file_system_super.inode_table.byte_count = INODE_SIZE * INODE_NUM_PER_BLOCK * INODE_BLOCK_NUM;//�����ڵ�ռ�õ��ֽڸ��� 

	file_system_super.data_area_bitmap.start_block = DATA_BITMAP_START;                //���ݿ�λʾͼ��ʼ���̿�
	file_system_super.data_area_bitmap.block_count = DATA_BITMAP_BLNUM;                //���ݿ�λʾͼռ�ݴ��̿���
	file_system_super.data_area_bitmap.byte_count = DATA_BLOCK_NUM / 8;                  //���ݿ�λʾͼռ�õ��ֽڸ���

	file_system_super.data_area.start_block = DATA_BLOCK_START;                //���ݿ鿪ʼ���̿�
	file_system_super.data_area.block_count = DATA_BLOCK_NUM;                 //���ݿ�ռ�ݴ��̿���
	file_system_super.data_area.byte_count = DATA_BLOCK_NUM * BLOCK_SIZE;     //���ݿ�ռ�õ��ֽڸ���

	fseek(file, 0L, SEEK_SET);
	fwrite(&file_system_super, BLOCK_SIZE, 1, file);

	//��ʼ�������ڵ�λʾͼ
	inodeBP[0] = 0x80;           
	for (i = 1; i < INODE_NUM / 8; i++)
	{
		inodeBP[i] = 0;
	}
	fseek(file, BLOCK_SIZE, SEEK_SET);
	fwrite(inodeBP, sizeof(inodeBP), 1, file);

	//��ʼ�������ڵ�
    iNode[0].type = 0;                 //��ʾ���ļ���Ŀ¼�ļ�
	iNode[0].size = 0;                 //Ŀ¼�ļ���size����0
	iNode[0].disk_address[0] = 0 + DATA_BLOCK_START;      //��Ŀ¼��ռ�ݵ����ݿ��<���ݿ�Ŵ�0#��511#>����Ӧ�ڴ��̺Ż�Ҫ����19
	fseek(file, BLOCK_SIZE * 2, SEEK_SET);
	fwrite(&iNode, sizeof(iNode), 1, file);

	//��ʼ�����ݿ�λʾͼ
	dataBP[0] = 0x80;         
	for (i = 1; i < DATA_BLOCK_NUM / 8; i++)
	{
		dataBP[i] = 0;
	}
	fseek(file, BLOCK_SIZE * 18, SEEK_SET);
	fwrite(dataBP, BLOCK_SIZE, 1, file);

	//��ʼ�����ݿ�
	Clear_Dir_Buf();
	strcpy(DirBuf[0].name, ".");
	DirBuf[0].ino = 0;
	strcpy(DirBuf[1].name, "..");
	DirBuf[1].ino = 0;
	fseek(file, BLOCK_SIZE * 19, SEEK_SET);
	fwrite(DirBuf, BLOCK_SIZE, 1, file);

	fclose(file);
	return true;
}

//װ�ش����ļ�ϵͳ
bool Install()
{
	int i;
	printf("Under load\n");
	file = fopen(filesystemname, "rb+");       //��ֻ����ʽ�򿪴����ļ�
	if (NULL == file)
	{
		printf("%sDisk file opening failed��\n", filesystemname);
		return false;
	}
	//�Ƚ�����������ڴ�
	fseek(file, 0L, SEEK_SET);
	fread(&file_system_super, sizeof(super_block_t), 1, file);
	//�ٰ������ڵ�λʾͼ�����ڴ�
	fseek(file, BLOCK_SIZE, SEEK_SET);
	fread(inodeBP, sizeof(inodeBP), 1, file);
	//�������ڵ�����ڴ���
	fseek(file, BLOCK_SIZE * 2, SEEK_SET);
	fread(iNode, sizeof(iNode), 1, file);

	//�ٰ����ݿ�λʾͼ�����ڴ�
	fseek(file, BLOCK_SIZE * 18, SEEK_SET);
	fread(dataBP, sizeof(dataBP), 1, file);

	//����Ŀ¼��Ŀ¼�ļ����뵽�ڴ���
	fseek(file, BLOCK_SIZE * 19, SEEK_SET);
	fread(DirBuf, BLOCK_SIZE, 1, file);

	fclose(file);
	return true;
}

bool Showhelp()
{
	printf("help      Show all the commands you can use.\n");
	printf("dumpfs    Displays the file system format(superblock content).\n");
	printf("ls        Displays all the files of the file system.\n");
	printf("cat       Displays the contents of a file in the file system.\n");
	printf("cp        Copy the virtual disk file to the virtual disk.\n");
	printf("cpin      Copy the host file to the virtual disk.\n");
	printf("cpout     Copy the virtual disk file to the host.\n");
	printf("mkdir     Create a new folder to the virtual disk.\n");
	printf("new       Create a new file to the virtual disk.\n");
	printf("rm        Delete a file from the file system.\n");
	printf("rmdir     Delete a folder from the file system.\n");
	printf("exit      Exit the current file system.\n");
	return true;
}

bool Show_Content(char FileName[])
{
	int file_index;                     //����ʾ���ļ�����
	int dir_file_index;                //Ŀ¼�ļ��������ڵ��
	char file_name[20];                //�ļ���
	char path[20];
	int middle;
	int count = 0;
	int i = 0;
	int j = 0;
	if (isPath(FileName))
	{
		while (FileName[i])
		{
			if (FileName[i] == '/')
				count++;
			i++;
		}
		for (i = 0; FileName[i]; i++)
		{
			if (FileName[i] == '/')
				j++;
			if (j == count)
				break;
		}
		middle = i;
		for (i = 0; i<middle; i++)
			path[i] = FileName[i];
		path[i] = 0;
		for (j = 0, i = middle + 1; FileName[i]; i++, j++)
			file_name[j] = FileName[i];
		file_name[j] = 0;
		dir_file_index = Lookup_File(path);
		Lookup_Dir(dir_file_index);
		for (i = 2; DirBuf[i].name[0]; i++)
		{
			if (strcmp(DirBuf[i].name, file_name) == 0)
				file_index = DirBuf[i].ino;
		}

	}
	else
	{
		Lookup_Dir(cur_inode_no);
		for (i = 2; DirBuf[i].name[0]; i++)
		{
			if (strcmp(DirBuf[i].name, FileName) == 0)
				file_index = DirBuf[i].ino;
		}
	}
	if (!Read_File(file_index))
	{
		printf("Can't read the directory!\n");
		return false;
	}
	for (i = 0; i<strlen(Buffer); i++)
	{
		printf("%c", Buffer[i]);
	}
	printf("\n");
	return true;
}
//�������ڵ������ýڵ��Ӧ���������ṩ����Ҫ�ĺ���
inode_t Find_Inode(int ino)
{
	inode_t inode;
	int i, j, k;
	i = ino / 8;                //i��ʾ����������ڵ㿪ʼ�飨INODE_BLOCK_START#���̿飩��λ�ã���2+i#���̿飩
	j = ino - 8 * i;            //j��ʶ�ڸô��̿��е�һ�������ڵ��¼

	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to open file��\n");
		
	}
	fseek(file, BLOCK_SIZE*(i + INODE_BLOCK_START), SEEK_SET);
	fread(iNode, sizeof(iNode), 1, file);
	fclose(file);

	inode.size = iNode[j].size;
	inode.type = iNode[j].type;
	for (k = 0; k<MAX_DISK_ADDRESS; k++)
		inode.disk_address[k] = iNode[j].disk_address[k];
	return inode;
}

//��ǰ�����ڵ�ţ�cur_inode_no
bool Create_File(char file_name[], int index_node)   //����Buffer���飬�ǵý��������ݺ�һ���ֽڸ�0����Ȼ�����ֽ���ʱ�����
{
	int inode_no;          //��ż�����������ļ��������ڵ�
	int buf_block_count;   //������������ռ���ݿ�ĸ���
	int buf_byte_count;    //���������ݵ��ֽ�����
	int block_num;         //��������ָ��������ڵ��������̿��(2#~17#)
	int inode_num;         //�������ڵ�ָ��������ڵ��ڴ������λ��(0~7)
	int i, j;
	int indirect_indexblooc_no; 
	inode_no = Scan_Inode_Bitmap();
	if (-1 == inode_no)
	{
		printf("The index code has run out!\nCreate failed!\n");
		return false;
	}
	
	buf_byte_count = strlen(Buffer);
	buf_block_count = buf_byte_count / BLOCK_SIZE;
	if (buf_byte_count % BLOCK_SIZE != 0)                  
		buf_block_count++;
	//���������ڵ�Ż�ø������ڵ�ľ���λ��
	block_num = inode_no / 8 + 2;
	inode_num = inode_no % 8;

	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to open file��\n");
		return false;
	}
	fseek(file, BLOCK_SIZE*block_num, SEEK_SET);
	fread(iNode, sizeof(iNode), 1, file);
	fclose(file);

	iNode[inode_num].type = 1;                           
	iNode[inode_num].size = buf_byte_count;             

	char data[513];                                    
													
														
	if (buf_block_count <= 10)                           
	{
		if (Scan_Data_Bitmap(buf_block_count))
		{
			for (i = 0; i<buf_block_count; i++)
			{
				iNode[inode_num].disk_address[i] = spare_datablk_num[i];
				
				memset(data, 0, sizeof(data));                          //�ȶԽ�Ҫд�Ĵ��̿����0����
				file = fopen(filesystemname, "rb+");
				fseek(file, BLOCK_SIZE*spare_datablk_num[i], SEEK_SET);
				fwrite(data, BLOCK_SIZE, 1, file);
				fclose(file);

				Spill(Buffer, data, i + 1, 512);               
				file = fopen(filesystemname, "rb+");
				fseek(file, BLOCK_SIZE*spare_datablk_num[i], SEEK_SET);
				fwrite(data, BLOCK_SIZE, 1, file);
				fclose(file);
			}

		}
		else
		{
			printf("Failed to create file��\n");
		}
	}
	else if (buf_block_count <= 128)                                  
	{
		Scan_Data_Bitmap(1);                                          
		indirect_indexblooc_no = spare_datablk_num[0];               
		iNode[inode_num].disk_address[10] = indirect_indexblooc_no;   
		if (Fill_Index_In_Datablk(indirect_indexblooc_no, buf_block_count))
		{
			for (i = 0; i<buf_block_count; i++)
			{
				memset(data, 0, sizeof(data));                        
				file = fopen(filesystemname, "rb+");
				fseek(file, BLOCK_SIZE*b_index[i], SEEK_SET);
				fwrite(data, BLOCK_SIZE, 1, file);
				fclose(file);

				Spill(Buffer, data, i + 1, 512);               
				file = fopen(filesystemname, "rb+");
				fseek(file, BLOCK_SIZE*b_index[i], SEEK_SET); 
				fwrite(data, BLOCK_SIZE, 1, file);
				fclose(file);
			}
		}
		else
		{
			printf("Failed to create file��\n");
		}
	}
	else if (buf_block_count <= 128 * 2)                     
	{
		Scan_Data_Bitmap(1);                                                
		indirect_indexblooc_no = spare_datablk_num[0];                       
		iNode[inode_num].disk_address[10] = indirect_indexblooc_no;
		if (Fill_Index_In_Datablk(indirect_indexblooc_no, 128))                 
		{
			for (i = 0; i<128; i++)
			{
				memset(data, 0, sizeof(data));                          //�ȶԽ�Ҫд�Ĵ��̿����0����
				file = fopen(filesystemname, "rb+");
				fseek(file, BLOCK_SIZE*b_index[i], SEEK_SET);
				fwrite(data, BLOCK_SIZE, 1, file);
				fclose(file);

				Spill(Buffer, data, i + 1, 512);
				file = fopen(filesystemname, "rb+");
				fseek(file, BLOCK_SIZE*b_index[i], SEEK_SET);
				fwrite(data, BLOCK_SIZE, 1, file);
				fclose(file);
			}
		}
		else
		{
			printf("Failed to create file��\n");
		}

		Scan_Data_Bitmap(1);                                                
		indirect_indexblooc_no = spare_datablk_num[0];                    
		iNode[inode_num].disk_address[11] = indirect_indexblooc_no;
		if (Fill_Index_In_Datablk(indirect_indexblooc_no, buf_block_count - 128))
		{
			for (i = 128, j = 0; i<buf_block_count; i++, j++)
			{
				memset(data, 0, sizeof(data));                          //�ȶԽ�Ҫд�Ĵ��̿����0����
				file = fopen(filesystemname, "rb+");
				fseek(file, BLOCK_SIZE*b_index[j], SEEK_SET);
				fwrite(data, BLOCK_SIZE, 1, file);
				fclose(file);

				Spill(Buffer, data, i + 1, 512);
				file = fopen(filesystemname, "rb+");
				fseek(file, BLOCK_SIZE*b_index[j], SEEK_SET);
				fwrite(data, BLOCK_SIZE, 1, file);
				fclose(file);
			}
		}
		else
		{
			printf("Failed to create file��\n");
		}
	}
	else                                               
	{
		Scan_Data_Bitmap(1);                           
		indirect_indexblooc_no = spare_datablk_num[0];
		iNode[inode_num].disk_address[12] = indirect_indexblooc_no;
		int index_count;                              
		index_count = buf_block_count / 128;
		if (buf_block_count % 128 != 0)
			index_count++;
		Fill_Index_In_Datablk(indirect_indexblooc_no, index_count);    
		int temp[128];
		for (i = 0; i<index_count; i++)
			temp[i] = b_index[i];
		int k;
		int buffer_part_count = 1;              //��¼���ݻ���buffer�Ŀ���
		for (k = 0; k<index_count - 1; k++)//temp[k]��ʾ��k�������������̿�
		{
			if (Fill_Index_In_Datablk(temp[k], 128))//������������̿�����д�ָ����ݵ����ݴ��̿��
			{
				
				for (i = 0; i<128; i++)
				{
					memset(data, 0, sizeof(data));                          //�ȶԽ�Ҫд�Ĵ��̿����0����
					file = fopen(filesystemname, "rb+");
					fseek(file, BLOCK_SIZE*b_index[i], SEEK_SET);
					fwrite(data, BLOCK_SIZE, 1, file);
					fclose(file);

					Spill(Buffer, data, buffer_part_count, 512);
					buffer_part_count++;
					file = fopen(filesystemname, "rb+");
					fseek(file, BLOCK_SIZE*b_index[i], SEEK_SET);
					fwrite(data, BLOCK_SIZE, 1, file);
					fclose(file);
				}
			}
			else
			{
				printf("Failed to create file��\n");
				return false;
			}
		}
		//���һ��������index_count���������������ݿ��п��ܲ���128��,
		if (Fill_Index_In_Datablk(temp[k], buf_block_count - 128 * (index_count - 1)))
		{
			//�����������������Ҫ��ʣ�µ��������Ӧ�Ĵ��̿���
			for (i = 0; i<buf_block_count - 128 * (index_count - 1); i++)
			{
				memset(data, 0, sizeof(data));                          //�ȶԽ�Ҫд�Ĵ��̿����0����
				file = fopen(filesystemname, "rb+");
				fseek(file, BLOCK_SIZE*b_index[i], SEEK_SET);
				fwrite(data, BLOCK_SIZE, 1, file);
				fclose(file);

				Spill(Buffer, data, buffer_part_count, 512);
				buffer_part_count++;
				file = fopen(filesystemname, "rb+");
				fseek(file, BLOCK_SIZE*b_index[i], SEEK_SET);
				fwrite(data, BLOCK_SIZE, 1, file);
				fclose(file);
			}
		}
		else
		{
			printf("Failed to create file��\n");
			return false;
		}
	
	}
	
	file = fopen(filesystemname, "rb+");
	fseek(file, BLOCK_SIZE*block_num, SEEK_SET);
	fwrite(iNode, sizeof(iNode), 1, file);  
	fclose(file);

	
	int dir_blkno;                      //��ǰ�ļ������ڵĴ��̿�
	block_num = index_node / 8 + 2;     
	inode_num = index_node % 8;
	file = fopen(filesystemname, "rb+");
	fseek(file, BLOCK_SIZE*block_num, SEEK_SET);
	fread(iNode, sizeof(iNode), 1, file);
	fclose(file);
	dir_blkno = iNode[inode_num].disk_address[0];   //���ݵ�ǰ�����ڵ�Ų��������ڵ㣬Ȼ���ҵ���Ӧ��Ŀ¼�ļ��ڵĴ��̿��
	Clear_Dir_Buf();
	file = fopen(filesystemname, "rb+");
	fseek(file, BLOCK_SIZE*dir_blkno, SEEK_SET);
	fread(DirBuf, sizeof(DirBuf), 1, file);
	fclose(file);
	i = 0;
	while (DirBuf[i].name[0])
		i++;
	strcpy(DirBuf[i].name, file_name);
	DirBuf[i].ino = inode_no;

	file = fopen(filesystemname, "rb+");
	fseek(file, BLOCK_SIZE*dir_blkno, SEEK_SET);
	fwrite(DirBuf, sizeof(DirBuf), 1, file);
	fclose(file);
}


int Scan_Inode_Bitmap()
{
	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to create file��\n");
		return false;
	}
	fseek(file, BLOCK_SIZE, SEEK_SET);
	fread(inodeBP, sizeof(inodeBP), 1, file);
	fclose(file);
	int column, row;                
	char temper;
	int inode_no;                    
	for (column = 0; column < 16; column++)
	{
		for (row = 7; row >= 0; row--)
		{
			temper = inodeBP[column];
			if (((temper >> row) & 0x01) == 0)           
			{
				inode_no = column * 8 + (7 - row);
				inodeBP[column] = inodeBP[column] | (0x01 << row);  
				file = fopen(filesystemname, "rb+");
				fseek(file, BLOCK_SIZE, SEEK_SET);
				fwrite(inodeBP, sizeof(inodeBP), 1, file);
				fclose(file);
				return inode_no;
			}
		}
	}
	return -1;    
}


bool Scan_Data_Bitmap(int data_block_count)
{
	int count = 0;                 
	int column, row;                
	char temper;
	int trans;
	memset(spare_datablk_num, 0, sizeof(spare_datablk_num));

	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to open file��\n");
		return false;
	}
	fseek(file, BLOCK_SIZE * 18, SEEK_SET);
	fread(dataBP, sizeof(dataBP), 1, file);

	for (column = 0; column < 64; column++)   
	{
		for (row = 7; row >= 0; row--)       
		{
			temper = dataBP[column];
			if (((temper >> row) & 0x01) == 0)          
			{
				trans = column * 8 + (7 - row);           
				spare_datablk_num[count++] = trans + 19;   
				dataBP[column] = dataBP[column] | (0x01 << row);   
				if (count == data_block_count)
				{
					file = fopen(filesystemname, "rb+");
					fseek(file, BLOCK_SIZE * 18, SEEK_SET);
					fwrite(dataBP, sizeof(dataBP), 1, file);
					fclose(file);
					return true;
				}
			}
		}
	}
	if (count < data_block_count)
	{
		return false;   
	}
}



bool Fill_Index_In_Datablk(int indexblock_no, int buf_block_count)
{
	int i;
	memset(b_index, 0, sizeof(b_index));

	if (Scan_Data_Bitmap(buf_block_count))         
	{
		for (i = 0; i < buf_block_count; i++)      
		{
			b_index[i] = spare_datablk_num[i];
		}
		file = fopen(filesystemname, "rb+");
		if (NULL == file)
		{
			printf("Failed to open file��\n");
			return false;
		}
		fseek(file, BLOCK_SIZE*indexblock_no, SEEK_SET);
		fwrite(b_index, sizeof(b_index), 1, file);
		fclose(file);
	}
	else
	{
		return false;
	}
	return true;
}

void Spill(char big[], char small[], int data_count, int size)
{
	int i, j;
	i = (data_count - 1)*size;
	j = 0;
	for (; big[i] && j<size; i++, j++)
	{
		small[j] = big[i];
	}
	small[j] = 0;
}

//�ļ��������������룺�ļ�·��  ������ļ��������ڵ��
int Lookup_File(char *path)
{
	int i, j;
	char filename[20];        //��ʱ����ļ���
	int index_node_num;      //�ļ���Ӧ�������ڵ��
	bool found;              //·���Ƿ��д�
	int file_count = 0;       //��·�����ж��ٸ��ļ���
	int file_start;           //��·���ָ�ʱ��¼�ϵ��
	inode_t inode;            //���������ڵ���ҵ���Ӧ�������ڵ��
	int dir_blk_num;         //Ŀ¼�ļ���Ӧ�Ĵ��̿��
	for (i = 0; path[i]; i++)
		if (path[i] == '/')
			file_count++;


	if (path[0] != '/')
	{
		return -1;
	}

	file_start = 1;
	index_node_num = 0;//��Ŀ¼�������ڵ�Ϊ0
	for (j = 0; j<file_count; j++)
	{
		found = false;
		for (i = file_start; path[i] != '/' && path[i]; i++)
			filename[i - file_start] = path[i];
		filename[i - file_start] = 0;
		file_start = i + 1;
		inode = Find_Inode(index_node_num);
		dir_blk_num = inode.disk_address[0];

		Clear_Dir_Buf();
		file = fopen(filesystemname, "rb+");
		if (NULL == file)
		{
			printf("Failed to open file��\n");
			return -1;
		}
		fseek(file, BLOCK_SIZE*dir_blk_num, SEEK_SET);
		fread(DirBuf, sizeof(DirBuf), 1, file);
		fclose(file);
		for (i = 0; DirBuf[i].name[0]; i++)
		{
			if (strcmp(DirBuf[i].name, filename) == 0)
			{
				index_node_num = DirBuf[i].ino;
				found = true;
				break;
			}
		}
		if (false == found)
		{
			printf("Error!\n");
			return -1;
		}
	}
	return index_node_num;
}

//�����ļ���֧�ֵ�ǰ�ļ����µ�ǰ�ļ���Ҳ֧��·���ļ�����
bool Copy_File(char filename1[], char filename2[])
{
	int block_num;         //��������ָ��������ڵ��������̿��(2#~17#)
	int inode_num;         //�������ڵ�ָ��������ڵ��ڴ������λ��(0~7)
	int file_index;        //�ļ���Ӧ��������
	int i, j;
	//��ȡ��һ����������Ӧ���ļ��������ڵ��
	if (isPath(filename1))
	{
		file_index = Lookup_File(filename1);
	}
	else
	{
		Lookup_Dir(cur_inode_no);
		for (i = 0; DirBuf[i].name[0]; i++)
		{
			if (strcmp(DirBuf[i].name, filename1) == 0)
				file_index = DirBuf[i].ino;
		}
	}
	if (!Read_File(file_index))
	{
		printf("Can't read the directory!\n");
		return false;
	}
	//��ȡ�ڶ�����������Ӧ���ļ��������ڵ��
	int dir_file_index;                //Ŀ¼�ļ��������ڵ��
	char file_name[20];                //�ļ���
	char path[20];                     //�ļ���ŵ�·��
	int middle;                       //�ֽ�
	int count = 0;
	i = 0;
	j = 0;
	if (isPath(filename2))
	{
		while (filename2[i])
		{
			if (filename2[i] == '/')
				count++;
			i++;
		}
		for (i = 0; filename2[i]; i++)
		{
			if (filename2[i] == '/')
				j++;
			if (j == count)
				break;
		}
		middle = i;              
		for (i = 0; i<middle; i++)
			path[i] = filename2[i];
		path[i] = 0;
		for (j = 0, i = middle + 1; filename2[i]; i++, j++)
			file_name[j] = filename2[i];
		file_name[j] = 0;
		dir_file_index = Lookup_File(path);   //����·�����õ������������ļ�Ӧ��ŵ��ļ�Ŀ¼��������
	}
	else
	{
		dir_file_index = cur_inode_no;
		strcpy(file_name, filename2);
	}
	if (isLegal(dir_file_index, file_name))
	{
		Create_File(file_name, dir_file_index);    //�������ļ��ļ������ļ�Ӧ�ڵ�Ŀ¼��Ŀ¼�ļ���������
	}
	else
	{
		printf("\tError!\nThe file has existed!\n");
		return false;
	}
	return true;
}

//�жϸò������ļ�������·��
bool isPath(char *name)
{
	int i = 0;
	while (name[i])
	{
		if (name[i] == '/')     //�ļ����в����ڡ�/��
			return true;
		i++;
	}
	return false;
}

//����Ŀ¼������Ŀ¼�ļ��ڵ㣬����ǽ�Ŀ¼�ڵ��Ӧ��Ŀ¼���뵽ȫ�ֱ���DirBuf[]��,ͬʱ����Ŀ¼�ļ����ڵĴ��̺ŷ���
int Lookup_Dir(int index_node)
{
	int block_num;         //��������ָ��������ڵ��������̿��(2#~17#)
	int inode_num;         //�������ڵ�ָ��������ڵ��ڴ������λ��(0~7)
	int dir_file_block;    //Ŀ¼�ļ���Ӧ�Ĵ��̿��
	block_num = index_node / 8 + 2;
	inode_num = index_node % 8;
	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to open file��\n");
		return false;
	}

	fseek(file, BLOCK_SIZE * block_num, SEEK_SET);
	fread(iNode, sizeof(iNode), 1, file);
	fclose(file);
	dir_file_block = iNode[inode_num].disk_address[0];     //�õ�Ŀ¼�ļ���Ӧ�Ĵ��̺�
	Clear_Dir_Buf();
	file = fopen(filesystemname, "rb+");
	fseek(file, BLOCK_SIZE*dir_file_block, SEEK_SET);
	fread(DirBuf, sizeof(DirBuf), 1, file);                //��Ŀ¼�ļ��е����ݶ��뻺��DirBuf��
	fclose(file);
	return dir_file_block;
}

//��ȡ�ļ��������ļ��������ڵ�ţ�����Ӧ���ļ������ݶ�ȡ���ڴ滺��Buffer��
bool Read_File(int file_index)
{

	int block_num;         //��������ָ��������ڵ��������̿��(2#~17#)
	int inode_num;         //�������ڵ�ָ��������ڵ��ڴ������λ��(0~7)
	int file_size;         //�����ڵ�ָʾ���ļ��Ĵ�С����λ���ֽ���
	int file_block;        //���ļ���С��������ļ���ӵ�еĴ������ĸ���
	block_num = file_index / 8 + 2;
	inode_num = file_index % 8;
	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to open file��\n");
		return false;
	}

	fseek(file, BLOCK_SIZE * block_num, SEEK_SET);
	fread(iNode, sizeof(iNode), 1, file);
	fclose(file);

	if (iNode[inode_num].type == 0)          //����������ļ���һ��Ŀ¼�ļ�ʱ����ȡʧ�ܣ�
		return false;
	file_size = iNode[inode_num].size;
	file_block = file_size / 512;
	if (file_size % 512 != 0)
	{
		file_block++;
	}
	
	int i, j;
	int block_index;
	if (file_block <= 10)
	{
		for (i = 0; i<file_block; i++)
		{
			block_index = iNode[inode_num].disk_address[i];
			Get_Data(block_index);
			Merge(i);
		}
	}
	//���̿���Ŀ������128ʱ,����һ����ַ
	else if (file_block <= 128)
	{
		block_index = iNode[inode_num].disk_address[10];
		file = fopen(filesystemname, "rb+");
		fseek(file, BLOCK_SIZE * block_index, SEEK_SET);
		fread(b_index, sizeof(b_index), 1, file);                    //�Ƚ��������������ݶ�ȡ����
		fclose(file);
		for (i = 0; i < file_block; i++)
		{
			block_index = b_index[i];                          //�������������е���õ��ļ��ĸ������̿��
			Get_Data(block_index);                           //��ȡ�������̿飬��ȡ���е�����
			Merge(i);                                        //�������̿�����ݺϲ�����Buffer��
		}
	}
	//���̿���Ŀ������256ʱ,��������һ����ַ
	else if (file_block <= 128 * 2)
	{
		block_index = iNode[inode_num].disk_address[10];
		file = fopen(filesystemname, "rb+");
		fseek(file, BLOCK_SIZE * block_index, SEEK_SET);
		fread(b_index, sizeof(b_index), 1, file);                    //�Ƚ��������������ݶ�ȡ����
		fclose(file);
		for (i = 0; i < 128; i++)
		{
			block_index = b_index[i];                          //�������������е���õ��ļ��ĸ������̿��
			Get_Data(block_index);                           //��ȡ�������̿飬��ȡ���е�����
			Merge(i);                                        //�������̿�����ݺϲ�����Buffer��
		}

		block_index = iNode[inode_num].disk_address[11];
		file = fopen(filesystemname, "rb+");
		fseek(file, BLOCK_SIZE * block_index, SEEK_SET);
		fread(b_index, sizeof(b_index), 1, file);                    //�Ƚ��������������ݶ�ȡ����
		fclose(file);
		for (i = 0; i < file_block - 128; i++)
		{
			block_index = b_index[i];                          //�������������е���õ��ļ��ĸ������̿��
			Get_Data(block_index);                           //��ȡ�������̿飬��ȡ���е�����
			Merge(i + 128);                                        //�������̿�����ݺϲ�����Buffer��
		}
	}
	//���򣬲���һ��������ַ
	else
	{

		int temp_len;
		int temp[128];
		int buffer_part = 0;
		block_index = iNode[inode_num].disk_address[12];
		file = fopen(filesystemname, "rb+");
		fseek(file, BLOCK_SIZE * block_index, SEEK_SET);
		fread(b_index, sizeof(b_index), 1, file);                    //�Ƚ��������������ݶ�ȡ����
		fclose(file);
		for (i = 0; b_index[i] != 0; i++)
		{
			temp[i] = b_index[i];
		}
		temp_len = i;
		for (i = 0; i<temp_len - 1; i++)
		{
			block_index = temp[i];
			file = fopen(filesystemname, "rb+");
			fseek(file, BLOCK_SIZE * block_index, SEEK_SET);
			fread(b_index, sizeof(b_index), 1, file);                    //�Ƚ��������������ݶ�ȡ����
			fclose(file);
			for (j = 0; j < 128; j++)
			{
				block_index = b_index[j];                          //�������������е���õ��ļ��ĸ������̿��
				Get_Data(block_index);                           //��ȡ�������̿飬��ȡ���е�����
				Merge(buffer_part);                              //�������̿�����ݺϲ�����Buffer��
				buffer_part++;
			}
		}
		block_index = temp[i];
		file = fopen(filesystemname, "rb+");
		fseek(file, BLOCK_SIZE * block_index, SEEK_SET);
		fread(b_index, sizeof(b_index), 1, file);                    //�Ƚ��������������ݶ�ȡ����
		fclose(file);
		for (j = 0; j < file_block - 128 * (temp_len - 1); j++)
		{
			block_index = b_index[j];                          //�������������е���õ��ļ��ĸ������̿��
			Get_Data(block_index);                           //��ȡ�������̿飬��ȡ���е�����
			Merge(buffer_part);                              //�������̿�����ݺϲ�����Buffer��
			buffer_part++;
		}
	}

	return true;
}

//ͨ�����̿�ţ��������ϵ����ݶ��뻺����Block_Data��
bool Get_Data(int block_index)
{
	memset(Block_Data, 0, sizeof(Block_Data));
	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("���ļ�ʧ�ܣ�\n");
		return false;
	}
	fseek(file, BLOCK_SIZE*block_index, SEEK_SET);
	fread(Block_Data, sizeof(Block_Data), 1, file);

	fclose(file);
}



bool Merge(int count)
{
	int i, j;
	j = count * BLOCK_SIZE;
	for (i = 0; Block_Data[i] != 0; i++, j++)
		Buffer[j] = Block_Data[i];
	Buffer[j] = 0;                             //������ܳ��ֵ���β��
	return true;
}

bool Copy_File_out(char filename1[], char filename2[])
{
	int file_index;        //�ļ���Ӧ��������
	bool find;
	int i, j;
	//��ȡ��һ����������Ӧ���ļ��������ڵ��
	if (isPath(filename1))
	{
		file_index = Lookup_File(filename1);
		if (file_index == -1)
		{
			printf("Error!\n");
			return false;
		}
	}
	else
	{
		Lookup_Dir(cur_inode_no);
		find = false;
		for (i = 0; DirBuf[i].name[0]; i++)
		{
			if (strcmp(DirBuf[i].name, filename1) == 0)
			{
				file_index = DirBuf[i].ino;
				find = true;
			}
		}
		if (find == false)
		{
			printf("Error��\n");
			return false;
		}
	}
	if (!Read_File(file_index))                      //������1ָ�����ļ��е����ݶ�ȡ��Buffer��
	{
		printf("Can't read the directory!\n");
		return false;
	}
	file = fopen(filename2, "w");
	if (NULL == file)
	{
		printf("Failed to creat file��\n");
		return false;
	}
	fseek(file, 0, SEEK_SET);
	fwrite(Buffer, strlen(Buffer), 1, file);
	fclose(file);
	return true;
}

bool Copy_File_In(char filename1[], char filename2[])
{
	file = fopen(filename1, "rb+");
	if (NULL == file)
	{
		printf("Failed to creat file��\n");
		return false;
	}
	fseek(file, 0, SEEK_SET);
	fread(Buffer, sizeof(Buffer), 1, file);        //����������ļ��е����ݶ���Buffer��
	fclose(file);

	//��ȡ�ڶ�����������Ӧ���ļ��������ڵ��
	int dir_file_index;                //Ŀ¼�ļ��������ڵ��
	char file_name[20];                //�ļ���
	char path[20];                     //�ļ���ŵ�·��
	int middle;                       //�ֽ�
	int count = 0;
	int i = 0;
	int j = 0;
	if (isPath(filename2))
	{
		while (filename2[i])
		{
			if (filename2[i] == '/')
				count++;
			i++;
		}
		for (i = 0; filename2[i]; i++)
		{
			if (filename2[i] == '/')
				j++;
			if (j == count)
				break;
		}
		middle = i;              
		for (i = 0; i<middle; i++)
			path[i] = filename2[i];
		path[i] = 0;
		for (j = 0, i = middle + 1; filename2[i]; i++, j++)
			file_name[j] = filename2[i];
		file_name[j] = 0;
		dir_file_index = Lookup_File(path);   //����·�����õ������������ļ�Ӧ��ŵ��ļ�Ŀ¼��������
	}
	else
	{
		dir_file_index = cur_inode_no;
		strcpy(file_name, filename2);
	}
	if (isLegal(dir_file_index, file_name))
	{
		Create_File(file_name, dir_file_index);    //�������ļ��ļ������ļ�Ӧ�ڵ�Ŀ¼��Ŀ¼�ļ���������
	}
	else
	{
		printf("\tError!\nThe file has existed!\n");
		return false;
	}
	return true;
}

bool Delete_File(char filename[])
{
	int i, j;
	bool find;
	int file_index;                         //��ɾ�����ļ���������
	int father_block;                       //��ǰ�ļ������ڵĴ��̿�
	father_block = Lookup_Dir(cur_inode_no);

	find = false;
	for (i = 0; DirBuf[i].name[0]; i++)
	{
		if (strcmp(DirBuf[i].name, filename) == 0)   //�ڵ�ǰĿ¼���ҵ����ļ����������ڸ�Ŀ¼�ļ���ɾ����ͬʱ����Ӧ���ļ������ڵ����
		{
			file_index = DirBuf[i].ino;
			find = true;
			for (j = i; DirBuf[j].name[0]; j++)
			{
				strcpy(DirBuf[j].name, DirBuf[j + 1].name);
				DirBuf[j].ino = DirBuf[j + 1].ino;
			}
			file = fopen(filesystemname, "rb+");
			if (NULL == file)
			{
				printf("Failed to open file��\n");
				return false;
			}
			fseek(file, BLOCK_SIZE*father_block, SEEK_SET);
			fwrite(DirBuf, sizeof(DirBuf), 1, file);
			fclose(file);
			break;
		}
	}
	if (find == false)
	{
		printf("Can't find the file to be deleted!\n");
		return false;
	}
	
	Delete_From_Inode_Bitmap(file_index);

	
	int block_num;         //��������ָ��������ڵ��������̿��(2#~17#)
	int inode_num;         //�������ڵ�ָ��������ڵ��ڴ������λ��(0~7)

	int file_size;        //�����ļ��Ĵ�С����λΪ�ֽ�
	int file_block;       //��¼�ļ���ռ�����ݿ���Ŀ
	block_num = file_index / 8 + 2;
	inode_num = file_index % 8;

	file = fopen(filesystemname, "rb+");
	fseek(file, BLOCK_SIZE*block_num, SEEK_SET);
	fread(iNode, sizeof(iNode), 1, file);
	fclose(file);

	file_size = iNode[inode_num].size;
	file_block = file_size / 512;
	if (file_size % 512 != 0)
		file_block++;
	int block_index;
	if (file_block <= 10)
	{
		for (i = 0; i<file_block; i++)
		{
			block_index = iNode[inode_num].disk_address[i];     //ÿ�λ��һ�����ݿ�������ڵ�
			Delete_From_Data_Bitmap(block_index);               //����������ݿ�������ڵ�ţ������ݿ�λʾͼ����Ӧ��bitλ��0
		}
	}
	else if (file_block <= 128)
	{
		block_index = iNode[inode_num].disk_address[10];
		file = fopen(filesystemname, "rb+");
		fseek(file, BLOCK_SIZE*block_index, SEEK_SET);
		fread(b_index, sizeof(b_index), 1, file);
		fclose(file);
		Delete_From_Data_Bitmap(block_index);               //���������ݿ������֮�󣬱�����ͷ��������ݿ����������
		for (i = 0; i<file_block; i++)
		{
			block_index = b_index[i];
			Delete_From_Data_Bitmap(block_index);               //����������ݿ�������ڵ�ţ������ݿ�λʾͼ����Ӧ��bitλ��0
		}
	}
	else if (file_block <= 128 * 2)
	{
		
		block_index = iNode[inode_num].disk_address[10];
		file = fopen(filesystemname, "rb+");
		fseek(file, BLOCK_SIZE*block_index, SEEK_SET);
		fread(b_index, sizeof(b_index), 1, file);
		fclose(file);
		Delete_From_Data_Bitmap(block_index);               //���������ݿ������֮�󣬱�����ͷ��������ݿ����������
		for (i = 0; i<128; i++)
		{
			block_index = b_index[i];
			Delete_From_Data_Bitmap(block_index);               //����������ݿ�������ڵ�ţ������ݿ�λʾͼ����Ӧ��bitλ��0
		}
		
		block_index = iNode[inode_num].disk_address[11];
		file = fopen(filesystemname, "rb+");
		fseek(file, BLOCK_SIZE*block_index, SEEK_SET);
		fread(b_index, sizeof(b_index), 1, file);
		fclose(file);
		Delete_From_Data_Bitmap(block_index);               //���������ݿ������֮�󣬱�����ͷ��������ݿ����������
		for (i = 0; i<file_block - 128; i++)
		{
			block_index = b_index[i];
			Delete_From_Data_Bitmap(block_index);               //����������ݿ�������ڵ�ţ������ݿ�λʾͼ����Ӧ��bitλ��0
		}
	}
	else
	{
		
		int temp[128];
		int temp_len;
		block_index = iNode[inode_num].disk_address[12];
		file = fopen(filesystemname, "rb+");
		fseek(file, BLOCK_SIZE*block_index, SEEK_SET);
		fread(b_index, sizeof(b_index), 1, file);
		fclose(file);
		for (i = 0; b_index[i]; i++)
			temp[i] = b_index[i];
		temp_len = i;
		Delete_From_Data_Bitmap(block_index);               //���������ݿ������֮�󣬱�����ͷ��������ݿ����������
		for (i = 0; i<temp_len - 1; i++)
		{
			block_index = temp[i];                         //���ҵ�i������������
			file = fopen(filesystemname, "rb+");
			fseek(file, BLOCK_SIZE*block_index, SEEK_SET);
			fread(b_index, sizeof(b_index), 1, file);
			fclose(file);
			Delete_From_Data_Bitmap(block_index);           //���������ݿ������֮�󣬱�����ͷ��������ݿ����������
			for (j = 0; j<128; j++)
			{
				block_index = b_index[j];
				Delete_From_Data_Bitmap(block_index);      //�ͷŶ����������Ӧ�����ݿ�
			}
		}
		
		block_index = temp[i];                         //�������һ������������
		file = fopen(filesystemname, "rb+");
		fseek(file, BLOCK_SIZE*block_index, SEEK_SET);
		fread(b_index, sizeof(b_index), 1, file);
		fclose(file);
		Delete_From_Data_Bitmap(block_index);           //���������ݿ������֮�󣬱�����ͷ��������ݿ����������
		for (j = 0; j<file_block - 128 * (temp_len - 1); j++)
		{
			block_index = b_index[j];
			Delete_From_Data_Bitmap(block_index);      //�ͷŶ����������Ӧ�����ݿ�
		}
	}
	fclose(file);
	return true;
}

//���ݸ������ļ��������ڵ㣬�������ڵ�λʾͼ�н���Ӧ��bitλ��0
bool Delete_From_Inode_Bitmap(int file_index)
{
	int column, row;
	row = file_index / 8;
	column = file_index % 8;
	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to open file��\n");
		return false;
	}
	fseek(file, BLOCK_SIZE, SEEK_SET);
	fread(inodeBP, sizeof(inodeBP), 1, file);
	fclose(file);
	switch (column)
	{
	case 0:
		inodeBP[row] = inodeBP[row] & 0x7f;
		break;
	case 1:
		inodeBP[row] = inodeBP[row] & 0xbf;
		break;
	case 2:
		inodeBP[row] = inodeBP[row] & 0xdf;
		break;
	case 3:
		inodeBP[row] = inodeBP[row] & 0xef;
		break;
	case 4:
		inodeBP[row] = inodeBP[row] & 0xf7;
		break;
	case 5:
		inodeBP[row] = inodeBP[row] & 0xfb;
		break;
	case 6:
		inodeBP[row] = inodeBP[row] & 0xfd;
		break;
	case 7:
		inodeBP[row] = inodeBP[row] & 0xfe;
		break;
	}
	file = fopen(filesystemname, "rb+");
	fseek(file, BLOCK_SIZE, SEEK_SET);
	fwrite(inodeBP, sizeof(inodeBP), 1, file);
	fclose(file);
	return true;
}

//���ݸ����Ĵ��̵������ڵ㣬�����ݿ�λʾͼ�н���Ӧ��bitλ��0
bool Delete_From_Data_Bitmap(int block_index_no)
{
	int column, row;
	int block_index = block_index_no - 19;
	row = block_index / 8;
	column = block_index % 8;
	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to open file��\n");
		return false;
	}
	fseek(file, BLOCK_SIZE * 18, SEEK_SET);
	fread(dataBP, sizeof(dataBP), 1, file);
	fclose(file);
	switch (column)
	{
	case 0:
		dataBP[row] = dataBP[row] & 0x7f;
		break;
	case 1:
		dataBP[row] = dataBP[row] & 0xbf;
		break;
	case 2:
		dataBP[row] = dataBP[row] & 0xdf;
		break;
	case 3:
		dataBP[row] = dataBP[row] & 0xef;
		break;
	case 4:
		dataBP[row] = dataBP[row] & 0xf7;
		break;
	case 5:
		dataBP[row] = dataBP[row] & 0xfb;
		break;
	case 6:
		dataBP[row] = dataBP[row] & 0xfd;
		break;
	case 7:
		dataBP[row] = dataBP[row] & 0xfe;
		break;
	}
	file = fopen(filesystemname, "rb+");
	fseek(file, BLOCK_SIZE * 18, SEEK_SET);
	fwrite(dataBP, sizeof(dataBP), 1, file);
	fclose(file);
	return true;
}

bool Clear_Dir_Buf()
{
	int i;
	for (i = 0; i<32; i++)
		memset(DirBuf[i].name, 0, sizeof(DirBuf[i].name));
	return true;
}



//����һ�����ļ����ļ��������û��Լ�����
bool New_file(char filename[])
{
	if (!isLegal(cur_inode_no, filename))
	{
		printf("Error!\n");
		return false;
	}
	char selection;
	int count = 0;
	int i;
	char temp[100];
	printf("print:(end with##)\n");
	gets(temp);
	while (!(temp[0] == '#' && temp[1] == '#'))
	{
		for (i = 0; temp[i]; i++)
			Buffer[count++] = temp[i];
		Buffer[count++] = 10;
		gets(temp);
	}
	count--;
	Buffer[count] = 0;
	Create_File(filename, cur_inode_no);
	return true;
}

//��ָ����Ŀ¼�£��жϸ��ļ����Ƿ�Ϸ�
bool isLegal(int dir_index, char filename[])
{
	Lookup_Dir(dir_index);
	int i;
	for (i = 2; DirBuf[i].name[0]; i++)
	{
		if (strcmp(DirBuf[i].name, filename) == 0)
			return false;
	}
	return true;
}


bool Show_Dir()          
						
{
	int size;                                   //��ʾ���ļ��Ĵ�С
	int type;                                   //��ʾ���ļ�������
	inode_t inode;                              //�����������ļ������ڵ㣬��ȡ�ļ������ڵ������
	char str1[50] = "Common file";
	char str2[50] = "Directory file";
	char str[50];
	inode = Find_Inode(cur_inode_no);
	int diskNo;
	int i;
	diskNo = inode.disk_address[0];      //Ĭ��Ŀ¼�ļ�ֻռ��һ�����̲��Ҹô��̺Ŵ���ڵ�ַ��ĵ�һ����ַ����

	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to open file��\n");
		return false;
	}
	Clear_Dir_Buf();
	fseek(file, BLOCK_SIZE*diskNo, SEEK_SET);
	fread(DirBuf, sizeof(DirBuf), 1, file);//�ҵ���ǰĿ¼��Ӧ�Ĵ��̿�󣬽����������ӡ�ô��̿���ȫ������
	fclose(file);


	int j;
	int name_len;
	int size_len;
	int len;
	i = 0;
	printf("inode_no   file name    file size  file type\n");
	while (DirBuf[i].name[0])
	{
		size_len = 0;
		inode = Find_Inode(DirBuf[i].ino);
		size = inode.size;
		type = inode.type;
		if (type == 0)
			strcpy(str, str2);
		else
			strcpy(str, str1);
		name_len = strlen(DirBuf[i].name);
		printf("%d          %s", DirBuf[i].ino, DirBuf[i].name);
		for (j = 0; j <= 12 - name_len; j++)
			printf(" ");
		printf("%d", size);
		len = size;
		if (len == 0)
			size_len++;
		while (len > 0)
		{
			len = len / 10;
			size_len++;
		}
		for (j = 0; j <= 13 - size_len; j++)
			printf(" ");
		printf("%s\n", str);
		i++;
	}
	return true;
}


//����һ��Ŀ¼,֧���ڵ�ǰĿ¼�´���һ����Ŀ¼��Ҳ֧�ָ���һ��·������ָ��·���´�����Ŀ¼
bool CreateDir(char dir_path[])
{
	int dir_index;                //Ŀ¼�ļ�������
	char path[50];                //����·��
	char dir_name[50];            //��������Ŀ¼�ļ���
	int middle;                       //�ֽ�
	int count = 0;
	int i = 0;
	int j = 0;
	if (isPath(dir_path))
	{
		while (dir_path[i])
		{
			if (dir_path[i] == '/')
				count++;
			i++;
		}
		for (i = 0; dir_path[i]; i++)
		{
			if (dir_path[i] == '/')
				j++;
			if (j == count)
				break;
		}
		middle = i;              
		for (i = 0; i<middle; i++)
			path[i] = dir_path[i];
		path[i] = 0;
		for (j = 0, i = middle + 1; dir_path[i]; i++, j++)
			dir_name[j] = dir_path[i];
		dir_name[j] = 0;
		dir_index = Lookup_File(path);
	}
	else
	{
		dir_index = cur_inode_no;
		strcpy(dir_name, dir_path);
	}
	if (!MakeDir(dir_index, dir_name))
		return false;
	return true;
}


bool MakeDir(int dir_index, char dir_name[])
{
	int inode_no;                 //���������������Ŀ¼�ļ��������ڵ��
	int dir_block_no;             //���������������Ŀ¼�ļ��Ĵ��̿��
	int block_num;         //��������ָ��������ڵ��������̿��(2#~17#)
	int inode_num;         //�������ڵ�ָ��������ڵ��ڴ������λ��(0~7)
	int father_dir_blk;         //�½����ļ��еĸ��ļ������ڵĴ��̿��
	int i, j;

	inode_no = Scan_Inode_Bitmap();
	if (-1 == inode_no)
	{
		printf("The index code has run out!\nCreate failed!\n");
		return false;
	}

	block_num = inode_no / 8 + 2;
	inode_num = inode_no % 8;


	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to open file��\n");
		return false;
	}
	fseek(file, BLOCK_SIZE*block_num, SEEK_SET);
	fread(iNode, sizeof(iNode), 1, file);
	fclose(file);
	iNode[inode_num].type = 0;                           //�������ļ���Ŀ¼�ļ�
	iNode[inode_num].size = 0;                          //Ŀ¼�ļ���size�ֶβ�������

	Scan_Data_Bitmap(1);
	dir_block_no = spare_datablk_num[0];
	iNode[inode_num].disk_address[0] = dir_block_no;
	file = fopen(filesystemname, "rb+");
	fseek(file, BLOCK_SIZE*block_num, SEEK_SET);
	fwrite(iNode, sizeof(iNode), 1, file);
	fclose(file);


	strcpy(DirBuf[0].name, ".");
	DirBuf[0].ino = inode_no;
	strcpy(DirBuf[1].name, "..");
	DirBuf[1].ino = dir_index;
	for (i = 2; i<32; i++)
		memset(DirBuf[i].name, 0, sizeof(DirBuf[i].name));
	//д���ļ�
	file = fopen(filesystemname, "rb+");
	fseek(file, BLOCK_SIZE*dir_block_no, SEEK_SET);
	fwrite(DirBuf, sizeof(DirBuf), 1, file);
	fclose(file);

	father_dir_blk = Lookup_Dir(dir_index);
	i = 0;
	while (DirBuf[i].name[0])
		i++;
	strcpy(DirBuf[i].name, dir_name);
	DirBuf[i].ino = inode_no;
	//д���ļ�
	file = fopen(filesystemname, "rb+");
	fseek(file, BLOCK_SIZE*father_dir_blk, SEEK_SET);
	fwrite(DirBuf, sizeof(DirBuf), 1, file);
	fclose(file);
	return true;
}

//ɾ���ļ���
bool DeleteDir(char dir_name[])
{
	int i, j;
	int father_block;                               //��ɾ�����ļ������ڵ��ļ��е�Ŀ¼�ļ����ڵĴ��̺�
	int dir_index;                                 //���ڴ�Ŵ�ɾ�����ļ��е�Ŀ¼�ļ�������
	int mark;                                      //�����ļ�����תʱ��¼�ļ����������
	father_block = Lookup_Dir(cur_inode_no);       //����ǰĿ¼ȫ����ȡ���ڴ���

	for (i = 2; DirBuf[i].name[0]; i++)
	{
		if (strcmp(DirBuf[i].name, dir_name) == 0)
		{
			dir_index = DirBuf[i].ino;

			for (j = i; DirBuf[j].name[0]; j++)
			{
				strcpy(DirBuf[j].name, DirBuf[j + 1].name);
				DirBuf[j].ino = DirBuf[j + 1].ino;
			}
			file = fopen(filesystemname, "rb+");
			if (NULL == file)
			{
				printf("Failed to open file��\n");
				return false;
			}
			fseek(file, BLOCK_SIZE*father_block, SEEK_SET);
			fwrite(DirBuf, sizeof(DirBuf), 1, file);
			fclose(file);
			break;
		}
	}

	mark = cur_inode_no;
	cur_inode_no = dir_index;
	Lookup_Dir(cur_inode_no);
	for (i = 2; DirBuf[i].name[0]; i++)
	{
		if (GetType(DirBuf[i].ino) == 1)   //�ļ���������ͨ�ļ�
		{
			Delete_File(DirBuf[i].name);
			Lookup_Dir(cur_inode_no);
			i = 1;
		}
		else if (GetType(DirBuf[i].ino) == 0)
		{
			DeleteDir(DirBuf[i].name);
			Lookup_Dir(cur_inode_no);
			i = 1;
		}
		else
			return false;
	}
	cur_inode_no = mark;

	Delete_From_Inode_Bitmap(dir_index);

	int block_num;
	int inode_num;
	int dir_block;
	char data[512];
	block_num = dir_index / 8 + 2;
	inode_num = dir_index % 8;
	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to open file��\n");
		return false;
	}
	fseek(file, BLOCK_SIZE*block_num, SEEK_SET);
	fread(iNode, sizeof(iNode), 1, file);
	fclose(file);
	dir_block = iNode[inode_num].disk_address[0];

	memset(data, 0, sizeof(data));
	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to open file��\n");
		return false;
	}
	fseek(file, BLOCK_SIZE*dir_block, SEEK_SET);
	fwrite(data, sizeof(data), 1, file);
	fclose(file);

	Delete_From_Data_Bitmap(dir_block);
	return true;
}

//�����ļ������ڵ���ж��ļ����ͣ��������ͨ�ļ����򷵻�1�������Ŀ¼�ļ����򷵻�0
int GetType(int file_index)
{
	int block_num;
	int inode_num;
	block_num = file_index / 8 + 2;
	inode_num = file_index % 8;

	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to open file��\n");
		return false;
	}
	fseek(file, BLOCK_SIZE*block_num, SEEK_SET);
	fread(iNode, sizeof(iNode), 1, file);
	fclose(file);
	return iNode[inode_num].type;
}
