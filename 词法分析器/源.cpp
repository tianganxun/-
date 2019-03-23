#include<iostream>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
using namespace std;

//��Ŵ������ַ��� 
char tempstr[255] = {};
//�ո��־ 
bool temp = false;
//��ʱ����
char word[255] = {};
//keyword�ؼ��� 
string keyword[26] = {
	"main","if","then","while","do","static","defualt","do","int","double","struct","break","else","long","swtich","case","typedf","char","return","const","float","short","continue","for","void","sizeof" };

int keyword_num[26] = { 1,2,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};
//������������������ 
char symbol[9] = { '+','-','*','/','=',';','(',')','#' };
//��Ӧ������ֵ
int symbol_num[9] = { 27,28,29,30,38,41,42,43,0 };

//�ж��Ƿ�Ϊ��ĸ 
bool IsLetter(char ch)
{
	if ((ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z'))
		return true;
	return false;
}

//�ж��Ƿ�Ϊ���� 
bool IsDigit(char ch)
{
	if (ch >= '0'&&ch <= '9')
		return true;
	return false;
}

//�ж��Ƿ�Ϊ�������
int IsSymbol(char ch)
{
	for (int i = 0; i<9; i++)
	{
		if (ch == symbol[i])
			return i;
	}
	return -1;
}

//�ж��Ƿ�Ϊ�ؼ��� 
int IsKeyword(string str)
{
	for (int i = 0; i<26; i++)
	{
		if (str == keyword[i])
		{
			return i;
		}
	}
	//���ǹؼ��ּ�ΪID
	return 39;
}

//�ո���
void HandleSpace(char a[])
{
	int j = 0;
	memset(word, 0, 255);//��Ҫ��գ���Ȼ���ܲ����ϴε��ַ���
	temp = false;
	for (int i = 0; i<strlen(a); i++)
	{

		if (a[i] != ' ' && a[i] != '\t')
		{
			word[j++] = a[i];
			temp = false;
		}
		else
		{

			if (!temp&&a[i] != '\t')
			{
				word[j++] = a[i];
				temp = true;
			}

		}
	}
}

//����"//"ע��
void prePro()
{
	int j = 0;
	memset(tempstr, 0, 255);
	for (int i = 0; i<strlen(word); i++)
	{
		if (word[i] == '/'&&word[i + 1] == '/')
		{
			while (i<strlen(word))
			{
				i++;
			}
		}

		else {
			tempstr[j++] = word[i];
		}
	}
}

int main()
{
	char instr[255] = {}; //���������ַ��� 
	bool flag = false; //����ע�ͱ�־,falseΪδ����ע������ 
	string Token;//����ַ��� 
	char *str = NULL;//���ÿ�е��ַ��� 
	char delims[] = " ";//�ָ��־ 
	freopen("test.cpp", "r", stdin);
	freopen("result.txt", "w", stdout); //����ע�ͺ󣬿���̨�����
										//�����ı����
	while ((gets_s(instr)) != NULL)
	{
		HandleSpace(instr);
		prePro();

		str = strtok(tempstr, delims);//�ָ��ַ��� 

		while (str != NULL)
		{
			//ͷ�ļ����궨��
			if (*(str) == '#')
			{
				printf("#\n");
				break;
			}

			for (int i = 0; i<strlen(str); i++)
			{
				if (*(str + i) == '/')
				{
					if (*(str + i + 1) == '*')
					{
						flag = true;
						break;
					}
				}
				//ע�ʹ���: */,ע��������� 
				if (*(str + i) == '*'&&flag)
				{
					if (*(str + i + 1) == '/')
					{
						flag = false;
						i++;
						break;
					}
				}
				//��ʶ�����ؼ��� 
				if (IsLetter(*(str + i)) && (!flag))
				{
					//                    printf("�����ʶ���ж�\n");
					while (IsLetter(*(str + i)) || IsDigit(*(str + i))
						|| *(str + i) == '_')
					{
						Token += *(str + i);
						i++;
					}

					if (IsKeyword(Token) != 39)
					{
						printf("%s---->%d\n", Token.c_str(),
							keyword_num[IsKeyword(Token)]);
					}
					else printf("%s---->39\n", Token.c_str());

					Token = "";
					//                printf("�˳���ʶ���ж�\n");
				}
				if (IsDigit(*(str + i)) && (!flag))
				{
					//                        printf("���������ж�\n");
					while (IsDigit(*(str + i)))
					{
						Token += *(str + i);
						i++;
					}
					printf("%s------>40\n", Token.c_str());
					Token = "";
				}

				//<,<=,<>
				if (*(str + i) == '<' && (!flag))
				{
					if (*(str + i) == '=') { printf("<=------>35\n"); i++; }
					if (*(str + i) == '>') { printf("<>------>34\n"); i++; }
					else printf("<------>33\n");
				}
				//>,>=
				else if (*(str + i) == '>' && (!flag))
				{
					if (*(str + i + 1) == '=') { printf(">------>37\n"); }
					else printf(">-------36\n");
				}
				//:,:=
				else  if (*(str + i) == ':' && (!flag))
				{
					if (*(str + i + 1) == '=') { printf(":=------->32\n"); }
					else printf(":-------->31\n");
				}
				//���¶������
				else if (IsSymbol(*(str + i)) != -1 && (!flag))
				{
					printf("%c------->%d\n", *(str + i),
						symbol_num[IsSymbol(*(str + i))]);
				}
			}
			str = strtok(NULL, delims);
		}
	}

	return 0;
}