#include <iostream>
#include <string>
#include<stdio.h> 
using namespace std;



//����F����
typedef struct
{
	char R;
	char r;
	int flag;
}shuzu;


//����û�б�־λ��F����
typedef struct
{
	char E;
	char e;
}charLode;



//����ջ
typedef struct
{
	charLode *base;
	int top;
}charstack;

int n;               //�ķ��Ĳ���ʽ��Ŀ
char str[100][100];  //�ķ��Ĳ���ʽ


char ft[100][100];  //firstvt�Ĵ洢����
char lt[100][100];  //lastvt�Ĵ洢����


shuzu F[20];        //�ս���ͷ��ս���Ķ�Ԫ�鼰��־λ
int m;              //F����Ķ�Ԫ����Ŀ

int FLAG=0;     //���ȼ���Ψһ��Ϊ1
int FF=1;     //Ϊ��������ķ���Ϊ1

char a[50];   //���ս��
char b[50];   //�ս��

char a1[50];   //ȥ�ط��ս��
char b1[50];   //ȥ���ս��

int ka = 0;    //���ս����Ŀ
int kb = 0;    //�ս����Ŀ

int pf;
int pl;


int crr[20][20];
char ccrr1[1][20], ccrr2[20][1];

//��������
void Cindata()
{
	

	cout << "�����ķ�����ʽ��Ŀ" << endl;
	cin >> n;
	cin.ignore();  //ȡ�����м��Ķ���

	cout << "�����ķ�����ʽ" << endl;
	int i, j;
	for ( i = 0; i<n; i++)
	{
		cin >> str[i];
		 j = strlen(str[i]);
		str[i][j] = '\0';
	}

}

//�ж��ظ�
void qc(char* m, char* n, int& d)

{
	char check[256] = { 0 };
	int i, j, len;
	len = strlen(m);
	for (i = j = 0; i < len; i++)
	{
		if (check[m[i]] == 0)
		{
			n[j++] = m[i];
			check[m[i]] = 1;
		}
	}
	n[j] = '\0';
	d = j;
};


//ջ�Ĳ�������
void Initstack(charstack &s)//����ջ 
{
	s.base = new charLode[20];
	s.top = -1;
}
void push(charstack &s, charLode &w)
{
	s.top++;
	s.base[s.top].E = w.E;
	s.base[s.top].e = w.e;
}
void pop(charstack &s, charLode &w)
{
	w.E = s.base[s.top].E;
	w.e = s.base[s.top].e;
	s.top--;
}
int IsEmpty(charstack s)
{
	if (s.top <0)
		return 1;
	else return 0;
}

//��F��Ԫ���־λ��1
void Z1(charLode w)
{
	for (int i = 0; i<m; i++)
		if (F[i].R == w.E&&F[i].r == w.e)
		{
			F[i].flag = 1; break;
		}
}

// �ж��Ƿ�Ϊ�ս�� ���ս���򷵻�1
int Term(char c)
{

	if (c >= 'A'&&c <= 'Z')    //���ս������1
		return 1;                       

	else
		return 0;              // �ս������0


};


// �ж��Ƿ�Ϊ����ķ�  ������ķ��򷵻�true(1)
int og(int n)
{
	
	int j = 3, flag = 0;
	for (int i = 0; i < n; i++)
	{
		if (Term(str[i][0]) == 1 && str[i][1] == '-'&&str[i][2] == '>'&&str[i][3] != '\0')
		{

			
			while (str[i][j+1] != '\0')
			{
				char a = str[i][j];
				char b = str[i][j + 1];
				if (Term(a) && Term(b))
				{
					flag = 1; break;
				}
				else j++;
			}
		}
		else flag = 1;
	};
	if (flag == 1)
	{
		return 0; 
	}
	else
		return 1;
};



// �ж��Ƿ�Ϊ��������ķ�
void opg(int n)
{   
	int i;
	for (i = 0; i <n; i++)
		if (str[i][3] == '/0' || og(n) == 0 || FLAG == 1)//'~'������� 
		{
			cout << "�ķ�G������������ķ�!" << endl; FF = 0; break;
		}
	if (i>=n)
		cout << "�ķ�G����������ķ�!" << endl;
};






