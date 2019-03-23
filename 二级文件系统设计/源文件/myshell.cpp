#include "filesystem.h"
#include "cmd.h"


extern char CurDir[20];                         //记录当前在那个文件夹中
extern directory DirBuf[BLOCK_SIZE / sizeof(directory)];   //目录数组，表示一个磁盘块可以存放32个目录项
extern super_block_t  file_system_super;         //声明结构体变量
extern unsigned char  inodeBP[INODE_NUM / 8];         //索引节点位示图，总位数=8(char)*16=128bit,所以一共可以表示128个索引节点
extern unsigned char  dataBP[DATA_BLOCK_NUM / 8];     //数据块位示图，总位数=8(char)*64=512bit,所以一共可以标识521个数据块


int main(int argc, char * argv[])
{
	char cmd[40];
	char cmd1[20], cmd2[20], cmd3[20];
	int i, j;
	int space;
	while (1)
	{

		printf("%s>", argv[0]);
		gets(cmd);
		if (cmd[0])
		{
			i = 0;
			space = 0;
			while (cmd[i])
				if (cmd[i++] == ' ')
					space++;
			if (0 == space)
			{
				if (strcmp(cmd, "help") == 0)               //help
					showhelpmain();
				else if (strcmp(cmd, "exit") == 0)               //exit
					break;
				else
					printf("Error!\n");
			}
			else if (1 == space)
			{
				for (i = 0; cmd[i] != ' '; i++)
					cmd1[i] = cmd[i];
				cmd1[i] = 0;
				i++;
				for (j = 0; cmd[i]; i++, j++)
					cmd2[j] = cmd[i];
				cmd2[j] = 0;
				if (strcmp(cmd1, "sf") == 0 && cmd2[0])
				{
					strcpy(filesystemname, cmd2);
					if (Install())
						Entersystem();

				}
				else
					printf("Error!!\n");
			}
			else if (2 == space)
			{
				for (i = 0; cmd[i] != ' '; i++)
					cmd1[i] = cmd[i];
				cmd1[i++] = 0;
				for (j = 0; cmd[i] != ' '; i++, j++)
					cmd2[j] = cmd[i];
				cmd2[j++] = 0;
				i++;
				for (j = 0; cmd[i]; i++, j++)
					cmd3[j] = cmd[i];
				cmd3[j] = 0;
				if ((strcmp(cmd1, "sf") == 0) && (strcmp(cmd2, "--mkfs") == 0) && (cmd3[0] != 0))
				{
					strcpy(filesystemname, cmd3);
					Format();
					printf("Initialization Success.\n");
				}
				else
					printf("Error!\n");
			}
			else
				printf("Error!\n");
		}
	}
	return 0;
}