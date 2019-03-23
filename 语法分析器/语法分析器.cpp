#include <iostream>
#include <string>
#include<stdio.h> 
using namespace std;



//设置F数组
typedef struct
{
	char R;
	char r;
	int flag;
}shuzu;


//设置没有标志位的F数组
typedef struct
{
	char E;
	char e;
}charLode;



//设置栈
typedef struct
{
	charLode *base;
	int top;
}charstack;

int n;               //文法的产生式数目
char str[100][100];  //文法的产生式


char ft[100][100];  //firstvt的存储数组
char lt[100][100];  //lastvt的存储数组


shuzu F[20];        //终结符和非终结符的二元组及标志位
int m;              //F数组的二元组数目

int FLAG=0;     //优先级不唯一则为1
int FF=1;     //为算符优先文法则为1

char a[50];   //非终结符
char b[50];   //终结符

char a1[50];   //去重非终结符
char b1[50];   //去重终结符

int ka = 0;    //非终结符数目
int kb = 0;    //终结符数目

int pf;
int pl;


int crr[20][20];
char ccrr1[1][20], ccrr2[20][1];

//输入数据
void Cindata()
{
	

	cout << "输入文法产生式数目" << endl;
	cin >> n;
	cin.ignore();  //取消换行键的读入

	cout << "输入文法产生式" << endl;
	int i, j;
	for ( i = 0; i<n; i++)
	{
		cin >> str[i];
		 j = strlen(str[i]);
		str[i][j] = '\0';
	}

}

//判断重复
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


//栈的操作函数
void Initstack(charstack &s)//定义栈 
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

//将F二元组标志位置1
void Z1(charLode w)
{
	for (int i = 0; i<m; i++)
		if (F[i].R == w.E&&F[i].r == w.e)
		{
			F[i].flag = 1; break;
		}
}

// 判断是否为终结符 非终结符则返回1
int Term(char c)
{

	if (c >= 'A'&&c <= 'Z')    //非终结符返回1
		return 1;                       

	else
		return 0;              // 终结符返回0


};


// 判断是否为算符文法  是算符文法则返回true(1)
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



// 判断是否为算符优先文法
void opg(int n)
{   
	int i;
	for (i = 0; i <n; i++)
		if (str[i][3] == '/0' || og(n) == 0 || FLAG == 1)//'~'代表空字 
		{
			cout << "文法G不是算符优先文法!" << endl; FF = 0; break;
		}
	if (i>=n)
		cout << "文法G是算符优先文法!" << endl;
};






// 创建 lastvt集合
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


// 创建 firstvt集合
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
		if (!Term(c))//产生式的后选式的第一个字符就是终结符的情况
		{
			w.e = c;
			//if(c!='\0')
		    push(sta, w);
			Z1(w);
			i++;
		}
		 else  if (Term(c) && b != '\0' && !Term(b))//产生式的后选式的第一个字符是非终结符的情况 
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

// 创建F数组
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

// 创建优先表
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
				if (!Term(aa) && !Term(bb))//优先及等于的情况，用1值表示等于 
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
				if (!Term(aa) && Term(bb) && str[I][J + 2] != '\0' && !Term(str[I][J + 2]))//优先及等于的情况 
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
				if (!Term(aa) && Term(bb))//优先及小于的情况，用2值表示小于 
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
				if (Term(aa) && !Term(bb))//优先及大于的情况，用3值表示大于 
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

void print(char s[], char STR[][20], int q, int u, int ii, int k)//打印归约的过程 
{
	cout << u << "      ";
	for (int i = 0; i <= k; i++)
		cout << s[i];
	cout << "      ";
	for (int i = q; i <= ii; i++)
		cout << STR[0][i];
	cout << "      ";
}
void process(char STR[][20], int ii)//对输入的字符串进行归约的过程 
{
	cout << "步骤" << "   " << "符号栈" << "   " << "输入串" << "   " << "动作" << endl;
	int k = 0, q = 0, u = 0, b, i, j;
	char s[40], a;
	s[k] = '#';
	print(s, STR, q, u, ii, k);
	cout << "预备" << endl;
	k++; u++;
	s[k] = STR[0][q];
	q++;
	print(s, STR, q, u, ii, k);

	cout << "移进" << endl;
	while (q <= ii)
	{
		a = STR[0][q];
		if (!Term(s[k])) j = k;
		else j = k - 1;
		b = judge(s[j], a);
		if (b == 3)//大于的情况进行归约 
		{
			while (Term(s[j - 1]))
				j--;
			for (i = j; i <= k; i++)
				s[i] = '\0';
			k = j; s[k] = 'N'; u++;
			print(s, STR, q, u, ii, k);
			cout << "归约" << endl;

		}
		else if (b == 2 || b == 1)//小于或等于的情况移进 
		{
			k++;
			s[k] = a;
			u++;
			q++;
			print(s, STR, q, u, ii, k);
			if (s[0] == '#'&&s[1] == 'N'&&s[2] == '#')
				cout << "接受" << endl;
			else cout << "移进" << endl;

		}
		else
		{
			cout << "出错" << endl; break;
		}
	}
	if (s[0] == '#'&&s[1] == 'N'&&s[2] == '#')


		cout << "归约成功" << endl;
	else cout << "归约失败" << endl;
}
void printb()
{
	if (FLAG == 0&&og(n)==1)

	{
		for (int i = 0; i <= pf; i++)//打印FirstVT 
		{
			cout << "FirstVT(" << ft[i][0] << ")={";
			int x;
			for (x = 1; ft[i][x + 1] != '\0'; x++)
				cout << ft[i][x] << ",";
			cout << ft[i][x] << "}" << endl;
		}
		cout << "FirstVT(Q)={#}" << endl;
		for (int i = 0; i <= pl; i++)//打印LastVT 
		{
			cout << "LastVT(" << ft[i][0] << ")={";
			int y;
			for (y = 1; lt[i][y + 1] != '\0'; y++)
				cout << lt[i][y] << ",";
			cout << lt[i][y] << "}" << endl;
		}
		cout << "LastVT(Q)={#}" << endl;
		cout << "优先表如下:" << endl;
		cout << "  ";
		for (int i = 1; i <= kb; i++)//打印优先关系表 
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
		cout << "请输入要规约的字符串:" << endl;
		cin >> STR[0];
		int ii = strlen(STR[0]);
		STR[0][ii] = '#';
		cout << "下面是规约的过程:" << endl;
		process(STR, ii);
	}
}

int main()
{


	Cindata();


	if (og(n) == 0)
		cout << "文法G不是算符文法！" << endl;
	if (og(n) == 1)
	{
		cout << "文法G是算符文法！" << endl;

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