// ���� lastvt����
void makeLT(int n)
{
	charstack sta;
	charLode w;
	for (int i = 0; i<m; i++)
		F[i].flag = 0;
	int i = 0;
	Initstack(sta);
	while (i < n)
	{
		int k = strlen(str[i]);
		w.E = str[i][0];
		char a = str[i][k - 1];
		char b = str[i][k - 2];
		if (!Term(a))
		{
			w.e = a;
			push(sta, w);
			Z1(w);
			i++;
		}
		else if (Term(a) && !Term(b))
		{
			w.e = b;
			push(sta, w);
			Z1(w);
			i++;
		}
		else i++;
	}
	charLode w2;
	while (!IsEmpty(sta))
	{
		pop(sta, w2);
		for (i = 0; i <= n; i++)
		{
			w.E = str[i][0];
			if (str[i][3] == w2.E&&str[i][4] == '\0')
			{
				w.e = w2.e;
				push(sta, w);
				Z1(w);
			}
		}
	}

	int k = 1; 
	 i = 1;
	 pl = 0;

	while (i<=m)
	{
		if (F[i - 1].flag == 1)
		{
			lt[pl][0] = F[i - 1].R;
			lt[pl][k] = F[i - 1].r;
		}
		while (F[i].flag == 0 && i<=m)
			i++;
		if (F[i].flag == 1)
		{
			if (F[i].R == lt[pl][0])
				k++;
			else { lt[pl][k + 1] = '\0'; pl++; k = 1; }
			i++;
		}
	}

};


// ���� firstvt����
void makeFT(int n)
{
	charstack sta;
	charLode w;
	int i = 0;
	Initstack(sta);
	while (i < n)
	{
		
		w.E = str[i][0];
		char c = str[i][3];
		char b = str[i][4];
		if (!Term(c))//����ʽ�ĺ�ѡʽ�ĵ�һ���ַ������ս�������
		{
			w.e = c;
			//if(c!='\0')
		    push(sta, w);
			Z1(w);
			i++;
		}
		 else  if (Term(c) && b != '\0' && !Term(b))//����ʽ�ĺ�ѡʽ�ĵ�һ���ַ��Ƿ��ս������� 
		{
			w.e = b;
			push(sta, w);
			Z1(w);
			i++;
		}
		else i++;
	}
	charLode ww;
	while (!IsEmpty(sta))
	{
		pop(sta, ww);
		for (i = 0; i < n; i++)
		{
			w.E = str[i][0];
			if (str[i][3] == ww.E&&str[i][3] != w.E)
			{
				w.e = ww.e;
				push(sta, w);
				Z1(w);
				break;
			}
		}
	}
	 pf = 0; int k = 1; i = 1;
	while (i<=m)
	{
		if (F[i - 1].flag == 1)
		{
			ft[pf][0] = F[i - 1].R;
			ft[pf][k] = F[i - 1].r;
		}
		while (F[i].flag == 0 && i<=m)
			i++;
		if (F[i].flag == 1)
		{
			if (F[i].R == ft[pf][0])
				k++;
			else { ft[pf][k + 1] = '\0'; pf++; k = 1; }
			i++;
		}
	}

};

// ����F����
void makeF(int n)
{
	

	int k=0;

	for (int i = 0; i < n; i++) 
	{
		a[i] = str[i][0];
		int j = 3;
		
		while (str[i][j] != '\0')
		{
			char c = str[i][j];
			if (Term(c) == 0)
			{
				b[k] = c; k++;
			}
			j++;
		}
	}

	qc(a, a1,ka);
	qc(b, b1,kb);

	m = 0;
	for (int i = 0; i<ka; i++)
		for (int j = 0; j<kb; j++)
		{
			F[m].R = a1[i];
			F[m].r = b1[j];
			F[m].flag = 0;
			m++;
		}

};

