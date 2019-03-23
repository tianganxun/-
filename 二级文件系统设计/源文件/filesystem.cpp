#include "filesystem.h"


FILE *file;            //文件指针，用于各种对文件的操作
char filesystemname[20];           //创建的磁盘文件名
super_block_t  file_system_super;    //声明结构体变量
char inodeBP[INODE_NUM / 8];         //索引节点位示图，总位数=8(char)*16=128bit,所以一共可以表示128个索引节点
char dataBP[DATA_BLOCK_NUM / 8];    //数据块位示图，总位数=8(char)*64=512bit,所以一共可以标识521个数据块
directory DirBuf[BLOCK_SIZE / sizeof(directory)];   //目录数组，表示一个磁盘块可以存放32个目录项
struct inode_t iNode[BLOCK_SIZE / sizeof(inode_t)];                              //索引节点


char CurDir[50];                     //记录当前在那个文件夹中
int cur_inode_no;                     //记载当前所在的目录的索引节点
int spare_datablk_num[512];           //记录空闲数据块的块号，用于分配数据块
int b_index[128];                             //间址时用于再寻址的中间磁盘块

char Buffer[1048576];                //1M的缓冲内存空间
char Block_Data[513];                //用于存放一个磁盘块中的数据的临时空间

									 //格式化磁盘文件，初始化其中的数据