// �������ȱ�
void makeYXB(int n)
{
	int i, j;
	for (j = 1; j <= kb; j++)
		ccrr1[0][j] = b[j - 1];
	for (i = 1; i <= kb; i++)
		ccrr2[i][0] = b[i - 1];
	for (i = 1; i <= kb; i++)
		for (j = 1; j <= kb; j++)
			crr[i][j] = 0;
	int I = 0, J = 3;
	while (I < n)
	{
		if (str[I][J + 1] == '\0')
		{
			I++; J = 3;
		}
		else
		{
			for (J = 3; str[I][J + 1] != '\0';J++)
			{
				char aa = str[I][J];
				char bb = str[I][J + 1];
				if (!Term(aa) && !Term(bb))//���ȼ����ڵ��������1ֵ��ʾ���� 
				{
					for (i = 1; i <= kb; i++)
					{
						if (ccrr2[i][0] == aa)
							break;
					}
					for (j = 1; j <= kb; j++)
					{
						if (ccrr1[0][j] == bb)
							break;
					}
					if (crr[i][j] == 0)
						crr[i][j] = 1;
					else { FLAG = 1;  }
					
				}
				if (!Term(aa) && Term(bb) && str[I][J + 2] != '\0' && !Term(str[I][J + 2]))//���ȼ����ڵ���� 
				{
					for (i = 1; i <= kb; i++)
					{
						if (ccrr2[i][0] == aa)
							break;
					}
					for ( j = 1; j <= kb; j++)
					{


						if (ccrr1[0][j] == str[I][J + 2])
							break;
					}
					if (crr[i][j] == 0)
						crr[i][j] = 1;
					else { FLAG = 1;  }
					
				}
				if (!Term(aa) && Term(bb))//���ȼ�С�ڵ��������2ֵ��ʾС�� 
				{
					for (i = 1; i <= kb; i++)
					{
						if (aa == ccrr2[i][0])
							break;
					}
					for (j = 0; j <= pf; j++)
					{
						if (bb == ft[j][0])
							break;
					}
					for (int mm = 1; ft[j][mm] != '\0'; mm++)
					{
						int pp;
						for ( pp = 1; pp <= kb; pp++)
						{
							if (ccrr1[0][pp] == ft[j][mm])
								break;
						}
						if (crr[i][pp] == 0)
							crr[i][pp] = 2;
						else { FLAG = 1;  }
					}
					
				}
				if (Term(aa) && !Term(bb))//���ȼ����ڵ��������3ֵ��ʾ���� 
				{
					for (i = 1; i <= kb; i++)
					{
						if (ccrr1[0][i] == bb)
							break;
					}
					for (j = 0; j <= pl; j++)
					{
						if (aa == lt[j][0])
							break;
					}
					int mm;
					for ( mm = 1; lt[j][mm] != '\0'; mm++)
					{
						int pp;
						for (pp = 1; pp <= kb; pp++)
						{
							if (ccrr2[pp][0] == lt[j][mm])
								break;
						}
						if (crr[pp][i] == 0)
							crr[pp][i] = 3;
						else { FLAG = 1;  }
					}
				
				}
			}
		}
	}
}


int judge(char ss, char aa)
{
	int i = 1, j = 1;
	for (i = 1; i <= kb; i++)
	{
		if (ccrr2[i][0] == ss)
			break;
	}
	for (j = 1; j <= kb; j++)
	{


		if (ccrr1[0][j] == aa)
			break;
	}
	if (aa == '#' && ss == '#') return 1;
	else if (crr[i][j] == 3 || aa == '#') return 3;
	else if (crr[i][j] == 2 || ss == '#')
		return 2;
	else if (crr[i][j] == 1)
		return 1;
	else return 0;
}

void print(char s[], char STR[][20], int q, int u, int ii, int k)//��ӡ��Լ�Ĺ��� 
{
	cout << u << "      ";
	for (int i = 0; i <= k; i++)
		cout << s[i];
	cout << "      ";
	for (int i = q; i <= ii; i++)
		cout << STR[0][i];
	cout << "      ";
}
void process(char STR[][20], int ii)//��������ַ������й�Լ�Ĺ��� 
{
	cout << "����" << "   " << "����ջ" << "   " << "���봮" << "   " << "����" << endl;
	int k = 0, q = 0, u = 0, b, i, j;
	char s[40], a;
	s[k] = '#';
	print(s, STR, q, u, ii, k);
	cout << "Ԥ��" << endl;
	k++; u++;
	s[k] = STR[0][q];
	q++;
	print(s, STR, q, u, ii, k);

	cout << "�ƽ�" << endl;
	while (q <= ii)
	{
		a = STR[0][q];
		if (!Term(s[k])) j = k;
		else j = k - 1;
		b = judge(s[j], a);
		if (b == 3)//���ڵ�������й�Լ 
		{
			while (Term(s[j - 1]))
				j--;
			for (i = j; i <= k; i++)
				s[i] = '\0';
			k = j; s[k] = 'N'; u++;
			print(s, STR, q, u, ii, k);
			cout << "��Լ" << endl;

		}
		else if (b == 2 || b == 1)//С�ڻ���ڵ�����ƽ� 
		{
			k++;
			s[k] = a;
			u++;
			q++;
			print(s, STR, q, u, ii, k);
			if (s[0] == '#'&&s[1] == 'N'&&s[2] == '#')
				cout << "����" << endl;
			else cout << "�ƽ�" << endl;

		}
		else
		{
			cout << "����" << endl; break;
		}
	}
	if (s[0] == '#'&&s[1] == 'N'&&s[2] == '#')


		cout << "��Լ�ɹ�" << endl;
	else cout << "��Լʧ��" << endl;
}
void printb()
{
	if (FLAG == 0&&og(n)==1)

	{
		for (int i = 0; i <= pf; i++)//��ӡFirstVT 
		{
			cout << "FirstVT(" << ft[i][0] << ")={";
			int x;
			for (x = 1; ft[i][x + 1] != '\0'; x++)
				cout << ft[i][x] << ",";
			cout << ft[i][x] << "}" << endl;
		}
		cout << "FirstVT(Q)={#}" << endl;
		for (int i = 0; i <= pl; i++)//��ӡLastVT 
		{
			cout << "LastVT(" << ft[i][0] << ")={";
			int y;
			for (y = 1; lt[i][y + 1] != '\0'; y++)
				cout << lt[i][y] << ",";
			cout << lt[i][y] << "}" << endl;
		}
		cout << "LastVT(Q)={#}" << endl;
		cout << "���ȱ�����:" << endl;
		cout << "  ";
		for (int i = 1; i <= kb; i++)//��ӡ���ȹ�ϵ�� 
		{

			cout << ccrr1[0][i];
			cout << "    ";
		}
		cout << endl;
		for (int i = 1; i <= kb; i++)
		{
			cout << ccrr2[i][0] << " ";
			for (int j = 1; j <= kb; j++)
			{

				if (crr[i][j] == 0)
					cout << "0  ";
				else if (crr[i][j] == 1)
					cout << "=  ";
				else if (crr[i][j] == 2)
					cout << "<  ";
				else if (crr[i][j] == 3)
					cout << ">  ";
				cout << "  ";
			}
			cout << endl;
		}
	}
}void guiyue()
{
	if (FF == 1)

	{
		char STR[1][20];
		cout << "������Ҫ��Լ���ַ���:" << endl;
		cin >> STR[0];
		int ii = strlen(STR[0]);
		STR[0][ii] = '#';
		cout << "�����ǹ�Լ�Ĺ���:" << endl;
		process(STR, ii);
	}
}

int main()
{


	Cindata();


	if (og(n) == 0)
		cout << "�ķ�G��������ķ���" << endl;
	if (og(n) == 1)
	{
		cout << "�ķ�G������ķ���" << endl;

		makeF(n);
		makeFT(n);
		makeLT(n);
		makeYXB(n);
	}
	opg(n);
	printb();
	guiyue();
	system("pause");
	return 0;
};