bool Format()
{
	int i;
	file = fopen(filesystemname, "wb");//创建虚拟磁盘文件
	if (NULL == file)
	{
		printf("Failed to create a virtual disk file！");
		return false;
	}

	//初始化超级块
	file_system_super.magic = 0;
	file_system_super.total_block_count = TOTAL_BLOCK_NUMBER;          //文件系统的总磁盘数
	file_system_super.inode_count = INODE_NUM;                         //文件系统的总索引节点数
	file_system_super.data_block_count = DATA_BLOCK_NUM;               //文件系统的总数据块数目

	file_system_super.inode_table_bitmap.start_block = INODE_BITMAP_START;                //索引节点位示图开始磁盘块
	file_system_super.inode_table_bitmap.block_count = INODE_BITMAP_BKNUM;                //索引节点位示图占据磁盘块数
	file_system_super.inode_table_bitmap.byte_count = INODE_NUM / 8;                       //索引节点位示图占用的字节个数 

	file_system_super.inode_table.start_block = INODE_BLOCK_START;                       //索引节点开始磁盘块
	file_system_super.inode_table.block_count = INODE_BLOCK_NUM;                         //索引节点占据磁盘块数
	file_system_super.inode_table.byte_count = INODE_SIZE * INODE_NUM_PER_BLOCK * INODE_BLOCK_NUM;//索引节点占用的字节个数 

	file_system_super.data_area_bitmap.start_block = DATA_BITMAP_START;                //数据块位示图开始磁盘块
	file_system_super.data_area_bitmap.block_count = DATA_BITMAP_BLNUM;                //数据块位示图占据磁盘块数
	file_system_super.data_area_bitmap.byte_count = DATA_BLOCK_NUM / 8;                  //数据块位示图占用的字节个数

	file_system_super.data_area.start_block = DATA_BLOCK_START;                //数据块开始磁盘块
	file_system_super.data_area.block_count = DATA_BLOCK_NUM;                 //数据块占据磁盘块数
	file_system_super.data_area.byte_count = DATA_BLOCK_NUM * BLOCK_SIZE;     //数据块占用的字节个数

	fseek(file, 0L, SEEK_SET);
	fwrite(&file_system_super, BLOCK_SIZE, 1, file);

	//初始化索引节点位示图
	inodeBP[0] = 0x80;           
	for (i = 1; i < INODE_NUM / 8; i++)
	{
		inodeBP[i] = 0;
	}
	fseek(file, BLOCK_SIZE, SEEK_SET);
	fwrite(inodeBP, sizeof(inodeBP), 1, file);

	//初始化索引节点
    iNode[0].type = 0;                 //表示该文件是目录文件
	iNode[0].size = 0;                 //目录文件的size都是0
	iNode[0].disk_address[0] = 0 + DATA_BLOCK_START;      //根目录所占据的数据块号<数据块号从0#到511#>，对应于磁盘号还要加上19
	fseek(file, BLOCK_SIZE * 2, SEEK_SET);
	fwrite(&iNode, sizeof(iNode), 1, file);

	//初始化数据块位示图
	dataBP[0] = 0x80;         
	for (i = 1; i < DATA_BLOCK_NUM / 8; i++)
	{
		dataBP[i] = 0;
	}
	fseek(file, BLOCK_SIZE * 18, SEEK_SET);
	fwrite(dataBP, BLOCK_SIZE, 1, file);

	//初始化数据块
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

//装载磁盘文件系统
bool Install()
{
	int i;
	printf("Under load\n");
	file = fopen(filesystemname, "rb+");       //以只读方式打开磁盘文件
	if (NULL == file)
	{
		printf("%sDisk file opening failed！\n", filesystemname);
		return false;
	}
	//先将超级块读入内存
	fseek(file, 0L, SEEK_SET);
	fread(&file_system_super, sizeof(super_block_t), 1, file);
	//再把索引节点位示图读入内存
	fseek(file, BLOCK_SIZE, SEEK_SET);
	fread(inodeBP, sizeof(inodeBP), 1, file);
	//将索引节点读入内存中
	fseek(file, BLOCK_SIZE * 2, SEEK_SET);
	fread(iNode, sizeof(iNode), 1, file);

	//再把数据块位示图读入内存
	fseek(file, BLOCK_SIZE * 18, SEEK_SET);
	fread(dataBP, sizeof(dataBP), 1, file);

	//将根目录的目录文件读入到内存中
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
	int file_index;                     //待显示的文件索引
	int dir_file_index;                //目录文件的索引节点号
	char file_name[20];                //文件名
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
//从索引节点区将该节点对应的数据区提供给需要的函数
inode_t Find_Inode(int ino)
{
	inode_t inode;
	int i, j, k;
	i = ino / 8;                //i表示相对于索引节点开始块（INODE_BLOCK_START#磁盘块）的位置（即2+i#磁盘块）
	j = ino - 8 * i;            //j标识在该磁盘块中第一个索引节点记录

	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to open file！\n");
		
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

//当前索引节点号：cur_inode_no
bool Create_File(char file_name[], int index_node)   //对于Buffer数组，记得将具体数据后一个字节赋0，不然计算字节数时会出错
{
	int inode_no;          //存放即将分配给该文件的索引节点
	int buf_block_count;   //缓冲区数据所占数据块的个数
	int buf_byte_count;    //缓冲区数据的字节总数
	int block_num;         //该索引号指向的索引节点所处的盘块号(2#~17#)
	int inode_num;         //该索引节点指向的索引节点在磁盘里的位置(0~7)
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
	//根据索引节点号获得该索引节点的具体位置
	block_num = inode_no / 8 + 2;
	inode_num = inode_no % 8;

	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to open file！\n");
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
				
				memset(data, 0, sizeof(data));                          //先对将要写的磁盘块进行0覆盖
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
			printf("Failed to create file！\n");
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
			printf("Failed to create file！\n");
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
				memset(data, 0, sizeof(data));                          //先对将要写的磁盘块进行0覆盖
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
			printf("Failed to create file！\n");
		}

		Scan_Data_Bitmap(1);                                                
		indirect_indexblooc_no = spare_datablk_num[0];                    
		iNode[inode_num].disk_address[11] = indirect_indexblooc_no;
		if (Fill_Index_In_Datablk(indirect_indexblooc_no, buf_block_count - 128))
		{
			for (i = 128, j = 0; i<buf_block_count; i++, j++)
			{
				memset(data, 0, sizeof(data));                          //先对将要写的磁盘块进行0覆盖
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
			printf("Failed to create file！\n");
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
		int buffer_part_count = 1;              //记录数据缓存buffer的块序
		for (k = 0; k<index_count - 1; k++)//temp[k]表示第k个二级索引磁盘块
		{
			if (Fill_Index_In_Datablk(temp[k], 128))//向二级索引磁盘块中填写分给数据的数据磁盘块号
			{
				
				for (i = 0; i<128; i++)
				{
					memset(data, 0, sizeof(data));                          //先对将要写的磁盘块进行0覆盖
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
				printf("Failed to create file！\n");
				return false;
			}
		}
		//最后一个（即第index_count个）二级索引数据块中可能不满128项,
		if (Fill_Index_In_Datablk(temp[k], buf_block_count - 128 * (index_count - 1)))
		{
			//填完数据索引块后还需要将剩下的数据填到相应的磁盘块中
			for (i = 0; i<buf_block_count - 128 * (index_count - 1); i++)
			{
				memset(data, 0, sizeof(data));                          //先对将要写的磁盘块进行0覆盖
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
			printf("Failed to create file！\n");
			return false;
		}
	
	}
	
	file = fopen(filesystemname, "rb+");
	fseek(file, BLOCK_SIZE*block_num, SEEK_SET);
	fwrite(iNode, sizeof(iNode), 1, file);  
	fclose(file);

	
	int dir_blkno;                      //当前文件夹所在的磁盘块
	block_num = index_node / 8 + 2;     
	inode_num = index_node % 8;
	file = fopen(filesystemname, "rb+");
	fseek(file, BLOCK_SIZE*block_num, SEEK_SET);
	fread(iNode, sizeof(iNode), 1, file);
	fclose(file);
	dir_blkno = iNode[inode_num].disk_address[0];   //根据当前索引节点号查找索引节点，然后找到对应的目录文件在的磁盘块号
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
		printf("Failed to create file！\n");
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
		printf("Failed to open file！\n");
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
			printf("Failed to open file！\n");
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

//文件检索操作，输入：文件路径  输出：文件的索引节点号
int Lookup_File(char *path)
{
	int i, j;
	char filename[20];        //临时存放文件名
	int index_node_num;      //文件对应的索引节点号
	bool found;              //路径是否有错
	int file_count = 0;       //该路径下有多少个文件夹
	int file_start;           //将路径分割时记录断点的
	inode_t inode;            //根据索引节点号找到对应的索引节点块
	int dir_blk_num;         //目录文件对应的磁盘块号
	for (i = 0; path[i]; i++)
		if (path[i] == '/')
			file_count++;


	if (path[0] != '/')
	{
		return -1;
	}

	file_start = 1;
	index_node_num = 0;//根目录的索引节点为0
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
			printf("Failed to open file！\n");
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

//复制文件，支持当前文件夹下当前文件，也支持路径文件复制
bool Copy_File(char filename1[], char filename2[])
{
	int block_num;         //该索引号指向的索引节点所处的盘块号(2#~17#)
	int inode_num;         //该索引节点指向的索引节点在磁盘里的位置(0~7)
	int file_index;        //文件对应的索引号
	int i, j;
	//获取第一个参数所对应的文件的索引节点号
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
	//获取第二个参数所对应的文件的索引节点号
	int dir_file_index;                //目录文件的索引节点号
	char file_name[20];                //文件名
	char path[20];                     //文件存放的路径
	int middle;                       //分界
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
		dir_file_index = Lookup_File(path);   //根据路径，得到即将创建的文件应存放的文件目录的索引号
	}
	else
	{
		dir_file_index = cur_inode_no;
		strcpy(file_name, filename2);
	}
	if (isLegal(dir_file_index, file_name))
	{
		Create_File(file_name, dir_file_index);    //创建的文件文件名和文件应在的目录的目录文件的索引号
	}
	else
	{
		printf("\tError!\nThe file has existed!\n");
		return false;
	}
	return true;
}

//判断该参数是文件名还是路径
bool isPath(char *name)
{
	int i = 0;
	while (name[i])
	{
		if (name[i] == '/')     //文件名中不存在‘/’
			return true;
		i++;
	}
	return false;
}

//检索目录，输入目录文件节点，结果是将目录节点对应的目录输入到全局变量DirBuf[]中,同时将该目录文件所在的磁盘号返回
int Lookup_Dir(int index_node)
{
	int block_num;         //该索引号指向的索引节点所处的盘块号(2#~17#)
	int inode_num;         //该索引节点指向的索引节点在磁盘里的位置(0~7)
	int dir_file_block;    //目录文件对应的磁盘块号
	block_num = index_node / 8 + 2;
	inode_num = index_node % 8;
	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to open file！\n");
		return false;
	}

	fseek(file, BLOCK_SIZE * block_num, SEEK_SET);
	fread(iNode, sizeof(iNode), 1, file);
	fclose(file);
	dir_file_block = iNode[inode_num].disk_address[0];     //得到目录文件对应的磁盘号
	Clear_Dir_Buf();
	file = fopen(filesystemname, "rb+");
	fseek(file, BLOCK_SIZE*dir_file_block, SEEK_SET);
	fread(DirBuf, sizeof(DirBuf), 1, file);                //将目录文件中的内容读入缓冲DirBuf中
	fclose(file);
	return dir_file_block;
}

//读取文件，根据文件的索引节点号，将对应的文件的内容读取到内存缓冲Buffer中
bool Read_File(int file_index)
{

	int block_num;         //该索引号指向的索引节点所处的盘块号(2#~17#)
	int inode_num;         //该索引节点指向的索引节点在磁盘里的位置(0~7)
	int file_size;         //索引节点指示该文件的大小，单位是字节数
	int file_block;        //有文件大小计算出该文件所拥有的磁盘数的个数
	block_num = file_index / 8 + 2;
	inode_num = file_index % 8;
	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to open file！\n");
		return false;
	}

	fseek(file, BLOCK_SIZE * block_num, SEEK_SET);
	fread(iNode, sizeof(iNode), 1, file);
	fclose(file);

	if (iNode[inode_num].type == 0)          //如果给定的文件是一个目录文件时，读取失败！
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
	//磁盘块数目不大于128时,采用一级间址
	else if (file_block <= 128)
	{
		block_index = iNode[inode_num].disk_address[10];
		file = fopen(filesystemname, "rb+");
		fseek(file, BLOCK_SIZE * block_index, SEEK_SET);
		fread(b_index, sizeof(b_index), 1, file);                    //先将索引磁盘中内容读取出来
		fclose(file);
		for (i = 0; i < file_block; i++)
		{
			block_index = b_index[i];                          //遍历索引磁盘中的项，得到文件的各个磁盘块号
			Get_Data(block_index);                           //读取各个磁盘块，获取其中的内容
			Merge(i);                                        //将各磁盘块的内容合并归入Buffer中
		}
	}
	//磁盘块数目不大于256时,两个采用一级间址
	else if (file_block <= 128 * 2)
	{
		block_index = iNode[inode_num].disk_address[10];
		file = fopen(filesystemname, "rb+");
		fseek(file, BLOCK_SIZE * block_index, SEEK_SET);
		fread(b_index, sizeof(b_index), 1, file);                    //先将索引磁盘中内容读取出来
		fclose(file);
		for (i = 0; i < 128; i++)
		{
			block_index = b_index[i];                          //遍历索引磁盘中的项，得到文件的各个磁盘块号
			Get_Data(block_index);                           //读取各个磁盘块，获取其中的内容
			Merge(i);                                        //将各磁盘块的内容合并归入Buffer中
		}

		block_index = iNode[inode_num].disk_address[11];
		file = fopen(filesystemname, "rb+");
		fseek(file, BLOCK_SIZE * block_index, SEEK_SET);
		fread(b_index, sizeof(b_index), 1, file);                    //先将索引磁盘中内容读取出来
		fclose(file);
		for (i = 0; i < file_block - 128; i++)
		{
			block_index = b_index[i];                          //遍历索引磁盘中的项，得到文件的各个磁盘块号
			Get_Data(block_index);                           //读取各个磁盘块，获取其中的内容
			Merge(i + 128);                                        //将各磁盘块的内容合并归入Buffer中
		}
	}
	//否则，采用一个二级间址
	else
	{

		int temp_len;
		int temp[128];
		int buffer_part = 0;
		block_index = iNode[inode_num].disk_address[12];
		file = fopen(filesystemname, "rb+");
		fseek(file, BLOCK_SIZE * block_index, SEEK_SET);
		fread(b_index, sizeof(b_index), 1, file);                    //先将索引磁盘中内容读取出来
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
			fread(b_index, sizeof(b_index), 1, file);                    //先将索引磁盘中内容读取出来
			fclose(file);
			for (j = 0; j < 128; j++)
			{
				block_index = b_index[j];                          //遍历索引磁盘中的项，得到文件的各个磁盘块号
				Get_Data(block_index);                           //读取各个磁盘块，获取其中的内容
				Merge(buffer_part);                              //将各磁盘块的内容合并归入Buffer中
				buffer_part++;
			}
		}
		block_index = temp[i];
		file = fopen(filesystemname, "rb+");
		fseek(file, BLOCK_SIZE * block_index, SEEK_SET);
		fread(b_index, sizeof(b_index), 1, file);                    //先将索引磁盘中内容读取出来
		fclose(file);
		for (j = 0; j < file_block - 128 * (temp_len - 1); j++)
		{
			block_index = b_index[j];                          //遍历索引磁盘中的项，得到文件的各个磁盘块号
			Get_Data(block_index);                           //读取各个磁盘块，获取其中的内容
			Merge(buffer_part);                              //将各磁盘块的内容合并归入Buffer中
			buffer_part++;
		}
	}

	return true;
}

//通过磁盘块号，将磁盘上的数据读入缓冲区Block_Data中
bool Get_Data(int block_index)
{
	memset(Block_Data, 0, sizeof(Block_Data));
	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("打开文件失败！\n");
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
	Buffer[j] = 0;                             //清楚可能出现的烂尾巴
	return true;
}

bool Copy_File_out(char filename1[], char filename2[])
{
	int file_index;        //文件对应的索引号
	bool find;
	int i, j;
	//获取第一个参数所对应的文件的索引节点号
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
			printf("Error！\n");
			return false;
		}
	}
	if (!Read_File(file_index))                      //将参数1指定的文件中的内容读取到Buffer中
	{
		printf("Can't read the directory!\n");
		return false;
	}
	file = fopen(filename2, "w");
	if (NULL == file)
	{
		printf("Failed to creat file！\n");
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
		printf("Failed to creat file！\n");
		return false;
	}
	fseek(file, 0, SEEK_SET);
	fread(Buffer, sizeof(Buffer), 1, file);        //将物理磁盘文件中的内容读入Buffer中
	fclose(file);

	//获取第二个参数所对应的文件的索引节点号
	int dir_file_index;                //目录文件的索引节点号
	char file_name[20];                //文件名
	char path[20];                     //文件存放的路径
	int middle;                       //分界
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
		dir_file_index = Lookup_File(path);   //根据路径，得到即将创建的文件应存放的文件目录的索引号
	}
	else
	{
		dir_file_index = cur_inode_no;
		strcpy(file_name, filename2);
	}
	if (isLegal(dir_file_index, file_name))
	{
		Create_File(file_name, dir_file_index);    //创建的文件文件名和文件应在的目录的目录文件的索引号
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
	int file_index;                         //待删除的文件的索引号
	int father_block;                       //当前文件夹所在的磁盘块
	father_block = Lookup_Dir(cur_inode_no);

	find = false;
	for (i = 0; DirBuf[i].name[0]; i++)
	{
		if (strcmp(DirBuf[i].name, filename) == 0)   //在当前目录下找到该文件名，将其在该目录文件中删除，同时将对应的文件索引节点记下
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
				printf("Failed to open file！\n");
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

	
	int block_num;         //该索引号指向的索引节点所处的盘块号(2#~17#)
	int inode_num;         //该索引节点指向的索引节点在磁盘里的位置(0~7)

	int file_size;        //记载文件的大小，单位为字节
	int file_block;       //记录文件所占的数据块数目
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
			block_index = iNode[inode_num].disk_address[i];     //每次获得一个数据块的索引节点
			Delete_From_Data_Bitmap(block_index);               //根据这个数据块的索引节点号，该数据块位示图中相应的bit位置0
		}
	}
	else if (file_block <= 128)
	{
		block_index = iNode[inode_num].disk_address[10];
		file = fopen(filesystemname, "rb+");
		fseek(file, BLOCK_SIZE*block_index, SEEK_SET);
		fread(b_index, sizeof(b_index), 1, file);
		fclose(file);
		Delete_From_Data_Bitmap(block_index);               //将索引数据块读出来之后，便可以释放索引数据块的索引号了
		for (i = 0; i<file_block; i++)
		{
			block_index = b_index[i];
			Delete_From_Data_Bitmap(block_index);               //根据这个数据块的索引节点号，该数据块位示图中相应的bit位置0
		}
	}
	else if (file_block <= 128 * 2)
	{
		
		block_index = iNode[inode_num].disk_address[10];
		file = fopen(filesystemname, "rb+");
		fseek(file, BLOCK_SIZE*block_index, SEEK_SET);
		fread(b_index, sizeof(b_index), 1, file);
		fclose(file);
		Delete_From_Data_Bitmap(block_index);               //将索引数据块读出来之后，便可以释放索引数据块的索引号了
		for (i = 0; i<128; i++)
		{
			block_index = b_index[i];
			Delete_From_Data_Bitmap(block_index);               //根据这个数据块的索引节点号，该数据块位示图中相应的bit位置0
		}
		
		block_index = iNode[inode_num].disk_address[11];
		file = fopen(filesystemname, "rb+");
		fseek(file, BLOCK_SIZE*block_index, SEEK_SET);
		fread(b_index, sizeof(b_index), 1, file);
		fclose(file);
		Delete_From_Data_Bitmap(block_index);               //将索引数据块读出来之后，便可以释放索引数据块的索引号了
		for (i = 0; i<file_block - 128; i++)
		{
			block_index = b_index[i];
			Delete_From_Data_Bitmap(block_index);               //根据这个数据块的索引节点号，该数据块位示图中相应的bit位置0
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
		Delete_From_Data_Bitmap(block_index);               //将索引数据块读出来之后，便可以释放索引数据块的索引号了
		for (i = 0; i<temp_len - 1; i++)
		{
			block_index = temp[i];                         //查找第i个二级索引块
			file = fopen(filesystemname, "rb+");
			fseek(file, BLOCK_SIZE*block_index, SEEK_SET);
			fread(b_index, sizeof(b_index), 1, file);
			fclose(file);
			Delete_From_Data_Bitmap(block_index);           //将索引数据块读出来之后，便可以释放索引数据块的索引号了
			for (j = 0; j<128; j++)
			{
				block_index = b_index[j];
				Delete_From_Data_Bitmap(block_index);      //释放二级索引块对应的数据块
			}
		}
		
		block_index = temp[i];                         //查找最后一个二级索引块
		file = fopen(filesystemname, "rb+");
		fseek(file, BLOCK_SIZE*block_index, SEEK_SET);
		fread(b_index, sizeof(b_index), 1, file);
		fclose(file);
		Delete_From_Data_Bitmap(block_index);           //将索引数据块读出来之后，便可以释放索引数据块的索引号了
		for (j = 0; j<file_block - 128 * (temp_len - 1); j++)
		{
			block_index = b_index[j];
			Delete_From_Data_Bitmap(block_index);      //释放二级索引块对应的数据块
		}
	}
	fclose(file);
	return true;
}

//根据给定的文件的索引节点，在索引节点位示图中将对应的bit位置0
bool Delete_From_Inode_Bitmap(int file_index)
{
	int column, row;
	row = file_index / 8;
	column = file_index % 8;
	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to open file！\n");
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

//根据给定的磁盘的索引节点，在数据块位示图中将对应的bit位置0
bool Delete_From_Data_Bitmap(int block_index_no)
{
	int column, row;
	int block_index = block_index_no - 19;
	row = block_index / 8;
	column = block_index % 8;
	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to open file！\n");
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



//创建一个新文件，文件内容由用户自己输入
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

//在指定的目录下，判断该文件名是否合法
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
	int size;                                   //显示该文件的大小
	int type;                                   //显示该文件的类型
	inode_t inode;                              //根据所给的文件索引节点，读取文件索引节点的内容
	char str1[50] = "Common file";
	char str2[50] = "Directory file";
	char str[50];
	inode = Find_Inode(cur_inode_no);
	int diskNo;
	int i;
	diskNo = inode.disk_address[0];      //默认目录文件只占用一个磁盘并且该磁盘号存放在地址项的第一个地址项中

	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to open file！\n");
		return false;
	}
	Clear_Dir_Buf();
	fseek(file, BLOCK_SIZE*diskNo, SEEK_SET);
	fread(DirBuf, sizeof(DirBuf), 1, file);//找到当前目录对应的磁盘块后，将其读出并打印该磁盘块中全部内容
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


//创建一个目录,支持在当前目录下创建一个新目录，也支持给定一个路径，在指定路径下创建新目录
bool CreateDir(char dir_path[])
{
	int dir_index;                //目录文件索引号
	char path[50];                //绝对路径
	char dir_name[50];            //待创建的目录文件名
	int middle;                       //分界
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
	int inode_no;                 //分配给即将创建的目录文件的索引节点号
	int dir_block_no;             //分配给即将创建的目录文件的磁盘块号
	int block_num;         //该索引号指向的索引节点所处的盘块号(2#~17#)
	int inode_num;         //该索引节点指向的索引节点在磁盘里的位置(0~7)
	int father_dir_blk;         //新建的文件夹的父文件夹所在的磁盘块号
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
		printf("Failed to open file！\n");
		return false;
	}
	fseek(file, BLOCK_SIZE*block_num, SEEK_SET);
	fread(iNode, sizeof(iNode), 1, file);
	fclose(file);
	iNode[inode_num].type = 0;                           //创建的文件是目录文件
	iNode[inode_num].size = 0;                          //目录文件的size字段不起作用

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
	//写回文件
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
	//写回文件
	file = fopen(filesystemname, "rb+");
	fseek(file, BLOCK_SIZE*father_dir_blk, SEEK_SET);
	fwrite(DirBuf, sizeof(DirBuf), 1, file);
	fclose(file);
	return true;
}

//删除文件夹
bool DeleteDir(char dir_name[])
{
	int i, j;
	int father_block;                               //待删除的文件夹所在的文件夹的目录文件所在的磁盘号
	int dir_index;                                 //用于存放待删除的文件夹的目录文件索引号
	int mark;                                      //用于文件夹跳转时记录文件夹索引编号
	father_block = Lookup_Dir(cur_inode_no);       //将当前目录全部读取到内存中

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
				printf("Failed to open file！\n");
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
		if (GetType(DirBuf[i].ino) == 1)   //文件类型是普通文件
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
		printf("Failed to open file！\n");
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
		printf("Failed to open file！\n");
		return false;
	}
	fseek(file, BLOCK_SIZE*dir_block, SEEK_SET);
	fwrite(data, sizeof(data), 1, file);
	fclose(file);

	Delete_From_Data_Bitmap(dir_block);
	return true;
}

//根据文件索引节点号判断文件类型，如果是普通文件，则返回1；如果是目录文件，则返回0
int GetType(int file_index)
{
	int block_num;
	int inode_num;
	block_num = file_index / 8 + 2;
	inode_num = file_index % 8;

	file = fopen(filesystemname, "rb+");
	if (NULL == file)
	{
		printf("Failed to open file！\n");
		return false;
	}
	fseek(file, BLOCK_SIZE*block_num, SEEK_SET);
	fread(iNode, sizeof(iNode), 1, file);
	fclose(file);
	return iNode[inode_num].type;
}
